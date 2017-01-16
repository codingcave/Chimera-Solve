#ifndef SIMULATION_H
#define SIMULATION_H

class Simulation:
    public NotificationManager
{
    public:
        Simulation(AbstractIntegrator* integrator);
        virtual ~Simulation();
        bool getYield() const;
        void setYield(bool value);
        bool nextStep();
        bool isRunning() const;
        AbstractIntegrator * getIntegrator();
    protected:
    private:
        AbstractIntegrator* _integrator;
        bool _yield;
        size_t _type;
        std::string _name;
        bool _running;
};

#endif // SIMULATION_H
