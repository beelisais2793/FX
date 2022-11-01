
require('kissfft')
require('kissfftr')

RectangularWindow = 0
HanningWindow = 1
HammingWindow = 2 
BlackmanWindow = 3 
TukeyWindow=4 

function array_new(size)
    local a = {} 
    a.array = kissfft.float_vector(size)
    a.size = function(a) return a.array:size() end 
    a.zero = function(a)
        for i = 1,a:size() do 
            a.array[i-1] = 0 
        end 
    end     
    a.copy = function(a,b) 
        a.array = kissfft.float_vector(b:size())        
        for i=1,b:size() do 
            a.array[i-1] = b.array[i-1]
        end     
    end 
    a.copy_vector = function(a,v)
        for i=1,a:size() do 
            a.array[i-1] = v[i-1]
        end 
    end 
    local meta = {} 
    meta.__index = function(a,i) return a.array[i-1] end 
    meta.__newindex = function(a,i,v) a.array[i-1] = v end     
    setmetatable(a,meta)
    return a 
end 

function matrix_new(i,j)
    local m = {} 
    m.matrix = {} 
    for i=1,i do 
        m.matrix[i] = array_new(j+1)
    end 
    local meta = {} 
    meta.__index = function(a,i) return m.matrix[i] end 
    meta.__newindex = function(a,i,v) m.matrix[i] = v end 
    setmetatable(m,meta)
    return m 
end 
    
function vectorize_table(buffer)
    local v = kissfft.float_vector(buffer:size())
    for i=1,buffer:size() do 
        v[i-1] = buffer[i] 
    end 
    return v 
end 

function unvectorize_table(vector)
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

function sqsum(v)
    local s = 0.0 
    for i=1,#v do 
        s = s + v[i]*v[i]
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
    for i=1,ms:size() do 
        sumAmplitudes           = sumAmplitudes + ms[i] 
        sumWeightedAmplitudes   = sumWeightedAmplitudes + ms[i]*(i-1)
    end        
    if (sumAmplitudes > 0) then return sumWeightedAmplitudes / sumAmplitudes end
    return 0.0    
end 

function spectralFlatness(ms)
    local sumVal = 0.0;
    local logSumVal = 0.0;
    local N = ms:size()
    local flatness;
    for i=1,ms:size() do 
        local v = 1 + ms[i]
        sumVal = sumVal + v 
        logSumVal = logSumVal + math.log(v)
    end
    sumVal = sumVal / N;
    logSumVal = logSumVal / N;
    if (sumVal > 0) then 
        flatness = (math.exp (logSumVal) / sumVal);
    else
        flatness = 0.0;
    end 
    return flatness;
end

function spectralCrest(ms)
    local sumVal = 0.0;
    local maxVal = 0.0;
    local  N = ms:size()
    for i=1,ms:size() do 
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
    local index = 1;    
    for i=1,ms:size() do 
        cumulativeSum = cumulativeSum + ms[i] 
        if(cumulativeSum > threshold) then 
            index = i 
            goto sr_rolloff_break
        end 
    end 
    ::sr_rolloff_break::
    local spectralRolloff = index / ms:size()    
    return spectralRolloff;
end

function spectralKurtosis(ms)    
    local sumOfMagnitudeSpectrum = sum(ms)    
    local mean = sumOfMagnitudeSpectrum / ms:size()    
    local moment2 = 0;
    local moment4 = 0;    
    for i=1,ms:size() do 
        local difference = ms[i] - mean 
        local squaredDifference = difference*difference
        moment2 = moment2 + squaredDifference 
        moment4 = moment4 + squaredDifference*squaredDifference 
    end 
    moment2 = moment2 / ms:size() 
    moment4 = moment4 / ms:size()        
    if (moment2 == 0) then     
        return -3.0;    
    else    
        return (moment4 / (moment2*moment2)) - 3.0;
    end
end 

function createHanningWindow(num_samples)
    local window = array_new(num_samples)
    for i=1,num_samples do 
        window[i] = 0.5 * (1 - math.cos(2*math.pi * (i/num_samples)))
    end 
    return window 
end 

function createHammingWindow(num_samples)
    local window = array_new(num_samples)
    for i=1,num_samples do 
        window[i] = 0.64 - (0.46 * math.cos(2*math.pi * (i/num_samples)))
    end 
    return window 
