function y = stpt(x, w, N, H, t)
% Analysis/synthesis of a sound using the peaks
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% of the short-time Fourier transform
% x: input sound, w: analysis window (odd size), N: FFT size, H: hop size, 
% t: threshold in negative dB, y: output sound
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

M = length(w);                                % analysis window size
N2 = N/2+1;                                   % size of positive spectrum
soundlength = length(x);                      % length of input sound aray
hM = (M-1)/2;                                 % half analysis window size
pin = 1+hM;        % initialize sound pointer at the middle of analysis window
pend = soundlength-hM;                        % last sample to start a frame
fftbuffer = zeros(N,1);                       % initialize buffer for FFT
yw = zeros(M,1);                              % initialize output sound frame
y = zeros(soundlength,1);                     % initialize output array
w = w/sum(w);                                 % normalize analysis window
sw = hanning(M);                              % synthesis window
sw = sw./sum(sw);
while pin<pend
  %-----analysis-----%
  xw = x(pin-hM:pin+hM).*w(1:M);              % window the input sound
  fftbuffer(:) = 0;                           % reset buffer  
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);       % zero-phase fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                         % compute the FFT
  mX = 20*log10(abs(X(1:N2)));    % magnitude spectrum of positive frequencies
  pX = unwrap(angle(X(1:N2)));                % unwrapped phase spectrum 
  ploc = 1 + find((mX(2:N2-1)>t) .* (mX(2:N2-1)>mX(3:N2)) ...
         .* (mX(2:N2-1)>mX(1:N2-2)));         % peaks
  pmag = mX(ploc);                            % magnitude of peaks
  pphase = pX(ploc);                          % phase of peaks
  %-----synthesis-----%
  Y = zeros(N,1); % initialize output spectrum
  Y(ploc) = 10.^(pmag/20).*exp(i.*pphase);      % generate positive freq.
  Y(N+2-ploc) = 10.^(pmag/20).*exp(-i.*pphase); % generate negative freq.
  fftbuffer = real(ifft(Y));                    % real part of the inverse FFT
  yw((M+1)/2:M) = fftbuffer(1:(M+1)/2);	        % undo zero phase window
  yw(1:(M-1)/2) = fftbuffer(N-(M-1)/2+1:N);
  y(pin-hM:pin+hM) = y(pin-hM:pin+hM) + H*N*sw.*yw(1:M);     % overlap-add
  pin  = pin+H;                                 % advance sound pointer    
end