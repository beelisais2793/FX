function f0 = f0detectiontwm(mX, fs, ploc, pmag, ef0max, minf0, maxf0)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Fundamental frequency detection function
% mX: magnitude spectrum, fs: sampling rate, ploc, pmag: peak loc and mag,
% ef0max: maximim error allowed, minf0: minimum f0, maxf0: maximum f0
% f0: fundamental frequency detected in Hz
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

N = length(mX)*2;                     % size of complex spectrum
nPeaks = length(ploc);                % number of peaks
f0 = 0;                               % initialize output
if(nPeaks>3)            % at least 3 peaks in spectrum for trying to find f0
  nf0peaks = min(50,nPeaks);          % use a maximum of 50 peaks
  [f0,f0error] = TWM(ploc(1:nf0peaks),pmag(1:nf0peaks),N,fs,minf0,maxf0);
  if (f0>0 && f0error>ef0max)         % limit the possible error by ethreshold
    f0 = 0; 
  end 
end;

function [f0, f0error] = TWM (ploc, pmag, N, fs, minf0, maxf0)
% Two-way mismatch algorithm (by Beauchamp&Maher)
% ploc, pmag: peak locations and magnitudes, N: size of complex spectrum
% fs: sampling rate of sound, minf0: minimum f0, maxf0: maximum f0
% f0: fundamental frequency detected, f0error: error measure 
pfreq = (ploc-1)/N*fs;                    % frequency in Hertz of peaks
[zvalue,zindex] = min(pfreq);
if (zvalue==0)                            % avoid zero frequency peak
  pfreq(zindex) = 1;
  pmag(zindex) = -100;
end
ival2 = pmag;
[Mmag1,Mloc1] = max(ival2);               % find peak with maximum magnitude
ival2(Mloc1) = -100;                      % clear max peak
[Mmag2,Mloc2]= max(ival2);                % find second maximum magnitude peak
ival2(Mloc2) = -100;                      % clear second max peak
[Mmag3,Mloc3]= max(ival2);                % find third maximum magnitude peak
nCand = 3;                % number of possible f0 candidates for each max peak
f0c = zeros(1,3*nCand);                   % initialize array of candidates
f0c(1:nCand)=(pfreq(Mloc1)*ones(1,nCand))./((nCand+1-(1:nCand))); % candidates 
f0c(nCand+1:nCand*2)=(pfreq(Mloc2)*ones(1,nCand))./((nCand+1-(1:nCand))); 
f0c(nCand*2+1:nCand*3)=(pfreq(Mloc3)*ones(1,nCand))./((nCand+1-(1:nCand))); 
f0c = f0c((f0c<maxf0)&(f0c>minf0));       % candidates within boundaries
if (isempty(f0c))                         % if no candidates exit
  f0 = 0; f0error=100;
  return
end
harmonic = f0c;
ErrorPM = zeros(fliplr(size(harmonic)));          % initialize PM errors
MaxNPM = min(10,length(ploc));
for i=1:MaxNPM % predicted to measured mismatch error
  difmatrixPM = harmonic' * ones(size(pfreq))';
  difmatrixPM = abs(difmatrixPM-ones(fliplr(size(harmonic)))*pfreq');
  [FreqDistance,peakloc] = min(difmatrixPM,[],2);
  Ponddif = FreqDistance .* (harmonic'.^(-0.5));
  PeakMag = pmag(peakloc);
  MagFactor = 10.^((PeakMag-Mmag1)./20);
  ErrorPM = ErrorPM+(Ponddif+MagFactor.*(1.4*Ponddif-0.5));
  harmonic = harmonic+f0c;
end
ErrorMP = zeros(fliplr(size(harmonic)));          % initialize MP errors
MaxNMP = min(10,length(pfreq));
for i=1:length(f0c)                   % measured to predicted mismatch error
  nharm = round(pfreq(1:MaxNMP)/f0c(i));
  nharm = (nharm>=1).*nharm + (nharm<1);
  FreqDistance = abs(pfreq(1:MaxNMP) - nharm*f0c(i));
  Ponddif = FreqDistance.* (pfreq(1:MaxNMP).^(-0.5));
  PeakMag = pmag(1:MaxNMP);
  MagFactor = 10.^((PeakMag-Mmag1)./20);
  ErrorMP(i) = sum(MagFactor.*(Ponddif+MagFactor.*(1.4*Ponddif-0.5)));
end
Error = (ErrorPM/MaxNPM) + (0.3*ErrorMP/MaxNMP);  % total errors
[f0error, f0index] = min(Error);                  % get the smallest error
f0 = f0c(f0index);                                % f0 with the smallest error