end 

function createBlackmanWindow(num_samples)
    local window = array_new(num_samples)
    for i=1,num_samples do 
        window[i] = 0.42 - (0.5 * math.cos(2*math.pi * (i/num_samples))) 
                    + (0.08 * math.cos(4 * math.pi * (i/num_samples)))
    end 
    return window 
end 

function createTukeyWindow(num_samples,alpha)
    local window = array_new(num_samples)
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
    local window = array_new(num_samples)
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
    for i=1,buffer:size() do 
        sum = sum + buffer[i]^2
    end 
    return math.sqrt(sum / buffer:size())
end 

function peakEnergy(buffer)
    local peak = -10000 
    for i=1,buffer:size() do 
        local absSample = math.abs(buffer[i])
        if(absSample > peak) then 
            peak = absSample 
        end 
    end 
    return peak 
end 

function zeroCrossingRate(buffer)
    local zcr = 0 
    for i=2,buffer:size() do 
        local current = buffer[i] > 0 
        local previous = (buffer[i-1] > 0)
        if( current ~= previous) then 
            zcr = zcr + 1 
        end 
    end 
    return zcr
end 

function mfcc_Initialize(mfcc)
    mfcc.magnitude_spectrum = mfcc.frame_size/2
    mfcc.min_frequency = 0 
    mfcc.max_frequency = mfcc.sample_rate/2     
    mfcc.mel_spectrum = array_new(mfcc.num_coefficients)
    mfcc.MFCCs = array_new(mfcc.num_coefficients)
    mfcc.dct_signal = array_new(mfcc.num_coefficients)        
    mfcc.mel_spectrum:zero()
    mfcc.MFCCs:zero() 
    mfcc.dct_signal:zero()    
    mfcc:CalcMelFilterBank() 
end 

function mfcc_SetNumCoefficients(mfcc, num_coeffs)
    mfcc.num_coefficients = num_coeffs
    mfcc:initialize() 
end 

function mfcc_SetFrameSize(mfcc, fs)
    mfcc.frame_size = fs 
    mfcc:initialize() 
end 

function mfcc_SetSampleRate(mfcc, sr)
    mfcc.sample_rate = sr 
    mfcc:initialize() 
end 

function mfcc_CalcMelCoefficients(mfcc, ms)
    for i=1,mfcc.num_coefficients do 
        local coeff = 0 
        for j=1,ms:size() do 
            coeff = coeff + (ms[j]*ms[j]) * mfcc.filter_bank[i][j]
        end 
        mfcc.mel_spectrum[i] = coeff 
    end 
end 

function mfcc_CalcMelSpectrum(mfcc, ms)
   mfcc:CalcMelSpectrum(ms)
   for i=1,ms:size() do 
        mfcc.MFCCs[i] = math.log(mfcc.mel_spectrum[i] + FLT_MIN)        
   end
   mfcc:DCT()
end 

function mfcc_discrete_cosine_transform(mfcc)
    for i=1,mfcc.MFCCs:size() do 
        mfcc.dct_signal[i] = mfcc.MFCCs[i] 
    end
    local N = mfcc.MFCCs:size() 
    local pi_over_n = math.pi / N 
    for k=1,N do 
        local sum = 0 
        local kVal = k 
        for n=1,N do 
            local tmp = pi_over_n * (n+0.5)*kVal 
            sum = sum + dct_signal[n]*math.cos(tmp)
        end 
        mfcc.MFCCs[k] = 2*sum 
    end 
end 

function frequency_to_mel(f)
    return 1127 * math.log(1 + (f/700.0)) 
end 

