function Automatic_Mixing_Framework()
% function Automatic_Mixing_Framework()
% Author: E. Perez-Gonzalez, J. Reiss
%
%--------------------------------------------------------------------------
% This source code is provided without any warranties as published in 
% DAFX book 2nd edition, copyright Wiley & Sons 2011, available at 
% http://www.dafx.de. It may be used for educational purposes and not 
% for commercial applications without further permission.
%--------------------------------------------------------------------------


%---AUDIO INPUT for 8 Mono Files, where x{m} is the input to channel m.
[x{1},Fs]=wavread('x1.wav'); %Read file
% ... 
[x{8},Fs]=wavread('x8.wav'); %Read file

%---RECORD FILE BEFORE AUTOMIXING
monoInputSum = 0;
for m=1:length(x) %Mono summing buss
    monoInputSum=monoInputSum + x{1};
end
monoInputSum=monoInputSum *.125; %Mono summing buss scaling
monoInputSumStereo=(repmat(monoInputSum*(1/sqrt(2)),1,2));%Split to Stereo
wavwrite(monoInputSumStereo,Fs,'preAutoMixSum.wav');

%---SIDE CHAIN
tr=0.002;  %%Fixed Threshold
[cv]=LoudnessSideChain_at_Fs44100(x,tr); %Side Chain

%---PROCESSING
[yL,yR]=LoudnessProcessing(x,cv); %Fader Gain

%---RECORD AUDIO OUTPUT
wavwrite([yL yR],Fs,'postAutoMixSum.wav'); %Record file after automixing

%==============================================================
function [cv]=LoudnessSideChain_at_Fs44100(x,tr)
%% LOUDNESS SIDE CHAIN FUNCTION %%%
cv=[0.5 0.5 0.5 0.5 0.5 0.5 0.5 0.5]; %Initial value

%--Noise removal
for m = 1:length(x)
    xg{m}=x{m}(x{m}>tr); %Gate
end

%---Obtain feature
for m=1:length(x)
    [xg{m}]=Loudness95dB_at_Fs44100(xg{m});
    clear peakA;
end

%---Accumulative feature processing
for m=1:length(x)
    max_vector(m)= max(xg{m});
end
[max_xg,argmax_xg]=max(max_vector);

for m=1:length(x)
    xg{m}=xg{m}/max_xg; %normalize
end

figure(1); %Figure showing accumulated loudness values per channel
for m=1:length(x)
    subplot(2,4,m)
    [maxhist,maxhist_arg]=max(hist(xg{m}));%Calc. max and maxarg of hist
    [num,histout]=hist(xg{m});%Calculate histogram
    bar(histout,num)%Plot histogram
    axis([0 1 0 maxhist+1])
    hold on;
    %Calculate most probable loudness per channel
    fv(m)=(maxhist_arg*(max(xg{m})+min(xg{m})))/length(hist(xg{m}));
    plot (fv(m),maxhist,'ro')%Plot most probable loudness
    hold off;
    clear maxhist maxhist_arg num histout xg{m};
end

%---CROSS ADAPTIVE PROCESSING
l=mean(fv); %obtain average Loudness
for m=1:length(x)
    cva(m)=l/fv(m); %compensate for average loudness
end

%---Unity gain normalisation to maintain system stability
cvasum=sum(cva); %obtain total non-nomalized
for m=1:length(x)
    cv(m)=cva(m)/cvasum; %normalize for cvasum
end

%Print Loudness, control variables and gain
Feature_Loudness=[fv(1) fv(2) fv(3) fv(4) fv(5) fv(6) fv(7) fv(8)]
Control_variables=[cv(1) cv(2) cv(3) cv(4) cv(5) cv(6) cv(7) cv(8)]
Overal_gain=sum(cv) %overal gain equals 1

%==============================================================
function [yL,yR]=LoudnessProcessing(x,cv)
%---AUDIO OUTPUT for 8 Mono Files, where y{m} is the output to channel m.
%---Fader GAIN PROCESSING
for m=1:length(x)
    y{m}=x{m}*cv(m);
    clear x{m}
end;

%---Split mono results to stereo for channels 1 to 8
yL=0; %Left summing bus initialisation
yR=0; %Right summing bus initialisation

for m=1:length(y)
    yL=yL + y{m}*(1/sqrt(2)); %Scale to split mono to stereo
    yR=yR + y{m}*(1/sqrt(2)); %Scale to split mono to stereo
    clear y{m};
end

%==============================================================
function [out]=Loudness95dB_at_Fs44100(in)%% LOUDNESS FEATURE EXTRACTION
%---Biquad Filter no.1 HPF
B = [1.176506 -2.353012 1.176506]; A = [1 -1.960601 0.961086]; 
in= filter(B,A,in);

%---Biquad Filter no.2 Peak Filter
B = [0.951539 -1.746297 0.845694]; A = [1 -1.746297 0.797233]; 
in= filter(B,A,in);

%---Biquad Filter no.3 Peak Filter
B = [1.032534 -1.42493 0.601922]; A = [1 -1.42493 0.634455];
in= filter(B,A,in);

%---Biquad Filter no.4 Peak Filter
B = [0.546949 -0.189981 0.349394]; A = [1 -0.189981 -0.103657]; 
in= filter(B,A,in);

%---Peak averaging
S=20000; %Frame size for peak averaging
cumin=[zeros(S,1); cumsum(in)];
avin=(cumin((S+1):end)-(cumin(1:(end-S))))/S; % Calculate running average
clear cumin;
Six = (S+1):S:(length(avin));% Times at wich peak amp will be returned
peakA=nan(size(Six));% Create vector holding peaks
for i=1:length(Six)% Calculete peak average
    Si = Six(i);
    peakA(i)=max(abs(avin((Si-S):Si)));%Output peak averaging
end

out=peakA;