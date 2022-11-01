% ===== This function plots the LPC for one block
%       calculation of prediction error and spectra       
% [DAFXbook, 2nd ed., chapter 8]
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------


fname = 'la.wav';
n0   = 5000; %start index
N    = 1024; %block length
Nfft = 1024; % FFT length
p    = 50; %prediction order
n1   = n0+N-1; %end index
pre  = p; %filter order= no. of samples required before n0

[xin,Fs] = wavread(fname,[n0-pre n1]);
xin = xin(:,1)';
win = hamming(N)';
x   = xin((1:N)+pre); % block without pre-samples

[a,g] = calc_lpc(x.*win,p); % calculate LPC coeffs and gain
% a = [1, -a_1, -a_2,..., -a_p]
g_db = 20*log10(g) % gain in dB

ein = filter(a,1,xin); % pred. error
e   = ein((1:N)+pre); % without pre-samples
Gp  = 10*log10(sum(x.^2)/sum(e.^2)) % prediction gain

Omega  = (0:Nfft-1)/Nfft*Fs/1000; % frequencies in kHz
offset = 20*log10(2/Nfft); % offset of spectrum in dB
A   = 20*log10(abs(fft(a,Nfft)));
H_g = -A+offset+g_db; % spectral envelope
X   = 20*log10(abs(fft(x.*win,Nfft)));
X   = X+offset;

n   =  0:N-1;
figure(1)
clf
subplot(221)
plot(n,e)
title('time signal of pred. error e(n)')
xlabel('n \rightarrow')
axis([0 N-1 -inf inf])

subplot(222)
plot(Omega,X); hold on
plot(Omega,H_g,'r','Linewidth',1.5); hold off
title('magnitude spectra |X(f)| and |G\cdot H(f)| in dB')
xlabel('f/kHz \rightarrow')
axis([0 8 -inf inf])