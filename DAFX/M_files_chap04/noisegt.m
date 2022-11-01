function y=noisegt(x,holdtime,ltrhold,utrhold,release,attack,a,Fs)
% function y=noisegt(x,holdtime,ltrhold,utrhold,release,attack,a,Fs)
% Author: R. Bendiksen

% noise gate with hysteresis
% holdtime	- time in seconds the sound level has to be below the 
%		      threshhold value before the gate is activated
% ltrhold	- threshold value for activating the gate
% utrhold	- threshold value for deactivating the gate > ltrhold
% release	- time in seconds before the sound level reaches zero
% attack	- time in seconds before the output sound level is the 
%		      same as the input level after deactivating the gate
% a		- pole placement of the envelope detecting filter <1
% Fs 		- sampling frequency
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

rel=round(release*Fs);  %number of samples for fade 
att=round(attack*Fs);   %number of samples for fade 
g=zeros(size(x));
lthcnt=0;
uthcnt=0;
ht=round(holdtime*Fs);
h=filter([(1-a)^2],[1.0000 -2*a a^2],abs(x));%envelope detection 
h=h/max(h);
for i=1:length(h)
  if (h(i)<=ltrhold) | ((h(i)<utrhold) & (lthcnt>0))
% Value below the lower threshold?
    lthcnt=lthcnt+1;		
    uthcnt=0;
    if lthcnt>ht			
% Time below the lower threshold longer than the hold time?
      if lthcnt>(rel+ht) 
        g(i)=0;
      else
	g(i)=1-(lthcnt-ht)/rel;		% fades the signal to zero
      end;
    elseif ((i<ht) & (lthcnt==i))
      g(i)=0;
    else
      g(i)=1;
    end;
  elseif (h(i)>=utrhold) | ((h(i)>ltrhold) & (uthcnt>0))	
% Value above the upper threshold or is the signal being faded in?
    uthcnt=uthcnt+1;
    if (g(i-1)<1)				
% Has the gate been activated or isn't the signal faded in yet?
      g(i)=max(uthcnt/att,g(i-1));
    else
      g(i)=1;
    end;
    lthcnt=0;
  else
    g(i)=g(i-1);
    lthcnt=0;
    uthcnt=0;
  end; 
end;
y=x.*g;
y=y*max(abs(x))/max(abs(y));