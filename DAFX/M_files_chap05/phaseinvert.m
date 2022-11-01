% phaseinvert.m
% Author: V. Pulkki, T. Lokki
% Create a spread virtual source by inverting phase in one loudspeaker
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100;
signal=mod([1:20000]',400)/400; %signal to be used
% Inverting one loudspeaker signal
loudsp_sig=[-signal signal];
% Play audio out with two loudspeakers
soundsc(loudsp_sig,Fs);