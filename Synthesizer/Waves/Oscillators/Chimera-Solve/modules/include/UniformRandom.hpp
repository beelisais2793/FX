#ifndef UNIFORMRANDOM_H
#define UNIFORMRANDOM_H

class UniformRandomModule:
    public chimera::simulation::RandomModule
{
    public:
        UniformRandomModule();
        virtual ~UniformRandomModule();
        virtual const std::string getGUID() const override;
        virtual const size_t getType() const override;
        virtual chimera::simulation::AbstractRandom* getRandomInstance(chimera::simulation::AbstractRandomGenerator*, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

class UniformDistribution:
    public chimera::simulation::AbstractRandom
{
    public:
        explicit UniformDistribution(chimera::simulation::AbstractRandomGenerator* generator, chimera::ChimeraContext* context, double a, double b);
        virtual ~UniformDistribution();
        virtual const size_t getType() const override;
        virtual void* getNext() override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        chimera::simulation::AbstractRandomGenerator* _generator;
        std::uniform_real_distribution<double>* _uniRealDist;
};

#endif // UNIFORMRANDOM_H
