#ifndef RANDOMINITIALIZER_H
#define RANDOMINITIALIZER_H

class RandomInitializerModule:
    public chimera::simulation::InitializerModule
{
    public:
        RandomInitializerModule();
        virtual ~RandomInitializerModule();
        virtual const std::string getGUID() const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
        virtual chimera::simulation::AbstractInitializer* getInitializerInstance(chimera::vec_t_LuaItem& parameters) const override;
};

class RandomInitializer:
    public chimera::simulation::AbstractInitializer
{
    public:
        explicit RandomInitializer(chimera::ChimeraContext* context, chimera::simulation::AbstractRandom* random);
        virtual ~RandomInitializer();
        virtual void initialize(chimera::simulation::AbstractSystemDynamic* system, void* state) override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        chimera::simulation::AbstractRandom* _random;
};

#endif // RANDOMINITIALIZER_H
