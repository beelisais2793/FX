%-----male to child-----%
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[x,fs] = wavread('basket.wav');
w=[blackmanharris(1024);0];
fscale = 2;
timbremapping = [ 0  3600  fs/2;   % input frequency
                  0  5000  fs/2 ]; % output frequency
[y,yh,yr] = hpsmodelparams(x,fs,w,2048,-150,200,100,400,1,.2,10,...
                           [],fscale,timbremapping);   