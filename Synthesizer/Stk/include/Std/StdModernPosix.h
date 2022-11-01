#pragma once

#define MP_NO_THROW

#ifdef MP_NO_THROW
    #define MP_RETURN_OR_THROW_EX(retval, type, ...) return retval
#else
    #include <stdexcept>
    #define MP_RETURN_OR_THROW_EX(retval, type, ...) throw type(__VA_ARGS__)
#endif

#include <dirent.h>
#include <cstring>
#include <cerrno>
#include <cstdio>
#include <set>
#include <string>
#include <dlfcn.h>
#include <vector>
#include <linux/limits.h>
#include <unistd.h>

namespace Std
{
    class dstream // aka directory stream
    {
    public: // D'tors
        virtual ~dstream()
        {
            try
            {
                close();
            }
            catch (std::runtime_error & ex)
            {
                // Prevents exception from leaving destructor
            }
        }

    public: // Operators
        inline operator bool() const
        {
            return _active;
        }

    public: // Methods
        int close()
        {
            if (NULL == _fd)
            {
                // We are already closed
                return 0;
            }

            int exitcode = closedir(_fd);
            if (0 != exitcode)
            {
                MP_RETURN_OR_THROW_EX(exitcode, std::runtime_error, error());
            }

            _fd = NULL;
            _active = false;
            
            return exitcode;
        }

        void rewind()
        {
            rewinddir(_fd);
        }

    protected: // C'tors
        dstream(const std::string & dirname)
            : _fd(NULL)
        {
            _active = open(dirname);
        }

    protected: // Methods
        inline std::string error() const
        {
            return std::string(strerror(errno));
        }

    protected: // Members
        DIR * _fd;
        bool  _active;

    private: // Methods
        inline bool open(const std::string & dirname)
        {
            _fd = opendir(dirname.c_str());
            if (NULL == _fd)
            {
                MP_RETURN_OR_THROW_EX(false, std::runtime_error, error());
            }

            return true;
        }
    };

    class idstream : public dstream
    {
    public: // C'tors
        idstream(const std::string & dirname)
            : dstream(dirname)
        {
            // Do nothing
        }

    public: // Operators
        // Read line by line
        // Order is not guaranteed
        idstream & operator>>(std::string & name)
        {
            if (_active)
            {
                const struct dirent * de = readdir(_fd);
                _active = (de != NULL);
                if (_active)
                {
                    name = de->d_name;
                }
            }
            
            return *this;
        }

        // Read all at once
        // Order is guaranteed to be alphabetical
        idstream & operator>>(std::set<std::string> & output)
        {
            std::string name;
            while (*this >> name)
            {
                output.insert(name);
            }

            return *this;
        }
    };

    class dynamiclib
    {
    public: // C'tors & D'tors
        dynamiclib(const std::string & file, int mode = RTLD_LAZY | RTLD_LOCAL)
            : _handle(NULL)
        {
            open(file, mode);
        }

        virtual ~dynamiclib()
        {
            close();
        }

    public: // Operators
        inline operator bool() const
        {
            return (NULL != _handle);
        }

    public: // Methods
        inline bool close()
        {
            if (NULL == _handle)
            {
                return true;
            }

            if (dlclose(_handle) != 0)
            {
                MP_RETURN_OR_THROW_EX(false, std::runtime_error, "Underlying dlclose failed");
            }
            
            _handle = NULL;
            return true;
        }

        inline std::string error()
        {
            const char * err = dlerror();
            return (NULL != err) ? err : "Unknown underlying error or error was already read";
        }

        inline void * symbol(const std::string & name)
        {
            if (NULL == _handle)
            {
                MP_RETURN_OR_THROW_EX(NULL, std::runtime_error, "Library not loaded");
            }

            void * addr = dlsym(_handle, name.c_str());
            if (NULL == addr)
            {
                MP_RETURN_OR_THROW_EX(NULL, std::runtime_error, error());
            }

            return addr;
        }

