% UX_pitch_pv_move.m   [DAFXbook, 2nd ed., chapter 7]
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
n1     = 512;  % analysis hop size
               % try n1=400 (pitch down) or 150 (pitch up)
n2     = 256;  % synthesis hop size
               % keep it a divisor of s_win (256 is pretty good)
s_win  = 2048; % window length
order  = 50;   % cut quefrency
coef   = 0.99;   % sound output normalizing ratio

%----- initializations -----
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;   % synthesis window
tscal    = n2/n1;  % time-scaling ratio
s_win2   = s_win/2;
L        = length(DAFx_in);
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in)); % 0-pad + norm
%-- for phase unwrapping
omega    = 2*pi*n1*[0:s_win-1]'/s_win;
phi0     = zeros(s_win,1);
psi      = zeros(s_win,1);
%-- for linear interpolation of a grain of length s_win
lx       = floor(s_win*n1/n2);
DAFx_out = zeros(lx+length(DAFx_in),1);
x        = 1 + (0:lx-1)'*s_win/lx;
ix       = floor(x);
ix1      = ix + 1;
dx       = x - ix;
dx1      = 1 - dx;
warp     = n1/n2   % warpinf coefficient, = 1/tscal
t        = 1 + floor((0:s_win-1)*warp);
lmax     = max(s_win,t(s_win))

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin    = 0;
pout   = 0;
pend   = L - lmax;

while pin<pend
   grain   = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
   f       = fft(fftshift(grain));
   r       = abs(f);
   phi     = angle(f);
   %---- unwrapping the phase ----
   delta_phi = omega + princarg(phi-phi0-omega); 
   phi0    = phi;
   psi     = princarg(psi+delta_phi*tscal);
   %---- moving formant ----
   grain1  = DAFx_in(pin+t) .* w1;
   f1      = fft(grain1)/s_win2;
   flog    = log(0.00001+abs(f1))-log(0.00001+abs(f));
   cep     = ifft(flog);
   cep_cut = [cep(1)/2; cep(2:order); zeros(s_win-order,1)];
   corr    = exp(2*real(fft(cep_cut))); % correction enveloppe
   %---- spec env modif.: computing output FT and grain ----
   ft      = (r.* corr.* exp(i*psi));
   grain   = fftshift(real(ifft(ft))).*w2;
   %---- pitch-shifting: interpolating output grain -----
   grain2  = [grain;0];
   grain3  = grain2(ix).*dx1+grain2(ix1).*dx;
   % plot(grain);drawnow;
%===========================================
   DAFx_out(pout+1:pout+lx) = DAFx_out(pout+1:pout+lx) + grain3;
   pin     = pin + n1;
   pout    = pout + n1;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in  = DAFx_in(s_win+1:s_win+L);
DAFx_out = coef * DAFx_out(s_win+1:s_win+L) / max(abs(DAFx_out));
soundsc(DAFx_out, SR);
wavwrite(DAFx_out, SR, 'la_pitch_pv_move.wav');