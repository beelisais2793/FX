#ifndef KURAMOTOCOUPLING_H
#define KURAMOTOCOUPLING_H

class KuramotoCouplingModule:
    public CouplingModule
    //public Registry
{
    public:
        KuramotoCouplingModule();
        virtual ~KuramotoCouplingModule();
        virtual AbstractCoupling* getCoupling(chimera::vec_t_LuaItem& parameters) const override;
        virtual const std::string getGUID() const override;
    protected:
    private:
};

class KuramotoCoupling
    :public TemplateCoupling
    <double, boost::numeric::ublas::vector<double> >
{
    public:
        KuramotoCoupling(double sigma, double alpha, double beta, double epsilon);
        virtual ~KuramotoCoupling();
        virtual void operator()(const INetwork<double, boost::numeric::ublas::vector<double> >* network, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const double& t, const int& i, boost::numeric::ublas::vector<double>& out) override;
    protected:
    private:
        void recalcSinCos(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const int& N);

        double _sigma;
        double _alpha;
        double _beta;
        double _epsilon;

        double _sinA;
        double _cosA;
        double _sinB;
        double _cosB;

        void* _sysCache;
        double _timeCache;
        boost::numeric::ublas::vector<double> _sinCache;
        boost::numeric::ublas::vector<double> _cosCache;
};

#endif // KURAMOTOCOUPLING_H
