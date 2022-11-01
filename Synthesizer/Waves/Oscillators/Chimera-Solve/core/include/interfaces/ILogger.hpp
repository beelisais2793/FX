#ifndef ILOGGER_H
#define ILOGGER_H

namespace chimera {
    class ILogger
    {
        public:
            ILogger() {}
            virtual ~ILogger() {}
            virtual void LogMessage(const std::string& message) = 0;
            virtual bool bouble() const = 0;
        protected:
        private:
    };
}

#endif // ILOGGER_H
