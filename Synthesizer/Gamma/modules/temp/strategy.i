%{
#include "Gamma/Strategy.h"
%}

namespace gam{
    namespace ipl{
        template <class T>
        struct Trunc{

            ipl::Type type() const;
            void type(ipl::Type v);

            
            %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const T * src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	
        };


        template <class T>
        struct Round{

            ipl::Type type() const;
            void type(ipl::Type v);

            %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const T * src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	
            
            /*
            template <class AccessStrategy>
            T operator()(const AccessStrategy& acc, const T * src, index_t iInt, double iFrac, index_t max, index_t min=0, index_t str=1) const{
                return ipl::nearest(
                    iFrac,
                    src[iInt*str],
                    src[acc.mapP1(iInt+1, max, min)*str]
                );
            }
            */
            
        };


        template <class T>
        struct Mean2{

            ipl::Type type() const;
            void type(ipl::Type v);

        %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const T * src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	
        };


        /// Linear random-access interpolation strategy

        /// \ingroup Strategy, ipl
        template <class T>
        struct Linear{

            ipl::Type type() const;
            void type(ipl::Type v);

            %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const T * src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	
        };


        /// Cubic random-access interpolation strategy

        /// \ingroup Strategy, ipl
        template <class T>
        struct Cubic{

            ipl::Type type() const;
            void type(ipl::Type v);

            %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const T * src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	
        };


        template <class T>
        struct AllPass{

            AllPass(T prev=0);

            ipl::Type type() const;
            void type(ipl::Type v;

            %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const T * src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	
            mutable T prev;
        };


        template <class T>
        struct Switchable{

            Switchable();
            
            ipl::Type type() const;
            void type(ipl::Type v);

            %extend {
                T Process(const gam::ArrayPow2<T> & a, uint32_t phase) { return (*$self)(a,phase); }

                T Exec(const Array<T> & src, gam::index_t iInt, double iFrac, gam::index_t max, gam::index_t min=0, gam::index_t str=1)
                {
                    return (*$self)(src,iInt,iFrac,max,min,str);
                }
            }	            
        };
    }

    namespace iplSeq{

        template <unsigned N, class T>
        struct Base{
            Base(const T& v=0);
            void push(T va);
            void set(T va);
            T val() const;
            void val(const T& va);
            T v[N];	///< Value buffer, 0 is newest, N-1 is oldest
        };


        template <class T>
        struct None : public Base<1,T>{
            using Base<1,T>::v;
            None(const T& v=0);
            T operator()(float f) const;
        };

        
        template <class T>
        struct Trunc : public Base<2,T>
        {
            using Base<2,T>::v;
            Trunc(const T& v=0);
            T operator()(float f) const;
        };

        
        template <class T>
        struct Round : public Base<2,T>{
            using Base<2,T>::v;
            Round(const T& v=0);
            T operator()(float f) const;
        };

        
        template <class T>
        struct Linear : public Base<2,T>{
            using Base<2,T>::v;
            Linear(const T& v=0);
            T operator()(float f) const;
        };

        
        template <class T>
        struct Cubic : public Base<4,T>{
            using Base<4,T>::v;
            Cubic(const T& v=0);
            T operator()(float f) const;
            T val() const;
            void val(const T& va);
        };
        
        template <class T>
        struct Cosine : public Base<2,T>{
            using Base<2,T>::v;
            Cosine(const T& v=0);
            T operator()(float f) const;
        };
    } // iplSeq::


    namespace phsInc{


        template <class T>
        T incClip(T v, T inc, T max, T min);

        /// \ingroup Strategy, phsInc
        struct Loop{
            void reset();
        
            uint32_t operator()(uint32_t& pos, uint32_t inc);

            %extend {
                uint32_t Tick(uint32_t & pos, uint32_t inc) { return (*$self)(pos,inc); }
                SampleType Process(SampleType v, SampleType inc, SampleType max, SampleType min) { return (*$self)(v,inc,max,min); }
            }
            
            bool done(uint32_t /*pos*/) const;
            
            
        };


        /// Play then hold waveform
        struct Pulse{

            Pulse();
            Pulse& pulse(uint32_t width, uint32_t period);
            void reset();

            %extend {
                uint32_t Tick(uint32_t & pos, uint32_t inc) { return (*$self)(pos,inc); }            
            }
            
            bool done(uint32_t /*pos*/) const;
        };


        struct OneShot{
            void reset();
        
            %extend {
                uint32_t Tick(uint32_t & pos, uint32_t inc) { return (*$self)(pos,inc); }            
                SampleType Process(SampleType v, SampleType inc, SampleType max, SampleType min) { return (*$self)(v,inc,max,min); }
            }
            bool done(uint32_t pos) const;            
        };


        struct NShot
        {
            NShot();
            
            void reset();

            bool done(uint32_t pos) const;
            NShot& repeats(uint32_t v);
            NShot& number(uint32_t v);

            %extend {
                uint32_t Tick(uint32_t & pos, uint32_t inc) { return (*$self)(pos,inc); }            
                SampleType Process(SampleType v, SampleType inc, SampleType max, SampleType min) { return (*$self)(v,inc,max,min); }
            }
        };


        struct PingPong
        {
            PingPong();
        
            void reset();
    
            bool done(uint32_t /*pos*/) const;
            uint32_t dir;

            %extend {
                uint32_t Tick(uint32_t & pos, uint32_t inc) { return (*$self)(pos,inc); }            
                SampleType Process(SampleType v, SampleType inc, SampleType max, SampleType min) { return (*$self)(v,inc,max,min); }
            }
                
        };


        /// Plays and holds waveform according to binary repeating pattern.

        /// \ingroup Strategy, phsInc
        struct Rhythm{
        
            Rhythm();
            void reset();
            
            bool done(uint32_t /*pos*/) const;
            
            Rhythm& pattern(uint64_t bits, uint8_t size);                        
            Rhythm& pattern(const char* bits, char offChar='.');
            Rhythm& pulse(uint8_t pulseWidth, uint8_t length);

            %extend {
                uint32_t Tick(uint32_t & pos, uint32_t inc) { return (*$self)(pos,inc); }            
                }
        };
    } // phsInc::
}