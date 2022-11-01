#ifndef TEMPLATETEMPORALINTEGRATOR_H
#define TEMPLATETEMPORALINTEGRATOR_H

/*
namespace chimera {
    namespace simulation {
        class IEventListener;
        class IEventListenerProvider;

        template<typename time_type, typename state_type>
        class TemplateTemporalStateObserver;

        template<typename time_type, typename state_type>
        class TemplateTemporalStateEventProvider;

        template<typename time_type, typename state_type>
        struct T_TimeStateArgs
        {
            time_type time;
            state_type  state;
        };

        template<typename time_type, typename state_type>
        class TemplateTemporalIntegrator:
            public AbstractTemporalIntegrator
        {
            public:
                TemplateTemporalIntegrator() {}
                virtual ~TemplateTemporalIntegrator() {}

                //void * currentTime() override { return (void *)getTime(); }
                //void * currentState() override { return (void *)getState(); }
                virtual const time_type& getTime() const = 0;
                virtual const state_type& getState() const = 0;

                //virtual AbstractEventProvider* createStateEvent() override
                //{
                //    return new TemplateTemporalStateEventProvider<time_type, state_type>(this);
                //}
            protected:
            private:
        };






        template<typename time_type, typename state_type>
        class TemplateTemporalStateEventProvider:
            public AbstractEventProvider
        {
            public:
                TemplateTemporalStateEventProvider(TemplateTemporalIntegrator<time_type, state_type>* integrator)
                {
                    _integrator = integrator;
                    _observer = new TemplateTemporalStateObserver<time_type, state_type>(integrator->getTimeType(), integrator->getStateType());
                    _args = new struct T_TimeStateArgs<time_type, state_type>();
                }
                virtual ~TemplateTemporalStateEventProvider()
                {
                    delete _observer;
                    delete _args;
                }
                virtual Observer* getObserver(NotificationManager* sender)
                {
                    return _observer;
                }
                virtual bool triggerCondition(NotificationManager* sender)
                {
                    return true;
                }
                virtual void* getEventArgs(NotificationManager* sender)
                {
                    _args->time = _integrator->getTime();
                    _args->state = _integrator->getState();
                    return _args;
                }
                virtual std::string getName() const
                {
                    return "step";
                }
            protected:
            private:
                Observer* _observer;
                TemplateTemporalIntegrator<time_type, state_type>* _integrator;
                struct T_TimeStateArgs<time_type, state_type>* _args;
        };

        template<typename time_type, typename state_type>
        class TemplateTemporalStateObserver:
            public Observer
        {
            public:
                TemplateTemporalStateObserver(size_t timeType, size_t stateType){
                    _timeType = timeType;
                    _stateType = stateType;
                    _listeners = new std::unordered_set<StateEventListener<time_type, state_type>* >();
                }
                virtual ~TemplateTemporalStateObserver(){
                    delete _listeners;
                }

                virtual void trigger(NotificationManager* sender, void* args) override
                {
                    for(auto it : *_listeners) {
                        struct T_TimeStateArgs<time_type, state_type>* pargs = (struct T_TimeStateArgs<time_type, state_type>*)args;
                        it->notify(pargs->time, pargs->state);
                    }
                }
                virtual void addListener(IEventListener* listener) override
                {
                    StateEventListener<time_type, state_type>* l = dynamic_cast<StateEventListener<time_type, state_type>* >(listener);
                    if(l)
                    {
                        _listeners->insert(l);
                    }
                }
                virtual void addListener(IEventListenerProvider* provider) override
                {
                    struct StateProviderArgs args = {_timeType, _stateType};
                    IEventListener* listener = provider->provideListener(1, &args);
                    StateEventListener<time_type, state_type>* l = dynamic_cast<StateEventListener<time_type, state_type>* >(listener);
                    if(l)
                    {
                        _listeners->insert(l);
                    }
                }
                virtual void removeListener(IEventListener* listener) override
                {
                    StateEventListener<time_type, state_type>* l = dynamic_cast<StateEventListener<time_type, state_type>* >(listener);
                    if(l)
                    {
                        _listeners->erase(l);
                    }
                }
                bool empty() const
                {
                    return _listeners->empty();
                }
            protected:
            private:
                std::unordered_set<StateEventListener<time_type, state_type>* >* _listeners;
                size_t _timeType;
                size_t _stateType;
        };
    }
}
*/

#endif // TEMPLATETEMPORALINTEGRATOR_H
