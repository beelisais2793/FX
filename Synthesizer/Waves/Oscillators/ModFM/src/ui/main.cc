#include <GLFW/glfw3.h>
#include <absl/strings/str_format.h>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include <csignal>
#include <unordered_map>

#include <portaudio.h>
#include <porttime.h>

#include "midi.h"
#include "player.h"
#include "ui/gui.h"

DEFINE_int32(midi, 0, "MIDI device to use for input. If not set, use default.");
DEFINE_string(device, "pulse", "Name of audio output device to use");

namespace {
constexpr int kSampleFrequency = 44100;

std::unique_ptr<GUI> kGUI;
std::unique_ptr<Patch> kPatch;
std::unique_ptr<MIDIReceiver> kMIDIReceiver;
std::unique_ptr<Player> kPlayer;

void SignalHandler(int signal) {
  LOG(ERROR) << "Signal #" << signal << " received, shutting down.";
  auto m_stop_status = kMIDIReceiver->Stop();
  if (!m_stop_status.ok()) {
    LOG(ERROR) << "Failure to stop MIDI receiver: " << m_stop_status.ok();
  }
  kGUI->Stop();
}

}  // namespace

int pa_output_callback(const void *in_buffer, void *out_buffer,
                       unsigned long frames_per_buffer,
                       const PaStreamCallbackTimeInfo *time_info,
                       PaStreamCallbackFlags status_flags, void *user_data) {
  auto *player = (Player *)user_data;
  if (status_flags != 0) {
    LOG(ERROR) << "Status: " << status_flags;
  }
  player->Perform(in_buffer, out_buffer, frames_per_buffer);
  return paContinue;
}

int main(int argc, char *argv[]) {
  FLAGS_logtostderr = true;
  google::InitGoogleLogging(argv[0]);
  gflags::ParseCommandLineFlags(&argc, &argv, false);

  glfwInit();

  LOG(INFO) << "Good morning.";

  LOG(INFO) << "Initializing PortAudio";
  PaError err = Pa_Initialize();
  CHECK(err == paNoError) << "PortAudio error: " << Pa_GetErrorText(err);

  PaDeviceIndex device = 0;
  for (int i = 0, end = Pa_GetDeviceCount(); i != end; ++i) {
    PaDeviceInfo const *info = Pa_GetDeviceInfo(i);
    if (!info) continue;
    std::string name = info->name;
    if (name == FLAGS_device) {
      device = i;
      break;
    }
  }
  CHECK(device != 0) << "Could not find device: " << FLAGS_device;

  PaStreamParameters audio_params;
  audio_params.device = device;
  audio_params.channelCount = 1;
  audio_params.sampleFormat = paFloat32;
  audio_params.suggestedLatency =
      Pa_GetDeviceInfo(audio_params.device)->defaultLowOutputLatency;
  audio_params.hostApiSpecificStreamInfo = nullptr;

  kPatch = std::make_unique<Patch>();
  kPlayer = std::make_unique<Player>(kPatch.get(), 8, kSampleFrequency);

  PaStream *stream;
  err = Pa_OpenStream(&stream, nullptr, &audio_params, kSampleFrequency, 4096,
                      paClipOff, pa_output_callback, kPlayer.get());
  CHECK_EQ(err, paNoError) << "PortAudio error: " << Pa_GetErrorText(err);

  // Set up the midi receiver and open the default device or what was passed in.
  kMIDIReceiver = std::make_unique<MIDIReceiver>();
  if (FLAGS_midi)
    CHECK(kMIDIReceiver->OpenDevice(FLAGS_midi).ok())
        << "Unable to open MIDI device";
  else
    CHECK(kMIDIReceiver->OpenDefaultDevice().ok())
        << "Unable to open MIDI device";

  // Wire in note on / off events to the player.
  kMIDIReceiver->NoteOffSignal.connect(&Player::NoteOff, kPlayer.get());
  kMIDIReceiver->NoteOnSignal.connect(&Player::NoteOn, kPlayer.get());

  err = Pa_StartStream(stream);
  CHECK_EQ(err, paNoError) << "PortAudio error: " << Pa_GetErrorText(err);

  LOG(INFO) << "Started PortAudio on device #" << audio_params.device;

  /* start timer, ms accuracy */
  Pt_Start(1, nullptr, nullptr);

  CHECK(kMIDIReceiver->Start().ok()) << "Unable to start MIDI device";

  kGUI = std::make_unique<GUI>(kPatch.get(), kMIDIReceiver.get());
  kGUI->Start();

  std::signal(SIGTERM, SignalHandler);
  std::signal(SIGSTOP, SignalHandler);
  std::signal(SIGABRT, SignalHandler);

  while (kGUI->running()) {
    Pa_Sleep(1000);
  }
  LOG(INFO) << "Closing...";
  if (kMIDIReceiver->running()) {
    CHECK(kMIDIReceiver->Close().ok()) << "Unable to close MIDI device";
    CHECK(kMIDIReceiver->Stop().ok()) << "Unable to stop MIDI device";
  }

  CHECK_EQ(Pa_StopStream(stream), paNoError);
  CHECK_EQ(Pa_CloseStream(stream), paNoError);

  LOG(INFO) << "Done.";

  return 0;
}
