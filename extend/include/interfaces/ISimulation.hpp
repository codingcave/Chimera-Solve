#ifndef ISIMULATION_H
#define ISIMULATION_H

namespace chimera {
    namespace simulation {
        class ISimulation
        {
            public:
                ISimulation() {}
                virtual ~ISimulation() {}
                virtual bool isRunning() const = 0;
                virtual AbstractIntegrator* getIntegrator() = 0;
            protected:
            private:
        };
    }
}

#endif // ISIMULATION_H