function mfcc_CalcMelFilterBank(mfcc)
    local max_mel = math.floor(frequency_to_mel(mfcc.max_frequency))
    local min_mel = math.floor(frequency_to_mel(mfcc.min_frequency))
    mfcc.filter_bank = matrix_new(mfcc.num_coefficients,mfcc.magnitude_spectrum)
    for i=1,mfcc.num_coefficients do 
        mfcc.filter_bank[i] = {} 
        for j=1,mfcc.magnitude_spectrum do 
            mfcc.filter_bank[i][j] = 0 
        end 
    end 
    local center_indices = {} 
    for i=1,mfcc.num_coefficients+2 do 
        local f = i * (max_mel - min_mel) / (mfcc.num_coefficients+1) + min_mel 
        local tmp = math.log(1 + 1000.0/700.0) / 1000.0 
        tmp = (math.exp(f*tmp) -1) / (mfcc.sample_rate /2)
        tmp = 0.5 + 700*mfcc.magnitude_spectrum * tmp 
        tmp = math.floor(tmp)
        table.insert(center_indices, tmp)
    end 
    for i=1,mfcc.num_coefficients do 
        local filter_begin_index = center_indices[i]
        local filter_center_index = center_indices[i+1]
        local filter_end_index = center_indices[i+2]
        local triangle_up = filter_center_index - filter_begin_index 
        local triangle_down = filter_end_index - filter_center_index
        for k=filter_begin_index,filter_center_index do 
            mfcc.filter_bank[i][k] = (k- filter_begin_index)/triangle_up 
        end
        for k=filter_center_index,filter_end_index do 
            mfcc.filter_bank[i][k] = (filter_end_index-k) / triangle_down 
        end 
    end 
end 

function mfcc_new(frame_size, sample_rate)
    local mfcc = {} 
    mfcc.frame_size = frame_size 
    mfcc.sample_rate= sample_rate 
    mfcc.num_coefficients = 18     
    mfcc.mel_spectrum = nil 
    mfcc.MFCCs = nil
    mfcc.dct_signal = nil
    mfcc.initialize = mfcc_Initialize
    mfcc.CalcMelCoefficients = mfcc_CalcMelCoefficient 
    mfcc.CalcMelSpectrum = mfcc_CalcMelSpectrum 
    mfcc.CalcMelFilterBank = mfcc_CalcMelFilterBank
    mfcc.SetNumCoefficients = mfcc_SetNumCoefficients 
    mfcc.SetFrameSize = mfcc_SetFrameSize  
    mfcc.SetSampleRate = mfcc_SetSampleRate 
    mfcc.DCT = mfcc_discrete_cosine_transform
    mfcc:initialize()
    return mfcc 
end 

function onset_SetFrameSize(onset, frame_size)
    onset.prev_magnitude_difference = array_new(frame_size) 
    onset.prev_magnitude_difference_hwr      = array_new(frame_size) 
    onset.prev_magnitude_complex_difference  = array_new(frame_size)  
    onset.prev_phase_difference   = array_new(frame_size)          
    onset.prev_phase2_difference  = array_new(frame_size)  

    onset.prev_magnitude_difference:zero()
    onset.prev_magnitude_difference_hwr:zero()
    onset.prev_magnitude_complex_difference:zero()
    onset.prev_phase_difference:zero()
    onset.prev_phase2_difference:zero()
    onset.prev_energy_sum = 0 
end 

function onset_EnergyDifference(onset,buffer)
    local sum = sqsum(buffer)
    local difference = sum - onset.prev_energy_sum
    onset.prev_energy_sum = sum 
    if(difference > 0) then return difference
    else return 0.0 end 
end 

function onset_SpectralDifference(onset, ms)
    local sum = 0 
    for i=1,ms:size() do 
        local diff = ms[i] - onset.prev_magnitude_difference[i]
        if(diff < 0) then diff = diff * -1  end 
        sum = sum + diff 
        onset.prev_magnitude_difference[i] = ms[i] 
    end 
    return sum 
end 

function onset_SpectralDifferenceHWR(onset, ms)
    local sum = 0 
    for i=1,ms:size() do 
        local diff = ms[i] - onset.prev_magnitude_difference_hwr[i]
        if(diff < 0) then sum = sum + diff  end 
        sum = sum + diff 
        onset.prev_magnitude_difference_hwr[i] = ms[i] 
    end 
    return sum 
end 

