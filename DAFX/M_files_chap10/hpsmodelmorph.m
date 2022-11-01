function [y,yh,ys] = hpsmodelmorph(x,x2,fs,w,N,t,nH,minf0,maxf0,f0et,...
                                    maxhd,stocf,f0intp,htintp,rintp)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%=> morph between two sounds using the harmonic plus stochastic model
% x,x2: input sounds, fs: sampling rate, w: analysis window (odd size), 
% N: FFT size (minimum 512), t: threshold in negative dB, 
% nH: maximum number of harmonics, minf0: minimum f0 frequency in Hz, 
% maxf0: maximim f0 frequency in Hz, 
% f0et: error threshold in the f0 detection (ex: 5),
% maxhd: max. relative deviation in harmonic detection (ex: .2)
% stocf: decimation factor of mag spectrum for stochastic analysis,
% f0intp: f0 interpolation factor,
% htintp: harmonic timbre interpolation factor,
% rintp: residual interpolation factor,
% y: output sound, yh: harmonic component, ys: stochastic component
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

if length(f0intp)==1
    f0intp =[ 0      length(x)/fs;    % input time
              f0intp f0intp       ];  % control value
end
if length(htintp)==1
    htintp =[ 0      length(x)/fs;    % input time
              htintp htintp       ];  % control value
end
if length(rintp)==1
    rintp =[ 0     length(x)/fs;      % input time
             rintp rintp         ];   % control value
end
M = length(w);                        % analysis window size
Ns = 1024;                            % FFT size for synthesis
H = 256;                              % hop size for analysis and synthesis
N2 = N/2+1;                           % half-size of spectrum
soundlength = length(x);              % length of input sound array
hNs = Ns/2;                           % half synthesis window size
hM = (M-1)/2;                         % half analysis window size
pin = max(H+1,1+hM);   % initialize sound pointer to middle of analysis window
pend = soundlength-max(hM,H);         % last sample to start a frame
fftbuffer = zeros(N,1);               % initialize buffer for FFT
yh = zeros(soundlength+Ns/2,1);       % output sine component
ys = zeros(soundlength+Ns/2,1);       % output residual component
w = w/sum(w);                         % normalize analysis window
sw = zeros(Ns,1);
ow = triang(2*H-1);                   % overlapping window
ovidx = Ns/2+1-H+1:Ns/2+H;            % overlap indexes
sw(ovidx) = ow(1:2*H-1);
bh = blackmanharris(Ns);              % synthesis window
bh = bh ./ sum(bh);                   % normalize synthesis window
wr = bh;                              % window for residual 
sw(ovidx) = sw(ovidx) ./ bh(ovidx);
sws = H*hanning(Ns)/2;                % synthesis window for stochastic
lastyhloc = zeros(nH,1);              % initialize synthesis harmonic locs.
yhphase = 2*pi*rand(nH,1);            % initialize synthesis harmonic phases
minpin2 = max(H+1,1+hM);              % minimum sample value for x2
maxpin2 = min(length(x2)-hM-1);       % maximum sample value for x2
while pin<pend
  %-----first sound analysis-----%
  [f0,hloc,hmag,mXsenv] = hpsanalysis(x,fs,w,wr,pin,M,hM,N,N2,Ns,hNs,...
                                      nH,t,f0et,minf0,maxf0,maxhd,stocf);
  %-----second sound analysis-----%
  pin2 = round(pin/length(x)*length(x2)); % linear time mapping between inputs
  pin2 = min(maxpin2,max(minpin2,pin2));
  [f02,hloc2,hmag2,mXsenv2] = hpsanalysis(x2,fs,w,wr,pin2,M,hM,N,N2,Ns,hNs,...
                                          nH,t,f0et,minf0,maxf0,maxhd,stocf);
  %-----morph-----%
  cf0intp = interp1(f0intp(1,:),f0intp(2,:),pin/fs);   % get control value 
  chtintp = interp1(htintp(1,:),htintp(2,:),pin/fs);   % get control value
  crintp = interp1(rintp(1,:),rintp(2,:),pin/fs);      % get control value
  if (f0>0 && f02>0)
      outf0 = f0*(1-cf0intp) + f02*cf0intp;         % both inputs are harmonic 
      yhloc = [1:nH]'*outf0/fs*Ns;         % generate synthesis harmonic serie
      idx = find(hloc>0 & hloc<Ns*.5);
      yhmag = interp1([0;hloc(idx);Ns], [hmag(1);hmag(idx);hmag(end)],yhloc);
                                                       % interpolated envelope
      idx2 = find(hloc2>0 & hloc2<Ns*.5);
      yhmag2 = interp1([0; hloc2(idx2); Ns],...
             [hmag2(1);hmag2(idx2);hmag2(end)],yhloc); % interpolated envelope
      yhmag = yhmag*(1-chtintp) + yhmag2*chtintp;      % timbre morphing
  else
      outf0 = 0;                                     % remove harmonic content
      yhloc = hloc.*0;
      yhmag = hmag.*0;
  end
  mYsenv = mXsenv*(1-crintp) + mXsenv2*crintp; 
  %-----synthesis-----%
  yhphase = yhphase + 2*pi*(lastyhloc+yhloc)/2/Ns*H; % propagate phases
  lastyhloc = yhloc;
  Yh = genspecsines(yhloc,yhmag,yhphase,Ns);    % generate sines
  mYs = interp(mYsenv,stocf);                   % interpolate to original size
  roffset = ceil(stocf/2)-1;                    % interpolated array offset
  mYs = [ mYs(1)*ones(roffset,1); mYs(1:Ns/2+1-roffset) ];
  mYs = 10.^(mYs/20);                           % dB to linear magnitude
  pYs = 2*pi*rand(Ns/2+1,1);      % generate phase spectrum with random values
  mYs1 = [mYs(1:Ns/2+1); mYs(Ns/2:-1:2)]; % create complete magnitude spectrum
  pYs1 = [pYs(1:Ns/2+1); -1*pYs(Ns/2:-1:2)]; % create complete phase spectrum
  Ys = mYs1.*cos(pYs1)+1i*mYs1.*sin(pYs1);   % compute complex spectrum
  yhw = fftshift(real(ifft(Yh)));            % sines in time domain using IFFT
  ysw = fftshift(real(ifft(Ys)));       % stochastic in time domain using IFFT
  ro = pin-hNs;                         % output sound pointer for overlap
  yh(ro:ro+Ns-1) = yh(ro:ro+Ns-1)+yhw(1:Ns).*sw;  % overlap-add for sines
  ys(ro:ro+Ns-1) = ys(ro:ro+Ns-1)+ysw(1:Ns).*sws; % overlap-add for stochastic
  pin = pin+H;                                    % advance the sound pointer
