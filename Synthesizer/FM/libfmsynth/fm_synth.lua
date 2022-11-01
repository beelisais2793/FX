-- use xoscope to see the waveform on the device
require('audiosystem')
require('fluidlite')
require('vector')
settings = fluidlite.new_fluid_settings()
synth    = fluidlite.new_fluid_synth(settings)
fluidlite.fluid_synth_sfload(synth,"piano.sf2",1)
freq = 220
left = vector.float_vector(256)
right = vector.float_vector(256)


function new_buffer(p)
    local b = {}
    b.buffer = p
    local mt = {} 
    mt.__index = function(b,i) return audiosystem.float_get(b.buffer,i) end
    mt.__newindex = function(b,i,v) audiosystem.float_set(b.buffer,i,v) end 
    setmetatable(b,mt)
    return b
end 

function noise(input,output,frames)            
    fluidlite.fluid_synth_write_float(synth,frames,output,0,2,output,0,2)    
end 

function freq_to_midi(f)
    return 12.0*math.log(f/440.0)/math.log(2) + 69
end 
function midi_to_freq(m)
    return math.pow(2.0, (m-69)/12)*440.0
end
function note_on(c,n,v)            
    fluidlite.fluid_synth_noteon(synth,c,n,127)
end
function note_off(c,n,v)    
    fluidlite.fluid_synth_noteoff(synth,c,n)
end


audiosystem.Init()
audiosystem.Pm_Initialize()

audiosystem.set_note_on_func(note_on)
audiosystem.set_note_off_func(note_off)

for i=0,audiosystem.GetNumMidiDevices()-1 do 
    print(i,audiosystem.GetMidiDeviceName(i))
end

audiosystem.set_audio_func(noise)
device=14
audiosystem.Pa_Initialize()
for i=0,audiosystem.GetNumAudioDevices()-1 do 
    if( audiosystem.GetAudioDeviceName(i) == 'pulse') then        
        device = i 
        goto done
    end    
end
::done::
audiosystem.InitMidiDevice(1,3,3)
audiosystem.InitAudioDevice(device,-1,2,44100,256)
audiosystem.RunAudio()
audiosystem.StopAudio()

fluidlite.delete_fluid_synth(synth);
fluidlite.delete_fluid_settings(settings);