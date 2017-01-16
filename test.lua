#!/home/kekstoaster/Programming/C++/NetworkSimulation/bin/Debug/run.sh

--i = #argv

function printMatrix(m)
    for i = 0, #m-1
    do
        for j = 0, #m[i]-1
        do
            write(m[i][j], " ")
        end
        write("\n")
    end
end

fhn = dynamics("FitzHughNagumo")
print(integrator)
rk = integrator("RungeKutta")

--fi = fhn(.5, .7)
--fi.test()

--print(fhn.keks)
print("-------------")
print("INT: ", rk)
zki = rk()
print("-------------")
sim = simulation(zki)
--sim.schnitzel()
--print(simulation)
--print("Simulation", sim)
function func(i, s)
    print("func(i)", i, s)
    if i==3 then
    --sim.stop()
    end
end
x = sim.start()
print(x)
--sim.start()

--print(complex)
--k = complex(1, "-.912343")
--k2 = complex.polar(4, 3)

--print(k, k.real, k.imag, k2.length, k2.arg)
--print(k2, math.sqrt(k2))
--print(#k2, #math.sqrt(k2))

--print(J * J)

--print(#argv, argv[0], argv[1])
