% UX_cross_synthesis_LPC.m   [DAFXbook, 2nd ed., chapter 8]
% ==== This function performs a cross-synthesis with LPC
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

clear;

%----- user data -----
[DAFx_in_sou, FS] = wavread('moore_guitar.wav');  % sound 1: source/excitation
DAFx_in_env   = wavread('toms_diner.wav');        % sound 2: spectral env.
long          = 400;        % block length for calculation of coefficients
hopsize       = 160;        % hop size (is 160)
env_order     = 20          % order of the LPC for source signal
source_order  = 6           % order of the LPC for excitation signal
r             = 0.99;       % sound output normalizing ratio

%----- initializations -----
ly = min(length(DAFx_in_sou), length(DAFx_in_env));
DAFx_in_sou = [zeros(env_order, 1); DAFx_in_sou; ...
  zeros(env_order-mod(ly,hopsize),1)] / max(abs(DAFx_in_sou));
DAFx_in_env = [zeros(env_order, 1); DAFx_in_env; ...
  zeros(env_order-mod(ly,hopsize),1)] / max(abs(DAFx_in_env));
DAFx_out = zeros(ly,1);     % result sound
exc      = zeros(ly,1);     % excitation sound
w        = hanning(long, 'periodic');   % window
N_frames = floor((ly-env_order-long)/hopsize); % number of frames

%----- Perform ross-synthesis -----
tic
for j=1:N_frames
  k        = env_order + hopsize*(j-1);     % offset of the buffer
  %!!! IMPORTANT: function "lpc" does not give correct results for MATLAB 6 !!!
  [A_env, g_env]  = calc_lpc(DAFx_in_env(k+1:k+long).*w, env_order);
  [A_sou, g_sou]  = calc_lpc(DAFx_in_sou(k+1:k+long).*w, source_order);
  gain(j) = g_env;
  ae      = - A_env(2:env_order+1); % LPC coeff. of excitation
  for n=1:hopsize
    excitation1   = (A_sou/g_sou) * DAFx_in_sou(k+n:-1:k+n-source_order);
    exc(k+n)      = excitation1;
    DAFx_out(k+n) = ae * DAFx_out(k+n-1:-1:k+n-env_order)+g_env*excitation1;
  end
end
toc

%----- playing and saving output signal -----
DAFx_out      = DAFx_out(env_order+1:length(DAFx_out)) / max(abs(DAFx_out));
soundsc(DAFx_out, FS)
DAFx_out_norm = r * DAFx_out/max(abs(DAFx_out)); % scale for wav output
wavwrite(DAFx_out_norm, FS, 'CrossLPC')