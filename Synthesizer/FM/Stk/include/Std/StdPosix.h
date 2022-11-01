#pragma once 

#include <cstdint>
#include <csignal>
#include <cassert>
#include <csetjmp>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <cstdlib>
#include <iostream>
#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/tcp.h>
#include <pthread.h>
#include <iso646.h>
#include <functional>
#include <errno.h>
#include <string>

#define PO6_MICROS 1000ULL
#define PO6_MILLIS (PO6_MICROS * 1000ULL)
#define PO6_SECONDS (PO6_MILLIS * 1000ULL)


namespace Std 
{
    typedef std::function<void (void)> function;

    // strerror is not thread-safe
    // strerror_r is thread-safe, but the XSI and GNU do different things, and it
    //      requres that the application set feature test macros correctly.
    //
    // po6::strerror hides all this behind a simple function that is thread-safe and
    // always returns the expected error string.
    std::string strerror(int err);

    // convert the given errno into a c-string constant.  e.g. strerrno(EINVAL)
    // returns the string "EINVAL".
    const char* strerrno(int err);

    // On some platforms, basename and dirname may use statically allocated storage,
    // or may modify the input string, neither of which are desirable.  These
    // basename and dirname calls will return a new string without modifying the
    // input.

    std::string
    basename(const std::string& path);

    std::string
    dirname(const std::string& path);

    // This corresponds to the realpath(3) library call.
    bool
    realpath(const std::string& path, std::string* real);

    // Concatenate pathnames in a manner that is reminiscent of os.path.join in
    // Python.

    std::string
    join(const std::string& p1,
        const std::string& p2);

    std::string
    join(const std::string& p1,
        const std::string& p2,
        const std::string& p3);

    std::string
    join(const std::string& p1,
        const std::string& p2,
        const std::string& p3,
        const std::string& p4);

    std::string
    join(const std::string& p1,
        const std::string& p2,
        const std::string& p3,
        const std::string& p4,
        const std::string& p5);

    uint64_t time();
    uint64_t monotonic_time();
    uint64_t wallclock_time();
    void sleep(uint64_t);

    inline void srand() { std::srand(::time(NULL)); }
    float random(float min = 0.0f, float max = 1.0f) { return min + (max-min)* ((float)rand() / (float)RAND_MAX); }
    int system(const char * cmd) { return std::system(cmd); }
    void abort() { std::abort(); }
    void exit(int exit_code=-1) { std::exit(exit_code); }
    char* getenv(const char * var) { return std::getenv(var); }
    int   setenv(const char * name, const char * val, int overwrite) { return ::setenv(name,val,overwrite); }
    int unsetenv(const char * name) { return ::unsetenv(name); }
    int putenv(char * string) { return ::putenv(string); }
    int raise(int sig) { return std::raise(sig); }

    /*
    struct JumpBuffer 
    {
        std::jmp_buf jump;
        JumpBuffer() { setjmp(); }
        int setjmp() { return std::setjmp(jump); }
        void longjmp(int status) { std::lonjump(jump,status); }
    };
    */
    class fd
    {
        public:
            fd();
            explicit fd(int f);
            virtual ~fd() throw ();

        public:
            int get() const { return m_fd; }
            void close();
            ssize_t read(void *buf, size_t nbytes);
            ssize_t xread(void* buf, size_t nbytes);
            ssize_t write(const void *buf, size_t nbytes);
            ssize_t xwrite(const void *buf, size_t nbytes);
            bool set_nonblocking();
            void swap(fd* other) throw ();

        public:
            fd& operator = (int f);

        private:
            int m_fd;

        private:
            fd(const fd&);
            fd& operator = (const fd&);
    };


    class mmap
    {
        public:
            mmap(void* addr, size_t length,
                int prot, int flags,
                int fd, off_t offset);
            ~mmap() throw ();

        public:
            void* base() const { return m_base; }
            size_t size() const { return m_length; }
            bool valid() const { return m_base != NULL; }
            int error() const { return m_error; }
            void close();

        private:
            void* m_base;
            size_t m_length;
            int m_error;

        private:
            mmap(const mmap&);
            mmap& operator = (const mmap&);
    };

    class mutex
    {        
        public:
            mutex();
            ~mutex() throw ();

        public:
            void lock();
            void unlock();

        private:
            friend class cond;

        private:
            pthread_mutex_t m_mutex;

        private:
            mutex(const mutex&);
            mutex& operator = (const mutex&);
    };

    class hold
    {
        public:
            hold(mutex* mtx);
            ~hold() throw ();

