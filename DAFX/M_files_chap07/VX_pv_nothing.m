% VX_pv_nothing.m   [DAFXbook, 2nd ed., chapter 7]
%===== this program implements a simple phase vocoder
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
fig_plot      = 0;     % use any value except 0 or [] to plot figures
n1            = 512;   % analysis step [samples]
n2            = n1;    % synthesis step [samples]
s_win         = 2048;  % window size [samples]
[DAFx_in, FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
w1            = hanning(s_win, 'periodic'); % input window 
w2            = w1;    % output window
L             = length(DAFx_in);
DAFx_in       = [zeros(s_win, 1); DAFx_in; ...
  zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in));  % 0-pad & normalize
DAFx_out      = zeros(length(DAFx_in),1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in) - s_win;

while pin<pend
  grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
  f     = fft(fftshift(grain));  % FFT
  r     = abs(f);                % magnitude
  phi   = angle(f);              % phase
  ft    = (r.* exp(i*phi));      % reconstructed FFT
  grain = fftshift(real(ifft(ft))).*w2;
% ===========================================
  DAFx_out(pout+1:pout+s_win) = ...
     DAFx_out(pout+1:pout+s_win) + grain;
  pin  = pin + n1;
  pout = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc
%----- listening and saving the output -----
% DAFx_in  = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:s_win+L) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_pv_nothing.wav');