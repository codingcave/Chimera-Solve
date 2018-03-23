#!/home/kekstoaster/Programming/C++/NetworkSimulation/bin/Debug/run.sh

PI = 3.141592654
delta_w = 0.09
N = 50
sigma = 1
alpha = 0.3 * PI
beta = -0.53 * PI
epsilon = 1e-2

print("Start new simulation")
print("Delta:", delta_w)
print("N:", N)
print("Sigma:", sigma)
print("Alpha:", alpha / PI, "π")
print("Beta:", beta / PI, "π")
print("Epsilon:", epsilon)

-- uniform random distribution for initialization
rndX1 = random("UniformRandom"){min=-1, max=1}
rnd2Pi = random("UniformRandom"){min=0, max=2*PI}
rndDelta = random("UniformRandom"){min=-delta_w, max=delta_w}

-- function for initialization
-- i - number or oszillator
-- j - element of sozillator
function sysInit(i, j)
    local nx
    if j == 0 then
        nx = rnd2Pi.next()
    else
        nx = rndX1.next()
    end
    return nx
end

-- Initializer, using the random distributions, for initilizing start conditions
-- first element omega has different distibution than other elements
funcInit = init("FunctionInitializer")(sysInit)

-- Module for Kuramoto oszillators
kuramoto = dynamics("Kuramoto")

-- output module: CSV file
-- first item time
-- each following 2 columns correspond to the i-th node
file = output("file_csv")("data.csv")

-- load network, load coupling EntryPoint
network = dynamics("Network")

-- use Kuramoto coupling, for Kuramoto oszillators
kc = coupling("KuramotoCoupling"){sigma=sigma, alpha=alpha, beta=beta, epsilon=epsilon}

-- create a new network of N elements, with Kuramoto coupling
-- initialize all N osizllators with random omega from rndDelta
netw = network{N=N, coupling=kc, oscillator=kuramoto, init={omega = rndDelta, N=N}}

-- new Runge Kutta integrator
rk = integrator("RungeKutta"){system=netw, h=0.05}

-- new simulation, initialized with sysInit function above
sim = simulation(rk, funcInit)

-- create an observer starting at 49000 time units, record every full unit that is calculated
evt1 = observer("TimedObserver"){start=49000, step=1}
-- observe this event on this simulation
sim.observe(evt1)

-- register CSV output on every step, consumes a lot of resources
--sim.onstep(file)

-- register CSV output for every full unit after starting time units have passed
sim.onelapsed(file)

-- start the simulation, for 51000 time units
x = sim.start(50001)

-- print last step
print(x)
