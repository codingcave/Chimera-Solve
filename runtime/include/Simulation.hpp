#ifndef SIMULATION_H
#define SIMULATION_H


namespace chimera {
    namespace runtime {

        class Simulation:
            public NotificationManager
        {
            public:
                Simulation(chimera::simulation::AbstractIntegrator* integrator);
                virtual ~Simulation();
                bool getYield() const;
                void setYield(bool value);
                bool nextStep();
                bool isRunning() const;
                chimera::simulation::AbstractIntegrator* getIntegrator();
            protected:
            private:
                chimera::simulation::AbstractIntegrator* _integrator;
                bool _yield;
                size_t _type;
                std::string _name;
                bool _running;
        };
    }
}
#endif // SIMULATION_H
