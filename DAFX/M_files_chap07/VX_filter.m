% VX_filter.m   [DAFXbook, 2nd ed., chapter 7]
%===== This program performs time-frequency filtering
%===== after calculation of the fir (here band pass)
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------


clear; clf

%----- user data -----
fig_plot      = 0;       % use any value except 0 or [] to plot figures
s_FIR         = 1280;    % length of the fir [samples]
s_win         = 2*s_FIR; % window size [samples] for zero padding
[DAFx_in, FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
L         = length(DAFx_in);
DAFx_in   = [DAFx_in;  zeros(s_win-mod(L,s_FIR),1)] ...
   / max(abs(DAFx_in));             % 0-pad & normalize
DAFx_out  = zeros(length(DAFx_in)+s_FIR,1);
grain     = zeros(s_win,1);         % input grain
vec_pad   = zeros(s_FIR,1);     % padding array

%----- initialize calculation of fir -----
x         = (1:s_FIR);
fr        = 1000/FS;
alpha     = -0.002;
fir       = (exp(alpha*x).*sin(2*pi*fr*x))'; % FIR coefficients 
fir2      = [fir; zeros(s_win-s_FIR,1)];
fcorr     = fft(fir2);

%----- displays the filter' simpulse response -----
if(fig_plot)
  figure(1); clf; 
  subplot(2,1,1); plot(fir); xlabel('n [samples] \rightarrow'); 
  ylabel('h(n) \rightarrow'); axis tight;
  title('Impulse response of the FIR')
  subplot(2,1,2); 
  plot((0:s_FIR-1)/s_FIR*FS, 20*log10(abs(fft(fftshift(fir)))));
  xlabel('k \rightarrow'); ylabel('|F(n,k)| / dB \rightarrow');
  title('Magnitude spectrum of the FIR'); axis([0 s_FIR/2, -40, 50])
  pause
end

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = length(DAFx_in) - s_FIR;

while pin<pend
  grain = [DAFx_in(pin+1:pin+s_FIR); vec_pad];  
%===========================================
  ft    = fft(grain) .* fcorr;
  grain = (real(ifft(ft)));
%===========================================
  DAFx_out(pin+1:pin+s_win) = ...
    DAFx_out(pin+1:pin+s_win) + grain;
  pin   = pin + s_FIR;  
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in  = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_filter.wav');