#!/home/kekstoaster/Programming/C++/NetworkSimulation/bin/Debug/run.sh
--print(#argv, "::", argv[0], argv[1])

-- Random Module to generate uniform numbers frm -5 to 5
rnd = random("UniformRandom"){min=-5, max=5}

-- Initializer, using the random instace, for initilizing start conditions
--rndInit = init("RandomInitializer")(rnd)
rndInit = init("RandomInitializer"){engine=rnd}

-- load all modules and create them with parameters
--oscillator = dynamics("FitzHughNagumo"){a = 1.0, epsilon=0.7}
--oscillator = dynamics("HarmonicOscillator")(4.0, .7)
oscillator = dynamics("HarmonicOscillator"){a=4.0, b=.7}
-- integrator for the system
--rk = integrator("RungeKutta")(oscillator, 0.01)
-- output module: CSV file
-- first item time
-- each following 2 columns correspond to the i-th node
file = output("file_csv")("test.csv")

-- load network, load coupling EntryPoint
network = dynamics("Network")

-- use nearest neighbour coupling, strength 1, 3 nodes left and right
-- no self coupling
nnc = coupling("NearestNeighbourCoupling"){sigma=1, R=2}

--netw = network(oscillator, 50, nnc)
netw = network{N=50, coupling=nnc, oscillator=oscillator}

--rk = integrator("RungeKutta")(netw, 0.01)
rk = integrator("RungeKutta"){system=netw, h=0.01}

-- new simulation
sim = simulation(rk, rndInit)

-- register output on every step
sim.onstep(file)

-- start the simulation
x = sim.start(20)

-- last step
print(x)
