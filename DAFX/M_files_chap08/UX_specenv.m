% UX_specenv.m   [DAFXbook, 2nd ed., chapter 8]
% ==== This function plots the spectral envelope as energy 
%      levels in frequency channels
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

y = wavread('la');
offset = 1000;

s_win = 2048; 
w     = hanning(s_win, 'periodic'); 
buf   = y(offset:offset+s_win-1).*w;
f     = fft(buf)/(s_win/2); 
freq  = (0:1:s_win-1)/s_win*44100;
flog  = 20*log10(0.00001+abs(f));
%---- frequency window ----
nob   = input('number of bins must be even = ');
w     = hanning(nob, 'periodic'); 
w1    = hanning(nob, 'periodic'); 
w1    = w1./sum(w1);
f_channel = [zeros((s_win-nob)/2,1); w1; zeros((s_win-nob)/2,1)];
%---- FFT of frequency window ----
fft_channel = fft(fftshift(f_channel));
f2    = f.*conj(f); % Squared FFT values
%---- circular convolution by FFT-Multiplication-IFFT ----
energy   = real(ifft(fft(f2).*fft_channel)); 
flog_rms = 10*log10(abs(energy)); % 10 => combination with sqrt operation
%---- plot result ----
plot(freq,flog,freq,flog_rms);
ylabel('X(f)/dB');
xlabel('f/Hz \rightarrow'); axis([0 8000 -110 0]);
title('Short-time spectrum and spectral envelope');
