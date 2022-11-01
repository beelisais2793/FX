--[[ BandedWG ]]--
require('sample')
require('adsr')

errorTypes = {
    STATUS=0,
    WARNING=1,
    DEBUG_PRINT=2,
    MEMORY_ALLOCATION=3,
    MEMORY_ACCESS=4,
    FUNCTION_ARGUMENT=5,
    FILE_NOT_FOUND=6,
    FILE_UNKNOWN_FORMAT=7,
    FILE_ERROR=8,
    PROCESS_THREAD=9,
    PROCESS_SOCKET=10,
    PROCESS_SOCKET_IPADDR=11,
    AUDIO_SYSTEM=12,
    MIDI_SYSTEM=13,
    UNSPECIFIED=14
}

stkTypes = {
    STK_SINT8=0x1,
    STK_SINT16=0x2,
    STK_SINT24=0x4,
    STK_SINT32=0x8,
    STK_FLOAT32=0x10,
    STK_FLOAT64=0x20
}

sampleRate  = 44100
rawWavePath = 'data'

function setSampleRate(sr)
    sampleRate = sr 
end 
function setRawWavePath(path)
    rawWavePath = path 
end 

function SampleVector(nFrames,nChannels)
    local f = {} 
    f.nFrames = nFrames or 256
    f.nChannels= nChannels or 1
    f.data = sample.FloatSampleVector(nFrames,nChannels)
    local mt = {}
    mt.__index = function(f,i) return f.data[i] end 
    mt.__newindex = function(f,i,v) f.data[i] = v end 
    mt.__add = function(a,b) return a.data + b.data end
    mt.__sub = function(a,b) return a.data - b.data end
    mt.__mul = function(a,b) return a.data * b.data end
    mt.__div = function(a,b) return a.data / b.data end
    mt.__unm = function(a) return -a.data end
    mt.__pow = function(a,x) return sample.pow(a.data,x) end     
    setmetatable(f,mt)
end

function BowTable_new()
    local bt = {} 
    bt.offset = 0.0
    bt.slope  = 0.0
    bt.minOutput = 0.01
    bt.maxOutput = 0.98
    bt.setOffset = BowTable_setOffset;
    bt.setSlope  = BowTable_setSlope;
    bt.setMinOutput = BowTable_setMinOutput;
    bt.setMaxOutput = BowTable_setMaxOutput;
    bt.tick1 = BowTable_tick1;
    bt.tick  = BowTable_tick;
    return bt
end

function BowTable_setOffset(bt,offset)
    bt.offset = offset
end 
function BowTable_setSlope(bt,slope)
    bt.slope = slope 
end 
function BowTable_setMinOutput(bt,min)
    bt.minOutput = min 
end 
function BowTable_setMaxOutput(bt,max)
    bt.maxOutput = max 
end 
function BowTable_tick1(bt, in)
    local sample = input + bt.offset
    sample = sample * bt.slope 
    local last   = math.fabs(sample) + 0.75 
    last = last ^ -4.0
    if(last < bt.minOutput) then 
        last = bt.minOutput 
    end
    if(last > bt.maxOutput) then 
        last = bt.maxOutput 
    end 
    return lat
end
function BowTable_tick1(bt, n, vec)
    for i = 0,n-1 do 
        vec[i] = bt:tick(vec[i])
    end 
    return vec
end

MAX_BANDED_NODES=20

function make_table(n) 
    local t = {} 
    for i=1,n do 
        t[i] = 0 
    end 
    return t
end 

function BandedWG_new()
    local bwg = {} 
    bwg.doPluck = true
    bwg.trackVelocity = false
    bwg.bowTable = BowTable_new()
    bwg.bowTable:setSlope(3.0)
    bwg.adsr = adsr_new(0.02,0.005,0.9.0.01)    
    bwg.frequency   = 220.0
    bwg.nModes      = 0
    bwg.presetModes = 0
    bwg.bandpass = make_table(MAX_BANDED_MODES)
    bwg.delay    = make_table(MAX_BANDED_MODES)
    bwg.modes    = make_table(MAX_BANDED_MODES)
    bwg.gains    = make_table(MAX_BANDED_MODES)
    bwg.basegains= make_table(MAX_BANDED_MODES)
    bwg.excitation= make_table(MAX_BANDED_MODES)
    bwg.integrationConstant = 0
    bwg.clear = BandedWG_clear;
    bwg.setStrikePosition = BandedWG_setStrikePosition;
    bwg.setPreset = BandedWG_setPreset;
    bwg.setFrequency = BandedWG_setFrequency;
    bwg.startBowing = BandedWG_startBowing;
    bwg.stopBowing = BandedWG_stopBowing;
    bwg.pluck = BandedWG_pluck;
    bwg.noteOn = BandedWG_noteOn;
    bwg.noteOff = BandedWG_noteOff;
    bwg.tick = BandedWG_tick;

    bwg:setPreset(0)
    bwg.bowPosition = 0
    bwg.bassGain = 0.999
    bwg.integrationConstant = 0.0
    bwg.trackVelocity = false
    bwg.bowVelocity = 0.0
    bwg.bowTarget = 0.0
    bwg.strikeAmp = 0.0
    return bwg 
end 

function BandedWG_clear(bwg)
    for i = 0; i < bwg.nModes
end

function BandedWG_setStrikePosition(bwg,pos)

end

function BandedWG_setPreset(bwg,preset)

end

function BandedWG_setFrequency(bwg, freq)

end

function BandedWG_startBowing( bwg, amp, rate)

end

function BandedWG_stopBowing( bwg, rate)

end

function BandedWG_pluck(bwg, amp)

end

function BandedWG_noteOn(bwg, freq, amp)

end

function BandedWG_noteOff(bwg, amp)

end

function BandedWG_tick1(bwg,in)

end

function BandedWG_tick(bwg,n, vec)
    for i=0,n-1 do
        vec[i] = BandedWG:tick1(bwg,vec[i])
    end 
    return vec
end