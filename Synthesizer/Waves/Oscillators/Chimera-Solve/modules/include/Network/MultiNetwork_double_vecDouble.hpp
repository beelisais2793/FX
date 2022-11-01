#ifndef MULTINETWORK_DOUBLE_VECDOUBLE_H
#define MULTINETWORK_DOUBLE_VECDOUBLE_H

class MultiNetwork_double_vecDouble:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >,
    public INetwork<double, boost::numeric::ublas::vector<double> >
{
    public:
        MultiNetwork_double_vecDouble(chimera::ChimeraContext* context, chimera::Module* unitModule, std::vector<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>& units, TemplateCoupling<double, boost::numeric::ublas::vector<double> >* coupling);

        virtual ~MultiNetwork_double_vecDouble();

        virtual chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* getNode(int i) const override;

        virtual void operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& x, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        chimera::Module* _unitModule;
        std::vector<chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >*>* _unitSystems;
        int _number;
        TemplateCoupling<double, boost::numeric::ublas::vector<double> >* _coupling;
        boost::numeric::ublas::vector<double>* _tmp;
};

#endif // MULTINETWORK_DOUBLE_VECDOUBLE_H
