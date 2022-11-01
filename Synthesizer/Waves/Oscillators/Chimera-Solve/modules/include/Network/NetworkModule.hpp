#ifndef NETWORKMODULE_H
#define NETWORKMODULE_H

class NetworkModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        NetworkModule();
        virtual ~NetworkModule();
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
        virtual void init(chimera::EntryPoint const * const entryPoint) override;
        virtual bool load(chimera::EntryPoint const * const entryPoint, void const * const params) override;
        virtual void unload(chimera::EntryPoint const * const entryPoint) override;
    private:
        chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* getSystem1(const chimera::ParameterValue& param) const;
        TemplateCoupling<double, boost::numeric::ublas::vector<double> >* getCoupling1(const chimera::ParameterValue& param) const;
        chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<std::complex<double> > >* getSystem2(const chimera::ParameterValue& param) const;
        TemplateCoupling<double, boost::numeric::ublas::vector<std::complex<double> > >* getCoupling2(const chimera::ParameterValue& param) const;

        CouplingEntryPoint* _ep;
        const chimera::EntryPoint* _callingEP;
};

#endif // NETWORKMODULE_H
