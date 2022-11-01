function Y = genspecsines(ploc, pmag, pphase, N)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Compute a spectrum from a series of sine values
% iploc, ipmag, ipphase: sine locations, magnitudes and phases
% N: size of complex spectrum
% Y: generated complex spectrum of sines
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Y =zeros(N,1); % initialize output spectrum
hN = N/2+1;                           % size of positive freq. spectrum
for i=1:length(ploc);                 % generate all sine spectral lobes
  loc = ploc(i);                      % location of peak (zero-based indexing)
                                      % it should be in range ]0,hN-1[
  if (loc<=1||loc>=hN-1) continue; end; % avoid frequencies out of range
  binremainder = round(loc)-loc;
  lb = [binremainder-4:binremainder+4]'; % main lobe (real value) bins to read
  lmag = genbh92lobe(lb)*10.^(pmag(i)/20); % lobe magnitudes of the 
                                           % complex exponential
  b = 1+[round(loc)-4:round(loc)+4]';      % spectrum bins to fill
                                           % (1-based indexing)
  for m=1:9
      if (b(m)<1)                          % peak lobe croses DC bin
          Y(2-b(m)) = Y(2-b(m)) + lmag(m)*exp(-1i*pphase(i));
      elseif (b(m)>hN)                     % peak lobe croses Nyquist bin
          Y(2*hN-b(m)) = Y(2*hN-b(m)) + lmag(m)*exp(-1i*pphase(i));
      else                                 % peak lobe in positive freq. range
          Y(b(m)) = Y(b(m)) + lmag(m)*exp(1i*pphase(i)) ...
                    + lmag(m)*exp(-1i*pphase(i))*(b(m)==1||b(m)==hN);
      end    
  end
  Y(hN+1:end) = conj(Y(hN-1:-1:2));        % fill the rest of the spectrum
end 