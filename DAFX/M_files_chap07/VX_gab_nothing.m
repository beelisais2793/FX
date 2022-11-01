% VX_gab_nothing.m   [DAFXbook, 2nd ed., chapter 7]
%==== This program performs signal convolution with gaborets
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf

%----- user data -----
n1            = 128;  % analysis step [samples]
n2            = n1;   % synthesis step [samples]
s_win         = 512;  % window size [samples]
[DAFx_in, FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
window   = hanning(s_win, 'periodic'); % input window 
nChannel = s_win/2;
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in));
DAFx_out = zeros(length(DAFx_in),1); % 0-pad & normalize

%----- initialize calculation of gaborets -----
t    = (-s_win/2:s_win/2-1);
gab  = zeros(nChannel,s_win);
for k=1:nChannel
  wk       = 2*pi*i*(k/s_win);
  gab(k,:) = window'.*exp(wk*t);
end

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in) - s_win;

while pin<pend
  grain = DAFx_in(pin+1:pin+s_win);
%===========================================
  %----- complex vector corresponding to a vertical line    
  vec = gab*grain;
  %----- reconstruction from the vector to a grain
  res = real(gab'*vec);
% ===========================================
  DAFx_out(pout+1:pout+s_win) = DAFx_out(pout+1:pout+s_win) + res;
  pin  = pin + n1;
  pout = pout + n2;
  end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in  = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:s_win+L) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_gab_nothing.wav');