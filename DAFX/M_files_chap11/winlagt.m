function sfw=winlagt(s,b,Nw,L)
% Author: G. Evangelista
% Frequency warping via STLT of the signal s with parameter b,
% output window length Nw and time-shift L
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

w=L*(1-cos(2*pi*(0:Nw-1)/Nw))/Nw;      % normalized Hanning window
N=ceil(Nw*(1-b)/(1+b));                % length of unwarped window h
M=round(L*(1-b)/(1+b));                % time-domain window shift
h=lagtun(w,-b,N); h=h(:)               % unwarped window
Ls=length(s);                          % pad signal with zeros
K=ceil((Ls-N)/M);                      % to fit an entire number
s=s(:); s=[s ; zeros(N+K\ast M-Ls,1)]; % of windows
Ti=1; To=1;                            % initialize I/O pointers
Q=ceil(N*(1+abs(b))/(1-abs(b)));       % length of Laguerre transform
sfw=zeros(Q,1);                        % initialize output signal
for k=1:K
 yy=lagt(s(Ti:Ti+N-1).*h,b,Q);         % Short-time Laguerre transf.
 sfw(To:end)=sfw(To:end)+yy;           % overlap-add STLT
 Ti=Ti+M;To=To+L;                      % advance I/O signal pointers
 sfw=[sfw; zeros(L,1)];                % zero pad for overlap-add
end