% N1: cut quefrency
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

y = wavread('la');
offset = 1000;

WLen = 2048; 
w    = hanning(WLen, 'periodic'); 
buf  = y(offset:offset+WLen-1).*w;
f    = fft(buf)/(WLen/2); 
flog = 20*log10(0.00001+abs(f));
subplot(2,1,1); plot(flog(1:WLen/2));

N1   = input('cut value for cepstrum ');
cep  = ifft(flog);
cep_cut  = [cep(1); 2*cep(2:N1); cep(N1+1); zeros(WLen-N1-1,1)];
flog_cut = real(fft(cep_cut));
subplot(2,1,2); plot(flog_cut(1:WLen/2));
