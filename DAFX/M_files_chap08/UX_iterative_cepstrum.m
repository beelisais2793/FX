% function [env,source] = iterative_cepstrum(FT,NF,order,eps,niter,Delta)
% [DAFXbook, 2nd ed., chapter 8]
% ==== This function computes the spectral enveloppe using the iterative 
%      cepstrum method
% Inputs:
%   - FT [NF*1 | complex]     Fourier transform X(NF,k)
%   - NF [1*1 | int]          number of frequency bins
%   - order [1*1 | float]     cepstrum truncation order
%   - eps [1*1 | float]       bias
%   - niter [1*1 | int]       maximum number of iterations
%   - Delta [1*1 | float]     spectral envelope difference threshold
% Outputs:
%   - env [NFx1 | float]      magnitude of spectral enveloppe
%   - source [NFx1 | complex] complex source
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

function [env,source] = iterative_cepstrum(FT,NF,order,eps,niter,Delta)

%---- use data ----
fig_plot = 0;

%---- drawing ----
if(fig_plot), SR = 44100; freq = (0:NF-1)/NF*SR; figure(3); clf; end

%---- initializing ----
Ep = FT;

%---- computing iterative cepstrum ----
for k=1:niter
  flog     = log(max(eps,abs(Ep)));
  cep      = ifft(flog);      % computes the cepstrum
  cep_cut  = [cep(1)/2; cep(2:order); zeros(NF-order,1)];
  flog_cut = 2*real(fft(cep_cut));
  env      = exp(flog_cut);   % extracts the spectral shape
  Ep       = max(env, Ep);    % get new spectrum for next iteration
  %---- drawing ----
  if(fig_plot)
    figure(3); % clf  %uncomment to not superimpose curves
    subplot(2,1,1); hold on
    plot(freq, 20*log10(abs(FT)), 'k')
    h = plot(freq, 20*log10(abs(env)), 'k');
    set(h, 'Linewidth', 1, 'Color', 0.5*[1 1 1])
    xlabel('f / Hz \rightarrow'); ylabel('\rho(f) / d \rightarrow')
    title('Original spectrum and its enveloppe')
    axis tight; ax = axis; axis([0 SR/5 ax(3:4)])
    drawnow
  end
  %---- convergence criterion ----
  if(max(abs(Ep)) <= Delta), break; end
end

%---- computing source from enveloppe ----
source = FT ./ env;