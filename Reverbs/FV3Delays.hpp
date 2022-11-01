#pragma once
#include "Reverbs/FreeVerb3.hpp"

namespace FreeVerb3
{
    ClassProcessor<fv3::blockDelay_> FltBlockDelay;
    ClassProcessor<fv3::blockDelay_> DblBlockDelay;

    ClassProcessor<fv3::delayline_> FltDelayLine;
    ClassProcessor<fv3::delayline_> DblDelayLine;
}

#include "FV3PlainDelay.hpp"
#include "FV3FeedbackDelay.hpp"
#include "FV3ModulationDelay.hpp"
#include "FV3DelayLine.hpp"
#include "FV3MultiDelayLine.hpp"