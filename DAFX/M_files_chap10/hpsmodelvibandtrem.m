%-----vibrato and tremolo-----% 
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

if (f0>0)
  vtf = 5;                                   % vibrato-tremolo frequency in Hz
  va  = 50;                                  % vibrato depth in cents
  td  = 3;                                   % tremolo depth in dB
  sfscale = fscale*2^(va/1200*sin(2*pi*vtf*pin/fs)); 
  yhloc = yhloc*sfscale;                     % synthesis harmonic locs
  yf0 = f0*sfscale;                          % synthesis f0
  idx = find(hloc>0 & hloc<Ns*.5);
  yhmag = interp1([0; hloc(idx); Ns],[ hmag(1); hmag(idx); ...
                  hmag(end) ],yhloc);        % interpolated envelope
  yhmag = yhmag + td*sin(2*pi*vtf*pin/fs);   % tremolo
end