end
y= yh+ys;                                         % sum sines and stochastic
end

function [f0,hloc,hmag,mXsenv] = hpsanalysis(x,fs,w,wr,pin,M,hM,N,N2,...
                                     Ns,hNs,nH,t,f0et,minf0,maxf0,maxhd,stocf)
  xw = x(pin-hM:pin+hM).*w(1:M);              % window the input sound
  fftbuffer = zeros(N,1);                     % initialize buffer for FFT
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);       % zero-phase window in fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                         % compute the FFT
  mX = 20*log10(abs(X(1:N2)));                % magnitude spectrum 
  pX = unwrap(angle(X(1:N/2+1)));             % unwrapped phase spectrum 
  ploc = 1 + find((mX(2:N2-1)>t) .* (mX(2:N2-1)>mX(3:N2)) ...
                  .* (mX(2:N2-1)>mX(1:N2-2)));    % find peaks
  [ploc,pmag,pphase] = peakinterp(mX,pX,ploc);    % refine peak values
  f0 = f0detectiontwm(mX,fs,ploc,pmag,f0et,minf0,maxf0);    % find f0
  hloc = zeros(nH,1);                        % initialize harmonic locations
  hmag = zeros(nH,1)-100;                    % initialize harmonic magnitudes
  hphase = zeros(nH,1);                      % initialize harmonic phases
  hf = (f0>0).*(f0.*(1:nH));                 % initialize harmonic frequencies
  hi = 1;                                    % initialize harmonic index
  npeaks = length(ploc);                     % number of peaks found
  while (f0>0 && hi<=nH && hf(hi)<fs/2)      % find harmonic peaks
    [dev,pei] = min(abs((ploc(1:npeaks)-1)/N*fs-hf(hi)));  % closest peak
    if ((hi==1 || ~any(hloc(1:hi-1)==ploc(pei))) && dev<maxhd*hf(hi))
      hloc(hi) = ploc(pei);                  % harmonic locations
      hmag(hi) = pmag(pei);                  % harmonic magnitudes
      hphase(hi) = pphase(pei);              % harmonic phases
    end
    hi = hi+1;                               % increase harmonic index
  end
  hloc(1:hi-1) = (hloc(1:hi-1)~=0).*((hloc(1:hi-1)-1)*Ns/N); % synth. locs
  ri = pin-hNs;                    % input sound pointer for residual analysis
  xr = x(ri:ri+Ns-1).*wr(1:Ns);    % window the input sound
  Xr = fft(fftshift(xr));          % compute FFT for residual analysis
  Xh = genspecsines(hloc(1:hi-1),hmag,hphase,Ns);        % generate sines
  Xr = Xr-Xh;                              % get the residual complex spectrum
  mXr = 20*log10(abs(Xr(1:Ns/2+1)));       % magnitude spectrum of residual
  mXsenv = decimate(max(-200,mXr),stocf);  % decimate the magnitude spectrum
end