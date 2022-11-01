% VX_tstretch_real_pv.m   [DAFXbook, 2nd ed., chapter 7]
%===== This program performs time stretching 
%===== using the FFT-IFFT approach, for real ratios
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
n1           = 200;    % analysis step [samples]
n2           = 512;    % synthesis step ([samples]
s_win        = 2048;   % analysis window length [samples]
[DAFx_in,FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
tstretch_ratio = n2/n1
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;    % synthesis window
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in));
DAFx_out = zeros(s_win+ceil(length(DAFx_in)*tstretch_ratio),1);
omega    = 2*pi*n1*[0:s_win-1]'/s_win;
phi0     = zeros(s_win,1);
psi      = zeros(s_win,1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in)-s_win;
while pin<pend
  grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
  f     = fft(fftshift(grain));
  r     = abs(f);
  phi   = angle(f);
  %---- computing input phase increment ----
  delta_phi = omega + princarg(phi-phi0-omega);
  %---- computing output phase increment ----
  psi   = princarg(psi+delta_phi*tstretch_ratio);
  %---- comouting synthesis Fourier transform & grain ----
  ft    = (r.* exp(i*psi));
  grain = fftshift(real(ifft(ft))).*w2;
  % plot(grain);drawnow;
% ===========================================
  DAFx_out(pout+1:pout+s_win) = ...
     DAFx_out(pout+1:pout+s_win) + grain;
  %----- for next block -----
  phi0 = phi;
  pin  = pin + n1;
  pout = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
%DAFx_in  = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:length(DAFx_out))/max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_tstretch_noint_pv.wav');