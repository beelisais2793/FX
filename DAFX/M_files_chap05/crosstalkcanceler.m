% crosstalkcanceler.m
% Author: A. Politis, V. Pulkki
% Simplified cross-talk canceler 
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

theta=10;  % spacing of stereo loudspeakers in azimuth 
Fs= 44100; % sample rate
b=10^-5;   % regularization factor
% loudspeaker HRIRs for both ears (ear_num,loudspeaker_num)
% If more realistic HRIRs are available, pls use them
HRIRs(1,1,:)=simpleHRIR(theta/2,Fs); 
HRIRs(1,2,:)=simpleHRIR(-theta/2,Fs); 
HRIRs(2,1,:)=HRIRs(1,2,:);
HRIRs(2,2,:)=HRIRs(1,1,:);
Nh=length(HRIRs(1,1,:));
%transfer to frequency domain
for i=1:2;for j=1:2
        C_f(i,j,:)=fft(HRIRs(i,j,:),Nh)
    end;end
% Regularized inversion of matrix C
H_f=zeros(2,2,Nh);
for k=1:Nh
    H_f(:,:,k)=inv((C_f(:,:,k)'*C_f(:,:,k)+eye(2)*b))*C_f(:,:,k)';
end
% Moving back to time domain
for k=1:2; for m=1:2
        H_n(k,m,:)=real(ifft(H_f(k,m,:)));
        H_n(k,m,:)=fftshift(H_n(k,m,:));
    end; end
% Generate binaural signals.  Any binaural recording shoud also be ok
binauralsignal=simplehrtfconv(70);
%binauralsignal=wavread('road_binaural.wav');
% Convolve the loudspeaker signals
loudspsig=[conv(reshape(H_n(1,1,:),Nh,1),binauralsignal(:,1)) + ...
    conv(reshape(H_n(1,2,:),Nh,1),binauralsignal(:,2)) ...
    conv(reshape(H_n(2,1,:),Nh,1),binauralsignal(:,1)) + ...
    conv(reshape(H_n(2,2,:),Nh,1),binauralsignal(:,2))];
soundsc(loudspsig,Fs)       % play sound for loudspeakers