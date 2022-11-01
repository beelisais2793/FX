% spreadnoise.m
% Author: V. Pulkki, T. Lokki
% Example how to spread a virtual source over N loudspeakers
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100;
signal=mod([1:20000]',400)/400; % Signal to be used
NChan=2; % Number of channels
% Generate noise bursts for all channels
nois=rand(round(0.05*Fs),NChan)-0.5;
% Convolve signal with bursts
loudsp_sig=conv(signal,nois(:,1));
for i=2:NChan
    loudsp_sig=[loudsp_sig conv(signal,nois(:,i))];
end
if NChan == 2
    % Play audio out with  loudspeakers
    soundsc(loudsp_sig,Fs);
else 
    % Write file to disk
    loudsp_sig=loudsp_sig/max(max(loudsp_sig))*0.9;
    wavwrite([loudsp_sig],Fs,16,'burstex.wav');
end