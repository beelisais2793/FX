%% wavetable

tableSize = 256;
%waveTable = zeros(1, tableSize);

% core parameters
N = 3; % schlalfi numerator
np = 1; % schalfi denominator (number of periods)
n = N/np; % order (schlalfi symbol) (n>2)
f0 = 80/np; % f0
T = 0.0; % teeth
phaseOffset = pi / 2; % initial phase
R = 1; % scale


t = [0:tableSize-1]/tableSize; % time vector
theta = 2*pi*t*np; % phase angle
p = zeros(1, tableSize); % radial amplitude of geometry
 
for i=1:tableSize % geometry
    p(i) = cos(pi/n) / cos(mod(theta(i)+phaseOffset, 2*pi/n) -pi/n + T) * R; % (modified Eq1) 
end

poly = zeros(1, tableSize); % sampled geometry
for i=1:tableSize
    poly(i) = p(i) * (cos(theta(i)) + 1j*sin(theta(i)));
end

waveTable = imag(poly); % projection to y axis

%% anti aliasing

dx = diff(waveTable); % first derivative of the waveform
dx = dx / max(abs(dx)); % normalisation
dx = [dx dx(1)]; % wrap around

waveTableAA = waveTable; % anti-aliased waveform
disc = zeros(1, N); % location of discontinuities expressed in samples

for k=1:N % iterates through discontinuities
    disc(k) = mod(tableSize/N*k+1 - tableSize/np/(2*pi / phaseOffset), tableSize);
    
    % boundary samples (wrapped around the wavetable)
    n3 = mod(ceil(disc(k)), tableSize);
    n1 = mod(n3-2+tableSize, tableSize);
    n2 = n1+1; %mod(n3-1+tableSize, tableSize);
    n4 = mod(n3+1+tableSize, tableSize);

    d = mod(n3 - disc(k), tableSize); % fractional delay between the discontinuity and the next sample (Eq.5)

    %u = abs(dx(n2)-dx(n3)); % change in amplitude at the discontinuities
    %u = (-sin(2*pi/N*k)*cos(pi/N)+sin(pi/N)*cos(2*pi/N*k) / cos(pi/n)) - (-sin(2*pi/N*k)*cos(pi/N)-sin(pi/N)*cos(2*pi/N*k) / cos(pi/n));
    u = abs(-2*tan(pi/N)*cos(2*pi/N*k)); % change in amplitude at the discontinuities (Eq.11)
    
    % 4-point polyBLAMP residual coefficients (Table.1)
    p0 = d^5/120;
    p1 = (-3*d^5 +5*d^4 +10*d^3 +10*d^2 +5*d +1)/120;
    p2 = (3*d^5 -10*d^4 +40*d^2 -60*d +28)/120;
    p3 = (-d^5 +5*d^4 -10*d^3 +10*d^2 -5*d +1)/120;
    
    % waveform correction on the four samples around the discontinuity
    waveTableAA(n1) = waveTable(n1) -p0*u *sign(waveTable(n1));
    waveTableAA(n2) = waveTable(n2) -p1*u *sign(waveTable(n2));
    waveTableAA(n3) = waveTable(n3) -p2*u *sign(waveTable(n3)); 
    waveTableAA(n4) = waveTable(n4) -p3*u *sign(waveTable(n4));
    
end

%% plot

subplot(2,1,1);
plot(real(poly), imag(poly), 'r');
axis([-1 1 -1 1]);
axis equal;
title('Complex plane');

subplot(2,1,2);
graph1 = plot(waveTable, 'b');
axis([0 tableSize -1 1]);
hold on;
graph2 = plot(dx, '--');
hold on;
graph3 = plot(waveTableAA, '-.m');
for i=1:N
    line([disc(i),disc(i)], [-1,1],'Color','red','LineStyle',':');
end
legend([graph1, graph2, graph3],{'Waveform', 'Derivative', 'Anti-aliased waveform'});
title('Projection');
%% synthesis

duration = 1; 
fs = 44100;
tableOverSamplingRatio = tableSize / fs;
tableDelta = f0 * tableOverSamplingRatio; % read increment for wavetable
readIndex = 1; % table read index

y = zeros(1, fs*duration); % output


for i=1:length(y) % synhtesis from wavetable
    
    % boundary indices and corresponding values
    i1 = floor(readIndex+1); 
    if i1 == tableSize
        i2 = 1;
    else
        i2 = i1+1;
    end
    v1 = waveTable(i1);
    v2 = waveTable(i2);
  
    frac = readIndex - i1; % sample fraction
    
    y(i) = v2 + (frac*(v2-v1)); %interpolation
    
    
    readIndex = readIndex + tableDelta;
    if readIndex > tableSize
        readIndex = readIndex-tableSize;
    end
    
end

%soundsc(y, fs);
figure();
plot(y);

audioDiscNum = fs*duration/(fs/f0)*N; % number of discontinuities in the audio signal
discAudio = zeros(1, audioDiscNum); % location of discontinuities in the audio signal expressed in samples

for i=1:audioDiscNum
    discAudio(i) = fs/(f0*N)*i - fs/f0/np/(2*pi / phaseOffset);
end

for i=1:audioDiscNum
    line([discAudio(i),discAudio(i)], [-1,1],'Color','red','LineStyle',':');
end

