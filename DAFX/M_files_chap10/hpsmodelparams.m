function [y,yh,ys] = hpsmodelparams(x,fs,w,N,t,nH,minf0,maxf0,f0et,...
                                    maxhd,stocf,timemapping)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%=> analysis/synthesis of a sound using the sinusoidal harmonic model
% x: input sound, fs: sampling rate, w: analysis window (odd size), 
% N: FFT size (minimum 512), t: threshold in negative dB, 
% nH: maximum number of harmonics, minf0: minimum f0 frequency in Hz, 
% maxf0: maximim f0 frequency in Hz, 
% f0et: error threshold in the f0 detection (ex: 5),
% maxhd: max. relative deviation in harmonic detection (ex: .2)
% stocf: decimation factor of mag spectrum for stochastic analysis
% timemapping: mapping between input and output time (sec)
% y: output sound, yh: harmonic component, ys: stochastic component
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

if length(timemapping)==0                % argument not specified
    timemapping =[   0  length(x)/fs;    % input time
                     0  length(x)/fs ];  % output time
end
M = length(w);                           % analysis window size
Ns = 1024;                               % FFT size for synthesis
H = 256;                                 % hop size for analysis and synthesis
N2 = N/2+1;                              % half-size of spectrum
hNs = Ns/2;                              % half synthesis window size
hM = (M-1)/2;                            % half analysis window size
fftbuffer = zeros(N,1);                  % initialize buffer for FFT
outsoundlength = 1+round(timemapping(2,end)*fs);     % length of output sound
yh = zeros(outsoundlength+Ns/2,1);       % output sine component
ys = zeros(outsoundlength+Ns/2,1);       % output residual component
w = w/sum(w);                            % normalize analysis window
sw = zeros(Ns,1);
ow = triang(2*H-1);                      % overlapping window
ovidx = Ns/2+1-H+1:Ns/2+H;               % overlap indexes
sw(ovidx) = ow(1:2*H-1);
bh = blackmanharris(Ns);                 % synthesis window
bh = bh ./ sum(bh);                      % normalize synthesis window
wr = bh;                                 % window for residual 
sw(ovidx) = sw(ovidx) ./ bh(ovidx);
sws = H*hanning(Ns)/2;               % synthesis window for stochastic
lastyhloc = zeros(nH,1);             % initialize synthesis harmonic locations
yhphase = 2*pi*rand(nH,1);           % initialize synthesis harmonic phases
poutend = outsoundlength-max(hM,H);  % last sample to start a frame
pout = max(H+1,1+hM);  % initialize sound pointer to middle of analysis window
minpin = max(H+1,1+hM);
maxpin = min(length(x)-hM-1);
while pout<poutend
  pin = round( interp1(timemapping(2,:),timemapping(1,:),pout/fs,'linear',...
                       'extrap') * fs );
  pin = max(minpin,pin);
  pin = min(maxpin,pin);
  %-----analysis-----%
  xw = x(pin-hM:pin+hM).*w(1:M);         % window the input sound
  fftbuffer(:) = 0;                      % reset buffer
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);  % zero-phase window in fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                    % compute the FFT
  mX = 20*log10(abs(X(1:N2)));           % magnitude spectrum 
  pX = unwrap(angle(X(1:N/2+1)));        % unwrapped phase spectrum 
  ploc = 1 + find((mX(2:N2-1)>t) .* (mX(2:N2-1)>mX(3:N2)) ...
                  .* (mX(2:N2-1)>mX(1:N2-2)));     % find peaks
  [ploc,pmag,pphase] = peakinterp(mX,pX,ploc);     % refine peak values
  yinws = round(fs*0.0125);          % using approx. a 12.5 ms window for yin
  yinws = yinws+mod(yinws,2);        % make it even
  yb = pin-yinws/2;
  ye = pin+yinws/2+yinws;
  if (yb<1 || ye>length(x))          % out of boundaries
     f0 = 0; 
  else
     f0 = f0detectionyin(x(yb:ye),fs,yinws,minf0,maxf0); % compute f0
  end
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
  xr = x(ri:ri+Ns-1).*wr(1:Ns);           % window the input sound
  Xr = fft(fftshift(xr));                 % compute FFT for residual analysis
  Xh = genspecsines(hloc(1:hi-1),hmag,hphase,Ns);            % generate sines
  Xr = Xr-Xh;                             % get the residual complex spectrum
  mXr = 20*log10(abs(Xr(1:Ns/2+1)));      % magnitude spectrum of residual
  mXsenv = decimate(max(-200,mXr),stocf); % decimate the magnitude spectrum
                                          % and avoid -Inf
  %-----synthesis data-----%
  yhloc = hloc;                           % synthesis harmonics locs
  yhmag = hmag;                           % synthesis harmonic amplitudes
  mYsenv = mXsenv;                        % synthesis residual envelope
  yf0 = f0;                               % synthesis f0
  %-----transformations-----%
  %-----synthesis-----%
  yhphase = yhphase + 2*pi*(lastyhloc+yhloc)/2/Ns*H;   % propagate phases
  lastyhloc = yhloc; 
  Yh = genspecsines(yhloc,yhmag,yhphase,Ns);    % generate sines
  mYs = interp(mYsenv,stocf);                   % interpolate to original size
  roffset = ceil(stocf/2)-1;                    % interpolated array offset
  mYs = [ mYs(1)*ones(roffset,1); mYs(1:Ns/2+1-roffset) ];
  mYs = 10.^(mYs/20);                           % dB to linear magnitude
  if (f0>0)
      mYs = mYs .* cos(pi*[0:Ns/2]'/Ns*fs/yf0).^2;  % filter residual
  end
  fc = 1+round(500/fs*Ns);                          % 500 Hz
  mYs(1:fc) = mYs(1:fc) .* ([0:fc-1]'/(fc-1)).^2;   % HPF
  pYs = 2*pi*rand(Ns/2+1,1);      % generate phase spectrum with random values
  mYs1 = [mYs(1:Ns/2+1); mYs(Ns/2:-1:2)]; % create complete magnitude spectrum
  pYs1 = [pYs(1:Ns/2+1); -1*pYs(Ns/2:-1:2)];  % create complete phase spectrum
  Ys = mYs1.*cos(pYs1)+1i*mYs1.*sin(pYs1);    % compute complex spectrum
  yhw = fftshift(real(ifft(Yh)));       % sines in time domain using IFFT
  ysw = fftshift(real(ifft(Ys)));       % stochastic in time domain using IFFT
  ro= pout-hNs;                             % output sound pointer for overlap
  yh(ro:ro+Ns-1) = yh(ro:ro+Ns-1)+yhw(1:Ns).*sw;  % overlap-add for sines
  ys(ro:ro+Ns-1) = ys(ro:ro+Ns-1)+ysw(1:Ns).*sws; % overlap-add for stochastic
  pout = pout+H;                                  % advance the sound pointer
end
y= yh+ys;                                         % sum sines and stochastic