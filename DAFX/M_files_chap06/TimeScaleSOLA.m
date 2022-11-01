% TimeScaleSOLA.m
% Authors: U. Zölzer, G. De Poli, P. Dutilleux
% Time Scaling with Synchronized Overlap and Add
%
% Parameters:
%
% analysis hop size     Sa = 256 (default parameter)		
% block length          N  = 2048 (default parameter)
% time scaling factor   0.25 <= alpha <= 2 
% overlap interval      L  = 256*alpha/2
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear all,close all

[signal,Fs]	=	wavread('x1.wav');
DAFx_in		=	signal';

% Parameters:
Sa	  =	256;    % Sa must be less than N
N     = 2048;   
alpha = 1;      % 0.25 <= alpha <= 2 
Ss	  = round(Sa*alpha);     
L	    = 128;    % L must be chosen to be less than N-Ss 

% Segmentation into blocks of length N every Sa samples
% leads to M segments
M     =	ceil(length(DAFx_in)/Sa);

DAFx_in(M*Sa+N)=0;
Overlap  =  DAFx_in(1:N);

% **** Main TimeScaleSOLA loop ****
for ni=1:M-1
  grain=DAFx_in(ni*Sa+1:N+ni*Sa);
  XCORRsegment=xcorr(grain(1:L),Overlap(1,ni*Ss:ni*Ss+(L-1)));		
  [xmax(ni),km(ni)]=max(XCORRsegment);

  fadeout=1:(-1/(length(Overlap)-(ni*Ss-(L-1)+km(ni)-1))):0;
  fadein=0:(1/(length(Overlap)-(ni*Ss-(L-1)+km(ni)-1))):1;
  Tail=Overlap(1,(ni*Ss-(L-1))+ ...
           km(ni)-1:length(Overlap)).*fadeout;
  Begin=grain(1:length(fadein)).*fadein;
  Add=Tail+Begin;
  Overlap=[Overlap(1,1:ni*Ss-L+km(ni)-1) ...
           Add grain(length(fadein)+1:N)];
end;
% **** end TimeScaleSOLA loop ****
% Output in WAV file	
sound(Overlap,44100);
wavwrite(Overlap,Fs,'x1_time_stretch');	