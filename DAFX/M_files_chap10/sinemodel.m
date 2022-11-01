function y = sinemodel(x, w, N, t)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Analysis/synthesis of a sound using the sinusoidal model
% x: input sound, w: analysis window (odd size), N: FFT size,  
% t: threshold in negative dB, y: output sound
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

M = length(w);                                % analysis window size
Ns= 1024;                                     % FFT size for synthesis (even)
H = 256;                                      % analysis/synthesishop size
N2= N/2+1;                                    % size of positive spectrum
soundlength = length(x);                      % length of input sound array
hNs = Ns/2;                                   % half synthesis window size
hM = (M-1)/2;                                 % half analysis window size
pin = max(H+1,1+hM); % initialize sound pointer to middle of analysis window
pend = soundlength-max(H,hM);                 % last sample to start a frame
fftbuffer = zeros(N,1);                       % initialize buffer for FFT
y = zeros(soundlength,1);	                    % initialize output array
w = w/sum(w);                                 % normalize analysis window
sw = zeros(Ns,1);
ow = triang(2*H-1);                           % overlapping window
ovidx = Ns/2+1-H+1:Ns/2+H;                    % overlap indexes
sw(ovidx) = ow(1:2*H-1);
bh = blackmanharris(Ns);                      % synthesis window
bh = bh ./ sum(bh);                           % normalize synthesis window
sw(ovidx) = sw(ovidx) ./ bh(ovidx);
while pin<pend
  %-----analysis-----%
  xw = x(pin-hM:pin+hM).*w(1:M);              % window the input sound
  fftbuffer(:) = 0;                           % reset buffer
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);       % zero-phase window in fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                              % compute the FFT
  mX = 20*log10(abs(X(1:N2)));    % magnitude spectrum of positive frequencies
  pX = unwrap(angle(X(1:N/2+1)));                  % unwrapped phase spectrum
  ploc = 1 + find((mX(2:N2-1)>t) .* (mX(2:N2-1)>mX(3:N2)) ...
                  .* (mX(2:N2-1)>mX(1:N2-2)));     % find peaks
  [ploc,pmag,pphase] = peakinterp(mX,pX,ploc);     % refine peak values
  %-----synthesis-----%
  plocs = (ploc-1)*Ns/N;             % adapt peak locations to synthesis FFT
  Y = genspecsines(plocs,pmag,pphase,Ns);          % generate spec sines
  yw = fftshift(real(ifft(Y)));                    % time domain of sinusoids
  y(pin-hNs:pin+hNs-1) = y(pin-hNs:pin+hNs-1) + sw.*yw(1:Ns);    % overlap-add
  pin  = pin+H;                                    % advance the sound pointer    
end