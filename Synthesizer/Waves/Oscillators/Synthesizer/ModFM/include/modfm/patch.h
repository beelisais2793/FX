#pragma once

#include <functional>
#include <mutex>
#include <sigslot/signal.hpp>
#include <vector>

struct GeneratorPatch {
 public:
  GeneratorPatch(float ratio, float amplitude);

  struct Osc {
    float C, A, M, K, R, S;

    bool operator==(const Osc &rhs) const;
  };
  struct Envelope {
    float A_R;  // attack rate
    float A_L;  // attack peak level
    float D_R;  // decay rate
    float S_L;  // sustain level
    float R_R;  // release rate
    bool operator==(const Envelope &rhs) const;
  };
  GeneratorPatch(const Osc &osc, const Envelope &a_env, const Envelope &k_env);

  bool operator==(const GeneratorPatch &rhs) const;

  void WithLock(
      std::function<void(const Osc &, const Envelope &, const Envelope &)> f)
      const;

  void Update(std::optional<Osc> osc, std::optional<Envelope> a_env,
              std::optional<Envelope> k_env);

 private:
  mutable std::mutex gp_mutex_;

  Osc osc_;
  Envelope a_env_;
  Envelope k_env_;
};

class Patch {
 public:
  GeneratorPatch *AddGenerator();
  void RmGenerator(GeneratorPatch *patch);
  sigslot::signal<GeneratorPatch *> AddGeneratorSignal;
  sigslot::signal<GeneratorPatch *, int> RmGeneratorSignal;

  std::vector<const GeneratorPatch *> generators() const;

 private:
  mutable std::mutex patches_mutex_;
  std::vector<std::unique_ptr<GeneratorPatch>> generators_;
};

constexpr GeneratorPatch::Envelope kDefaultAmpEnvelope{0.025, 0.175, 0.25,
                                                       0.75};
constexpr GeneratorPatch::Envelope kDefaultCarEnvelope{0.05, 0.33, 0.25, 0.5};
