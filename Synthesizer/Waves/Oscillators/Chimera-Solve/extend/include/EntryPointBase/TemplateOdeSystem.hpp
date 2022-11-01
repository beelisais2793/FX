#ifndef TEMPLATESYSTEMDYNAMIC_H
#define TEMPLATESYSTEMDYNAMIC_H

namespace chimera {
    namespace simulation {
        template<typename time_type, typename state_type>
        class TemplateOdeSystem:
            public AbstractSystemDynamic
        {
            public:
                TemplateOdeSystem() {}
                virtual ~TemplateOdeSystem() {}
                virtual void operator()(const state_type& x, state_type& dxdt, const time_type& t) = 0;
            protected:
            private:
        };
    }
}

#endif // TEMPLATESYSTEMDYNAMIC_H
