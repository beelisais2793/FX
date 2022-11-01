fitlpc 20 800 100 spechs16.wav temp.lpc temp.res
lpcresyn temp.lpc temp.flt
float2wv temp.flt resyn.wav -s 16000 -g 0.0
lpcresyn temp.lpc temp.flt -t 3.0 -p 1.4
float2wv temp.flt resyn2.wav -s 16000

del temp.flt
del temp.res
del temp.lpc

