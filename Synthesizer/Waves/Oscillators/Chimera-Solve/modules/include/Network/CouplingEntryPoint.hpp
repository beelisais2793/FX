#ifndef COUPLINGENTRYPOINT_H
#define COUPLINGENTRYPOINT_H


class CouplingEntryPoint:
    public chimera::EntryPoint
{
    public:
        CouplingEntryPoint();
        virtual ~CouplingEntryPoint();
        virtual const std::string getGUID() const override;
    protected:
        virtual bool checkModule(chimera::Module const * const module) const override;
        virtual void init() override;
    private:
};


#endif // COUPLINGENTRYPOINT_H
