% kappa.m
% Author: V. Pulkki, T. Lokki
% Simple example of cardioid decoding of B-format signals
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100;
% mono signal
signal=(mod([0:Fs*2],220)/220); 
% Simulated horizontal-only B-format recording of single 
% sound source in direction of theta azimuth.
% This can be replaced with a real B-format recording. 
theta=0;
w=signal/sqrt(2);
x=signal*cos(theta/180*pi);
y=signal*sin(theta/180*pi);
% Virtual microphone directions 
% (In many cases the values equal to the directions of loudspeakers)
ls_dir=[30 90 150 -150 -90 -30]/180*pi; 
ls_num=length(ls_dir);
% Compute virtual cardioids (kappa = 1) out of the B-format signal
kappa=1;
for i=1:ls_num
    LSsignal(:,i)=(2-kappa)/2*w...
        +kappa/(2*sqrt(2))*(cos(ls_dir(i))*x+sin(ls_dir(i))*y);
end
% File output
wavwrite(LSsignal,Fs,16,'firstorderB-formatexample.wav')