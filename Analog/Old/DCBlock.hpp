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

namespace Filters::IIR
{
    class DCBlock : public FilterProcessor {
    public:
        double inputs, outputs, lastOutput;
        double cutoff;
        // cut is freq/sampleRate
        // don't forget to normalize by sampleRate [0..1]
        // default is 10hz at 44100
        DCBlock(double cut = 10.0/44100.0) : FilterProcessor() {
            lastOutput = inputs = outputs = 0.0f;
            cutoff = cut;
        }

        ~DCBlock() {}

        inline void tick(double *sample, double cutoff) {
            outputs     = *sample - inputs + (0.999f - cutoff * 0.4f) * outputs;
            inputs      = *sample;
            lastOutput  = outputs;
            *sample     = lastOutput;
        }

        double Tick(double I, double A=1, double X=0, double Y=0)
        {
            double out = I;
            tick(&out,cutoff);
            return out;
        }
    };
}