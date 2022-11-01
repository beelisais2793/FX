#pragma once
#include "FMPatch.h"
#include "FMSynth.h"
#include <jack/jack.h>
#include <jack/midiport.h>
#include <mutex>
#include <algorithm>
#include <memory>
#include <cstdio>
#include <cstring>

struct jack_client_deleter {
    void operator()(jack_client_t *x) const noexcept { jack_client_close(x); }
};

typedef std::unique_ptr<jack_client_t, jack_client_deleter> jack_client_u;

///
struct FILE_deleter {
    void operator()(FILE *x) const noexcept { fclose(x); }
};

typedef std::unique_ptr<FILE, FILE_deleter> FILE_u;