        public:
            void release();

        private:
            bool m_held;
            mutex* m_mtx;

        private:
            hold(const hold&);
            hold& operator = (const hold&);
    };

    class rwlock
    {
        public:
            rwlock();
            ~rwlock() throw ();

        public:
            void rdlock();
            void wrlock();
            void unlock();

        private:
            pthread_rwlock_t m_rwlock;

        private:
            rwlock(const rwlock&);
            rwlock& operator = (const rwlock&);
    };

    class rdhold
    {
        public:
            rdhold(rwlock* rwl);
            ~rdhold() throw ();

        public:
            void release();

        private:
            bool m_held;
            rwlock* m_rwl;

        private:
            rdhold(const rdhold&);
            rdhold& operator = (const rdhold&);
    };

    class wrhold
    {
        public:
            wrhold(rwlock* rwl);
            ~wrhold() throw ();

        public:
            void release();

        private:
            bool m_held;
            rwlock* m_rwl;

        private:
            wrhold(const wrhold&);
            wrhold& operator = (const wrhold&);
    };

    class cond
    {
        public:
            cond(mutex* mtx);
            ~cond() throw ();

        public:
            void lock();
            void unlock();
            void wait();
            void signal();
            void broadcast();

        private:
            mutex* m_mtx;
            pthread_cond_t m_cond;

        private:
            cond(const cond&);
            cond& operator = (const cond&);
    };

    class barrier
    {
        public:
            barrier(uint64_t count);
            ~barrier() throw ();

        public:
            bool wait();

        private:
            mutex m_lock;
            cond m_cv;
            uint64_t m_height;
            uint64_t m_level;
            uint64_t m_generation;

        private:
            barrier(const barrier&);
            barrier& operator = (const barrier&);
    };

    class thread
    {
        public:
            thread(function func);
            ~thread() throw ();

        public:
            void start();
            void join();

        private:
            static void* start_routine(void * arg);

        private:
            bool m_started;
            bool m_joined;
            function m_func;
            pthread_t m_thread;

        private:
            thread(const thread&);
            thread& operator = (const thread&);
    };

    // This bullshit shouldn't be needed, but as standard libraries evolve, they
    // have had a tendency to break compatibility with what once worked.  So to have
    // something that works on, say CentOS 6, latest GCC, and FreeBSD, you need
    // these extra bits of code.

    template <class T>
    struct class_wrapper0
    {
        typedef void (T::*func)();
        class_wrapper0(func f, T* t)
            : m_func(f), m_ptr(t) {}
        class_wrapper0(const class_wrapper0& o)
            : m_func(o.m_func), m_ptr(o.m_ptr) {}
        void operator () () { (m_ptr->*m_func)(); }
        func m_func;
        T* m_ptr;
    };

    template <class T>
    class_wrapper0<T>
    make_obj_func(typename class_wrapper0<T>::func f, T* t)
    {
        return class_wrapper0<T>(f, t);
    }

    template <class T, typename A1>
    struct class_wrapper1
    {
        typedef void (T::*func)(A1 a1);
        class_wrapper1(func f, T* t, const A1& a1)
            : m_func(f), m_ptr(t), m_a1(a1) {}
        class_wrapper1(const class_wrapper1& o)
            : m_func(o.m_func), m_ptr(o.m_ptr), m_a1(o.m_a1) {}
        void operator () () { (m_ptr->*m_func)(m_a1); }
        func m_func;
        T* m_ptr;
        A1 m_a1;
    };

    template <class T, typename A1>
    class_wrapper1<T, A1>
    make_obj_func(typename class_wrapper1<T, A1>::func f, T* t, const A1& a1)
    {
        return class_wrapper1<T, A1>(f, t, a1);
    }

    template <typename A1>
    struct func_wrapper1
    {
        typedef void (*func)(A1 a1);
        func_wrapper1(func f, const A1& a1)
            : m_func(f), m_a1(a1) {}
        func_wrapper1(const func_wrapper1& o)
            : m_func(o.m_func), m_a1(o.m_a1) {}
        void operator () () { m_func(m_a1); }
        func m_func;
        A1 m_a1;
    };

    template <typename A1>
    func_wrapper1<A1>
    make_func(typename func_wrapper1<A1>::func f, const A1& a1)
    {
        return func_wrapper1<A1>(f, a1);
    }

