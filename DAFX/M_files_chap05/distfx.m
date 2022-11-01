% distfx.m
% Author: V. Pulkki, T. Lokki
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

h = filter([0.5,0.5],1, ...
    random('norm',0,1,1,lenh).*exp(-[1:lenh]*0.01/distwall)/100);
    % reverb impulse response
offset = 100;
st = Fs/2;

for i = 1:1:distwall-1 % several distances listener-source
  del1 = floor(i/c*Fs);
  del2 = floor((distwall*2 - i)/c*Fs);
  y(i*st+1:i*st+del1) = zeros(1,del1);
  y(i*st+del1+1:i*st+del1+length(x)) = x./(1+i); % direct signal 
  w(i*st+del2+1:i*st+del2+length(x)) = ...
         y(i*st+del2+1:i*st+del2+length(x)) + ...
         x./(1+(2*distwall-i));    % direct signal + echo
  z(i*st+del2+1:i*st+del2+length(x)+lenh-1+offset) = ...
         y(i*st+del2+1:i*st+del2+length(x)+lenh-1+offset) + ...
         [zeros(1,offset),conv(x,h)]./sqrt(1+i); 
                                   % direct signal + delayed reverb
end