/*
    ==============================================================================
    This file is part of Tal-NoiseMaker by Patrick Kunz.
    Copyright(c) 2005-2010 Patrick Kunz, TAL
    Togu Audio Line, Inc.
    http://kunz.corrupt.ch
    This file may be licensed under the terms of of the
    GNU General Public License Version 2 (the ``GPL'').
    Software distributed under the License is distributed
    on an ``AS IS'' basis, WITHOUT WARRANTY OF ANY KIND, either
    express or implied. See the GPL for the specific language
    governing rights and limitations.
    You should have received a copy of the GPL along with this
    program. If not, go to http://www.gnu.org/licenses/gpl.html
    or write to the Free Software Foundation, Inc.,
    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
    ==============================================================================
 */

#pragma once

#include "SoundObject.hpp"

namespace FX::Filters
{
    class DCBlock : public FilterProcessor {
    public:
        DspFloatType inputs, outputs, lastOutput;
        DspFloatType cutoff;
        // cut is freq/sampleRate
        // don't forget to normalize by sampleRate [0..1]
        // default is 10hz at 44100
        DCBlock(DspFloatType cut = 10.0/44100.0) : FilterProcessor() {
            lastOutput = inputs = outputs = 0.0f;
            cutoff = cut;
        }

        ~DCBlock() {}

        inline void tick(DspFloatType *sample, DspFloatType cutoff) {
            outputs     = *sample - inputs + (0.999f - cutoff * 0.4f) * outputs;
            inputs      = *sample;
            lastOutput  = outputs;
            *sample     = lastOutput;
        }

        DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=0, DspFloatType Y=0)
        {
            DspFloatType out = I;
            tick(&out,cutoff);
            return out;
        }
    };
}