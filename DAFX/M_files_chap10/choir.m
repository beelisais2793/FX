% Authors: J. Bonada, X. Serra, X. Amatriain, A. Loscos
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

[x,fs]=wavread('tedeum.wav');
w=[blackmanharris(2048);0];
dur = length(x)/fs;
fn = ceil(dur/0.2);
fscale = zeros(2,fn);
fscale(1,:) = [0:fn-1]/(fn-1)*dur;
tn = ceil(dur/0.5);
timemapping = zeros(2,tn);
timemapping(1,:) = [0:tn-1]/(tn-1)*dur;
timemapping(2,:) = timemapping(1,:);
ysum = [ x x ]; % make it stereo
for i=1:15 % generate 15 voices
  disp([ ' processing ',num2str(i) ]);
  fscale(2,:) = 2.^(randn(1,fn)*30/1200);
  timemapping(2,2:end-1) = timemapping(1,2:end-1) + ...
                           randn(1,tn-2)*length(x)/fs/tn/6;
  timbremapping = [ 0 1000:1000:fs/2-1000  fs/2;
   0 (1000:1000:fs/2-1000).*(1+.1*randn(1,length(1000:1000:fs/2-1000))) fs/2];    
  [y,yh,yr] = hpsmodelparams(x,fs,w,2048,-150,200,100,400,1,.2,10,...
                             timemapping,fscale,timbremapping);
  pan = max(-1,min(1,randn(1)/3.)); % [0,1]
  l = cos(pan*pi/2);%.^2;
  r = sin(pan*pi/2);%1-l;
  ysum = ysum + [l*y(1:length(x)) r*y(1:length(x))];
end