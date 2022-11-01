function out=psolaF(in,m,alpha,beta,gamma)
% Authors: G. De Poli, U. Zölzer, P. Dutilleux
%     . . .
%     gamma newFormantFreq/oldFormantFreq
%     . . .
%     the internal loop as
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

tk = P(1)+1;                  %output pitch mark
while round(tk)<Lout
 [minimum i]=min(abs(alpha*m-tk) );    % find analysis segment
 pit=P(i);pitStr=floor(pit/gamma);	
 gr=in(m(i)-pit:m(i)+pit).*hanning(2*pit+1);
 gr=interp1(-pit:1:pit,gr,-pitStr*gamma:gamma:pit);% stretch segm.
 iniGr=round(tk)-pitStr;endGr=round(tk)+pitStr;
 if endGr>Lout, break; end
 out(iniGr:endGr)=out(iniGr:endGr)+gr; % overlap new segment
 tk=tk+pit/beta;
end % end of while