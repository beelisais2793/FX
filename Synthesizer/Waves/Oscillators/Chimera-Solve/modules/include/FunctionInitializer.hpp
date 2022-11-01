#ifndef FUNCTIONINITIALIZER_H
#define FUNCTIONINITIALIZER_H

class FunctionInitializerModule:
    public chimera::simulation::InitializerModule
{
    public:
        FunctionInitializerModule();
        virtual ~FunctionInitializerModule();
        virtual const std::string getGUID() const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
        virtual chimera::simulation::AbstractInitializer* getInitializerInstance(chimera::vec_t_LuaItem& parameters) const override;
};

class FunctionInitializer:
    public chimera::simulation::AbstractInitializer
{
    public:
        explicit FunctionInitializer(chimera::ChimeraContext* context, const chimera::ParameterValue& functionValue);
        virtual ~FunctionInitializer();
        virtual void initialize(chimera::simulation::AbstractSystemDynamic* system, void* state) override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        chimera::ParameterValue* _functionValue;
};

#endif // FUNCTIONINITIALIZER_H
