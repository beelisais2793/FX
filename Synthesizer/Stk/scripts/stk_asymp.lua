-- the idea is to use this in an adsr


function asymp_new(sample_rate)
    local a = {} 
    a.lastFrame = 0
    a.value = 0.0 
    a.target = 0.0 
    a.state = false 
    a.sample_rate = sample_rate
    a.factor = math.exp(-1. / (0.3 * sample_rate))
    a.constant = 0.0 

    a.keyOn = function(a) a:setTarget(1.0) end 
    a.keyOff = function(a) a:setTarget(0.0) end 
    a.setTau = asymp_setTau
    a.setTime= asymp_setTime
    a.setT60 = asymp_setT60 

    a.setTarget = asymp_setTarget
    a.setValue  = asymp_setValue 
    
    return a
end 

function asymp_setTau(a, tau)
    assert(tau > 0.0)
    a.factor = math.exp(-1.0 / (tau * a.sample_rate))
    a.constant = (1.0-a.factor)*a.target 
end 

function asymp_setTime(a,time)
    assert(time > 0.0)
    local tau = -time / math.log(1e-6)
    a.factor = math.exp(-1.0/(tau*a.sample_rate))
    a.constant = (1.0 - a.factor) * a.target 
end 

function asymp_setT60(a,t60)
    assert(t60 > 0.0)
    a:setTau(t60 / 6.91)
end 

function asymp_setTarget(a,target)
    a.target = target 
    if(a.value ~= a.target) a.state = true 
    a.constant = (1.0 - a.factor) * a.target 
end 

function asymp_setValue(a, value)
    a.state = false 
    a.target = value 
    a.value = value 
end 

function asymp_tick(a)
    if(a.state == true) then 
        a.value = a.factor * a.value + a.constant 
        if(a.target > a.value) then 
            if(( a.target - a.value ) <= 1e-6 ) then 
                a.value = a.target 
                a.state = false 
            end 
        else
            if((a.value - a.target) <= 1e-6) then 
                a.value = a.target 
                a.state = false 
            end 
        end 
        a.lastFrame = value
    end 
    return a.value 
end 

