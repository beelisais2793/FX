% VX_tstretch_bank.m   [DAFXbook, 2nd ed., chapter 7]
%===== This program performs time stretching 
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
n1             = 256;  % analysis step increment [samples]
n2             = 512;  % synthesis step increment [samples]
s_win          = 2048; % analysis window length [samples]
[DAFx_in, FS]  = wavread('la.wav');
%----- initialize windows, arrays, etc -----
tstretch_ratio = n2/n1
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;         % synthesis window
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
  zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in)); % 0-pad & normalize
DAFx_out = zeros(s_win+ceil(length(DAFx_in)*tstretch_ratio),1);
grain    = zeros(s_win,1);
ll       = s_win/2;
omega    = 2*pi*n1*[0:ll-1]'/s_win;
phi0     = zeros(ll,1);
r0       = zeros(ll,1);
psi      = zeros(ll,1);
res      = zeros(n2,1);
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in)-s_win;
while pin<pend
  grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
  fc  = fft(fftshift(grain));
  f   = fc(1:ll);
  r   = abs(f);
  phi = angle(f);
  %----- calculate phase increment per block -----
  delta_phi = omega + princarg(phi-phi0-omega);
  %----- calculate phase & mag increments per sample -----
  delta_r   = (r-r0) / n2;    % for synthesis
  delta_psi = delta_phi / n1; % derived from analysis
  %----- computing output samples for current block ----- 
  for k=1:n2
    r0  = r0 + delta_r;
    psi = psi + delta_psi;
    res(k) = r0'*cos(psi);
  end
  %----- values for processing next block -----
  phi0 = phi;
  r0   = r;
  psi  = princarg(psi);
% ===========================================
%  DAFx_out(pout+1:pout+n2) = DAFx_out(pout+1:pout+n2)+res;
  DAFx_out(pout+1:pout+n2) = res;
  pin  = pin + n1;
  pout = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
%----- listening and saving the output -----
% DAFx_in = DAFx_in(s_win+1:s_win+L);
DAFx_out=DAFx_out(s_win/2+n1+1:length(DAFx_out))/max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_tstretch_bank.wav');
