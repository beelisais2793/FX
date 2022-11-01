% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[x,fs]=wavread('tedeum2.wav'); 
w=[blackmanharris(1024);0];
% female voice
fscale = 2^(4/12); % 4 semitones upwards
timbremapping = [ 0  4000  fs/2;   % input frequency
                  0  5000  fs/2 ]; % output frequency
[y,yh,yr] = hpsmodelparams(x,fs,w,2048,-150,200,100,400,1,.2,10,...
                           [],fscale,timbremapping);
% male voice
fscale = 2^(-5/12); % 5 semitones downwards
timbremapping = [ 0  5000  fs/2;   % input frequency
                  0  4000  fs/2 ]; % output frequency
[y2,yh2,yr2] = hpsmodelparams(x,fs,w,2048,-150,200,100,400,1,.2,10,...
                             [],fscale,timbremapping);
% add voices
l = min([length(x) length(y) length(y2) ]);
ysum = x(1:l)+y(1:l)+y2(1:l);