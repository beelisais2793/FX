% PitchScaleSOLA.m
% Authors: G. De Poli, U. Zölzer, P. Dutilleux
% Parameters:
%    analysis hop size     Sa = 256 (default parmater)		
%    block length          N  = 2048 (default parameter)
%    pitch scaling factor  0.25 <= alpha <= 2 
%    overlap interval      L  = 256*alpha/2
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

Sa=256;N=2048;	          % time scaling parameters
M=ceil(length(DAFx_in)/Sa);

n1=512;n2=256;            % pitch scaling n1/n2
Ss=round(Sa*n1/n2);
L=256*(n1/n2)/2;

DAFx_in(M*Sa+N)=0;
Overlap=DAFx_in(1:N);

% ****** Time Stretching with alpha=n2/n1******
....... % include main loop TimeScaleSOLA.m
% ****** End Time Stretching ******

% ****** Pitch shifting with alpha=n1/n2 ******
lfen=2048;lfen2=lfen/2;
w1=hanningz(lfen);w2=w1;

% for linear interpolation of a grain of length lx to length lfen
lx=floor(lfen*n1/n2);
x=1+(0:lfen-1)'*lx/lfen;
ix=floor(x);ix1=ix+1;
dx=x-ix;dx1=1-dx;
%
lmax=max(lfen,lx);
Overlap=Overlap';
DAFx_out=zeros(length(DAFx_in),1);

pin=0;pout=0;
pend=length(Overlap)-lmax;
%  Pitch shifting by resampling a grain of length lx to length lfen
while pin<pend
  grain2=(Overlap(pin+ix).*dx1+Overlap(pin+ix1).*dx).* w1; 
  DAFx_out(pout+1:pout+lfen)=DAFx_out(pout+1:pout+lfen)+grain2;
  pin=pin+n1;pout=pout+n2;
end;