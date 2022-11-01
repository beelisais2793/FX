// this should serve as a central point of configuration for broadcast apps.
// right now, broadcast address and host configuration (simulator, audio,
// visual, device) is supported here. but in the future, node geometries may be
// kept here in support of spatiallized sound and graphics.
//

#ifndef __HOSTROLE__
#define __HOSTROLE__

#include <stdlib.h>  // perror
#include <string.h>  // strncmp
#include "Cuttlebone/Log.hpp"

struct ConfigurationData {
  const char* name;
  const char* broadcast;
  bool simulation, audio, visual, device;
};

ConfigurationData defaultConfigurationData[] = {
    //name    broadcast         sim   audio  vis   device  
    {"gr01", "192.168.10.255", true, false, false, false},
    {"audio.10g", "", false, true, false, false},
    {"gr02", "", false, false, true, false},
    {"gr03", "", false, false, true, false},
    {"gr04", "", false, false, true, false},
    {"gr05", "", false, false, true, false},
    {"gr06", "", false, false, true, false},
    {"gr07", "", false, false, true, false},
    {"gr08", "", false, false, true, false},
    {"gr09", "", false, false, true, false},
    {"gr10", "", false, false, true, false},
    {"gr11", "", false, false, true, false},
    {"gr12", "", false, false, true, false},
    {"gr13", "", false, false, true, false},
    {"gr14", "", false, false, true, false}, };

struct Configuration : ConfigurationData {
  bool foundSelf;

  Configuration() : Configuration(defaultConfigurationData) {
    LOG("loaded default configuration");
  }

  template <unsigned N>
  Configuration(ConfigurationData (&configurationCandidate)[N]) {

    char hostname[256];
    if (gethostname(hostname, 256) == -1) {
      perror("gethostname");
      exit(-1);
    }

    unsigned self = 0;
    foundSelf = false;
    for (unsigned i = 0; i < N; ++i)
      if (strncmp(configurationCandidate[i].name, hostname, 256) == 0) {
        foundSelf = true;
        self = i;
        break;
      }

    if (foundSelf) {
      name = configurationCandidate[self].name;
      simulation = configurationCandidate[self].simulation;
      audio = configurationCandidate[self].audio;
      visual = configurationCandidate[self].visual;
      device = configurationCandidate[self].device;
      broadcast = configurationCandidate[self].broadcast;

    } else {
      name = hostname;
      broadcast = "127.0.0.1";
      simulation = true;
      audio = true;
      visual = true;
      device = true;
    }
    log();
  }

  void log() {
    if (foundSelf)
      LOG("i am %s; i do%s%s%s%s.", name, simulation ? " simulation" : "",
          audio ? " audio" : "", visual ? " visual" : "",
          device ? " device" : "");
    else
      LOG("running in \"laptop mode\".");
  }
};

#endif
