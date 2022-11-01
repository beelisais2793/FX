#ifndef HARMONICOSCILLATOR_H
#define HARMONICOSCILLATOR_H

class HarmonicOscillatorModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        HarmonicOscillatorModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

class HarmonicOscillator:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >
{
    public:
        explicit HarmonicOscillator(chimera::ChimeraContext* context, double a, double b);
        virtual ~HarmonicOscillator();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

    protected:
    private:
        chimera::ChimeraContext* _context;
        double _a;
        double _b;
};

#endif // HARMONICOSCILLATOR_H