    template <class T, typename A1, typename A2>
    struct class_wrapper2
    {
        typedef void (T::*func)(A1 a1, A2 a2);
        class_wrapper2(func f, T* t, const A1& a1, const A2& a2)
            : m_func(f), m_ptr(t), m_a1(a1), m_a2(a2) {}
        class_wrapper2(const class_wrapper2& o)
            : m_func(o.m_func), m_ptr(o.m_ptr), m_a1(o.m_a1), m_a2(o.m_a2) {}
        void operator () () { (m_ptr->*m_func)(m_a1, m_a2); }
        func m_func;
        T* m_ptr;
        A1 m_a1;
        A2 m_a2;
    };

    template <class T, typename A1, typename A2>
    class_wrapper2<T, A1, A2>
    make_obj_func(typename class_wrapper2<T, A1, A2>::func f, T* t, const A1& a1, const A2& a2)
    {
        return class_wrapper2<T, A1, A2>(f, t, a1, a2);
    }

    template <typename A1, typename A2>
    struct func_wrapper2
    {
        typedef void (*func)(A1 a1, A2 a2);
        func_wrapper2(func f, const A1& a1, const A2& a2)
            : m_func(f), m_a1(a1), m_a2(a2) {}
        func_wrapper2(const func_wrapper2& o)
            : m_func(o.m_func), m_a1(o.m_a1), m_a2(o.m_a2) {}
        void operator () () { m_func(m_a1, m_a2); }
        func m_func;
        A1 m_a1;
        A2 m_a2;
    };

    template <typename A1, typename A2>
    func_wrapper2<A1, A2>
    make_func(typename func_wrapper2<A1, A2>::func f, const A1& a1, const A2& a2)
    {
        return func_wrapper2<A1, A2>(f, a1, a2);
    }

    template <class T, typename A1, typename A2, typename A3>
    struct class_wrapper3
    {
        typedef void (T::*func)(A1 a1, A2 a2, A3 a3);
        class_wrapper3(func f, T* t, const A1& a1, const A2& a2, const A3& a3)
            : m_func(f), m_ptr(t), m_a1(a1), m_a2(a2), m_a3(a3) {}
        class_wrapper3(const class_wrapper3& o)
            : m_func(o.m_func), m_ptr(o.m_ptr), m_a1(o.m_a1), m_a2(o.m_a2), m_a3(o.m_a3) {}
        void operator () () { (m_ptr->*m_func)(m_a1, m_a2, m_a3); }
        func m_func;
        T* m_ptr;
        A1 m_a1;
        A2 m_a2;
        A3 m_a3;
    };

    template <class T, typename A1, typename A2, typename A3>
    class_wrapper3<T, A1, A2, A3>
    make_obj_func(typename class_wrapper3<T, A1, A2, A3>::func f, T* t, const A1& a1, const A2& a2, const A3& a3)
    {
        return class_wrapper3<T, A1, A2, A3>(f, t, a1, a2, a3);
    }

    template <typename A1, typename A2, typename A3>
    struct func_wrapper3
    {
        typedef void (*func)(A1 a1, A2 a2, A3 a3);
        func_wrapper3(func f, const A1& a1, const A2& a2, const A3& a3)
            : m_func(f), m_a1(a1), m_a2(a2), m_a3(a3) {}
        func_wrapper3(const func_wrapper3& o)
            : m_func(o.m_func), m_a1(o.m_a1), m_a2(o.m_a2), m_a3(o.m_a3) {}
        void operator () () { m_func(m_a1, m_a2, m_a3); }
        func m_func;
        A1 m_a1;
        A2 m_a2;
        A3 m_a3;
    };

    template <typename A1, typename A2, typename A3>
    func_wrapper3<A1, A2, A3>
    make_func(typename func_wrapper3<A1, A2, A3>::func f, const A1& a1, const A2& a2, const A3& a3)
    {
        return func_wrapper3<A1, A2, A3>(f, a1, a2, a3);
    }

    

    class ipaddr
    {
        public:
            static ipaddr ANY() { return ipaddr(INADDR_ANY); }
            static uint64_t hash(const ipaddr& ip);

        public:
            ipaddr();
            explicit ipaddr(const in_addr& ipv4);
            explicit ipaddr(const in_addr_t& ipv4);
            explicit ipaddr(const in6_addr& ipv6);
            ipaddr(const ipaddr& other);
            ~ipaddr() throw ();

