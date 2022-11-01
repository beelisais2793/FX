#ifndef NEARESTNEIGHBOURCOUPLING_H
#define NEARESTNEIGHBOURCOUPLING_H

class NearestNeighbourCouplingModule:
    public CouplingModule
    //public Registry
{
    public:
        NearestNeighbourCouplingModule();
        virtual ~NearestNeighbourCouplingModule();
        virtual AbstractCoupling* getCoupling(chimera::vec_t_LuaItem& parameters) const override;
        virtual const std::string getGUID() const override;
    protected:
    private:
};

class NearestNeighbourCoupling
    :public TemplateCoupling
    <double, boost::numeric::ublas::vector<double> >
{
    public:
        NearestNeighbourCoupling(double sigma, int count);
        virtual ~NearestNeighbourCoupling();
        virtual void operator()(const INetwork<double, boost::numeric::ublas::vector<double> >* network, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const double& t, const int& i, boost::numeric::ublas::vector<double>& out) override;
    protected:
    private:
        double _sigma;
        int _count;
};

#endif // NEARESTNEIGHBOURCOUPLING_H
