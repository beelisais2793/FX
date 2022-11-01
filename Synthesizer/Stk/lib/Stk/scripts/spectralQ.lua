require('luakissfft')
require('luakissfftr')

RectangularWindow = 0
HanningWindow = 1
HammingWindow = 2 
BlackmanWindow = 3 
TukeyWindow=4 

function vectorize(buffer)
    local v = kissfft.float_vector(#buffer)
    for i=1,#buffer do 
        v[i-1] = buffer[i] 
    end 
    return v 
end 

function unvectorize(vector)
    local buffer={} 
    for i=0,vector.size()-1 do 
        buffer[i+1] = vector[i] 
    end 
    return buffer 
end 

function sum(v)
    local s = 0.0 
    for i=1,#v do 
        s = s + v[i] 
    end 
    return s 
end 

function FrequencyDomain_new() 
    local fd = {} 
    fd.Centroid = spectralCentroid 
    fd.Flatness = spectralFlatness 
    fd.Crest    = spectralCrest 
    fd.Rolloff  = spectralRollOff
    fd.Kurtosis = spectralKurtosis 
    return fd 
end 

function spectralCentroid(ms)     
    local sumAmplitudes = 0.0;
    local sumWeightedAmplitudes = 0.0;
    for i=1,#ms do 
        sumAmplitudes           = sumAmplitudes + ms[i] 
        sumWeightedAmplitudes   = sumWeightedAmplitudes + ms[i]*i
    end        
    if (sumAmplitudes > 0) then return sumWeightedAmplitudes / sumAmplitudes end
    return 0.0    
end 

function spectralFlatness(ms)
    local sumVal = 0.0;
    local logSumVal = 0.0;
    local N = (double)magnitudeSpectrum.size();
    local flatness;
    for i=1,#ms do 
        local v = 1 + ms[i]
        sumVal = sumVal + v 
        logSumVal = logSumVal + math.log(v)
    end
    sumVal = sumVal / N;
    logSumVal = logSumVal / N;
    if (sumVal > 0) then 
        flatness = (T)(exp (logSumVal) / sumVal);
    else
        flatness = 0.0;
    end 
    return flatness;
end

function spectralCrest(ms)
    local sumVal = 0.0;
    local maxVal = 0.0;
    local  N = (T)magnitudeSpectrum.size();

    for i=1,#ms do 
        local v = ms[i]*ms[i] 
        sumVal = sumVal + v 
        if(v > maxVal) then maxVal = v end 
    end 
        
    local spectralCrest;

    if (sumVal > 0) then     
        local meanVal = sumVal / N;
        spectralCrest = maxVal / meanVal;
    else        
        spectralCrest = 1.0;
    end

    return spectralCrest;
end

function spectralRollOff(ms,p)
    local percentile = p or 0.5 
    local sumOfMagnitudeSpectrum = sum(ms)
    local threshold = sumOfMagnitudeSpectrum * percentile;    
    local cumulativeSum = 0;
    local index = 0;    
    for i=1,#ms do 
        cumulativeSum = cumulativeSum + ms[i] 
        if(cumulativeSum > threshold) then 
            index = i 
            goto sr_rolloff_break
        end 
    end 
    ::sr_rolloff_break::
    local spectralRolloff = index / #ms    
    return spectralRolloff;
end

function spectralKurtosis(ms)    
    local sumOfMagnitudeSpectrum = sum(ms)    
    local mean = sumOfMagnitudeSpectrum / #ms    
    local moment2 = 0;
    local moment4 = 0;
    
    for i=1,#ms do 
        local difference = ms[i] - mean 
        local squaredDifference = difference*difference
        moment2 = moment2 + squaredDifference 
        moment4 = moment4 + squaredDifference*squaredDifference 
    end 
    
    moment2 = moment2 / #ms 
    moment4 = moment4 / #ms
        
    if (moment2 == 0) then     
        return -3.0;    
    else    
        return (moment4 / (moment2*moment2)) - 3.0;
    end
end 

function createHanningWindow(num_samples)
    local window = {}
    for i=1,num_samples do 
        window[i] = 0.5 * (1 - math.cos(2*math.pi * (i/num_samples)))
    end 
    return window 
end 

function createHammingWindow(num_samples)
    local window = {}     
    for i=1,num_samples do 
        window[i] = 0.64 - (0.46 * math.cos(2*math.pi * (i/num_samples)))
    end 
    return window 
end 

function createBlackmanWindow(num_samples)
    local window = {} 
    for i=1,num_samples do 
        window[i] = 0.42 - (0.5 * math.cos(2*math.pi * (i/num_samples))) 
                    + (0.08 * math.cos(4 * math.pi * (i/num_samples)))
    end 
    return window 
end 

function createTukeyWindow(num_samples,alpha)
    local window = {} 
    local value = (-1*(num_samples/2)) + 1
    alpha = alpha or 0.5
    for i=1,num_samples do 
        if(value >= 0 and value <= (alpha * (num_samples/2))) then 
            window[i] = 1.0 
        elseif(value <= 0 and (value >= (-1*alpha*(num_samples/2)))) then 
            window[i] = 1.0 
        else 
            window[i] = 0.5 * (1 + math.cos(math.pi *(((2*value)/(alpha*num_samples))-1)))
        end 
        value = value + 1
    end 
    return window 
end 

function createRectangularWindow(num_samples)
    local window = {} 
    for i=1,num_samples do 
        window[i] = 1.0 
    end 
    return window 
end 

function createWindow(num_samples, window_type)
    if(window_type == RectangularWindow) then 
        return createRectangularWindow(num_samples)
    elseif(window_type == HanningWindow) then 
        return createHanningWindow(num_samples)    
    elseif(window_type == HammingWindow) then 
        return createHammingWindow(num_samples)
    elseif(window_type == BlackmanWindow) then 
        return createBlackmanWindow(num_samples)
    else 
        return createTukeyWindow(num_samples,0.5)
    end        
end 

function rootMeanSquare(buffer)
    local sum = 0 
    for i=1,#buffer do 
        sum = sum + buffer[i]^2
    end 
    return math.sqrt(sum / #buffer)
end 

function peakEnergy(buffer)
    local peak = -10000 
    for i=1,#buffer do 
        local absSample = math.abs(buffer[i])
        if(absSample > peak) then 
            peak = absSample 
        end 
    end 
    return peak 
end 

function zeroCrossingRate(buffer)
    local zcr = 0 
    for i=2,#buffer do 
        local current = buffer[i] > 0 
        local previous = (buffer[i-1] > 0)
        if( current ~= previous) then 
            zcr = zcr + 1 
        end 
    end 
    return zcr
end 

function mfcc_new(frame_size, sample_rate)
    local mfcc = {} 
    mfcc.frame_size = frame_size 
    mfcc.sample_rate= sample_rate 
    mfcc.MelCoefficients = calculateMelCoefficient 
    mfcc.MelSpectrum = calculateMelSpectrum 
    mfcc.mel_spectrum = {} 
    mfcc.MFCCs = {} 
    return mfcc 
end 

function yin_SetSampleRate(yin,sr)
    local oldFs = yin.sample_rate
    yin.sample_rate = sr 
    yin.min_period = (sr / oldFs) * yin.min_period 
end 

function yin_SetMaxFrequency(yin,max_freq)
    local minPeriodFloating 
    if(max_freq <= 200) then max_freq = 2000.0 end 
    minPeriodFloating = yin.sample_rate / max_freq 
    yin.min_period = math.ceil(minPeriodFloating)
end 

function yin_Pitch(yin,frame)
    local period 
    local fPeriod 
    yin.MeanNormalise(frame)
    local cp = yin:SearchRecentMinima(yin.delta)
    if(cp == -1) then 
        period = yin:GetPeriodCandidate(yin.delta)
    else 
        period = cp 
    end
    if(period > 0 and period < (#yin.delta -1)) then 
        fPeriod = yin:parabolicInterp(period,yin.delta[period-1], yin.delta[period], yin.delta[period+1])
    else 
        fPeriod = period 
    end 
    yin.prev_period_estimate = fPeriod 
    return yin:PeriodToPitch(fPeriod)
end 

function yin_MeanNormalize(yin,frame)
    local cs = 0 
    local L = #frame/2 
    local dp = yin.delta 
    local x = 1
    for tau=1,L do 
        dp[x] = 0.0         
        for j=1,L do 
            local diff = frame[j] - fram[j+tau]
            dp[x] = dp[x] + (diff*diff)
        end 
        cs = cs + delta[tau]
        if(cs > 0) then 
            dp[x] = dp[x] * tau/cs 
        end 
        x = x + 1 
    end 
    yin.delta[1] = 1 
end 

function yin_GetPeriodCandidate(yin,delta)
    local min_period = 30 
    local period = 0 
    local thresh = 0.1 
    local pcf = false 
    local min_val = 100000 
    local min_ind = 0 

    for i=min_period,#delta-1 do 
        if(delta[i] < min_val) then 
            min_val = delta[i] 
            min_ind = i 
        end 
        if(delta[i] < thresh) then 
            if((delta[i] < delta[i-1]) and (delta[i] < delta[i+1])) then 
                period = i 
                pcv = true 
                goto ygpc_break 
            end 
        end 
    end 
    ::ygpc_break::
    if(pcf == false) then 
        period = min_ind 
    end 
    return period
end


function yin_ParabolicInterp(period, y1, y2, y3)
    if((y3 == y2) and (y2 == y1)) then 
        return period 
    else 
        return period + (y3-y1) / (2.0 * (2.0 * y2-y3-y1))
    end 
end 

function yin_new(sample_rate)
    local yin = {} 
    yin.sample_rate = sample_rate 
    yin.prev_period_estimate = 1.0     
    yin.delta = {} 

    yin.SetMaxFrequency = yin_SetMaxFrequency 
    yin.SetSampleRate = yin_SetSampleRate
    yin.Pitch = yin_Pitch 
    yin.GetPeriodCandidate = yin_GetPeriodCandidate 
    yin.MeanNormalise = yin_MeanNormalize 
    yin.ParabolicInterp = yin_ParabolicInterp 

    yin:SetSampleRate(sample_rate)
    yin:SetMaxFrequency(1500)
    return yin 
end 


function spectralQ(frame_size,sample_rate,window_type)
    local Q = {} 
    Q.window_type = window_type 
    Q.frame_size = frame_size 
    Q.sample_rate = sample_rate 
    Q.fft_configured = false 
    Q.audio_frame = {}
    Q.window_function = {} 
    Q.fft_real = {} 
    Q.fft_imag = {}     
end 