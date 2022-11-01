#ifndef FITZHUGHNAGUMO_H
#define FITZHUGHNAGUMO_H

class FitzHughNagumo:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >
{
    public:
        explicit FitzHughNagumo(chimera::ChimeraContext* context, double epsilon, double a);
        virtual ~FitzHughNagumo();
        virtual void operator()(const boost::numeric::ublas::vector<double>& x, boost::numeric::ublas::vector<double>& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;

    protected:
    private:
        chimera::ChimeraContext* _context;
        double _epsilon;
        double _a;
};

#endif // FITZHUGHNAGUMO_H
