% delaypan.m
% Author: V. Pulkki, T. Lokki
% Creating spatially spread virtual source by delaying one channel
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100;
% Delay parameter for channel 1 in seconds
delay=0.005; 
% Corresponding number of delayed samples
delaysamp=round(delay*Fs)
% Signal to be used
signal=mod([1:20000]',400)/400; 
signal(1:2000)=signal(1:2000).*[1:2000]'/2000; % Fade in
% Delaying first channel
loudsp_sig=[[zeros(delaysamp,1); signal(1:end-delaysamp)] signal];
% Play audio with loudspeakers
soundsc(loudsp_sig,Fs);