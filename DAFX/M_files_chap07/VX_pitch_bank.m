% VX_pitch_bank.m   [DAFXbook, 2nd ed., chapter 7]
%===== This program performs pitch shifting 
%===== using the oscillator bank approach
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
n1           = 512;    % analysis step [samples]
pit_ratio    = 1.2     % pitch-shifting ratio
s_win        = 2048;   % analysis window length [samples]
[DAFx_in,FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;    % synthesis window
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in));
DAFx_out = zeros(length(DAFx_in),1);
grain    = zeros(s_win,1);
hs_win   = s_win/2;
omega    = 2*pi*n1*[0:hs_win-1]'/s_win;
phi0     = zeros(hs_win,1);
r0       = zeros(hs_win,1);
psi      = phi0;
res      = zeros(n1,1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in)-s_win;

while pin<pend
  grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
  fc  = fft(fftshift(grain));
  f   = fc(1:hs_win);
  r   = abs(f);
  phi = angle(f);
  %---- compute phase & mangitude increments ----
  delta_phi = omega + princarg(phi-phi0-omega);
  delta_r   = (r-r0)/n1;
  delta_psi = pit_ratio*delta_phi/n1;
  %---- compute output buffer ----
  for k=1:n1
    r0      = r0 + delta_r;
    psi     = psi + delta_psi;
    res(k)  = r0' * cos(psi);
  end
  %---- store for next block ----
  phi0 = phi;
  r0   = r;
  psi  = princarg(psi);
%  plot(res);pause;
% ===========================================
  DAFx_out(pout+1:pout+n1) = DAFx_out(pout+1:pout+n1) + res;
  pin  = pin + n1;
  pout = pout + n1;
  end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(hs_win+n1+1:hs_win+n1+L) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_pitch_bank.wav');