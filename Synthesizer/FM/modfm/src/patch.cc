#include "patch.h"

GeneratorPatch *Patch::AddGenerator() {
  std::lock_guard<std::mutex> patches_lock(patches_mutex_);
  generators_.push_back(std::make_unique<GeneratorPatch>(1.0, 0.5));
  GeneratorPatch *n_gp = generators_.back().get();
  AddGeneratorSignal(n_gp);
  return n_gp;
}

void Patch::RmGenerator(GeneratorPatch *patch) {
  std::lock_guard<std::mutex> patches_lock(patches_mutex_);
  for (auto it = generators_.begin(); it != generators_.end(); it++) {
    if (it->get() == patch) {
      RmGeneratorSignal(patch, it - generators_.begin());
      generators_.erase(it);
      break;
    }
  }
}

std::vector<const GeneratorPatch *> Patch::generators() const {
  std::lock_guard<std::mutex> patches_lock(patches_mutex_);
  std::vector<const GeneratorPatch *> gs;
  for (const auto &g : generators_) {
    gs.push_back(g.get());
  }
  return gs;
}

GeneratorPatch::GeneratorPatch(float ratio, float amplitude)
    : osc_{ratio, amplitude, 1, 0, 1, 0},
      a_env_(kDefaultAmpEnvelope),
      k_env_(kDefaultCarEnvelope){};

bool GeneratorPatch::operator==(const GeneratorPatch &rhs) const {
  return osc_ == rhs.osc_ && a_env_ == rhs.a_env_ && k_env_ == rhs.k_env_;
}

GeneratorPatch::GeneratorPatch(const GeneratorPatch::Osc &osc,
                               const GeneratorPatch::Envelope &a_env,
                               const GeneratorPatch::Envelope &k_env)
    : osc_(osc), a_env_(a_env), k_env_(k_env) {}

void GeneratorPatch::WithLock(
    std::function<void(const Osc &, const Envelope &, const Envelope &)> f)
    const {
  std::lock_guard<std::mutex> lg(gp_mutex_);
  f(osc_, a_env_, k_env_);
}

void GeneratorPatch::Update(std::optional<Osc> osc,
                            std::optional<Envelope> a_env,
                            std::optional<Envelope> k_env) {
  std::lock_guard<std::mutex> lg(gp_mutex_);
  if (osc) {
    osc_ = osc.value();
  }
  if (a_env) {
    a_env_ = a_env.value();
  }
  if (k_env) {
    k_env_ = k_env.value();
  }
}

bool GeneratorPatch::Osc::operator==(const GeneratorPatch::Osc &rhs) const {
  return C == rhs.C && A == rhs.A && M == rhs.M && K == rhs.K && R == rhs.R &&
         S == rhs.S;
}

bool GeneratorPatch::Envelope::operator==(
    const GeneratorPatch::Envelope &rhs) const {
  return A_R == rhs.A_R && A_L == rhs.A_L && D_R == rhs.D_R && S_L == rhs.S_L &&
         R_R == rhs.R_R;
}