function onset_ComplexSpectralDifference(onset,fftReal,fftImag)
    local dev,pdev 
    local sum 
    local mag_diff,phase_diff 
    local value 
    local phase_val 
    local mag_val 

    sum = 0 
    for i=1,fftReal:size() do 
        phase_val = math.atan2(fftImag[i],fftReal[i])
        mag_val = math.sqrt((fftReal[i]*fftReal[i]+(fftImag[i]*fftImag[i])))
        dev = phase_val - (2*onset.prev_phase_difference[i]) + onset.prev_phase2_difference[i]
        pdev = onset:princarg(dev)
        mag_diff = mag_val - onset.prev_magnitude_complex_difference[i]
        phase_diff = -mag_val * math.sin(pdev)
        value = math.sqrt((mag_diff*mag_diff)+(phase_diff*phase_diff))
        sum = sum + value 
        onset.prev_phase2_difference[i] = onset.prev_phase_difference[i]
        onset.prev_phase_difference[i] = phase_val 
        onset.prev_magnitude_complex_difference[i] = mag_val 
    end 
    return sum 
end 

function onset_HighFrequencyContent(onset, ms)
    local sum = 0 
    for i=1,ms:size() do 
        sum = sum + (ms[i] * i) 
    end 
    return sum 
end 

function onset_princarg(onset,phaseval)
    while(phaseval <= -math.pi) do 
        phaseval = phaseval + (2*math.pi) 
    end 
    while(phaseval > math.pi) do 
        phaseval = phaseval - (2*math.pi)
    end 
    return phaseval 
end 

function onsetdetection_new(frame_size)
    local onset = {} 
    onset.frame_size = frame_size 
    onset.prev_magnitude_difference= nil
    onset.prev_magnitude_difference_hwr = nil
    onset.prev_magnitude_complex_difference = nil
    onset.prev_phase_difference = nil
    onset.prev_phase2_difference = nil
    onset.prev_energy_sum = 0 
    onset.SetFrameSize = onset_SetFrameSize 
    onset:SetFrameSize(frame_size)
    onset.EnergyDifference = onset_EnergyDifference
    onset.SpectralDifference = onset_SpectralDifference 
    onset.SpectralDifferenceHWR = onset_SpectralDifferenceHWR 
    onset.ComplexSpectralDifference = onset_ComplexSpectralDifference
    onset.HighFrequencyContent = onset_HighFrequencyContent
    onset.princarg = onset_princarg
    return onset 
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
    yin.delta = array_new(frame:size()/2)     
    yin:MeanNormalise(frame)    
    local cp = yin:SearchRecentMinima(yin.delta)
    if(cp == -1) then 
        period = yin:GetPeriodCandidate(yin.delta)
    else 
        period = cp 
    end
    if(period > 1 and period < yin.delta:size()) then 
        fPeriod = yin:ParabolicInterp(period,yin.delta[period-1], yin.delta[period], yin.delta[period+1])
    else 
        fPeriod = period 
    end 
    yin.prev_period_estimate = fPeriod 
    return yin:PeriodToPitch(fPeriod)
end 

function yin_MeanNormalize(yin,frame)
    local cs = 0 
    local L = frame:size()/2 
    local dp = yin.delta 
    local x = 1      
    for tau=1,L do 
        dp[x] = 0.0         
        for j=1,L do 
            local diff = frame[j] - frame[j+tau]
            dp[x] = dp[x] + (diff*diff)
        end 
        cs = cs + yin.delta[tau]
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
    for i=min_period,delta:size() do 
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


function yin_ParabolicInterp(yin,period, y1, y2, y3)
    if((y3 == y2) and (y2 == y1)) then 
        return period 
    else 
        return period + (y3-y1) / (2.0 * (2.0 * y2-y3-y1))
    end 
end 

function yin_SearchRecentMinima(yin, delta)
    local new_minima = -1 
    local prev_est = math.ceil(yin.prev_period_estimate+0.5)
    for i=prev_est,prev_est+1 do 
        if((i>1) and (i < delta:size())) then 
            if((delta[i] < delta[i-1]) and (delta[i] < delta[i+1])) then 
                new_minima = i
            end 
        end 
    end 
    return new_minima
end

function yin_PeriodToPitch(yin, period)
    return yin.sample_rate / period 
end 

function yin_new(sample_rate)
    local yin = {} 
    yin.sample_rate = sample_rate 
    yin.prev_period_estimate = 1.0         
    yin.SetMaxFrequency = yin_SetMaxFrequency 
    yin.SetSampleRate = yin_SetSampleRate
    yin.Pitch = yin_Pitch 
    yin.GetPeriodCandidate = yin_GetPeriodCandidate 
    yin.MeanNormalise = yin_MeanNormalize 
    yin.ParabolicInterp = yin_ParabolicInterp 
    yin.SearchRecentMinima = yin_SearchRecentMinima 
    yin.PeriodToPitch = yin_PeriodToPitch     
    yin:SetMaxFrequency(1500)
    return yin 