        public:
            int family() const { return m_family; }
            void pack(struct sockaddr* addr, socklen_t* addrlen, in_port_t port) const;
            void pack(struct sockaddr_in* addr, in_port_t port) const;
            void pack(struct sockaddr_in6* addr, in_port_t port) const;
            const in_addr& v4addr() const { assert(m_family == AF_INET); return m_ip.v4; }
            const in6_addr& v6addr() const { assert(m_family == AF_INET6); return m_ip.v6; }
            int compare(const ipaddr& rhs) const;

        public:
            bool set(const char* addr);
            bool set(const std::string& s);
            void set(const in_addr& ipv4);
            void set(const in6_addr& ipv6);

        public:
            ipaddr& operator = (const ipaddr& rhs);
            bool operator < (const ipaddr& rhs) const;
            bool operator <= (const ipaddr& rhs) const;
            bool operator == (const ipaddr& rhs) const;
            bool operator != (const ipaddr& rhs) const;
            bool operator >= (const ipaddr& rhs) const;
            bool operator > (const ipaddr& rhs) const;

            
        private:
            // doesn't allow access??
            //friend std::ostream& operator << (std::ostream& lhs, const ipaddr& rhs);

        private:
            int m_family;
            union
            {
                in_addr v4;
                in6_addr v6;
            } m_ip;
    };
    /*
    std::ostream& operator << (std::ostream& lhs, const ipaddr& rhs);

    std::istream& operator >> (std::istream& lhs, ipaddr& rhs);
    */
    
    class location
    {
        public:
            static uint64_t hash(const location& loc);

        public:
            location();
            explicit location(const ipaddr& address);
            location(const ipaddr& address, in_port_t port);
            explicit location(const sockaddr_in* sa);
            explicit location(const sockaddr_in6* sa);
            location(const location& other);
            ~location() throw ();

        public:
            void pack(struct sockaddr* addr, socklen_t* addrlen) const;
            bool set(const char* address);
            bool set(const char* address, in_port_t port);
            bool set(const sockaddr* sa, socklen_t salen);
            void set(const sockaddr_in* sa);
            void set(const sockaddr_in6* sa);

        public:
            location& operator = (const location& rhs);
            bool operator < (const location& rhs) const;
            bool operator <= (const location& rhs) const;
            bool operator == (const location& rhs) const;
            bool operator != (const location& rhs) const;
            bool operator >= (const location& rhs) const;
            bool operator > (const location& rhs) const;

        public:
            ipaddr address;
            in_port_t port;

        private:
            int compare(const location& rhs) const;
    };

    class socket;
    class hostname
    {
        public:
            hostname();
            hostname(const char* _address, in_port_t _port);
            explicit hostname(const location&);
            hostname(const hostname& other);
            ~hostname() throw ();

        public:
            location connect(int domain, int type, int protocol, socket* sock) const;
            // non-throwing, non-connecting version
            location lookup(int type, int protocol) const;

        public:
            bool operator < (const hostname& rhs) const;
            bool operator <= (const hostname& rhs) const;
            bool operator == (const hostname& rhs) const;
            bool operator != (const hostname& rhs) const;
            bool operator >= (const hostname& rhs) const;
            bool operator > (const hostname& rhs) const;

        public:
            std::string address;
            in_port_t port;

        private:
            int compare(const hostname& rhs) const;
    };

    std::ostream& operator << (std::ostream& lhs, const hostname& rhs);


    class socket : public fd
    {
        public:
            socket();
            explicit socket(int fd);
            ~socket() throw ();

        public:
            bool reset(int domain, int type, int protocol);

            bool bind(const ipaddr& addr, in_port_t port);
            bool bind(const ipaddr& addr);
            bool bind(const location& loc);
            bool connect(const ipaddr& addr, in_port_t port);
            bool connect(const location& loc);
            bool listen(int backlog);
            bool accept(socket* newsock);
            bool shutdown(int how);

            bool getpeername(location* loc);
            bool getsockname(location* loc);

            bool set_sockopt(int level, int optname,const void *optval, socklen_t optlen);
            bool set_reuseaddr();
            bool set_tcp_nodelay();
            bool sndbuf(size_t size);
            bool rcvbuf(size_t size);
            bool sndlowat(size_t size);
            bool rcvlowat(size_t size);

            ssize_t recv(void *buf, size_t len, int flags);
            ssize_t xrecv(void *buf, size_t len, int flags);
            ssize_t send(const void *buf, size_t len, int flags);
            ssize_t xsend(const void *buf, size_t len, int flags);

        public:
            socket& operator = (int f);

        private:
            socket(const socket&);
            socket& operator = (const socket&);
    };

    std::ostream&
    operator << (std::ostream& lhs, const location& rhs);
}