%-----change to old-----%
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[x,fs] = wavread('fairytale.wav');
w=[blackmanharris(1024);0];
fscale = 2;
timbremapping = [ 0  fs/2;   % input frequency
                  0  fs/2 ]; % output frequency
vtf = 6.5;                   % vibrato-tremolo frequency in Hz
va  = 400;                   % vibrato depth in cents
td  = 3;                     % tremolo depth in dB
[y,yh,yr] = hpsmodelparams(x,fs,w,4096,-150,200,50,600,2,.3,1,...
                           [],fscale,timbremapping,vtf,va,td);