end 

function zeros(n)    
    local b = array_new(n)
    b:zero() 
    return b
end 

function sq_AudioFrameSize(sq,fs)
    sq.frame_size = fs     
    sq.audio_frame = zeros(fs)    
    sq.window_function = createWindow(fs,sq.window_type)
    sq.fft_real = zeros(fs)
    sq.fft_imag = zeros(fs)
    sq.ms = zeros(fs/2)
    sq:ConfigureFFT() 
    sq.onset:SetFrameSize(fs)
    sq.mfcc:SetFrameSize(fs)
end 

function sq_SetSampleRate(sq,sr)
    sq.sample_rate = sr 
    sq.yin:SetSampleRate(sr)
    sq.mfcc:SetSampleRate(sr)
end 


function sq_ProcessAudioFrame(sq,a)
    sq.audio_frame:copy(a)        
    sq:FFT() 
end 

function sq_ConfigureFFT(q)
    -- these need to be in std::vector
    q.fft_in  = kissfft.complex_vector(q.frame_size)
    q.fft_out = kissfft.complex_vector(q.frame_size)    
    q.fft = kissfft.KissFFT(q.frame_size)
    q.fft_configured = true     
end 

function sq_FFT(q)        
    for i=0,q.frame_size-1 do                 
        local c = kissfft.kiss_fft_cpx()
        c.r =  (q.audio_frame[i+1] * q.window_function[i+1])                 
        c.i = 0        
        q.fft_in[i] = c         
        --[[ swig bug 
            q.fft_in[i].r = (q.audio_frame[i+1] * q.window_function[i+1])                 
            q.fft_in[i].i = 0             
        ]]
    end     
    q.fft:forward(q.fft_in, q.fft_out)         
    q.fft_real = array_new(q.frame_size)
    q.fft_imag = array_new(q.frame_size)
    for i=0,q.frame_size-1 do 
        q.fft_real[i+1] = q.fft_out[i].r 
        q.fft_imag[i+1] = q.fft_out[i].i          
    end         
    q.magnitude_spectrum = array_new(q.frame_size/2)
    for i=1,q.frame_size/2 do 
        q.magnitude_spectrum[i] = math.sqrt((q.fft_real[i]*q.fft_real[i]) + (q.fft_imag[i]*q.fft_imag[i]))        
    end     
end 

function sq_IFFT(q)
    for i=0,q.frame_size-1 do 
        q.fft_in[i].r = q.magnitude_spectrum[i]
        q.fft_in[i].i = 0.0 
    end 
    q.fft:inverse(q.fft_in, q.fft_out) 
    for i=0,q.frame_size-1 do 
        q.fft_real[i+1] = q.fft_out[i].r 
        q.fft_imag[i+1] = q.fft_out[i].i 
    end 
    q.magnitude_spectrum={}       
    for i=1,q.frame_size/2 do         
        q.magnitude_spectrum[i] = math.sqrt((q.fft_real[i]*q.fft_real[i]) + (q.fft_imag[i]*q.fft_imag[i]))        
    end 
end 

