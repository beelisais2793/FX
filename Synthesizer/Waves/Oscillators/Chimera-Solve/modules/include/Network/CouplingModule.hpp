#ifndef COUPLINGMODULE_H
#define COUPLINGMODULE_H


class CouplingModule
    :public chimera::Module
{
    public:
        CouplingModule();
        virtual ~CouplingModule();
        virtual AbstractCoupling* getCoupling(chimera::vec_t_LuaItem& parameters) const = 0;
    protected:
    private:

        virtual void* getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

#endif // COUPLINGMODULE_H
