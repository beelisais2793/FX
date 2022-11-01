function [g,fr,Q] = wahcontrols(wah)
% function [g,fr,Q] = wahcontrols(wah)
% Authors: Välimäki, Bilbao, Smith, Abel, Pakarinen, Berners
%
% Parameter: wah = wah-pedal-angle normalized to lie between 0 and 1
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

g  = 0.1*4^wah;       % overall gain for second-order s-plane transfer funct.
fr = 450*2^(2.3*wah); % resonance frequency (Hz) for the same transfer funct.
Q  = 2^(2*(1-wah)+1); % resonance quality factor for the same transfer funct.