function magphasresponse(h) 
% Author: U. Zölzer
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

FS=40000;
fosi=10;
if nargin==0
  h=[-.1 .15 .3 .15 -.1];
end
hmax=max(h);
hmin=min(h);
dh=hmax-hmin;
hmax=hmax+.1*dh;
hmin=hmin-.1*dh;

N=length(h);
% denominator polynomial:
a=zeros(1,N);
a(1)=1;

subplot(221)
stem(0:N-1,h)
axis([-1 N, hmin hmax])
title('a) Impulse Response h(n)','Fontsize',fosi);
xlabel('n \rightarrow','Fontsize',fosi)
grid on;

subplot(223)
zplane(h,a)
title('c) Pole/Zero plot','Fontsize',fosi);
xlabel('Re(z)','Fontsize',fosi)
ylabel('Im(z)','Fontsize',fosi)

subplot(222)
[H,F] =freqz(h,a,1024,'whole',FS);
plot(F/1000,abs(H))
xlabel('f in kHz  \rightarrow','Fontsize',fosi);
ylabel('|H(f)| \rightarrow','Fontsize',fosi);
title('b) Magnitude response |H(f)|','Fontsize',fosi);
grid on;

subplot(224)
plot(F/1000,unwrap(angle(H))/pi)
xlabel('f  in kHz \rightarrow','Fontsize',fosi)
ylabel('\angle H(f)/\pi \rightarrow','Fontsize',fosi)
title('d) Phase Response \angle H(f)','Fontsize',fosi);
grid on;