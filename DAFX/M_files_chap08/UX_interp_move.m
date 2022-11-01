% UX_interp_move.m   [DAFXbook, 2nd ed., chapter 8]
% ==== This function performs a ptch-shifting that preserves
%      the spectral enveloppe 
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; 

%----- user data -----
[DAFx_in, SR] = wavread('la.wav'); % sound file
n1     = 400;  % analysis hop size
               % try n1=400 (pitch down) or 150 (pitch up)
n2     = 256;  % synthesis hop size
               % keep it a divisor of s_win (256 is pretty good)
s_win  = 2048; % window length
order  = 50;   % cut quefrency
coef   = 0.99; % sound output normalizing ratio

%----- initializations -----
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;      % synthesis window
tscal    = n2/n1    % time-scaling ratio
hs_win   = s_win/2; % half window size
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in)); % 0-pad + norm
%-- for linear interpolation of a grain of length s_win
lx       = floor(s_win*n1/n2);
DAFx_out = zeros(ceil(tscal*length(DAFx_in)),1);
x        = 1 + (0:s_win-1)'*lx/s_win;
ix       = floor(x);
ix1      = ix + 1;
dx       = x - ix;
dx1      = 1 - dx;
warp     = n1/n2   % warpinf coefficient, = 1/tscal
lmax     = max(s_win,lx)

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = L - lmax;

while pin<pend
%===========================================
   %----- FT of interpolated grain
   grain1  = (DAFx_in(pin+ix).*dx1 + DAFx_in(pin+ix1).*dx).* w1;
   f1      = fft(grain1)/hs_win;
   %----- FT of reference grain, for formant matching
   grain2  = DAFx_in(pin+1:pin+s_win).* w1;
   f2      = fft(grain2)/hs_win;
   %----- correction factor for spectral enveloppe
   flog    = log(0.00001+abs(f2))-log(0.00001+abs(f1));
   cep     = ifft(flog);
   cep_cut = [cep(1)/2; cep(2:order); zeros(s_win-order,1)];
   corr    = exp(2*real(fft(cep_cut)));
   %----- so now make the formant move
   grain   = fftshift(real(ifft(f1.*corr))).*w2;
   % plot(grain);drawnow;
%===========================================
   DAFx_out(pout+1:pout+s_win) = DAFx_out(pout+1:pout+s_win) + grain;
   pin     = pin + n1;
   pout    = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in = DAFx_in(s_win+1:s_win+L);
DAFx_out = coef * DAFx_out(s_win+1:length(DAFx_out)) / max(abs(DAFx_out));
soundsc(DAFx_out, SR);
wavwrite(DAFx_out, SR, 'la_interp_move.wav');