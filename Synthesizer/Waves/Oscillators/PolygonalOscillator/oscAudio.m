%% synthesis

% core parameters
a = 4; % schlalfi numerator
b = 1; % schalfi denominator (a > 2b)
n = a/b; % order (schlalfi symbol)
f0 = 440; % f0
T = 0.0; % teeth
phaseOffset = pi/5; % initial phase
R = 0.9; % scale

fs = 44100;
dPhase = 2*pi*f0*(1/fs); % phase increment
sizeP = fs/f0*b; % period size in samples

buffSize = 256;
nFrames = 10;

theta = 0; % init phase with offset, for radial amplitude calculation

p = zeros(1, buffSize); % radial amplitude of geometry
poly = zeros(1, buffSize); % sampled geometry
waveform = []; % projection

for i=1:nFrames

    for j=1:buffSize % geometry
        p(j) = cos(pi/n) / cos(2*pi/n * mod((theta+phaseOffset)*n/(2*pi), 1) -pi/n + T) * R;
        
 
        poly(j) = p(j) * (cos(theta) + 1j*sin(theta));
        theta = theta+dPhase;
    end
    
    waveform = [waveform imag(poly)]; % projection to y axis
end

pDraw = zeros(1, buffSize);
theta = 0;
for i=1:buffSize % geometry
    pDraw(i) = cos(pi/n) / cos(mod(theta+phaseOffset, 2*pi/n) -pi/n + T) * R;
    pDraw(i) = pDraw(i) * (cos(theta) + 1j*sin(theta));
    
    theta = theta + (2*pi*b/buffSize);
end

%% anti aliasing

waveformAA = waveform; % anti-aliased waveform

dx = diff(waveform,1); % first derivative of the waveform
discSlope = diff(waveform,2); % second derivative
discSlope = [0, discSlope]; % offset
    
if(T == 0) % this method only works when the teeth parameter is excluded

    nDisc = floor(length(waveform)/sizeP*a);
    disc = zeros(1, nDisc); % location of discontinuities expressed in samples
    discPhase = zeros(1, nDisc); % location of discontinuities expressed in radians
 
    u = zeros(1, nDisc); % change in amplitude at discontinuities

    for k=1:nDisc % iterate through the discontinuities in the first period
    
        disc(k) = fs/(n*f0)*k - fs/f0/(2*pi/phaseOffset)+1;
        discPhase(k) = 2*pi/n*k;
   
        % boundary samples
        n3 = ceil(disc(k));
        n1 = n3-2;
        n2 = n1+1;
        n4 = n3+1;
    
        d = n3 - disc(k); % fractional delay between the discontinuity and the next sample 
    
        u(k) = abs(discSlope(n2)+ (disc(k)-n2) * ((discSlope(n3)-discSlope(n2)) / (n3-n2)) ) *2;
    
        % 4-point polyBLAMP residual coefficients
        p0 = d^5/120;
        p1 = (-3*d^5 +5*d^4 +10*d^3 +10*d^2 +5*d +1)/120;
        p2 = (3*d^5 -10*d^4 +40*d^2 -60*d +28)/120;
        p3 = (-d^5 +5*d^4 -10*d^3 +10*d^2 -5*d +1)/120;
    
        % waveform correction on the four samples around the discontinuity
        waveformAA(n1) = waveform(n1) -p0*u(k) *sign(waveform(n1));
        waveformAA(n2) = waveform(n2) -p1*u(k) *sign(waveform(n2));
        waveformAA(n3) = waveform(n3) -p2*u(k) *sign(waveform(n3)); 
        waveformAA(n4) = waveform(n4) -p3*u(k) *sign(waveform(n4));
    end

end
%% plot

subplot(2,2,1);
plot(real(pDraw), imag(pDraw), 'r');
axis([min(real(pDraw))-0.1 max(real(pDraw))+0.1 min(imag(pDraw))-0.1 max(imag(pDraw))+0.1]);
axis equal;
title('Complex plane');

subplot(2,2,3);
graph1 = plot(waveform, 'b');
axis([1 sizeP min(waveform) max(waveform)]); % display the first period only
hold on;
graph2 = plot(dx, '--');
hold on;
graph3 = plot(waveformAA, '-.m');
hold on;
graph4 = plot(discSlope, 'g');
for i=1:nDisc
    line([disc(i),disc(i)], [-1,1],'Color','red','LineStyle',':');
end
legend([graph1, graph2, graph3, graph4],{'Original waveform', 'Derivative', 'Anti-aliased waveform', 'Second derivative'});
title('Projection');

% magnitude spectrum comparison
subplot(2,2,2);
plot(db(abs(fft(waveform, fs))), 'k');
axis([0 fs/2 -80 80]);
title('Original waveform');

subplot(2,2,4);
plot(db(abs(fft(waveformAA, fs))), 'k');
axis([0 fs/2 -80 80]);
title('Anti-aliased waveform');

%% SNR

magSpec = abs(fft(waveform, fs));
magSpec = magSpec(2:length(magSpec)); % drop dc
dFreq = length(magSpec)/fs; % frequency resolution

fH = zeros(1, 20); % frequency of the first k harmonics (Hz)
for k=1:length(fH) % sum of the energy of the first k harmonics
    fH(k) = f0*(2*floor(k/2)+1+(n-2)*(1+floor((k-1)/2))); 
end
fH = [f0, fH]; % fundamental + harmonics

bwH = 34; % estimated bandwidth for the fundamental and harmonics (main lobe only)

% extract magnitude from fft
eSig = 0;
for i=1:length(fH)
    for j=0:bwH
        eSig = eSig + magSpec(fH(i)-bwH/2+j)^2; % energy of the fundamental + harmonics
    end
end

% calculate individual bins
% dftBin = zeros(1, length(fH)); % dft of fundamental and harmonics
% for i=1:length(dftBin)
%     for n=1:length(waveform)
%         dftBin(i) = dftBin(i) + waveform(n)*exp(-1i*2*pi*n*(fH(i)/fs));
%     end
% end
% eSig = sum(abs(dftBin).^2);


eNoise = sum(magSpec(1:ceil(length(magSpec)/2)).^2) - eSig; % energy of the noise

snr = 20*log10(eSig / eNoise);
disp('SNR: ');disp(snr);

%% output

soundsc(waveformAA, fs);