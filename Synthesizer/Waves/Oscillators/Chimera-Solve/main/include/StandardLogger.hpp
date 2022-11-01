#ifndef STANDARDLOGGER_H
#define STANDARDLOGGER_H


class StandardLogger:public chimera::ILogger
{
    public:
        StandardLogger();
        virtual ~StandardLogger();
        virtual void LogMessage(const std::string& message) override;
        virtual bool bouble() const override;
    protected:
    private:
};


#endif // STANDARDLOGGER_H
