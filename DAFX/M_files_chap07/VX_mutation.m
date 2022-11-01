% VX_mutation.m  [DAFXbook, 2nd ed., chapter 7]
%===== this program performs a mutation between two sounds,
%===== taking the phase of the first one and the modulus 
%===== of the second one, and using:
%===== w1 and w2 windows (analysis and synthesis)
%===== WLen is the length of the windows
%===== n1 and n2: steps (in samples) for the analysis and synthesis
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
n1            = 512;
n2            = n1;
WLen          = 2048;
w1            = hanningz(WLen);
w2            = w1;
[DAFx_in1,FS] = wavread('x1.wav');
DAFx_in2      = wavread('x2.wav');

%----- initializations -----
L             = min(length(DAFx_in1),length(DAFx_in2));
DAFx_in1      = [zeros(WLen, 1); DAFx_in1; ...
   zeros(WLen-mod(L,n1),1)] / max(abs(DAFx_in1));
DAFx_in2      = [zeros(WLen, 1); DAFx_in2; ...
   zeros(WLen-mod(L,n1),1)] / max(abs(DAFx_in2));
DAFx_out      = zeros(length(DAFx_in1),1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in1) - WLen;

while pin<pend
  grain1 = DAFx_in1(pin+1:pin+WLen).* w1;
  grain2 = DAFx_in2(pin+1:pin+WLen).* w1;
%===========================================
  f1     = fft(fftshift(grain1));
  r1     = abs(f1);
  theta1 = angle(f1);
  f2     = fft(fftshift(grain2));
  r2     = abs(f2);
  theta2 = angle(f2);
  %----- the next two lines can be changed according to the effect
  r      = r1;
  theta  = theta2;
  ft     = (r.* exp(i*theta));
  grain  = fftshift(real(ifft(ft))).*w2;
% ===========================================
  DAFx_out(pout+1:pout+WLen) = ...
    DAFx_out(pout+1:pout+WLen) + grain;
  pin    = pin + n1;
  pout   = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
%DAFx_in = DAFx_in(WLen+1:WLen+L);
DAFx_out = DAFx_out(WLen+1:WLen+L) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'r1p2.wav');