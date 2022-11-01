% UX_cross_synthesis_cepstrum.m   [DAFXbook, 2nd ed., chapter 8]
% ==== This function performs a cross-synthesis with cepstrum
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear all; close all

%----- user data -----
% [DAFx_sou, SR] = wavread('didge_court.wav');  % sound 1: source/excitation
% DAFx_env       = wavread('la.wav');           % sound 2: spectral enveloppe
[DAFx_sou, SR] = wavread('moore_guitar.wav'); % sound 1: source/excitation
DAFx_env  = wavread('toms_diner.wav');        % sound 2: spectral enveloppe
s_win     = 1024;   % window size
n1        = 256;    % step increment
order_sou = 30;     % cut quefrency for sound 1
order_env = 30;     % cut quefrency for sound 2
r         = 0.99;   % sound output normalizing ratio

%----- initialisations -----
w1          = hanning(s_win, 'periodic');  % analysis window
w2          = w1;               % synthesis window
hs_win      = s_win/2;          % half window size
grain_sou   = zeros(s_win,1);   % grain for extracting source
grain_env   = zeros(s_win,1);   % grain for extracting spec. enveloppe
pin         = 0;                % start index
L           = min(length(DAFx_sou),length(DAFx_env));
pend        = L - s_win;        % end index
DAFx_sou    = [zeros(s_win, 1); DAFx_sou; ...
  zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_sou));
DAFx_env    = [zeros(s_win, 1); DAFx_env; ...
  zeros(s_win-mod(L,n1),1)] / max(abs(DAFx_env));
DAFx_out    = zeros(L,1);

%----- cross synthesis -----
while pin<pend
  grain_sou = DAFx_sou(pin+1:pin+s_win).* w1;
  grain_env = DAFx_env(pin+1:pin+s_win).* w1;
  %===========================================
  f_sou     = fft(grain_sou);             % FT of source
  f_env     = fft(grain_env)/hs_win;      % FT of filter
  %---- computing cepstrum ----
  flog      = log(0.00001+abs(f_env));    
  cep       = ifft(flog);                 % cepstrum of sound 2
  %---- liftering cepstrum ----
  cep_cut   = zeros(s_win,1);
  cep_cut(1:order_sou) = [cep(1)/2; cep(2:order_sou)];
  flog_cut  = 2*real(fft(cep_cut));
  %---- computing spectral enveloppe ----
  f_env_out = exp(flog_cut);              % spectral shape of sound 2
  grain     = (real(ifft(f_sou.*f_env_out))).*w2; % resynthesis grain
  % ===========================================
  DAFx_out(pin+1:pin+s_win) = DAFx_out(pin+1:pin+s_win) + grain;
  pin       = pin + n1;
end

%----- listening and saving the output -----
% DAFx_in = DAFx_in(s_win+1:s_win+L);
DAFx_out = DAFx_out(s_win+1:length(DAFx_out)) / max(abs(DAFx_out));
soundsc(DAFx_out, SR);
DAFx_out_norm = r * DAFx_out/max(abs(DAFx_out)); % scale for wav output
wavwrite(DAFx_out_norm, SR, 'CrossCepstrum')