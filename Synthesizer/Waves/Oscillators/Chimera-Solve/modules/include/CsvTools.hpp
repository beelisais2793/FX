#ifndef CSVTOOLS_H
#define CSVTOOLS_H

class CsvToolsModule:
    public chimera::Module
{
    public:
        CsvToolsModule();
        //virtual const std::string getEntrypoint() const override;
        virtual const std::string getGUID() const override;
        virtual void* getInstance(chimera::EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
        virtual void destroyInstance(chimera::EntryPoint const * const entrypoint, void * const instance) const override;
};

class CsvTools
{
    public:
        CsvTools(std::string filename, std::string delimiter);
        virtual ~CsvTools();
        std::string getFilename() const;
    protected:
    private:
        std::string _filename;
        std::string _delimiter;
};

chimera::vec_t_LuaItem fn_CsvTools_load(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params);
chimera::vec_t_LuaItem fn_CsvTools_save(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params);
chimera::vec_t_LuaItem fn_CsvTools_read(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params);
chimera::vec_t_LuaItem fn_CsvTools_write(chimera::EntryPoint const * const entrypoint, chimera::Module const * const module, void* instance, const chimera::vec_t_LuaItem& params);

#endif // CSVTOOLS_H
