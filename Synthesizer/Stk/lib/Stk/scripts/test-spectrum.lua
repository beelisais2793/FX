G = require('luagist')
snd = require('sndfile')
Q = require('spectrum')

-- compare with gist
local b = bit.bor(snd.SF_FORMAT_WAV,snd.SF_FORMAT_PCM_16)
wav = snd.SoundFile("libsndfile/BabyElephantWalk60.wav",snd.SFM_READ,b)
local len = 2^10
v = snd.short_vector(len)
wav:seek(0,0)
for i=1,100 do
    r = wav:read(v,len)
end
x = Q.Vector()


Q.short_to_float_vector(v,x)


q =  G.GistF(len,44100)
sq = Q.Spectrum(len,44100,Q.HanningWindow)

--while r > 0 do
t1 = snd.float_vector(len)

for i=1,len do t1[i-1] = v[i-1]/32768 end 

print('gist')
q:processAudioFrame(t1)

print("ed=",q:energyDifference())
print("sd=",q:spectralDifference())
print("sdhw=",q:spectralDifferenceHWR())
print(q:complexSpectralDifference())
print(q:highFrequencyContent())
print(q:rootMeanSquare())
print(q:peakEnergy())
print(q:zeroCrossingRate())
print(q:spectralCentroid())
print(q:spectralCrest())
print(q:spectralFlatness())
print(q:spectralRolloff())
print(q:spectralKurtosis())
print(q:pitch())

print()
print("q")

sq:ProcessAudioFrame(x)
print(sq:EnergyDifference())
print(sq:SpectralDifference())
print(sq:SpectralDifferenceHWR())
print(sq:ComplexSpectralDifference())
print(sq:HighFrequencyContent())
print(sq:RMS())
print(sq:PeakEnergy())
print(sq:ZeroCrossingRate())
print(sq:Centroid())
print(sq:Crest())
print(sq:Flatness())
print(sq:Rolloff())
print(sq:Kurtosis())

print(sq:Pitch())

--r = wav:read(v,len)
--Q.short_to_float_vector(v,x)
--end