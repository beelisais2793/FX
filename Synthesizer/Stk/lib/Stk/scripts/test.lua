snd = require('sndfile')

local b = bit.bor(snd.SF_FORMAT_WAV,snd.SF_FORMAT_PCM_16)
wav = snd.SoundFile("BabyElephantWalk60.wav",snd.SFM_READ,b)
v = snd.short_vector(4096)
wav:seek(0,0)
r = wav:read(v,4096)
while(r>0) do 
    for i=0,4095 do print(v[i]) end
    r = wav:read(v,4096)
end