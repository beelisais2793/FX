#ifndef FITZHUGHNAGUMO_REGISTRY_H
#define FITZHUGHNAGUMO_REGISTRY_H

class FitzHughNagumoModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        FitzHughNagumoModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

#endif // FITZHUGHNAGUMO_REGISTRY_H
