%{
#include "Gamma/Voices.h"
%}

namespace gam{

    class IndexPool{
    public:

        typedef uint64_t bits_t;
        typedef unsigned index_t;
        
        static constexpr index_t npos = ~index_t(0);
        static constexpr index_t maxSize = sizeof(bits_t)*8;

        IndexPool(unsigned size = 8);
        unsigned size() const;
        unsigned left() const;
        unsigned used() const;
        bool active(index_t i) const;
        bool playing(index_t i) const;
        bits_t activeBits() const;
        bits_t playingBits() const;
        void autoPlay(bool v);
        void playing(index_t i, bool v);
        index_t obtain();
        index_t obtainWithID(unsigned ID);
        void active(index_t i, bool v);
        void recycle(index_t i);
        void recycleAll();
        index_t indexWithID(unsigned ID);

        std::map<unsigned, index_t>& idToIndex();

        struct iterator;        
        iterator begin() const;
        iterator end() const;

        void print(FILE * fp = stderr) const;
    };

    struct IndexPool::iterator{

            iterator(bits_t bits);
            iterator(const iterator& mit);
            iterator& operator++();
            iterator operator++(int);
            bool operator==(const iterator& rhs);
            bool operator!=(const iterator& rhs);            
            index_t operator*();
        };
        
    template<unsigned N, class T>
    T ampSqr(const gam::Vec<N,T>& v);

    template<class T>
    T ampSqr(const gam::Complex<T>& v);

    template <class T>
    static T ampSqr(T v);

    class VoicesBase;

    template <class Tv = float>
    class Voice
    {
    public:
        typedef Tv value_type;

        Voice()
        
        void onAttack();        
        void onRelease();
        bool done();
        long age() const;
        void delay(long samples);
        void silenceThreshold(float v);
        unsigned silenceCount() const;
        float getParam(unsigned idx) const;
        float getParamSafe(unsigned idx) const;

    private:
        template<class,unsigned> friend class Voices;
        //friend class VoicesBase;

        Tv mOutput = Tv(0);
        float mSilenceThresh;
        unsigned short mSilenceCount = 0;
        unsigned short mSilenceCountMax = 12000;
        long mAge = 0; // max 27 hours @ 44.1 kHz
        unsigned mIndex = 0;
        VoicesBase * mParent = nullptr;

        // Returns true if the input has been below the threshold for the
        // maximum number of samples.
        bool detectSilence();
        void voiceReset();
        void voiceUpdate(const Tv& newOutput);
    };


    class Param{
    public:

        Param(float initVal=0, float smooth=0.7);

        float value() const;

        Param& operator= (float target);
        Param& init(float v);
        Param& smooth(float v);
        void update();
    };

    class VoicesBase {
    public:
        
        
        void numParams(unsigned size);
        unsigned numParams() const;
        Param& param(unsigned idx);
        float getParam(unsigned idx) const;        
        void initParam(unsigned idx, float val);
        void initParam(unsigned idx, float val, float smooth);
        void setParam(unsigned idx, float val);
        void updateParams();
    };

    template <class VoiceGen, unsigned Nvoices = 16>
    class Voices : public VoicesBase {
    public:
        
        typedef typename VoiceGen::value_type Tv;

        /// Voice stealing policy
        enum StealPolicy{
            OLDEST=0,		/** Steal oldest voice */
            NEWEST,			/** Steal newest voice */
            //QUIETEST		/** Steal quietest voice */
            //PRIORITY,		/* User-defined priority */
            //SAME_ID,		/* Steal voice with same ID */
            NONE,			/* No stealing */
        };


        Voices();
        unsigned size() const;
        unsigned numActive() const;
        void autoPlay(bool v);
        
        void play(VoiceGen& v);
        VoiceGen * voices();
        VoiceGen& voice(unsigned i);
        VoiceGen * begin();
        const VoiceGen * begin() const;
        VoiceGen * end();
        const VoiceGen * end() const;
        
        VoiceGen& voiceWithID(unsigned ID);

        unsigned obtainIndex();        
        VoiceGen& obtain();
        VoiceGen& attack();
        VoiceGen& attack(double a, double b);
        VoiceGen& attackWithID(unsigned ID);
        VoiceGen& attackWithID(unsigned ID, double a, double b);
        VoiceGen& releaseWithID(unsigned ID);
        Voices& releaseAll();
        void updateStealIndex();

        %extend {
            Tv Tick() { return (*$self)()}; 
        }
        
        /*
        Voices& traverseActive(const std::function<void(unsigned i)>& onVisit)

        template <class Obj, class R>
        Voices& call(R (Obj::*func)()){
            return traverseActive([&,this](unsigned i){ (voice(i).*func)(); });
        }

        // Call member function on all active voices
        template <class Obj, class R, class A, class L>
        Voices& call(R (Obj::*func)(A), L l){
            return traverseActive([&,this](unsigned i){ (voice(i).*func)(l); });
        }

        // Call member function on all active voices
        template <class Obj, class R, class A, class B, class L, class M>
        Voices& call(R (Obj::*func)(A,B), L l, M m){
            return traverseActive([&,this](unsigned i){ (voice(i).*func)(l,m); });
        }
        */
        
        Voices& stealPolicy(StealPolicy v);
        void print() const;
    };
} // gam::