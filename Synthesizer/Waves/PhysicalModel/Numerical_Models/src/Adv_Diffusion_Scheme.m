% 
% 
% Alex Greenhalgh
% 	MATLAB program for Advection-Diffusion Scheme capable of modeling both if correct parameters set to zero
% 
clear all,close all;
Headerlines = 1;
delimiterin = ' ';
fname = 'dat.txt';
data = importdata(fname,delimiterin,Headerlines);

%Initialize variables
MM = data.data(1);
tend = data.data(2);
dtout = data.data(3);
factor = data.data(4);
D = data.data(5);
a = data.data(6);
b = data.data(7);
v = 0;
dx = 1.0 / MM; 
M = (b-a) * MM;
%compensating for matlab
M = M + 1;
x = MESH( M, a, b, dx);

t0     = 0;
u_0    = 1;


% // tnstep CFL condition
dtEXPL = dx*dx/((v*dx) + (2*D));
dt     = factor*dtEXPL;
Nend   = floor((tend-t0)/dt) ;
Nend   = Nend + 1; 
nsteps = 0;
tn     = 0.0;
 
%Initialize
U = INIT( M, D,dx,tn,x);
trapz_vec(1) =  trapz(U(:)) * dx;

ERR = 0.0;
tout = max( [dtout dt] );

uEXACT(1) = u_0;
for i=1:length(x)
    uEXACT(i) = 0.0;
end
figure(1)
plot(x(:), U(:))
hold on
counter = 2;

filename = 'Lab8_Output.txt';
fid = fopen(filename,'w'); 
OUTPUT(fid, M,  tn,  nsteps, U, x);
%  Begin tnstepping
for i=1:Nend
    tn = i*dt;
 
    F = FLUX( M, U, D, dx, b,tn,v);
    U = PDE(U, F, dt, dx, M);
	if (tn >= tout)
        plot(x(:), U(:))
        OUTPUT(fid, M,  tn,  nsteps, U, x);
        tout = tout + dtout;
        trapz_vec(counter) = trapz(U(:)) * dx;
        counter = counter + 1;
    end
    nsteps = nsteps+1;
    
    
end

if(tn >= tend)
    fprintf(fid,' DONE, at tn= %f  nsteps= %f \n',tn, nsteps);
else
    fprintf(fid,'... out of tnsteps: need bigger Nend\n');
end

legend('T0','Tout', 'Tend')
axis([a b 0 5.2])
xlabel('Space [x]')
ylabel('Concentration [U]')

% Account for the flux's contribution to the concentration with increasing
% time
function U = PDE( U,  F, dt,  dx, M)
    %impermeable bc's
    U(1) = 0;
    U(M+1) = 0;
    for i=2:M
        U(i) = U(i) + (dt / dx) * (F(i) - F(i+1));
    end
end

% Solve for the flux's
function F = FLUX( M, U, D, dx, b,tn,v)
    
    %impermeable bc's
	F(1) = 0;
    F(M+1) = 0; 
    
    %Addition of the Flux terms for the total flux
	for i=2:M
        if (v >= 0)
            F(i) = v*U(i-1) - (D * (U(i) - U(i-1)) / dx);
        else
            F(i) = v*U(i) - (D * (U(i) - U(i-1)) / dx); 
        end
    end

		
end


%Format output to file 
function OUTPUT(fid,M, tn, nsteps,U,x)
    fprintf(fid,'# Profile at tn: %f  nsteps= %f \n',tn, nsteps);

    %fprintf(fid,'# Error up to this tn: %.8f \n',ERR);
    for i=1:length(x)
        fprintf(fid, '%f %f\n', x(i), U(i));
    end
end

%Initialize the concentration vector at time 0
function U = INIT(M, D, dx, tn, x)
	for i=1:length(x)
        if (x(i) >= 1 && x(i) <= 2)
            U(i) = 5.0;
        else
            U(i) = 0.0;
        end
    end

end

%Discretization of the 1D space
function x = MESH(M, a, b, dx)
    x(1) = a;
    x(2) =  x(1) + (dx / 2);
    
	for i=3:M
		x(i) = x(2) + ((i - 2) * dx);
    end
    
    x(M+1) = b;

end


