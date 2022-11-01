% VX_bank_nothing.m   [DAFXbook, 2nd ed., chapter 7]
%===== This program performs an FFT analysis and oscillator bank synthesis
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
n1            = 200;   % analysis step [samples]
n2            = n1;    % synthesis step [samples]
s_win         = 2048;  % window size [samples]
[DAFx_in, FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
w1    = hanning(s_win, 'periodic'); % input window 
w2    = w1;    % output window
L     = length(DAFx_in);
DAFx_in = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in)); % 0-pad & normalize
DAFx_out = zeros(length(DAFx_in),1);
ll    = s_win/2;
omega = 2*pi*n1*[0:ll-1]'/s_win;
phi0  = zeros(ll,1);
r0    = zeros(ll,1);
psi   = zeros(ll,1);
grain = zeros(s_win,1);
res   = zeros(n2,1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in) - s_win;

while pin<pend
  grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
  fc  = fft(fftshift(grain)); % FFT
  f   = fc(1:ll);             % positive frequency spectrum
  r   = abs(f);               % magnitudes
  phi = angle(f);             % phases
  %----- unwrapped phase difference on each bin for a n2 step
  delta_phi = omega + princarg(phi-phi0-omega);
  %----- phase and magnitude increment, for linear
  %      interpolation and reconstruction -----
  delta_r   = (r-r0)/n1;    % magnitude increment
  delta_psi = delta_phi/n1; % phase increment
  for k=1:n2 % compute the sum of weighted cosine
    r0     = r0 + delta_r;
    psi    = psi + delta_psi;
    res(k) = r0'*cos(psi);
  end
  %----- for next time -----
  phi0 = phi;
  r0   = r;
  psi  = princarg(psi);
% ===========================================
  DAFx_out(pout+1:pout+n2) = DAFx_out(pout+1:pout+n2) + res;
  pin  = pin + n1;
  pout = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win/2+n1+1:s_win/2+n1+L) / max(abs(DAFx_out));
soundsc(DAFx_out,FS);
wavwrite(DAFx_out, FS, 'la_bank_nothing.wav');