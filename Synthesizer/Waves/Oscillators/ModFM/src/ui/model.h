#pragma once

#include "patch.h"

struct GeneratorModel {
  bool on;
  GeneratorPatch::Osc osc;
  GeneratorPatch::Envelope a_env;
  GeneratorPatch::Envelope k_env;
};
