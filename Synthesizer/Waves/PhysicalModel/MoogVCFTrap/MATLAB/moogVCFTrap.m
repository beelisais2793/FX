%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
% Moog VCF Model (Trapezoidal Approximation)
% 
% Author: Chad McKell
% Date: 27.03.17
%
% Description: Virtual analog model of the Moog VCF. This script uses the
% trapezoidal integration rule to approximate the impulse response of the
% filter. The approximated transfer function is then compared with the
% exact calculation of the Moog VCF transfer function.
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
tic; close; clc; clear;

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
% Define global variables
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

SR = 44100; % sample rate [samples/sec]
k = 1/SR; % time step [sec]
f0 = 1000; % resonant frequency [Hz]
w0 = 2*pi*f0; % angular frequency [rad/sec]
r = 0.5; % tuning parameter (a number between 0 and 1) 
Tf = 3; % total time length of simulation [sec]
Nf = floor(Tf*SR); % total sample length of simulation [samples]
A = w0*[-1 0 0 -4*r; 1 -1 0 0; 0 1 -1 0; 0 0 1 -1]; % system matrix
b = w0*[1; 0; 0; 0]; % forcing vector
c = [0; 0; 0; 1]; % state mixture vector

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
% Error handling: Terminate code for the following errors
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

% r is out of bounds
if r > 1 || r < 0
   error('r must be a number in the range [0,1].')
end

% SR has a non-zero decimal
if rem(SR,1) ~= 0
   error('SR must be zero-decimal.')
end

% SR, f0, Tf, or r is not a number
if ~isfloat(SR) || ~isfloat(f0) || ~isfloat(Tf) || ~isfloat(r) 
   error('SR, f0, Tf, and r must each be a number.')
end

% SR, f0, Tf, or r is not real
if ~isreal(SR) || ~isreal(f0) || ~isreal(Tf) || ~isreal(r)   
   error('SR, f0, Tf, and r must each be real.')
end

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
%  Initialize the input sequence
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
u = zeros(Nf,1); 
u(1) = 1;

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
%  Implement trapezoidal integration method
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

% Initialize state vectors
xTrap = zeros(4,1); % current time step
xTrap1 = zeros(4,1); % one time step back

% Initialize output vector
yTrap = zeros(Nf,1);

% Compute coefficients
coef1 = eye(4) + k*A/2;
coef2 = eye(4) - k*A/2;

for n = 1:Nf
   
   % Main algorithm
   d = coef1*xTrap1 +k*coef1*b*u(n);
   xTrap = coef2\d;
   yTrap(n) = c'*xTrap;
   
   % Set value of xBackward1 equal to next grid line
   xTrap1 = xTrap;
end

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
%  Compute transfer function of trapezoidal output signal
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

% Calculate Fourier transforms
U = fft(u);
YTrap = fft(yTrap);

% Compute transfer functions
HTrap = U./YTrap;

% Convert to loglog form
HTrapLog = log(HTrap) - max(log(HTrap));

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
%  Compute exact transfer function
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

fmax = 22500;
fbin = 0:fmax/1000:fmax;
fnum = length(fbin);
Hexact = zeros(fnum,1);

for n = 1:fnum
    inv = (2*pi*1i*fbin(n)*eye(4)-A)\b;
    Hexact(n) = c'*inv;
end

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
% Define plotting parameters 
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

n = 0:Nf-1; % length bins [samples]
t = (n/SR)'; % time bins [sec]
fk = n'*SR/Nf; % frequency bins [Hz]
nyquist = SR/2; % Nyquist frequency [Hz]
font = 14; % font size for plots

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
% Generate plot of transfer functions
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%

figure('units','normalized','outerposition',[0 0 1 1]);
orient landscape
plot(fk, abs(HTrapLog), 'LineWidth', 2);
hold on
plot(fbin, real(log(Hexact)), 'LineWidth', 2);
xlim([0 nyquist]);
title('Transfer Functions of Moog VCF Ladder Filter');
xlabel('Frequency (Hz)'); 
ylabel('Magnitude');
legend('Trapezoidal', 'Exact')
set(gca,'fontsize',font)

%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
% Check code efficiency
%~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~%
toc % print elapsed time


