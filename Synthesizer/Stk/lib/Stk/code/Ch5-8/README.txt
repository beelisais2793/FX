Hi there.  These are the c code files for Chapters 1-4 of
the book, "Real Sound Synthesis for Interactive Applications."
by Perry R. Cook, AK Peters Publishing, 2002.
If you have this code but don't have the book, don't fear.
There are plenty of books still available for sale, so rush
to your favorite online or physical nerd book source and
get one!

Please read the README.txt file in the root Code directory for
instructions on compilation.  Check out the *Bin directories
for pre-compiled executables for Windows, Linux, and SGI.

The files in this directory are:

convolve.c    Convolve two .wav(snd) files to yield a third file
reverse.c     Reverse a .wav(snd) file
pitch.c       Simple autocorrelation pitch estimator, also synthesizes
                a file (psynth.wav) that is a pulse resynthesis of pitch

[fft] directory
fft.c         Convert a .wav(snd) file to an fft (.fft) file
peaksfft.c    Find N peaks from each frame of an .fft file, synthesis/removal option
notchfft.c    Remove enumerated peaks from each frame of an .fft file
subband.c     Split an .fft file into octave subbands, each goes in a new file
vocod.c       Cross-synthesizing octave channel-vocoder (digital) emulator
                 imparts "filter" file spectrum onto "source" file.

[lpc] directory
fitlpc.c      LPC analysis of a .wav(snd), writes an .lpc and a .res (residual) file
lpcresyn.c    Resynthesize a .lpc file back into a .wav(snd) file, with options
float2wv.c    convert a headerless raw float file into a .wav(snd) file.
wv2float.c    convert a .wav(snd) file into a headerless raw float file
              
Bonus Tracks!!:
frames.c      Chop a .wav(snd) file into windowed chunks.
squarewv.c    Generate a square wave from odd sinusoidal harmonics
Spechs22.wav(snd)  Source file for pitch detection and other processing
Synthsiz.wav(snd)  Source file for pitch detection and other processing
Music22.wav(snd)   Source file for processing

lpc/demphsiz.c    Helpful pre/de-emphasis functions for lpc.
lpc/emphasiz.c

fft/notchcup.bat  Script which removes the modes from the coffee cup file

fft/subbands.bat  Script that calls subband and converts them all back into .wav(snd) files

lpc/lpcspech.bat  Script for analysis/resynthesis warping using LPC

lpc/Spechs16.wav(snd)  Source file for processing
fft/cup22.wav     Source file for processing

MakeAll.bat     dos script to compile and test everything.  Assumes lc
                compiler.  Should work with UNIX/LINUX with appropriate
                modifications.  Might need to link -lm for math libraries.


