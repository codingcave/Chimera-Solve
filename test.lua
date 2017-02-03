#!/home/kekstoaster/Programming/C++/NetworkSimulation/bin/Debug/run.sh

--i = #argv

f = dynamics("FitzHughNagumo")
fi = f(1.0, .7)

rk = integrator("RungeKutta")
rki = rk(fi, 0.1)

o = output("file.csv")
print(o)
oi = o("test.csv")
print(oi.path())
--oi.open()

vec1 = vector(2, 0.0);
vec1[0] = 1.0

sim = simulation(rki, vec1)

function func(i, s)
    print("func(i)", i, s[0], s[1])

    if i==3 then
    --sim.stop()
    end
end

sim.onstep(oi)

x = sim.start(10)
--print(x)

print(#argv, argv[0], argv[1])

