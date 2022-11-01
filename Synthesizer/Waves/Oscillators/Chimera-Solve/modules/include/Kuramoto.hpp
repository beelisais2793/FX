#ifndef KURAMOTO_H
#define KURAMOTO_H

class KuramotoModule:
    public chimera::simulation::SystemDynamicModule
{
    public:
        KuramotoModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

class KuramotoOscillator:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >
{
    public:
        explicit KuramotoOscillator(chimera::ChimeraContext* context, int N, double omega);
        virtual ~KuramotoOscillator();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

    protected:
    private:
        chimera::ChimeraContext* _context;
        int _N;
        double _omega;
};

#endif // KURAMOTO_H
