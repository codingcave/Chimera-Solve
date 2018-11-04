#ifndef UNIFORMRANDOM_H
#define UNIFORMRANDOM_H

class NormalRandomModule:
    public chimera::simulation::RandomModule
{
    public:
        NormalRandomModule();
        virtual ~NormalRandomModule();
        virtual const std::string getGUID() const override;
        virtual const std::string getVersion() const override;
        virtual const size_t getType() const override;
        virtual chimera::simulation::AbstractRandom* getRandomInstance(chimera::simulation::AbstractRandomGenerator*, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(void * const instance) const override;
};

class NormalDistribution:
    public chimera::simulation::AbstractRandom
{
    public:
        explicit NormalDistribution(chimera::simulation::AbstractRandomGenerator* generator, chimera::ParameterTypeSystem* ps, double mean, double sigma);
        virtual ~NormalDistribution();
        virtual const size_t getType() const override;
        virtual void* getNext() override;
    protected:
    private:
        chimera::ParameterTypeSystem* _ps;
        chimera::simulation::AbstractRandomGenerator* _generator;
        std::normal_distribution<double>* _normDist;
};

#endif // UNIFORMRANDOM_H
