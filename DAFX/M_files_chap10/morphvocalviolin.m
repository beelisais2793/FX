% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[x,fs]=wavread('soprano-E4.wav');
[x2,fs]=wavread('violin-B3.wav');
w=[blackmanharris(1024);0];
f0intp = 0;
htintp = 1;
rintp = 0;
[y,yh,ys] = hpsmodelmorph(x,x2,fs,w,2048,-150,200,100,400,1500,1.5,10,...
                          f0intp,htintp,rintp);