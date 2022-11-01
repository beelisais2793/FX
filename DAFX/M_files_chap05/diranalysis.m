% diranalysis.m 
% Author: V. Pulkki
% Example of directional analysis of simulated B-format recording
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------

Fs=44100; % Generate signals
sig1=2*(mod([1:Fs]',40)/80-0.5) .* min(1,max(0,(mod([1:Fs]',Fs/5)-Fs/10)));
sig2=2*(mod([1:Fs]',32)/72-0.5) .* min(1,max(0,(mod([[1:Fs]+Fs/6]',Fs/3)-Fs/6)));
% Simulate two sources in directions of -45 and 30 degrees
w=(sig1+sig2)/sqrt(2);
x=sig1*cos(50/180*pi)+sig2*cos(-170/180*pi);
y=sig1*sin(50/180*pi)+sig2*sin(-170/180*pi);
% Add fading in diffuse noise with  36 sources evenly in the horizontal plane
for dir=0:10:350
    noise=(rand(Fs,1)-0.5).*(10.^((([1:Fs]'/Fs)-1)*2));
    w=w+noise/sqrt(2);
    x=x+noise*cos(dir/180*pi);
    y=y+noise*sin(dir/180*pi);
end
hopsize=256; % Do directional analysis with STFT
winsize=512; i=2; alpha=1./(0.02*Fs/winsize);
Intens=zeros(hopsize,2)+eps; Energy=zeros(hopsize,2)+eps;
for time=1:hopsize:(length(x)-winsize)
    % moving to frequency domain
    W=fft(w(time:(time+winsize-1)).*hanning(winsize));
    X=fft(x(time:(time+winsize-1)).*hanning(winsize));
    Y=fft(y(time:(time+winsize-1)).*hanning(winsize));
    W=W(1:hopsize);X=X(1:hopsize);Y=Y(1:hopsize);
    %Intensity computation
    tempInt = real(conj(W) * [1 1 ] .* [X Y])/sqrt(2);%Instantaneous
    Intens = tempInt * alpha + Intens * (1 - alpha); %Smoothed
    % Compute direction from intensity vector
    Azimuth(:,i) = round(atan2(Intens(:,2), Intens(:,1))*(180/pi));
    %Energy computation
    tempEn=0.5 * (sum(abs([X Y]).^2, 2) * 0.5 + abs(W).^2 + eps);%Inst
    Energy(:,i) = tempEn*alpha + Energy(:,(i-1)) * (1-alpha); %Smoothed
    %Diffuseness computation
    Diffuseness(:,i) = 1 - sqrt(sum(Intens.^2,2)) ./ (Energy(:,i));
    i=i+1;
end
% Plot variables
figure(1); imagesc(log(Energy)); title('Energy');set(gca,'YDir','normal')
xlabel('Time frame'); ylabel('Freq bin');
figure(2); imagesc(Azimuth);colorbar; set(gca,'YDir','normal')
title('Azimuth'); xlabel('Time frame'); ylabel('Freq bin');
figure(3); imagesc(Diffuseness);colorbar; set(gca,'YDir','normal')
title('Diffuseness'); xlabel('Time frame'); ylabel('Freq bin');