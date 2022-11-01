%{
#include "Gamma/Scheduler.h
}

%include "lua_fnptr.i"

namespace gam
{
    struct SchedulerAudioIOData{

        SchedulerAudioIOData();

        const float * buffersIn;	///< Non-interleaved input buffers
        float * buffersOut;			///< Non-interleaved output buffers
        double framesPerSecond;		///< Frames per second (frame rate)
        unsigned framesPerBuffer;	///< Frames per buffer (block size)
        unsigned channelsIn;		///< Number of input channels
        unsigned channelsOut;		///< Number of output channels
        unsigned startFrame;		///< Start frame to begin processing

        /*
        /// Set user data
        template <class T>
        void userData(T * user){
            mUserData = user;
            mUserDataTypeID = typeID<T>();
        }
        */

        /// Get user data
        void * userData() const;
        
        double secondsPerBuffer() const;

        /*
        template <class TAudioIOData>
        void mapAudioIOData(TAudioIOData& externalIO);
        */
        
        /*
        template <class TAudioIOData>
        TAudioIOData& unmapAudioIOData();
        */
    
    };


    class ProcessNode : public Node3<ProcessNode>{
    public:

        ProcessNode(double delay=0.);

        virtual ~ProcessNode();

        virtual void onProcessNode(SchedulerAudioIOData& io);        
        virtual void onReset();

        /// Set starting time offset, in seconds
        ProcessNode& dt(double v);        
        ProcessNode& free();        
        ProcessNode& active(bool v);
        ProcessNode& reset();

        bool deletable() const;
        bool done() const;
        bool active() const;
        bool inactive() const
        void print();
    };


    template <class TAudioIOData>
    class Process : public ProcessNode{
    public:

        virtual void onProcess(TAudioIOData& io) = 0;
    
    };


    class ControlFunc
    {
    public:
        ControlFunc(const Func& f, double dt=0);

        ControlFunc& dt(double v);
        ControlFunc& period(double v);

        %extend {
            void call() { ($self)(); }
        }

    };

  class Scheduler : public ProcessNode{
    public:

        typedef std::queue<ProcessNode *> FreeList;
        typedef std::list<ControlFunc> Funcs;

        Scheduler();
        ~Scheduler();

        bool empty() const;
        int reclaim();
        const SchedulerAudioIOData& io() const;
        SchedulerAudioIOData& io();
        
        /*
        template <class AProcess>
        AProcess& add(){
            AProcess * v = new AProcess;
            cmdAdd(v); return *v;
        }

        template <class AProcess, class A>
        AProcess& add(const A& a){
            AProcess * v = new AProcess(a);
            cmdAdd(v); return *v;
        }

        template <class AProcess, class A, class B>
        AProcess& add(const A& a, const B& b){
            AProcess * v = new AProcess(a,b);
            cmdAdd(v); return *v;
        }

        template <class AProcess, class A, class B, class C>
        AProcess& add(const A& a, const B& b, const C& c){
            AProcess * v = new AProcess(a,b,c);
            cmdAdd(v); return *v;
        }

        template <class AProcess, class A, class B, class C, class D>
        AProcess& add(const A& a, const B& b, const C& c, const D& d){
            AProcess * v = new AProcess(a,b,c,d);
            cmdAdd(v); return *v;
        }

        template <class AProcess, class A, class B, class C, class D, class E>
        AProcess& add(const A& a, const B& b, const C& c, const D& d, const E& e){
            AProcess * v = new AProcess(a,b,c,d,e);
            cmdAdd(v); return *v;
        }

        template <class AProcess, class A, class B, class C, class D, class E, class F>
        AProcess& add(const A& a, const B& b, const C& c, const D& d, const E& e, const F& f){
            AProcess * v = new AProcess(a,b,c,d,e,f);
            cmdAdd(v); return *v;
        }


        /// Add dynamically allocated process as first child of specified node
        template <class AProcess>
        AProcess& add(ProcessNode& parent){
            AProcess * v = new AProcess;
            pushCommand(Command::ADD_FIRST_CHILD, &parent, v);
            return *v;
        }
        */

        /// Add deferred function call
        ControlFunc& add(const Func& f);
        void update();

        /*
        template <class TAudioIOData>
        void update(TAudioIOData& externalIO){
            io().mapAudioIOData(externalIO);
            update();
        }
        */

        
        Scheduler& period(float v);        
        void start();
        void stop();
        void recordNRT(const char * soundFilePath, double durSec);

        /*
        template <class TAudioIOData>
        void recordNRT(TAudioIOData& aio, const char * soundFilePath, double durSec){
            io().mapAudioIOData(aio);
            recordNRT(soundFilePath, durSec);
        }
        */

    
        /*
        template <class TAudioIOData>
        static void audioCB(TAudioIOData& aio);
        */
    };
}

%inline %{

    struct LuaFunc
    {
        SWIGLUA_REF cb;
    
    };

    struct LuaProcess : public gam::Process
    {
        SWIGLUA_REF cb;
    }
%}