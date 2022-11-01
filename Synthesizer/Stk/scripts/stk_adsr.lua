
adsr_t = {
    ATTACK=0,
    DECAY=1,
    SUSTAIN=2,
    RELEASE=3,
    IDLE = 4
}

sample_rate = 44100

function adsr_new(attack,decay,sustain,release)
    local a = {} 
    a.state = IDLE 
    a.value = 0 
    a.target = 0 
    a.attackRate = 0.001 
    a.releaseRate = 0.005
    a.decayRate = 0.001 
    a.releaseTime = -1.0 
    a.sustainLevel = 0.5
    a.lastFrame = 0 

    a.keyOn = function(a) 
        if(a.target <= 0.0) then a.target = 1.0 end 
        a.state = adsr_t.ATTACK
    end
    
    a.keyOff= function(a) 
        a.target = 0.0 
        a.state  = adsr_t.RELEASE 
        if(a.releaseTime > 0.0) then 
            a.releaseRate = a.value / (a.releaseTime * sample_rate)
        end 
    end 

    a.setAttackRate = function(a,rate) 
        assert(rate >= 0.0)
        a.attackRate = rate
    end 
    a.setAttackTarget = function(a,target)
        assert(target >= 0.0)
        a.target = target 
    end 
    a.setDecayRate = function(a,rate)
        assert(rate >= 0.0)
        a.decayRate = rate
    end
    a.setSustainLevel = function(a,level)
        assert(level >= 0.0)
        a.sustainLevel = level 
    end 
    a.setReleaseRate = function(a,rate)
        assert(rate >= 0.0)
        a.releaseRate = rate 
    end 
    a.setAttackTime = function(a,time)
        assert(time > 0.0)
        a.attackRate = 1.0 / (time * sample_rate)
    end
    a.setDecayTime = function(a,time)
        assert(time > 0.0)
        a.decayRate = 1.0 / (time * sample_rate)
    end
    a.setReleaseTime = function(a,time)
        assert(time > 0.0)
        a.releaseRate = a.sustainLevel / (time * sample_rate)
        a.releaseTime = time
    end
    a.setAllTimes = function(a,attack,decay,sustain,release)
        a:setAttackTime(attack)
        a:setDecayTime(decay)
        a:setSustainLevel(sustain)
        a:setReleaseTime(release)
    end 
    a.tick = adsr_tick
    a.tick_frames = adsr_prcoess

    a:setAllTimes(attack,decay,sustain,release)
    return a
end 

function adsr_tick(a)
    if(a.state == adsr_t.ATTACK) then 
        a.value = a.value + a.attackRate 
        if(a.value >= a.target) then 
            a.value = a.target
            a.target = a.sustainLevel 
            a.state = adsr_t.DECAY
        end 
        a.lastFrame = a.value 
    elseif(a.state == adsr_t.DECAY) then 
        if(a.value > a.sustainLevel) then 
            a.value = a.value - a.decayRate 
            if(a.value <= a.sustainLevel) then 
                a.value = a.sustainLevel 
                a.state = adsr_t.SUSTAIN 
            end
        else 
            a.value = a.value + a.decayRate 
            if(a.value >= a.sustainLevel) then 
                a.value = a.sustainLevel 
                a.state = adsr_t.SUSTAIN 
            end 
        end 
        a.lastFrame = a.value 
    elseif(a.state == adsr_t.RELEASE) then 
        a.value = a.value - a.releaseRate 
        if(a.value <= 0.0) then 
            a.value = 0.0 
            a.state = adsr_t.IDLE 
        end 
        a.lastFrame = a.value 
    end 
    return a.value 
end 


function adsr_process(a, frames, channels, channel)   
    channels = channels or 1 
    channel  = channel or 0 
    for i=1,frames,channels do 
        frames[i+channel] = a:tick() 
    end 
    return frames 
end
