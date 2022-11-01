% Tromba Marina model
% Stefania bow model together with a mass spring model
clear; clc; close all;
% setup

Fs = 44100;
N = 4*Fs; % nr of samples

% body
[body, fs] = audioread('aguitar.wav'); % random guitar body IR
body = body(:,1);

% string and bow model
freq = 110;
stringLength = floor(Fs/freq);

Pb = 0.1; % bowing point
Fb = 0.15;  % force (N)
Vb = 0.2;  % velocity (m/s)

nutLength = floor(stringLength*(1-Pb));
brigdeLength = floor(stringLength*Pb);
nutDelay = zeros(1,nutLength);
brigdeDelay = zeros(1,brigdeLength);

Z = 0.55;
mus = 0.8; % static coeffient of friction
zslope = 2*Z;
stick = 0;

Von = 0; Vob = 0;
Vin = 0;Vib = 0;
Vh = 0;dVib = 0;dVin = 0;

output = zeros(N,1);
frictionOutput = zeros(N,1);
vOutput = zeros(N,1);


% mass spring

T = 1/Fs;

b = 7;       % Resistance
k = 100000;  % spring stiffness
m = 0.2;        % mass

% calculate k to find a precise mass spring frequency
springFreq = freq; % could be dependent on the string frequency
k = (springFreq*(2*pi) * sqrt(m))^2;

% frequency of spring
f0 = 1/(2*pi) * sqrt(k/m)

% mass spring states
x = [0.,0]';

% input to mass spring
u = 0;
du = 0;

% state space matrices
A = [0, 1; -k/m, -b/m];

B = [0,1/m]';

C = [1,0];

D = 0;

a = 2/T;
I = eye(2);
H = inv(a*I - A);
Ad = H * (a*I + A);
Bd = H * B;

% Impact

fmax = 1;
impactTime = Fs/freq * T;%0.0003;

t=[0:1/Fs:impactTime - 1/Fs];

impactIndex = 1;
doImpact = 0;
impactIsDone = 1;
impactLength = length(t);
di = 1.2;
for ind = 1:impactLength
    %F(ind) = fmax *(1 - cos ((2*pi*t(ind)/T)));
   % F(ind) = tanh(F(ind)*di);
    
end
f1 = linspace(0,1, impactLength/2);
f2 = linspace(1,0, impactLength/2);
F = [f1, f2];
% sample loop

for i = 1:N
    % make input stop to hear how that sounds
    if i > 2*Fs
        Fb = 0;
    end
    
    Vin = nutDelay(nutLength);
    Vib = brigdeDelay(brigdeLength);
    
    LPVib = 0.5*(Vib + dVib);
    dVib = LPVib;
    
    LPVin = 0.5 * (Vin + dVin);
    dVin = LPVin;
    
    Vh = Vin + LPVib;
    
    % Bow friction calculations
    
    B2=-0.2*zslope-0.3*Fb-zslope*Vb-zslope*Vh;
    C2=0.06*Fb+zslope*Vh*Vb+0.2*zslope*Vh+0.3*Vb*Fb+0.1*Fb;
    delta2=B2*B2-4*zslope*C2;
    
    if Vh == Vb % MIDDLE
        
        v = Vb;
        stick = 1;
        
    elseif (delta2 < 0)
        
        v = Vb;    % the only possible solution is the stickone
        stick = 1;
        
    else
        
        if (stick==1)    % if we are in the stick condition, we keep it........
            
            f = zslope*(Vb-Vh);
            
            if ((f <= mus * Fb) && (f > 0))  %.....if we did not find a value above the maximum bow force.
                v = Vb;
            else
                
                v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
                
                vtemp = min(v1, v2);  % we choose the minimum solution, because it is NOT the one in the middle
                
                stick = 0;
                
                if(vtemp > Vb) % we do not want a solution v>Vb
                    v = Vb;
                    stick = 1;
                else
                    v = vtemp;
                end
            end
        else % we are in SLIP, so we stay in SLIP.....
            
            v1 = (-B2+sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v2 = (-B2-sqrt(B2*B2-4*zslope*C2))/(2*zslope);
            v = min(v1,v2);    % we choose the minimum solution, because it is NOT the one in the middle
            
            stick = 0;
            
            if(v > Vb) % on reste en SLIP sauf si v > Vb
                v = Vb;
                stick = 1;
            end
        end
    end
    
    % update outgoing velocities with estimated v
    f = zslope*(v-Vh);
    
    Von = -(LPVib + (f/(2*Z))); % new outgoing velocity to nut
    Vob = -(Vin +  (f/(2*Z)));  % new outgoing velocity to bridge

    % attach mass spring to bow
    % output of spring
    massSpringOutput(i) = x(1);
    threshold = 0.5;%*10e-5 ;
    if (abs(Vob) > threshold && x(1) > threshold * 10e-7 && doImpact == 0)
        doImpact = 1;
        impactIsDone = 0;
       % x(1) = -x(1)*0.5;
        x(2) = -x(2);
        %massSpringOutput(i) = x(1);
    end

    impact = 0;
    if doImpact == 1
        impact = F(impactIndex);       
        impactIndex = impactIndex + 1;
        if impactIndex > impactLength/2
            impactIndex = 1;
            impactIsDone = 1;
            doImpact = 0;
        end
%         impact = 1;%F(impactIndex);       
%        
%             impactIsDone = 1;
%             doImpact = 0;
%    
        
    end
     impactOutput(i) = impact;
     output(i) = Vob ;
     nutDelay = [Von + 0.1*impact, nutDelay(1:nutLength-1)];
     brigdeDelay = [Vob + 0.1*impact, brigdeDelay(1:brigdeLength-1)];
        
    % update spring states
    u = Vob;  
    x = Ad*x + Bd*(u + du);
    du = u;
    
    frictionOutput(i) = f;
    vOutput(i) = v;
end
bodyImpact = conv(impactOutput, body);
outWithBody = conv(output, body);

for i = 1:N
    withImpact(i) = output(i) + 0.5*bodyImpact(i);
end
plot(withImpact)

%figure
%plot(massSpringOutput)

soundsc(withImpact,Fs)

