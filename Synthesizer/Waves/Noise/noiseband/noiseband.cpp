
//
// noiseband.cpp
//
// Mark Zadel, 2010
//
//
// bandlimited noise by a brute force, additive synthesis method
//
// alternatives: FFT^-1, or a bandpass filter on broadband noise
//
//
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

// FIXME / TODO
//  add -h help message
//  seed random generator with srand (eg, say you want to do it twice for
//   two-channel decorrelated noise)
//  add option for 16 or 24-bit output instead of 64-bit floats

#include "SineWave.h"
#include "FileWrite.h"

#include  <getopt.h>
#include  <math.h>
#include  <stdlib.h>

enum { WHITENOISE = 0, PINKNOISE };

static struct option long_options[] = {
    {"length",      required_argument, 0, 'l'},
    {"startfreq",   required_argument, 0, 's'},
    {"endfreq",     required_argument, 0, 'e'},
    {"ppo",         required_argument, 0, 'p'}, // partials per octave
    {"numpartials", required_argument, 0, 'n'}, // total number of partials
    {"white",       no_argument,       0, 'w'},
    {"pink",        no_argument,       0, 'k'},
    {"outfile",     required_argument, 0, 'o'},
    {0, 0, 0, 0}
};

void normalize( StkFrames& buffer )
{

    // this code is from the STK FileWvIn class

    size_t i;
    StkFloat max = 0.0;

    for ( i=0; i<buffer.size(); i++ ) {
        if ( fabs( buffer[i] ) > max )
            max = (StkFloat) fabs((double) buffer[i]);
    }

    std::cout << "normalize(): max amplitude found is " << max << std::endl;

    if (max > 0.0) {
        max = 1.0 / max;
        for ( i=0; i<buffer.size(); i++ )
            buffer[i] *= max;
    }

}

StkFloat getlograndfreq( StkFloat lofreq, StkFloat hifreq ) {

    StkFloat randval = rand() / (RAND_MAX + 1.0);

    return pow(hifreq/lofreq, randval) * lofreq;

}

StkFloat getlinrandfreq( StkFloat lofreq, StkFloat hifreq ) {

    StkFloat randval = rand() / (RAND_MAX + 1.0);

    return randval * (hifreq-lofreq) + lofreq;

}

StkFloat numoctaves( StkFloat lofreq, StkFloat hifreq ) {
    // return the number of octaves spanned by (lofreq,hifreq)
    return log2( hifreq / lofreq );
}

int main( int argc, char **argv ) {

    StkFloat lengthseconds = 2;
    StkFloat lofreq = 10;
    StkFloat hifreq = 22000;
    StkFloat partialsperoctave = 1500.0;
    unsigned long numpartials = 0;
    unsigned int noisetype = WHITENOISE;
    std::string outfilename( "outfile.wav" );

    // -(option parsing)--------------------------------------------------------

    int optionfound = -1;
    unsigned char ppospecified = 1;
    while( 1 ) {

        int option_index = 0; // getopt_long stores the option index here.

        optionfound = getopt_long (argc, argv, "l:s:e:p:n:wko:", long_options, &option_index);

        // end of options
        if (optionfound == -1)
            break;

        switch (optionfound) {

            case 'l':
                lengthseconds = atof( optarg );
                break;

            case 's':
                lofreq = atof( optarg );
                break;

            case 'e':
                hifreq = atof( optarg );
                break;

            case 'p':
                partialsperoctave = atof(optarg);
                ppospecified = 1;
                break;

            case 'n':
                numpartials = atol(optarg);
                ppospecified = 0;
                break;

            case 'w':
                noisetype = WHITENOISE;
                break;

            case 'k':
                noisetype = PINKNOISE;
                break;

            case 'o':
                outfilename = optarg;
                break;

            default:
                abort();

        }

    };

    unsigned long numsamples = static_cast<unsigned long>(lengthseconds * Stk::sampleRate());

    // need to do this at the end since it depends on lofreq/hifreq and can be
    // overridden by --numpartials
    if (ppospecified)
        numpartials = static_cast<unsigned long>(partialsperoctave * numoctaves( lofreq, hifreq ));

    if ( lofreq >= hifreq ) {
        std::cout << "ERROR: low freq (" << lofreq
            << ") is not lower than high freq (" << hifreq
            << ")" << std::endl;
        exit(0);
    };

    // -(main routine)----------------------------------------------------------

    // final output
    StkFrames output(0.0, numsamples, 1);

    // sine ugen
    SineWave thissine;

    std::cout << "computing " << numpartials << " partials" << std::endl;

    for ( unsigned long i = 0; i < numpartials; i++ ) {

        // progress indicator
        std::cout << "." << std::flush;

        if ( i % 20 == 0 )
            std::cout << " " << i << " " << std::flush;

        if ( noisetype == WHITENOISE ) {
            thissine.setFrequency( getlinrandfreq( lofreq, hifreq ) );
        } else { // PINKNOISE
            thissine.setFrequency( getlograndfreq( lofreq, hifreq ) );
        }

        // NB just changing the frequency in this way but using the same
        // SineWave ugen effectively randomises the starting phase.  we don't
        // want all of the partials phase aligned at the start since that leads
        // to a big amplitude spike (throwing off the normalisation step).

        for ( unsigned long j = 0; j < numsamples; j++ ) {
            // NB we're assuming here that we're not going to overflow the
            // doubles.  max I've seen is ~200.0, so we should be safe.
            output[j] += thissine.tick();
        }

    }

    std::cout << "done computing" << std::endl;

    normalize( output );

    std::cout << "done normalising" << std::endl;

    // write to file
    FileWrite outfile( outfilename, 1, FileWrite::FILE_WAV, Stk::STK_SINT16 );
    outfile.write( output );
    outfile.close();

    std::cout << "wrote to " << outfilename << std::endl;

    std::cout << "all done" << std::endl;

}


// vim:sw=4:ts=4:et:ai:ic:
