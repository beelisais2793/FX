% File: adaptive_Hopf.m
% Purpose: Matlab script for testing and tuning the Adaptive Frequency Oscillator Arduino program.
% Version: 1.1.0
% Date: 10-01-2020
% Created by: Martin Stokroos
% URL: https://github.com/MartinStokroos/adaptiveFreqOsc
% License: MIT License

% The adaptive Hopf frequency oscillator model was taken from:
% A.J.Ijspeert, Adaptive Frequency Oscillators, URL: https://www.epfl.ch/labs/biorob/research/neuromechanical/page-36365-en-html
%
% This m-file is not an example of efficient matlab programming. It was written with the purpose to develop C-code.

clear all;
close all;
clc;

f_init = 10.0;
f_step = 10.1;

Fs=3000; %Sampling frequency
Tfinal=3.0; %s simulation time
T=1/Fs; %Sampling Time = 1/Fs
t=linspace(0,Tfinal,Tfinal/T); %Simulation Time vector

plotvar1 = linspace(0,0,Tfinal/T);
plotvar2 = linspace(0,0,Tfinal/T);
plotvar4 = linspace(0,0,Tfinal/T);
x = 1;  % starting point of the oscillator. x=1 and y=0 starts as cosine. 
x_new = 0;
x_d = 0;
y = 0;
y_new = 0;
y_d = 0;
ohm = 2*pi*f_init
ohm_new = 0;
ohm_d = 0;

Eps = 20; % coupling strength (>0).
       % Locks faster for a bigger value, but the ripple in frequency is larger. 

gamma = 10.0; % follows the amplitude of the stimulus better for high values
           % may cause oscillations for high values  

mu = 5.0; % seems to scale the amplitude.


for n=1:Tfinal/T
    
    %stimulus
    if n < Tfinal/(10*T) % apply step at 1/10th of the simulation time
        F = 0;
        %F = sin(2*pi*n*T);
    end
    if (n >= Tfinal/(10*T)) && (n < Tfinal/(10*T/9))
        %F = 0;
        F = sin(f_step*2*pi*n*T);
    end
    if (n >= Tfinal/(10*T/9))
        F = 0;
    end

    % discretized adaptive Hopf
    x_d = gamma*( mu-(x^2 + y^2) )*x - ohm*y + Eps*F;
    y_d = gamma*( mu-(x^2 + y^2) )*y + ohm*x;
    ohm_d = -Eps * F * y/( sqrt(x^2 + y^2) );
    x_new = x + T*x_d;
    y_new = y + T*y_d;
    ohm_new = ohm + T*ohm_d;

    plotvar1(n) = F;
    plotvar2(n) = x;
    plotvar3(n) = y;
    plotvar4(n) = ohm/(2*pi); %frequency
    
    x = x_new;
    y = y_new;
    ohm = ohm_new;
end


figure;
plot(t,plotvar1,'r', t,plotvar2,'b',  t,plotvar3,'c'), title('red: excitation, blue: output I, cyan: output Q - as function of time[s]');
grid on
figure;
plot(t, plotvar4), title('blue: output frequency [Hz] as function of time[s]');
grid on

