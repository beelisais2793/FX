#ifndef RUNGEKUTTAMODULE_H
#define RUNGEKUTTAMODULE_H

class RungeKuttaModule:
    public chimera::simulation::IntegratorModule
    //public Registry
{
    public:
        RungeKuttaModule();
        virtual ~RungeKuttaModule();
        virtual bool checkFeatures(const chimera::map_t_size& features) const override;
        virtual const std::string getSystemName() const override;
        virtual chimera::simulation::AbstractIntegrator* getIntegratorInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
        virtual const std::string getGUID() const override;
        virtual bool load(chimera::EntryPoint const * const entryPoint, void const * const params) override;
    protected:
    private:
        chimera::EntryPoint* _init;
        static constexpr double DEFAULT_DT = 0.1;
        chimera::simulation::AbstractIntegrator* getSystem(const chimera::ParameterValue& param, const double& dt) const;
};

#endif // RUNGEKUTTAMODULE_H
