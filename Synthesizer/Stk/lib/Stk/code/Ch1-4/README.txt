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

quantize.c    Quantize a .wav(snd) file to N bits resolution
decimate.c    Decimate a .wav(snd) file by factor N (keep only each Nth sample)

mulaw.c       Code a .wav(snd) file to 8 bit non-linear mu-law quantization
demulaw.c     Decode a .mu file back into a .wav(snd) file
adpcmcod.c      Codes a .wav(snd) file to 4 bit ADPCM, suggest extension .adp
adpcmdec.c      Decodes an .adp file back into a .wav(snd) file

srconvrt.c   Resample a file, different methods,
                option modify header alone or modify SRATE
timeshif.c   Timeshift a file, options on window hopping

masprdmp.c   Direct and discrete physical simulation of Mass/Spring/Damper system

[Filters]
   filter.c  Filter a .wav(snd) file according to a spec. (.spc) file.

Bonus code:  
sweepsin.c   Synthesize a swept sine wave.
Makeall.bat  dos command line script which compiles and tests everything.
                assumes lc compiler.  Change lc to whatever else to try
                alternate compiler.  Making this file executable under
                UNIX/LINUX should work too (but you might need to -lm
                (link to math library) under some OS/compiler combinations).
MakeAll.SGI  script to compile and test everything on SGI, assumes gcc compiler

Trumps22.wav(snd)  Soundfile for testing
