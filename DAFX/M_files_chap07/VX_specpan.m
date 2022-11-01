% VX_specpan.m  [DAFXbook, 2nd ed., chapter 7]
%===== This program makes a spectral panning of a sound
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf
%----- user data -----
fig_plot     = 1;    % use any value except 0 or [] to plot figures
n1           = 512;  % analysis step [samples]
n2           = n1;   % synthesis step [samples]
s_win        = 2048; % analysis window length [samples]
[DAFx_in,FS] = wavread('redwheel.wav');

%----- initialize windows, arrays, etc -----
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;       % synthesis window
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
  zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in));
DAFx_out = zeros(length(DAFx_in),2);
hs_win   = s_win/2;
coef     = sqrt(2)/2;
%---- control: clipped sine wave with a few periods; in [-pi/4;pi/4]
theta    = min(1,max(-1,2*sin((0:hs_win)/s_win*200))).' * pi/4;
% %---- control: rough left/right split at Fs/30 ~ 1470 Hz
% theta    = (((0:hs_win).'/2 < hs_win/30)) * pi/2 - pi/4; 
%---- preserving phase symmetry ----
theta    = [theta(1:hs_win+1); flipud(theta(1:hs_win-1))];

%---- drawing panning function ----
if (fig_plot)
  figure; 
  plot((0:hs_win)/s_win*FS/1000, theta(1:hs_win+1)); 
  axis tight; xlabel('f / kHz \rightarrow');
  ylabel('\theta / rad \rightarrow');
  title('Spectral panning angle as a function of frequency')
end
tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in) - s_win;

while pin<pend
    grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
    f      = fft(grain);
    %---- compute left and right spectrum with Blumlein law at 45°
    ftL    = coef * f .* (cos(theta) + sin(theta));
    ftR    = coef * f .* (cos(theta) - sin(theta));
    grainL = (real(ifft(ftL))).*w2;
    grainR = (real(ifft(ftR))).*w2;
% ===========================================
    DAFx_out(pout+1:pout+s_win,1) = ...
        DAFx_out(pout+1:pout+s_win,1) + grainL;
    DAFx_out(pout+1:pout+s_win,2) = ...
        DAFx_out(pout+1:pout+s_win,2) + grainR;
    pin  = pin + n1;
    pout = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc
%----- listening and saving the output -----
% DAFx_in  = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:s_win+L,:) / max(max(abs(DAFx_out)));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'x1_specpan.wav');