require('luapa')
require('luastk')
require('daisysp')

osc = daisysp.BlOsc()
osc:Init(44100)
osc:SetFreq(220.0)
osc:SetAmp(1.0)
osc:SetWaveform(daisysp.BlOsc.WAVE_TRIANGLE)
function noise(input,output,frames)
    for i = 0,2*frames-1,2 do
        local x = osc:Process()       
        luapa.float_set(output,i,x)
        luapa.float_set(output,i+1,x)
    end
    
end 

luapa.set_audio_func(noise)
luapa.InitAudio(44100,64)
luapa.RunAudio()
luapa.StopAudio()