function [y,yh,ys] = spsmodel(x,fs,w,N,t,maxnS,stocf)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%=> analysis/synthesis of a sound using the sinusoidal harmonic model
% x: input sound, fs: sampling rate, w: analysis window (odd size), 
% N: FFT size (minimum 512), t: threshold in negative dB, 
% maxnS: maximum number of sinusoids,
% stocf: decimation factor of mag spectrum for stochastic analysis
% y: output sound, yh: harmonic component, ys: stochastic component
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
ys = zeros(soundlength+Ns/2,1);          % output residual component
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
lastysloc = zeros(maxnS,1);          % initialize synthesis harmonic locations
ysphase = 2*pi*rand(maxnS,1);        % initialize synthesis harmonic phases
fridx = 0;
while pin<pend
  %-----analysis-----%
  xw = x(pin-hM:pin+hM).*w(1:M);         % window the input sound
  fftbuffer = fftbuffer*0;               % reset buffer;
  fftbuffer(1:(M+1)/2) = xw((M+1)/2:M);  % zero-phase window in fftbuffer
  fftbuffer(N-(M-1)/2+1:N) = xw(1:(M-1)/2);
  X = fft(fftbuffer);                    % compute the FFT
  mX = 20*log10(abs(X(1:N2)));           % magnitude spectrum 
  pX = unwrap(angle(X(1:N/2+1)));        % unwrapped phase spectrum 
  ploc = 1 + find((mX(2:N2-1)>t) .* (mX(2:N2-1)>mX(3:N2)) ...
                  .* (mX(2:N2-1)>mX(1:N2-2)));          % find peaks
  [ploc,pmag,pphase] = peakinterp(mX,pX,ploc);          % refine peak values  
  % sort by magnitude
  [smag,I] = sort(pmag(:),1,'descend');
  nS = min(maxnS,length(find(smag>t)));
  sloc = ploc(I(1:nS));
  sphase = pphase(I(1:nS));  
  if (fridx==0)
      % update last frame data for first frame
      lastnS = nS;
      lastsloc = sloc;
      lastsmag = smag;
      lastsphase = sphase;
  end
  % connect sinusoids to last frame lnS (lastsloc,lastsphase,lastsmag)
  sloc(1:nS) = (sloc(1:nS)~=0).*((sloc(1:nS)-1)*Ns/N);  % synth. locs
  lastidx = zeros(1,nS);
  for i=1:nS
    [dev,idx] = min(abs(sloc(i) - lastsloc(1:lastnS)));  
    lastidx(i) = idx;
  end
  ri= pin-hNs;                     % input sound pointer for residual analysis
  xr = x(ri:ri+Ns-1).*wr(1:Ns);    % window the input sound
  Xr = fft(fftshift(xr));          % compute FFT for residual analysis
  Xh = genspecsines(sloc,smag,sphase,Ns);  % generate sines
  Xr = Xr-Xh;                              % get the residual complex spectrum
  mXr = 20*log10(abs(Xr(1:Ns/2+1)));       % magnitude spectrum of residual
  mXsenv = decimate(max(-200,mXr),stocf);  % decimate the magnitude spectrum
                                           % and avoid -Inf
  %-----synthesis data-----%
  ysloc = sloc;                            % synthesis locations
  ysmag = smag(1:nS);                      % synthesis magnitudes
  mYsenv = mXsenv;                         % synthesis residual envelope
  %-----transformations-----%
  %-----synthesis-----%
  if (fridx==0)
      lastysphase = ysphase;
  end
  if (nS>lastnS)
      lastysphase = [ lastysphase ; zeros(nS-lastnS,1) ];
      lastysloc = [ lastysloc ; zeros(nS-lastnS,1) ];
  end
  ysphase = lastysphase(lastidx(1:nS)) + 2*pi*( ...
            lastysloc(lastidx(1:nS))+ysloc)/2/Ns*H; % propagate phases
  lastysloc = ysloc;
  lastysphase = ysphase;  
  lastnS = nS;                               % update last frame data
  lastsloc = sloc;                           % update last frame data
  lastsmag = smag;                           % update last frame data
  lastsphase = sphase;                       % update last frame data
  Yh = genspecsines(ysloc,ysmag,ysphase,Ns); % generate sines
  mYs = interp(mYsenv,stocf);                % interpolate to original size
  roffset = ceil(stocf/2)-1;                 % interpolated array offset
  mYs = [ mYs(1)*ones(roffset,1); mYs(1:Ns/2+1-roffset) ];
  mYs = 10.^(mYs/20);                        % dB to linear magnitude
  pYs = 2*pi*rand(Ns/2+1,1);      % generate phase spectrum with random values
  mYs1 = [mYs(1:Ns/2+1); mYs(Ns/2:-1:2)]; % create complete magnitude spectrum
  pYs1 = [pYs(1:Ns/2+1); -1*pYs(Ns/2:-1:2)]; % create complete phase spectrum
  Ys = mYs1.*cos(pYs1)+1i*mYs1.*sin(pYs1);   % compute complex spectrum
  yhw = fftshift(real(ifft(Yh)));     % sines in time domain using inverse FFT
  ysw = fftshift(real(ifft(Ys)));     % stochastic in time domain using IFFT
  yh(ri:ri+Ns-1) = yh(ri:ri+Ns-1)+yhw(1:Ns).*sw;  % overlap-add for sines
  ys(ri:ri+Ns-1) = ys(ri:ri+Ns-1)+ysw(1:Ns).*sws; % overlap-add for stochastic
  pin = pin+H;                                    % advance the sound pointer
  fridx = fridx+1;
end
y= yh+ys;                                         % sum sines and stochastic