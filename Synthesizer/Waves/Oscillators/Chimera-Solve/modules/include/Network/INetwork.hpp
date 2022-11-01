#ifndef INETWORK_H
#define INETWORK_H

template<typename time_type, typename state_type>
class INetwork
{
    public:
        INetwork() { }
        virtual ~INetwork() { }
        virtual chimera::simulation::TemplateOdeSystem<time_type, state_type>* getNode(int i) const = 0;
    protected:
    private:
};

#endif // INETWORK_H
