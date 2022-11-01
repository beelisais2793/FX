function waterfspec(signal,start,steps,N,fS,clippingpoint,baseplane)
% Authors: J. Schattschneider, U. Zölzer
% waterfspec( signal, start, steps, N, fS, clippingpoint, baseplane)
%
% shows short-time spectra of signal, starting
% at k=start, with increments of STEP with N-point FFT 
% dynamic range from -baseplane in dB up to 20*log(clippingpoint)
% in dB versus time axis
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

echo off;
if nargin<7, baseplane=-100; end
if nargin<6, clippingpoint=0; end
if nargin<5, fS=48000; end
if nargin<4, N=1024; end         % default FFT
if nargin<3, steps=round(length(signal)/25); end
if nargin<2, start=0; end

windoo=blackman(N);              % window - default
windoo=windoo*N/sum(windoo);     % scaling
% Calculation of number of spectra nos
  n=length(signal);
  rest=n-start-N;
  nos=round(rest/steps);
  if nos>rest/steps, nos=nos-1; end
% vectors for 3D representation
  x=linspace(0, fS/1000 ,N+1);
  z=x-x;
  cup=z+clippingpoint;
  cdown=z+baseplane;
  
  signal=signal+0.0000001;  
% Computation of spectra and visual representation
  for i=1:1:nos,
    spek1=20.*log10(abs(fft(windoo.*signal(1+start+...
        i*steps:start+N+i*steps)))./(N)/0.5);
    spek=[-200 ; spek1(1:N)];
    spek=(spek>cup').*cup'+(spek<=cup').*spek;
    spek=(spek<cdown').*cdown'+(spek>=cdown').*spek;
    spek(1)=baseplane-10;
    spek(N/2)=baseplane-10;
    y=x-x+(i-1);
    if i==1, 
       p=plot3(x(1:N/2),y(1:N/2),spek(1:N/2),'k');
       set(p,'Linewidth',0.1);
    end
       pp=patch(x(1:N/2),y(1:N/2),spek(1:N/2),'w','Visible','on');
       set(pp,'Linewidth',0.1);
   end;
set(gca,'DrawMode','fast');
axis([-0.3 fS/2000+0.3 0 nos baseplane-10 0]);
set(gca,'Ydir','reverse');
title('Waterfall Representation of Short-time FFTs');
xlabel('f in Hz \rightarrow');
ylabel('n \rightarrow');zlabel('Magnitude in dB \rightarrow');
view(12,40);