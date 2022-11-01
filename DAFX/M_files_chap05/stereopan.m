% stereopan.m
% Author: V. Pulkki, T. Lokki
% Stereophonic panning example with tangent law
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100;
theta=-20; % Panning direction
% Half of opening angle of loudspeaker pair
lsbase=30;
% Moving to radians
theta=theta/180*pi;
lsbase=lsbase/180*pi;
% Computing gain factors with tangent law
g(2)=1; % initial value has to be one
g(1)=- (tan(theta)-tan(lsbase)) / (tan(theta)+tan(lsbase)+eps);
% Normalizing the sum-of-squares
g=g/sqrt(sum(g.^2));
% Signal to be panned
signal=mod([1:20000]',200)/200;
% Actual panning
loudsp_sig=[signal*g(1) signal*g(2)];
% Play audio out with two loudspeakers
soundsc(loudsp_sig,Fs);