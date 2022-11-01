% VX_het_nothing.m   [DAFXbook, 2nd ed., chapter 7]
clear; clf
%===== This program (i) implements a heterodyne filter bank,
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
fig_plot      = 0;    % use any value except 0 or [] to plot figures
s_win         = 256;  % window size
n_channel     = 128;  % nb of channels
s_block       = 1024; % computation block size (must be a multiple of s_win)
[DAFx_in, FS] = wavread('la.wav');

%----- initialize windows, arrays, etc -----
window   = hanning(s_win, 'periodic');
s_buffer = length(DAFx_in);
DAFx_in  = [DAFx_in; zeros(s_block,1)] / max(abs(DAFx_in)); % 0-pad & normalize
DAFx_out = zeros(length(DAFx_in),1);
X        = zeros(s_block, n_channel);
z        = zeros(s_win-1, n_channel);

%----- initialize the heterodyn filters -----
t   = (0:s_block-1)';
het = zeros(s_block,n_channel);
for k=1:n_channel
  wk        = 2*pi*i*(k/s_win);
  het(:,k)  = exp(wk*(t+s_win/2));
  het2(:,k) = exp(-wk*t);
end

%----- displays the phase of the filter -----
if(fig_plot)
  colormap(gray); imagesc(angle(het)'); colorbar; 
  axis('xy'); xlabel('n \rightarrow'); ylabel('k \rightarrow'); 
  title('Heterodyn filter bank: initial \phi(n,k)'); pause;
end

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pend = length(DAFx_in) - s_block;
while pin<pend
  grain = DAFx_in(pin+1:pin+s_block);
%===========================================
  %----- filtering through the filter bank -----
  for k=1:n_channel
    [X(:,k), z(:,k)] = filter(window, 1, grain.*het(:,k), z(:,k));
  end
  X_tilde = X.*het2;
  %----- drawing -----
  if(fig_plot)
    imagesc(angle(X_tilde')); axis('xy'); colorbar; 
    xlabel('n \rightarrow'); ylabel('k \rightarrow'); 
    txt = sprintf('Heterodyn filter bank: \\phi(n,k), t=%6.3f s', (pin+1)/FS);
    title(txt); drawnow;
  end
  %----- sound reconstruction -----
  res = real(sum(X_tilde,2));
%===========================================
  DAFx_out(pin+1:pin+s_block) = res;
  pin = pin + s_block;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
DAFx_out = DAFx_out(n_channel+1:n_channel+s_buffer) / max(abs(DAFx_out));
soundsc(DAFx_out, FS);
wavwrite(DAFx_out, FS, 'la_het_nothing.wav');
