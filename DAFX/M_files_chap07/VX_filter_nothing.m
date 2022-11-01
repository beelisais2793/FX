% VX_filter_nothing.m   [DAFXbook, 2nd ed., chapter 7]
clear; clf
%===== This program (i) performs a complex-valued filter bank
%===== then (ii) filters a sound through the filter bank 
%===== and (iii) reconstructs a sound
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------


%----- user data -----
fig_plot     = 0;    % use any value except 0 or [] to plot figures
s_win        = 256;  % window size
nChannel     = 128;  % nb of channels
n1           = 1024; % block size for calculation
[DAFx_in,FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
window   = hanning(s_win, 'periodic');
L        = length(DAFx_in);
DAFx_in  = [DAFx_in; zeros(n1,1)] / max(abs(DAFx_in)); % 0-pad & normalize
DAFx_out = zeros(length(DAFx_in),1);
X_tilde  = zeros(n1,nChannel);
z        = zeros(s_win-1,nChannel);

%----- initialize the complex-valued filter bank -----
t    = (-s_win/2:s_win/2-1)';
filt = zeros(s_win, nChannel);
for k=1:nChannel
  wk        = 2*pi*i*(k/s_win);
  filt(:,k) = window.*exp(wk*t);
end

if(fig_plot), colormap(gray); end

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pend = length(DAFx_in) - n1;
while pin<pend
  grain = DAFx_in(pin+1:pin+n1);
%===========================================
  %----- filtering -----
  for k=1:nChannel
    [X_tilde(:,k),z(:,k)] = filter(filt(:,k),1,grain,z(:,k));
  end
  if(fig_plot)
    imagesc(angle(X_tilde')); axis('xy'); colorbar; 
    xlabel('n \rightarrow'); ylabel('k \rightarrow'); 
    txt = sprintf('Complex-valued fil. bank: \\phi(n,k), t=%6.3f s', (pin+1)/FS);
    title(txt); drawnow;
  end
    %----- sound reconstruction -----
  res = real(sum(X_tilde,2));
%===========================================
  DAFx_out(pin+1:pin+n1) = res;
  pin = pin + n1;
  end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
DAFx_out = DAFx_out(nChannel+1:nChannel+L) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_filter_nothing.wav');