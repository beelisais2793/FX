% VX_stable.m   [DAFXbook, 2nd ed., chapter 7]
%===== this program extracts the stable components of a signal
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
test         = 0.4
n1           = 256;    % analysis step [samples]
n2           = n1;     % synthesis step [samples]
s_win        = 2048;   % analysis window length [samples]
[DAFx_in,FS] = wavread('redwheel.wav');

%----- initialize windows, arrays, etc -----
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;    % synthesis window
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
  zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in));
DAFx_out = zeros(length(DAFx_in),1);
devcent  = 2*pi*n1/s_win;
vtest    = test * devcent
grain    = zeros(s_win,1);
theta1   = zeros(s_win,1);
theta2   = zeros(s_win,1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in)-s_win;

while pin<pend
  grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
  f      = fft(fftshift(grain));
  theta  = angle(f);
  dev    = princarg(theta - 2*theta1 + theta2);
%  plot(dev);drawnow;
  %---- set to 0 magnitude values below 'test' threshold 
  ft     = f.*(abs(dev) < vtest);
  grain  = fftshift(real(ifft(ft))).*w2;
  theta2 = theta1;
  theta1 = theta;
% ===========================================
  DAFx_out(pout+1:pout+s_win) = ...
    DAFx_out(pout+1:pout+s_win) + grain;
  pin  = pin + n1;
  pout = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:s_win+L) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'redwheel_stable.wav');