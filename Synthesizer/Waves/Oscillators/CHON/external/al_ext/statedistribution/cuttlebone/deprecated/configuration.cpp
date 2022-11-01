#include "Cuttlebone/Configuration.hpp"

ConfigurationData configurationData[] = {
    {"quux", "127.0.0.1", true, true, true},
    {"audio", "", false, true, false, false},
    {"gr02", "", false, false, false, false},
    {"gr03", "", false, false, false, false},
    {"gr04", "", false, false, false, false}, };

int main() {
  Configuration a;
  Configuration b(configurationData);
}
