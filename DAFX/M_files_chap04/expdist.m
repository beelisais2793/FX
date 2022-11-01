function y=expdist(x, gain, mix)
% function y=expdist(x, gain, mix)
% Author: Bendiksen, Dutilleux, Zölzer, Dempwolf
% y=expdist(x, gain, mix)
% Distortion based on an exponential function
% x    - input
% gain - amount of distortion, >0
% mix  - mix of original and distorted sound, 1=only distorted
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

q=x*gain;
z=sign(q).*(1-exp(-abs(q)));
y=mix*z+(1-mix)*x;