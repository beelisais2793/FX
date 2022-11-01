function f0 = f0detectionyin(x,fs,ws,minf0,maxf0)
% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%==> fundamental frequency detection function
% x: input signal, fs: sampling rate, ws: integration window length
% minf0: minimum f0, maxf0: maximum f0
% f0: fundamental frequency detected in Hz
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

maxlag = ws-2;                            % maximum lag
th = 0.1;                                 % set threshold
d = zeros(maxlag,1);                      % init variable
d2 = zeros(maxlag,1);                     % init variable
% compute d(tau)
x1 = x(1:ws); 
cumsumx = sum(x1.^2);
cumsumxl = cumsumx;
xy = xcorr(x(1:ws*2),x1);
xy = xy(ws*2+1:ws*3-2);
for lag=0:maxlag-1
    d(1+lag) = cumsumx + cumsumxl - 2*xy(1+lag);
    cumsumxl = cumsumxl - x(1+lag).^2 + x(1+lag+ws+1)^2;
end
cumsum = 0;
% compute d'(tau)
d2(1) = 1;
for lag=1:maxlag-1
    cumsum = cumsum + d(1+lag);
    d2(1+lag) = d(1+lag)*lag./cumsum;
end
% limit the search to the target range
minf0lag = 1+round(fs./minf0);            % compute lag corresponding to minf0
maxf0lag = 1+round(fs./maxf0);            % compute lag corresponding to maxf0
if (maxf0lag>1 && maxf0lag<maxlag)
    d2(1:maxf0lag) = 100;                 % avoid lags shorter than maxf0lag
end
if (minf0lag>1 && minf0lag<maxlag)
    d2(minf0lag:end) = 100;               % avoid lags larger than minf0lag
end
% find the best candidate
mloc = 1 + find((d2(2:end-1)<d2(3:end)).*(d2(2:end-1)<d2(1:end-2)));  % minima
candf0lag = 0;
if (length(mloc)>0)
    I = find(d2(mloc)<th);
    if (length(I)>0)
        candf0lag = mloc(I(1));
    else
        [Y,I2] = min(d2(mloc));
        candf0lag = mloc(I2);
    end
    candf0lag = candf0lag;                % this is zero-based indexing    
    if (candf0lag>1 & candf0lag<maxlag)
        % parabolic interpolation
        lval = d2(candf0lag-1); 
        val = d2(candf0lag); 
        rval= d2(candf0lag+1); 
        candf0lag = candf0lag + .5*(lval-rval)./(lval-2*val+rval);  
    end
end        
ac = min(d2); 
f0lag = candf0lag-1;                      % convert to zero-based indexing
f0 = fs./f0lag;                           % compute candidate frequency in Hz
if (ac > 0.2)                             % voiced/unvoiced threshold
    f0 = 0;                               % set to unvoiced
end