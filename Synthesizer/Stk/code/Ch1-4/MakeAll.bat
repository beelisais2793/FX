lc adpcmcod.c
lc adpcmdec.c
lc decimate.c
lc demulaw.c
lc mulaw.c
lc quantize.c
lc srconvrt.c
lc timeshif.c

adpcmcod Trumps22.wav test.adp
adpcmdec test.adp testADP.wav
mulaw Trumps22.wav test.mu
demulaw test.mu testMU.wav
decimate 4 Trumps22.wav testDEC.wav
quantize 5 Trumps22.wav testQUAN.wav
srconvrt 0.5 -f 2.0 Trumps22.wav testSRC.wav
timeshif 0.4 Trumps22.wav testTIME.wav

