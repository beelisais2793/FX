function [iploc, ipmag, ipphase] = peakinterp(mX, pX, ploc)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
% Parabolic interpolation of spectral peaks
% mX: magnitude spectrum, pX: phase spectrum, ploc: locations of peaks
% iploc, ipmag, ipphase: interpolated values
% note that ploc values are assumed to be between 2 and length(mX)-1
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

val = mX(ploc);                                    % magnitude of peak bin
lval = mX(ploc-1);                                 % magnitude of bin at left
rval= mX(ploc+1);                                  % magnitude of bin at right 
iploc = ploc + .5*(lval-rval)./(lval-2*val+rval);  % center of parabola
ipmag = val-.25*(lval-rval).*(iploc-ploc);         % magnitude of peaks
ipphase = interp1(1:length(pX),pX,iploc,'linear'); % phase of peaks 