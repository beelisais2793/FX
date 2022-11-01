% UX_fomove_cepstrum.m   [DAFXbook, 2nd ed., chapter 8]
% ==== This function performs a formant warping with cepstrum
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; clf;

%----- user data -----
fig_plot = 0;     % use any value except 0 or [] to plot figures
[DAFx_in, SR] = wavread('la.wav'); % sound file
warping_coef  = 2.0
n1       = 512;    % analysis hop size
n2       = n1;     % synthesis hop size
s_win    = 2048;   % window length
order    = 50;     % cut quefrency
r        = 0.99;   % sound output normalizing ratio

%----- initializations -----
w1       = hanning(s_win,  'periodic'); % analysis window
w2       = w1;              % synthesis window
hs_win   = s_win/2;         % half window size
L        = length(DAFx_in); % signal length
DAFx_in  = [zeros(s_win, 1); DAFx_in; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in)); % 0-pad + normalize
DAFx_out = zeros(L,1);
x0       = floor(min((1+(0:hs_win)/warping_coef), 1+hs_win)); 
         % apply the warping
x        = [x0, x0(hs_win:-1:2)];% symmetric extension

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = L - s_win;

while pin<pend
   grain = DAFx_in(pin+1:pin+s_win).* w1;
%===========================================
   f     = fft(grain)/hs_win;
   flog  = log(0.00001+abs(f));
   cep   = ifft(flog);
   cep_cut   = [cep(1)/2; cep(2:order); zeros(s_win-order,1)];
   %---- flog_cut1|2 =  spectral shapes before/after formant move
   flog_cut1 = 2*real(fft(cep_cut));
   flog_cut2 = flog_cut1(x);
   corr  = exp(flog_cut2-flog_cut1);
   grain = (real(ifft(f.*corr))).*w2;
% ===========================================
   DAFx_out(pout+1:pout+s_win) = DAFx_out(pout+1:pout+s_win) + grain;
   pin   = pin + n1;
   pout  = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
soundsc(DAFx_out, SR);
DAFx_out_norm = r * DAFx_out/max(abs(DAFx_out)); % scale for wav output
wavwrite(DAFx_out_norm, SR, 'la_fomove.wav');