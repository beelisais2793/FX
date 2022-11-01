#ifndef TIMEDOBSERVER_H
#define TIMEDOBSERVER_H

const char * const TIMED_OBSERVER_EVENT_NAME = "elapsed";

struct T_StateProviderArgs {
    size_t time_type;
    size_t state_type;
};

class TimedObserverModule:
    public chimera::simulation::ObserverModule
{
    public:
        TimedObserverModule();
        virtual ~TimedObserverModule();
        virtual const std::string getGUID() const override;
        virtual chimera::simulation::AbstractEventManager* getEventInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

class TimedObserver:
    public chimera::simulation::AbstractEventManager,
    public chimera::simulation::IEventListener
{
    public:
        explicit TimedObserver(chimera::simulation::NotificationManager* nm, double start, double step, double end);
        virtual ~TimedObserver();
        virtual NotificationManager* getObservationObject() override;
        virtual void assignObservation(NotificationManager* baseObject) override;
        virtual IEventListener* provideListener(size_t id, void const * const args) override;
        virtual void notify(chimera::simulation::NotificationManager const * const sender, void const * const args) override;
    protected:
    private:
        chimera::simulation::NotificationManager* _nm;
        chimera::simulation::TemporalStateEventProvider* _elapsedEvent;
        double _start;
        double _step;
        double _end;
        double _last;
};


#endif // TIMEDOBSERVER_H
