function SineOsc(sample_rate, table_size)
    local s = {} 
    s.table_size = table_size
    s.sample_rate= sample_rate
    s.phase = 0
    s.table = {}
    for i=0,table_size-1 do 
        local x = math.pi * 2 * i/table_size
        s.table[i] = math.sin(x)
    end 
    local maxi = -9999
    for i=0,table_size-1 do 
        if(s.table[i] > maxi) then maxi = s.table[i] end 
    end 
    
    for i=0,table_size-1 do 
        s.table[i] = s.table[i] / maxi
    end 
    s.SetTable = function(s,t)
        s.table_size = #t 
        s.table = t 
    end
    s.wrap = function(s,inp)
        while(inp < 0) do inp = inp + s.table_size end 
        while(inp >= s.table_size) do inp = inp - s.table_size end 
        return inp 
    end
        
    s.Generate = function(s,f)
        local lp = s.phase         
        s.phase = s.phase + s.table_size*(f/s.sample_rate)
        s.phase = s:wrap(s.phase)        
        return s:lookup(lp)
    end 
    s.GenerateFM = function(s,f,fm)
        local lp = s.phase 
        s.phase = s.phase + (f+fm)*s.table_size/s.sample_rate 
        s.phase = s:wrap(s.phase)        
        return s:lookup(lp)
    end 
    s.GeneratePM = function(s,f,pm)
        local lp = s.phase + s.table_size  * pm
        s.phase = s.phase + f*s.table_size/s.sample_rate 
        s.phase = s:wrap(s.phase)
        return s:lookup(lp)
    end 
    s.lookup = function(s,phase) 
        local i1 = s.table[math.floor(phase)]         
        local i2
        if(math.ceil(phase) >= s.table_size) then 
            i2 = s.table[0] 
        else             
            i2 = s.table[math.ceil(phase)]         
        end 
        local t  =  phase - math.floor(phase)
        return  (i1+((i2-i2)*t))
    end 
    s.SetSampleRate = function(s,sr) s.sample_rate = sr end 
    s.SetPhase = function(s,p) s.phase = p / (2*math.pi)*s.table_size end
    local meta = {}
    meta.__newindex =  function(o,i,v) o.table[i] = v end
    meta.__index = function(o,i) return o.table[i] end 
    setmetatable(s,meta)
    return s 
end 
