#include "player.h"

#include <glog/logging.h>

#include <cmath>
#include <execution>
#include <memory>
#include <mutex>

#include "oscillator.h"

namespace {

constexpr float kNoteConversionMultiplier = 440.0f / 32.0f;

float NoteToFreq(float note) {
  return kNoteConversionMultiplier * std::pow(2.0f, ((note - 9.0f) / 12.0f));
}

} // namespace

Player::Player(Patch *patch, int num_voices, int sample_frequency)
    : patch_(patch), num_voices_(num_voices),
      sample_frequency_(sample_frequency) {
  std::vector<const GeneratorPatch *> generators = patch_->generators();

  for (int i = 0; i < num_voices; i++) {
    Voice v;
    for (auto &g : generators) {
      v.generators_.emplace_back(std::make_unique<Generator>(sample_frequency));
    }
    voices_.push_back(std::move(v));
  }

  patch_->RmGeneratorSignal.connect(
      [this](GeneratorPatch *g_patch, int gennum) {
        std::lock_guard<std::mutex> l(voices_mutex_);
        for (auto &voice : voices_) {
          voice.generators_.erase(voice.generators_.begin() + gennum);
        }
      });

  patch_->AddGeneratorSignal.connect([this](GeneratorPatch *g_patch) {
    std::lock_guard<std::mutex> l(voices_mutex_);
    for (auto &voice : voices_) {
      voice.generators_.push_back(
          std::make_unique<Generator>(sample_frequency_));
    }
  });
}

bool Player::Perform(const void *in_buffer, void *out_buffer,
                     size_t frames_per_buffer) {
  memset(out_buffer, 0, frames_per_buffer * sizeof(float));

  std::mutex buffer_mutex;
  std::vector<std::unique_ptr<std::complex<float>[]>> mix_buffers;

  {
    std::lock_guard<std::mutex> player_lock(voices_mutex_);
    auto g_patches = patch_->generators();

    std::for_each(
        std::execution::par_unseq, voices_.begin(), voices_.end(),
        [frames_per_buffer, this, &mix_buffers, &buffer_mutex,
         &g_patches](Voice &voice) {
          // Probably faster without the branch in a loop...
          for (int g_num = 0; g_num < voice.generators_.size(); g_num++) {
            auto &g = voice.generators_[g_num];
            if (!g->Playing())
              continue;
            auto mix_buffer =
                std::make_unique<std::complex<float>[]>(frames_per_buffer);
            g->Perform(*g_patches[g_num], mix_buffer.get(), voice.base_freq,
                       frames_per_buffer);
            {
              std::lock_guard<std::mutex> buffer_lock(buffer_mutex);
              mix_buffers.push_back(std::move(mix_buffer));
            }
          }
        });
  }

  // Mix down.
  if (!mix_buffers.empty()) {
    std::vector<std::complex<float>> mix_buffer(frames_per_buffer, 0.0f);
    for (auto &v_buffer : mix_buffers) {
      if (v_buffer)
        for (int i = 0; i < frames_per_buffer; i++) {
          mix_buffer[i] += v_buffer.get()[i];
        }
    }

    auto *f_buffer = (float *)out_buffer;
    for (int i = 0; i < frames_per_buffer; i++) {
      f_buffer[i] = mix_buffer[i].real();
    }
  }
  return true;
}

void Player::NoteOn(unsigned long ts, uint8_t velocity, uint8_t note) {
  std::lock_guard<std::mutex> player_lock(voices_mutex_);

  // A note with no velocity is not a note at all.
  if (!velocity)
    return;

  float base_freq = NoteToFreq(note);
  float vel = (float)velocity / 80;

  Voice *v = NewVoice();
  CHECK(v) << "No voice available";
  v->note = note;
  v->on_time = ts;
  v->base_freq = base_freq;
  v->velocity = vel;

  auto generators = patch_->generators();
  for (int g_num = 0; g_num < v->generators_.size(); g_num++) {
    auto &g = v->generators_[g_num];
    auto &gp = generators[g_num];
    g->NoteOn(*gp, ts, velocity, note);
  }
  // TODO legato, portamento, etc.
}

void Player::NoteOff(uint8_t note) {
  std::lock_guard<std::mutex> player_lock(voices_mutex_);
  auto gp_patches = patch_->generators();

  // Find the oscillator playing this and send it a note-off event.
  auto *v = VoiceFor(note);
  if (v == nullptr) {
    LOG(ERROR) << "Unable to find voice for: " << std::hex << note;
    return;
  }
  for (int g_num = 0; g_num < v->generators_.size(); g_num++) {
    auto &g = v->generators_[g_num];
    auto &gp = *gp_patches[g_num];
    g->NoteOff(gp, note);
  }
}

