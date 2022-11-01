function y = stftenv(x, w, N, H)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Analysis/synthesis of a sound using the short-time Fourier transform
% x: input sound, w: analysis window (odd size), N: FFT size, H: hop size
% y: output sound
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

M = length(w);                            % analysis window size
N2 = N/2+1;                               % size of positive spectrum
soundlength = length(x);                  % length of input sound array
hM = (M-1)/2;                             % half analysis window size
pin = 1+hM;            % initialize sound pointer in middle of analysis window
pend = soundlength-hM;                    % last sample to start a frame
fftbuffer = zeros(N,1);                   % initialize buffer for FFT
yw = zeros(M,1);                          % initialize output sound frame
y = zeros(soundlength,1);                 % initialize output array
yenv = y;                                 % initialize window overlap envelope
w = w/sum(w);                             % normalize analysis window
while pin<pend
  %-----analysis-----%
  xw = x(pin-hM:pin+hM).*w(1:M);          % window the input sound
  fftbuffer(:) = 0;                       % reset buffer
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);   % zero-phase window in fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                     % compute FFT
  mX = 20*log10(abs(X(1:N2)));    % magnitude spectrum of positive frequencies
  pX = unwrap(angle(X(1:N2)));    % unwrapped phase spect. of positive freq.
  %-----synthesis-----%
  Y = zeros(N,1);                                  % initialize spectrum
  Y(1:N2) = 10.^(mX/20).*exp(i.*pX);               % generate positive freq.
  Y(N2+1:N) = 10.^(mX(N2-1:-1:2)/20).*exp(-i.*pX(N2-1:-1:2)); 
                                                   % generate neg.freq.
  fftbuffer = real(ifft(Y));                       % inverse FFT
  yw(1:(M-1)/2) = fftbuffer(N-(M-1)/2+1:N);        % undo zero-phase window
  yw((M+1)/2:M) = fftbuffer(1:(M+1)/2); 
  y(pin-hM:pin+hM) = y(pin-hM:pin+hM) + yw(1:M);   % output signal overlap-add
  yenv(pin-hM:pin+hM) = yenv(pin-hM:pin+hM) + w;   % window overlap-add
  pin  = pin+H;                                    % advance sound pointer
end
yenvth = max(yenv)*0.1;                            % envelope threshold
yenv(find(yenv<yenvth)) = yenvth;
y = y./yenv;