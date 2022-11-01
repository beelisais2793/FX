function out=psola(in,m,alpha,beta)
% Authors: G. De Poli, U. Zölzer, P. Dutilleux
%     in    input signal
%     m     pitch marks
%     alpha time stretching factor
%     beta  pitch shifting factor
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

P = diff(m);   %compute pitch periods

if m(1)<=P(1), %remove first pitch mark
   m=m(2:length(m)); 
   P=P(2:length(P)); 
end

if m(length(m))+P(length(P))>length(in) %remove last pitch mark
    m=m(1:length(m)-1);
    else
    P=[P P(length(P))]; 
end

Lout=ceil(length(in)*alpha);   
out=zeros(1,Lout); %output signal

tk = P(1)+1;       %output pitch mark

while round(tk)<Lout
  [minimum i] = min( abs(alpha*m - tk) ); %find analysis segment
  pit=P(i); 
  gr = in(m(i)-pit:m(i)+pit) .* hanning(2*pit+1);
  iniGr=round(tk)-pit;      
  endGr=round(tk)+pit;
  if endGr>Lout, break; end
  out(iniGr:endGr) = out(iniGr:endGr)+gr; %overlap new segment
  tk=tk+pit/beta;
end %while