Player::Voice *Player::NewVoice() {
  for (auto &v : voices_) {
    if (!v.Playing())
      return &v;
  }

  // No free voice? Find the one with the lowest timestamp and steal it.
  unsigned long least_ts = INT_MAX;
  int stolen_voice_num = -1;
  for (int i = 0; i < num_voices_; i++) {
    if (voices_[i].on_time < least_ts) {
      stolen_voice_num = i;
      least_ts = voices_[i].on_time;
    }
  }
  if (stolen_voice_num == -1) {
    LOG(INFO) << "No voice to steal";
    return nullptr;
  }

  Voice &voice = voices_[stolen_voice_num];
  return &voice;
}

Player::Voice *Player::VoiceFor(uint8_t note) {
  for (auto &v : voices_) {
    if (v.note == note && v.Playing())
      return &v;
  }
  return nullptr;
}

bool Player::Voice::Playing() const {
  for (const auto &g : generators_) {
    if (g->Playing())
      return true;
  }
  return false;
}

Generator::Generator(int sample_frequency)
    : sample_frequency_(sample_frequency), e_a_(sample_frequency),
      e_k_(sample_frequency) {}

void Generator::Perform(const GeneratorPatch &patch,
                        std::complex<float> *out_buffer, float base_freq,
                        size_t frames_per_buffer) {
  std::vector<float> level_a(frames_per_buffer, 0.0f);
  std::vector<float> level_k(frames_per_buffer, 0.0f);
  std::vector<float> level_c(frames_per_buffer, 0.0f);
  std::vector<float> level_r(frames_per_buffer, 0.0f);
  std::vector<float> level_s(frames_per_buffer, 0.0f);
  std::vector<float> level_m(frames_per_buffer, 0.0f);

  patch.WithLock([&level_c, &level_m, &level_s, &level_r, &level_k, &level_a,
                  this,
                  &frames_per_buffer](const GeneratorPatch::Osc &osc,
                                      const GeneratorPatch::Envelope &a_env,
                                      const GeneratorPatch::Envelope &k_env) {
    std::fill(level_c.begin(), level_c.begin() + frames_per_buffer, osc.C);
    std::fill(level_r.begin(), level_r.begin() + frames_per_buffer, osc.R);
    std::fill(level_s.begin(), level_s.begin() + frames_per_buffer, osc.S);
    std::fill(level_m.begin(), level_m.begin() + frames_per_buffer, osc.M);
    std::fill(level_a.begin(), level_a.begin() + frames_per_buffer, osc.A);
    std::fill(level_k.begin(), level_k.begin() + frames_per_buffer, osc.K);
    for (int i = 0; i < frames_per_buffer; i++) {
      level_a[i] *= e_a_.NextSample(a_env);
      level_k[i] *= e_k_.NextSample(k_env);
    }
  });
  o_.Perform(frames_per_buffer, sample_frequency_, out_buffer, base_freq,
             level_a.data(), level_c.data(), level_m.data(), level_r.data(),
             level_s.data(), level_k.data());
}

void Generator::NoteOn(const GeneratorPatch &patch, unsigned long ts,
                       uint8_t velocity, uint8_t note) {
  patch.WithLock([this](const GeneratorPatch::Osc &osc,
                        const GeneratorPatch::Envelope &a_env,
                        const GeneratorPatch::Envelope &k_env) {
    e_a_.EnterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, a_env);
    e_k_.EnterStage(EnvelopeGenerator::ENVELOPE_STAGE_ATTACK, k_env);
  });
}

void Generator::NoteOff(const GeneratorPatch &patch, uint8_t note) {
  patch.WithLock([this](const GeneratorPatch::Osc &osc,
                        const GeneratorPatch::Envelope &a_env,
                        const GeneratorPatch::Envelope &k_env) {
    if (e_a_.Playing())
      e_a_.EnterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, a_env);
    if (e_k_.Playing())
      e_k_.EnterStage(EnvelopeGenerator::ENVELOPE_STAGE_RELEASE, k_env);
  });
}

bool Generator::Playing() const { return e_a_.Playing(); }

void Generator::Stop() {
  e_a_.EnterStage(EnvelopeGenerator::ENVELOPE_STAGE_OFF, {});
  e_k_.EnterStage(EnvelopeGenerator::ENVELOPE_STAGE_OFF, {});
}
