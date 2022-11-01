require('luapa')
require('maximilian')

osc1 = maximilian.maxiPolyBLEP()


function noise(input,output,frames)
    for i = 0,2*frames-1,2 do
        local x = osc1:play(220.0)        
        luapa.float_set(output,i,x)
        luapa.float_set(output,i+1,x)        
    end
    
end 

luapa.set_audio_func(noise)
luapa.InitAudio(44100,64)
luapa.RunAudio()
luapa.StopAudio()