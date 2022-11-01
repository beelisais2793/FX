%-----pitch discretization to temperate scale-----%
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

if (f0>0) % it has harmonics
  nst = round(12*log2(f0/55));     % closest semitone
  discpitch = 55*2^(nst/12);       % discretized pitch
  fscale = discpitch/f0 ;          % pitch transposition factor
  yf0 = f0*fscale;                 % synthesis f0
  yhloc = yhloc*fscale;
end