#ifndef CSV_FILE_H
#define CSV_FILE_H

struct T_TimeStateArgs
{
    void* time;
    void* state;
};

struct T_StateProviderArgs {
    size_t time_type;
    size_t state_type;
};

class CsvFileModule:
    public chimera::simulation::OutputMultiModule
{
    public:
        CsvFileModule();
        virtual ~CsvFileModule();
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
        virtual chimera::simulation::IEventListenerProvider* getOutputInstance(chimera::vec_t_LuaItem& parameters) const override;
        virtual const std::string getGUID() const override;
    protected:
    private:
};

class CsvFileWriter:
    public chimera::simulation::IEventListenerProvider
{
    public:
        CsvFileWriter(chimera::ChimeraContext* context, const std::string& path);
        virtual ~CsvFileWriter();
        void open();
        void close();
        const std::string getPath() const;
        virtual chimera::simulation::IEventListener* provideListener(size_t id, void const * const args) override;
        //virtual void notifyStep(void const * const sender, size_t timeType, void* time, size_t stateType, void* state) override;
    protected:
    private:
        chimera::ChimeraContext* _context;
        std::ofstream* _file;
        const std::string _path;

};

class FileOutput_real_vecReal:
    public chimera::simulation::IEventListener
{
    public:
        FileOutput_real_vecReal(std::ofstream* file);
        virtual ~FileOutput_real_vecReal();
        virtual void notify(chimera::simulation::NotificationManager const * const sender, void const * const args) override;
    private:
        std::ofstream* _file;
};

class FileOutput_real_vecvecReal:
    public chimera::simulation::IEventListener
{
    public:
        FileOutput_real_vecvecReal(std::ofstream* file);
        virtual ~FileOutput_real_vecvecReal();
        virtual void notify(chimera::simulation::NotificationManager const * const sender, void const * const args) override;
    private:
        std::ofstream* _file;
};

class FileOutput_real_vecComplex:
    public chimera::simulation::IEventListener
{
    public:
        FileOutput_real_vecComplex(std::ofstream* file);
        virtual ~FileOutput_real_vecComplex();
        virtual void notify(chimera::simulation::NotificationManager const * const sender, void const * const args) override;
    private:
        std::ofstream* _file;
};

class FileOutput_real_vecvecComplex:
    public chimera::simulation::IEventListener
{
    public:
        FileOutput_real_vecvecComplex(std::ofstream* file);
        virtual ~FileOutput_real_vecvecComplex();
        virtual void notify(chimera::simulation::NotificationManager const * const sender, void const * const args) override;
    private:
        std::ofstream* _file;
};

#endif // CSV_FILE_H
