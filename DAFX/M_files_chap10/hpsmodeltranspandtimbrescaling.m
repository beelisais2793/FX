%-----pitch transposition and timbre scaling-----%
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

yhloc = yhloc*fscale;                % scale harmonic frequencies
yf0 = f0*fscale;                     % synthesis fundamental frequency
% harmonics
if (f0>0)
  thloc = interp1( timbremapping(2,:), timbremapping(1,:), ...
                   yhloc/Ns*fs) / fs*Ns; % mapped harmonic freqs.
  idx = find(hloc>0 & hloc<Ns*.5);       % harmonic indexes in frequency range
  yhmag = interp1([0; hloc(idx); Ns],[ hmag(1); hmag(idx); hmag(end) ],thloc); 
                                         % interpolated envelope
end
% residual
  % frequency (Hz) of the last  coefficient
  frescoef = fs/2*length(mYsenv)*stocf/length(mXr); 
  % mapped coef. indexes
  trescoef = interp1( timbremapping(2,:), timbremapping(1,:), ...
             min(fs/2,[0:length(mYsenv)-1]'/(length(mYsenv)-1)*frescoef) );
  % interpolated envelope                                   
  mYsenv = interp1([0:length(mYsenv)-1],mYsenv, ...
                 trescoef/frescoef*(length(mYsenv)-1));