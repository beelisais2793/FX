% virtualloudspeaker.m
% Author: V. Pulkki, T. Lokki
% Virtual playback of 5.0 surround signal over headphones using HRIRs
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100;
% generate example 5.0 surround signal
cnt=[0:20000]';
signal=[(mod(cnt,200)/200) (mod(cnt,150)/150) (mod(cnt,120)/120)...
    (mod(cnt,90)/90) (mod(cnt,77)/77)];
i=1;
% go through the input channels
outsigL=0; outsigR=0;
for theta=[30 -30 -110 110 0]
  HRIRl=simpleHRIR(theta,Fs);
  HRIRr=simpleHRIR(-theta,Fs);
  outsigL=outsigL+conv(HRIRl,signal(:,i));
  outsigR=outsigR+conv(HRIRr,signal(:,i));
  i=i+1;
end
% sound output to headphones
soundsc([outsigL outsigR],Fs) 