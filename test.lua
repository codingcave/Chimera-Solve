--print(#argv, "::", argv[0], argv[1])

-- Random Module to generate uniform numbers frm -5 to 5
rnd = random("UniformRandom")(-5, 5)
-- Initializer, using the random instace, for initilizing start conditions
rndInit = init("RandomInitializer")(rnd)
-- load all modules and create them with parameters
oscillator = dynamics("FitzHughNagumo")(1.0, .7)
--oscillator = dynamics("HarmonicOscillator")(4.0, .7)

-- integrator for the system
rk = integrator("RungeKutta")(oscillator, 0.01)
-- output module: CSV file
-- first item time
-- each following 2 columns correspond to the i-th node
file = output("file_csv")("test.csv")

-- load network, load coupling EntryPoint
netw = dynamics("Network")

-- use nearest neighbour coupling, strength 1, 3 nodes left and right
-- no self coupling
nnc = coupling("NearestNeighbourCoupling")(1, 3)

--
netw = netw(oscillator, 50, nnc)
rk = integrator("RungeKutta")(netw, 0.01)

-- new simulation
sim = simulation(rk, rndInit)

-- register output on every step
sim.onstep(file)

-- start the simulation
x = sim.start(100)

-- last step
print(x)