function sq_new(frame_size,sample_rate,window_type)
    local Q = {} 
    Q.window_type = window_type 
    Q.frame_size = frame_size     
    Q.sample_rate = sample_rate 
    Q.fft_configured = false 
    Q.audio_frame = nil
    Q.magnitude_spectrum = nil
    Q.window_function = nil
    Q.fft_real = nil 
    Q.fft_imag = nil
    Q.yin = yin_new(sample_rate)
    Q.onset = onsetdetection_new(frame_size)
    Q.mfcc = mfcc_new(frame_size,sample_rate)        
    Q.ConfigureFFT = sq_ConfigureFFT
    Q.FFT = sq_FFT 
    Q.IFFT = sq_IFFT
    Q.RMS = function(q) return rootMeanSquare(q.audio_frame) end 
    Q.PeakEnergy = function(q) return peakEnergy(q.audio_frame) end 
    Q.ZeroCrossingRate = function(q) return zeroCrossingRate(q.audio_frame) end 
    Q.Centroid = function(q) return spectralCentroid(q.magnitude_spectrum) end 
    Q.Crest = function(q) return spectralCrest(q.magnitude_spectrum) end 
    Q.Flatness = function(q) return spectralFlatness(q.magnitude_spectrum) end 
    Q.Rolloff = function(q) return spectralRollOff(q.magnitude_spectrum) end
    Q.Kurtosis = function(q) return spectralKurtosis(q.magnitude_spectrum) end 
    Q.EnergyDifference = function(q) return q.onset:EnergyDifference(q.audio_frame) end 
    Q.SpectralDifference = function(q) return q.onset:SpectralDifference(q.magnitude_spectrum) end 
    Q.SpectralDifferenceHWR  = function(q) return q.onset:SpectralDifferenceHWR(q.magnitude_spectrum) end 
    Q.ComplexSpectralDifference = function(q) return q.onset:ComplexSpectralDifference(q.fft_imag,q.fft_real) end 
    Q.HighFrequencyContent = function(q) return q.onset:HighFrequencyContent(q.magnitude_spectrum) end 
    Q.Pitch = function(q) return q.yin:Pitch(q.audio_frame) end 
    Q.GetMelFrequencySpectrum = function(q)
        q.mfcc:CalcMelSpectrum(q.magnitude_spectrum)
        return q.mfcc.mel_spectrum 
    end 
    Q.GetMelCoefficients = function(q)
        q.mfcc:CalcMelCoefficients(q.magnitude_spectrum) 
        return q.mfcc.MFCCs 
    end 
    Q.SetAudioFrameSize = sq_AudioFrameSize
    Q.ProcessAudioFrame = sq_ProcessAudioFrame    
    Q.SetSampleRate = sq_SetSampleRate     
    Q:SetSampleRate(sample_rate)
    Q:SetAudioFrameSize(frame_size)    
    return Q
end 

require('luagist')
snd = require('sndfile')

-- compare with gist
local b = bit.bor(snd.SF_FORMAT_WAV,snd.SF_FORMAT_PCM_16)
wav = snd.SoundFile("BabyElephantWalk60.wav",snd.SFM_READ,b)
local len = 256
v = snd.short_vector(len)
wav:seek(0,0)
r = wav:read(v,len)
x = array_new(len)
q = luagist.GistF(len,44100)
sq = sq_new(len,44100,1)

while(r > 0) do

for i=1,len do x[i] = v[i-1]/128 end 
q:processAudioFrame(x.array)
print(q:rootMeanSquare())
print(q:peakEnergy())
print(q:zeroCrossingRate())
print(q:spectralCentroid())
print(q:spectralCrest())
print(q:spectralFlatness())
print(q:spectralRolloff())
print(q:spectralKurtosis())
print("ed=",q:energyDifference())
print("sd=",q:spectralDifference())
print("sdhw=",q:spectralDifferenceHWR())
print(q:complexSpectralDifference())
print(q:highFrequencyContent())
print(q:pitch())


sq:ProcessAudioFrame(x)
print(sq:RMS())
print(sq:PeakEnergy())
print(sq:ZeroCrossingRate())
print(sq:Centroid())
print(sq:Crest())
print(sq:Flatness())
print(sq:Rolloff())
print(sq:Kurtosis())
print("ed=",sq:EnergyDifference())
print("sd=",sq:SpectralDifference())
print("sdhw=",sq:SpectralDifferenceHWR())
print(sq:ComplexSpectralDifference())
print(sq:HighFrequencyContent())
print(sq:Pitch())
r = wav:read(v,len)
end

--[[
q = sq_new(len,44100,1)
while r > 0 do 
    for i=1,len do q.audio_frame[i] = v[i-1]/65536 end 
    q:FFT()
    print(q:RMS(),q:PeakEnergy(),q:ZeroCrossingRate(),q:pitch())
    print("centroid=",q:Centroid(),"crest=",q:Crest(),"flatness=",q:Flatness(),"rolloff=",q:Rolloff(), "kurtosis=",q:Kurtosis())
    print(q:EnergyDifference(),q:SpectralDifference())
    r = wav:read(v,len)
end
]]