    private: // Methods
        inline bool open(const std::string & file, int mode)
        {
            _handle = dlopen(file.c_str(), mode);
            if (NULL == _handle)
            {
                MP_RETURN_OR_THROW_EX(false, std::runtime_error, error());
            }

            return true;
        }

    private: // Members
        void * _handle;
    };

    class pstream // aka process stream
    {
    public: // D'tors
        virtual ~pstream()
        {
            try
            {
                close();
            }
            catch (std::runtime_error & ex)
            {
                // Prevents exception from leaving destructor
            }
        }

    public: // Operators
        inline operator bool() const
        {
            return _active;
        }

    public: // Methods
        int close()
        {
            if (NULL == _fd)
            {
                // We are already closed
                return 0;
            }

            int exitcode = pclose(_fd);
            if (-1 == exitcode)
            {
                MP_RETURN_OR_THROW_EX(exitcode, std::runtime_error, error());
            }

            _fd = NULL;
            _active = false;
            
            return exitcode;
        }

    protected: // C'tors
        pstream(const std::string & command, const std::string & type)
            : _fd(NULL)
        {
            _active = open(command, type);
        }

    protected: // Methods
        inline std::string error() const
        {
            return std::string(strerror(errno));
        }

    protected: // Members
        FILE * _fd;
        bool   _active;

    private: // Methods
        inline bool open(const std::string & command, const std::string & type)
        {
            _fd = popen(command.c_str(), type.c_str());
            if (NULL == _fd)
            {
                MP_RETURN_OR_THROW_EX(false, std::runtime_error, error());
            }

            return true;
        }
    };

    class ipstream : public pstream
    {
    public: // C'tors
        ipstream(const std::string & command, size_t bufferSize = DEFAULT_BUFFER_SIZE)
            : pstream(command, "r"), _bufferSize(bufferSize)
        {
            // Do nothing
        }

    public: // Operators
        ipstream & operator>>(std::string & line)
        {
            if (_active)
            {
                char buffer[_bufferSize];
                _active = (fgets(buffer, sizeof(buffer), _fd) != NULL);
                if (_active)
                {
                    line.assign(buffer);
                }
            }
            
            return *this;
        }

        ipstream & operator>>(std::vector<std::string> & output)
        {
            std::string line;
            while (*this >> line)
            {
                output.push_back(line);
            }

            return *this;
        }

    private: // Consts
        static const size_t DEFAULT_BUFFER_SIZE = 1024;

    private: // Members
        const size_t _bufferSize;
    };

    class opstream : public pstream
    {
    public: // C'tors
        opstream(const std::string & command)
            : pstream(command, "w")
        {
            // Do nothing
        }

    public: // Operators
        opstream & operator<<(const char * buffer)
        {
            if (_active)
            {
                _active = (fputs(buffer, _fd) != EOF);
            }

            return *this;
        }

        opstream & operator<<(std::string & buffer)
        {
            return *this << buffer.c_str();
        }
    };

    class workdir
    {
    public:
        static bool getcwd(std::string & dir)
        {
            char buffer[PATH_MAX];
            if (::getcwd(buffer, sizeof(buffer)) == NULL)
            {
                MP_RETURN_OR_THROW_EX(false, std::runtime_error,
                                    "Current working directory path longer than PATH_MAX");
            }
            
            dir.assign(buffer);
            return true;
        }

        static bool chdir(const std::string & dir)
        {
            if (::chdir(dir.c_str()) != 0)
            {
                MP_RETURN_OR_THROW_EX(false, std::runtime_error,
                                    "Change directory failed");
            }

            return true;
        }

        static bool mkdir(const std::string & dir) {
            return std::mkdir(dir);
        }
        static int system(const std::string & cmd) {
            return std::system(cmd);
        }

        static std::string error()
        {
            return std::string(strerror(errno));
        }
    };
}