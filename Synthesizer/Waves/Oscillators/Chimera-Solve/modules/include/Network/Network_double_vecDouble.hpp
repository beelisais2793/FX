#ifndef NETWORK_DOUBLE_VECDOUBLE_H
#define NETWORK_DOUBLE_VECDOUBLE_H

class Network_double_vecDouble:
    //template<typename time_type, typename state_type>
    public chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> > >,
    public INetwork<double, boost::numeric::ublas::vector<double> >
{
    public:
        Network_double_vecDouble(chimera::ChimeraContext* context, chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* unit, const int& number, TemplateCoupling<double, boost::numeric::ublas::vector<double> >* coupling);

        virtual ~Network_double_vecDouble();

        virtual chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* getNode(int i) const override;

        virtual void operator()(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& x, boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& dxdt, const double& t) override;

        virtual std::unordered_map<std::string, size_t> getFeatures() const override;
        virtual const std::string getSystemName() const override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        chimera::simulation::TemplateOdeSystem<double, boost::numeric::ublas::vector<double> >* _unitSys;
        int _number;
        TemplateCoupling<double, boost::numeric::ublas::vector<double> >* _coupling;
        boost::numeric::ublas::vector<double>* _tmp;
};

#endif // NETWORK_DOUBLE_VECDOUBLE_H
