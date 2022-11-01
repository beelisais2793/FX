#ifndef UNIFORMRANDOM_H
#define UNIFORMRANDOM_H

class NormalRandomModule:
    public chimera::simulation::RandomModule
{
    public:
        NormalRandomModule();
        virtual ~NormalRandomModule();
        virtual const std::string getGUID() const override;
        virtual const size_t getType() const override;
        virtual chimera::simulation::AbstractRandom* getRandomInstance(chimera::simulation::AbstractRandomGenerator*, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

class NormalDistribution:
    public chimera::simulation::AbstractRandom
{
    public:
        explicit NormalDistribution(chimera::simulation::AbstractRandomGenerator* generator, chimera::ChimeraContext* context, double mean, double sigma);
        virtual ~NormalDistribution();
        virtual const size_t getType() const override;
        virtual void* getNext() override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        chimera::simulation::AbstractRandomGenerator* _generator;
        std::normal_distribution<double>* _normDist;
};

#endif // UNIFORMRANDOM_H
