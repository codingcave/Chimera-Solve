#!/home/kekstoaster/Programming/C++/NetworkSimulation/bin/Debug/run.sh

print(#argv, "::", argv[0], argv[1])

-- load all modules and create them with parameters
FitzHughNagumo = dynamics("FitzHughNagumo")(1.0, .7)
rk = integrator("RungeKutta")(FitzHughNagumo, 0.01)
file = output("file.csv")("test.csv")

-- initial condition
vec1 = vector(2, 0.0)
vec1[0] = 1.0

-- new simulation
sim = simulation(rk, vec1)

-- register output on every step
sim.onstep(file)

-- start the simulation
x = sim.start()

-- last step
print(x)

