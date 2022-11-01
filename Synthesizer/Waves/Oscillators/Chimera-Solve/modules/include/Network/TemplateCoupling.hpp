#ifndef TEMPLATECOUPLING_H
#define TEMPLATECOUPLING_H

template<typename time_type, typename inner_state_type>
class TemplateCoupling:
    public AbstractCoupling
{
    public:
        TemplateCoupling() {}
        virtual ~TemplateCoupling() {}
        virtual void operator()(const INetwork<time_type, inner_state_type>* network, const boost::numeric::ublas::vector<inner_state_type>& system, const time_type& t, const int& i, inner_state_type& out) = 0;
        //virtual bool checkCoupling(const boost::numeric::ublas::vector<inner_state_type>& system) = 0;
    protected:
    private:
};

#endif // TEMPLATECOUPLING_H
