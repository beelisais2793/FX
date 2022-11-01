function [y,yh,yr] = hprmodel(x,fs,w,N,t,nH,minf0,maxf0,f0et,maxhd)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Analysis/synthesis of a sound using the sinusoidal harmonic model
% x: input sound, fs: sampling rate, w: analysis window (odd size), 
% N: FFT size (minimum 512), t: threshold in negative dB, 
% nH: maximum number of harmonics, minf0: minimum f0 frequency in Hz, 
% maxf0: maximim f0 frequency in Hz, 
% f0et: error threshold in the f0 detection (ex: 5),
% maxhd: max. relative deviation in harmonic detection (ex: .2)
% y: output sound, yh: harmonic component, yr: residual component
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

M = length(w);                           % analysis window size
Ns = 1024;                               % FFT size for synthesis
H = 256;                                 % hop size for analysis and synthesis
N2 = N/2+1;                              % half-size of spectrum
soundlength = length(x);                 % length of input sound array
hNs = Ns/2;                              % half synthesis window size
hM = (M-1)/2;                            % half analysis window size
pin = max(H+1,1+hM);   % initialize sound pointer to middle of analysis window
pend = soundlength-max(hM,H);            % last sample to start a frame
fftbuffer = zeros(N,1);                  % initialize buffer for FFT
yh = zeros(soundlength+Ns/2,1);          % output sine component
yr = zeros(soundlength+Ns/2,1);          % output residual component
w = w/sum(w);                            % normalize analysis window
sw = zeros(Ns,1);
ow = triang(2*H-1);                      % overlapping window
ovidx = Ns/2+1-H+1:Ns/2+H;               % overlap indexes
sw(ovidx) = ow(1:2*H-1);
bh = blackmanharris(Ns);                 % synthesis window
bh = bh ./ sum(bh);                      % normalize synthesis window
wr = bh;                                 % window for residual 
sw(ovidx) = sw(ovidx) ./ bh(ovidx);
while pin<pend
  %-----analysis-----%
  xw = x(pin-hM:pin+hM).*w(1:M);         % window the input sound
  fftbuffer(:) = 0;                      % reset buffer
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);  % zero-phase window in fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                    % compute the FFT
  mX = 20*log10(abs(X(1:N2)));           % magnitude spectrum 
  pX = unwrap(angle(X(1:N/2+1)));        % unwrapped phase spectrum 
  ploc = 1 + find((mX(2:N2-1)>t) .* (mX(2:N2-1)>mX(3:N2)) ...
                  .* (mX(2:N2-1)>mX(1:N2-2)));         % find peaks
  [ploc,pmag,pphase] = peakinterp(mX,pX,ploc);         % refine peak values
  f0 = f0detectiontwm(mX,fs,ploc,pmag,f0et,minf0,maxf0);  % find f0
  hloc = zeros(nH,1);                    % initialize harmonic locations
  hmag = zeros(nH,1)-100;                % initialize harmonic magnitudes
  hphase = zeros(nH,1);                  % initialize harmonic phases
  hf = (f0>0).*(f0.*(1:nH));             % initialize harmonic frequencies
  hi = 1;                                % initialize harmonic index
  npeaks = length(ploc);                 % number of peaks found
  while (f0>0 && hi<=nH && hf(hi)<fs/2)  % find harmonic peaks
    [dev,pei] = min(abs((ploc(1:npeaks)-1)/N*fs-hf(hi)));   % closest peak
    if ((hi==1 || ~any(hloc(1:hi-1)==ploc(pei))) && dev<maxhd*hf(hi))
      hloc(hi) = ploc(pei);              % harmonic locations
      hmag(hi) = pmag(pei);              % harmonic magnitudes
      hphase(hi) = pphase(pei);          % harmonic phases
    end
    hi = hi+1;                           % increase harmonic index
  end
  hloc(1:hi-1) = (hloc(1:hi-1)~=0).*((hloc(1:hi-1)-1)*Ns/N);   % synth. locs
  ri= pin-hNs;                     % input sound pointer for residual analysis
  xr = x(ri:ri+Ns-1).*wr(1:Ns);          % window the input sound
  Xr = fft(fftshift(xr));                % compute FFT for residual analysis
  %-----synthesis-----%
  Yh = genspecsines(hloc(1:hi-1),hmag,hphase,Ns);             % generate sines
  Yr = Xr-Yh;                      % get the residual complex spectrum
  yhw = fftshift(real(ifft(Yh)));  % sines in time domain using inverse FFT
  yrw = fftshift(real(ifft(Yr)));  % residual in time domain using inverse FFT
  yh(ri:ri+Ns-1) = yh(ri:ri+Ns-1)+yhw(1:Ns).*sw;   % overlap-add for sines
  yr(ri:ri+Ns-1) = yr(ri:ri+Ns-1)+yrw(1:Ns).*sw;   % overlap-add for residual
  pin = pin+H;                                     % advance the sound pointer
end
y= yh+yr;     