%===== This function plots the spectral envelope
%      estimation by LPC
% [DAFXbook, 2nd ed., chapter 8]
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------


fname = 'la.wav';
n0    = 5000;   % start index
N     = 1024;   % block length
Nfft  = 1024;   % FFT length
p     = 50;     % prediction order
n1    = n0+N-1; % end index

[xin,Fs] = wavread(fname,[n0 n1]);
x     = xin(:,1)';   % row vecor of left channel
win   = hamming(N)'; % window for input block

a = calc_lpc(x.*win,p); % calculate LPC coeffs
% a = [1, -a_1, -a_2,..., -a_p]

Omega  = (0:Nfft-1)/Nfft*Fs/1000; % frequencies in kHz
offset = 20*log10(2/Nfft);        % spectrum offset in dB
A = 20*log10(abs(fft(a,Nfft)));
H = -A+offset;
X = 20*log10(abs(fft(x.*win,Nfft)));
X = X+offset;

n = 0:N-1;
figure(1); clf
subplot(221)
plot(n,x)
title('time signal of original x(n)')
xlabel('n \rightarrow')
axis([0 N-1 -inf inf])

subplot(222); hold on
plot(Omega,X)
plot(Omega,H,'r','Linewidth',1.5)
title('magnitude spectra |X(f)| and |H(f)| in dB')
xlabel('f/kHz \rightarrow')
axis([0 8 -inf inf])