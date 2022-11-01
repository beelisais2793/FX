% UX_spectral_interp.m   [DAFXbook, 2nd ed., chapter 8]
% ==== This function performs a spectral interpolation with cepstrum
%
% k: spectral mix, calculated at every step in this example, as
%    starts with gain=0 for sound 1 and gain=1 for sound 2
%    finishes with gain=1 for sound 1 and gain=0 for sound 2
%    so we move from sound 1 to sound 2
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear; 

%----- user data -----
[DAFx_in1,SR] = wavread('claire_oubli_voix.WAV');  % sound 1 
DAFx_in2 = wavread('claire_oubli_flute.WAV'); % sound 2
n1       = 512;            % analysis hop size
n2       = n1;             % synthesis hop size
s_win    = 2048;           % window length
w1       = hanning(s_win, 'periodic'); % analysis window
w2       = w1;             % synthesis window
cut      = 50              % cut-off quefrency

%----- initializations -----
L        = min(length(DAFx_in1), length(DAFx_in2));
DAFx_in1 = [zeros(s_win, 1); DAFx_in1; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in1)); % 0-pad + norm
DAFx_in2 = [zeros(s_win, 1); DAFx_in2; ...
   zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_in2)); % 0-pad + norm
DAFx_out = zeros(length(DAFx_in1),1);

tic
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
pin  = 0;
pout = 0;
pend = L - s_win;
while pin<pend
   %---- k factor (spectral mix) wich varies between 0 and 1
   k      = pin / pend;
   kp     = 1 - k;
   %---- extracting input grains 
   grain1 = DAFx_in1(pin+1:pin+s_win) .* w1;
   grain2 = DAFx_in2(pin+1:pin+s_win) .* w1;
%===========================================
   %---- computing spectral shape of sound 1
   f1    = fft(fftshift(grain1));
   flog  = log(0.00001+abs(f1));
   cep   = fft(flog);
   cep_coupe   = [cep(1)/2; cep(2:cut); zeros(s_win-cut,1)];
   flog_coupe1 = 2*real(ifft(cep_coupe));
   spec1 = exp(flog_coupe1);
   %---- computing spectral shape of sound 2
   f2    = fft(fftshift(grain2));
   flog  = log(0.00001+abs(f2));
   cep   = fft(flog);
   cep_coupe   = [cep(1)/2; cep(2:cut); zeros(s_win-cut,1)];
   flog_coupe2 = 2*real(ifft(cep_coupe));
   spec2 = exp(flog_coupe2);
   %----- interpolating the spectral shapes in dBs
   spec  = exp(kp*flog_coupe1+k*flog_coupe2);
   %----- computing the output spectrum and grain
   ft    = (kp*f1./spec1+k*f2./spec2).*spec;
   grain = fftshift(real(ifft(ft))).*w2;
%===========================================
   DAFx_out(pout+1:pout+s_win) = DAFx_out(pout+1:pout+s_win) + grain;
   pin   = pin + n1;
   pout  = pout + n2;
end
%UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU
toc

%----- listening and saving the output -----
% DAFx_in = DAFx_in1(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:s_win+L) / max(abs(DAFx_out));
soundsc(DAFx_out, SR);
wavwrite(DAFx_out, SR, 'spec_interp.wav');