#ifndef CONFIGURATION_H
#define CONFIGURATION_H

class Configuration
{
    public:
        Configuration();
        virtual ~Configuration();
        void load(const std::string& path);
        void addLoader(std::string name, chimera::runtime::IModulePathProvider* item);
        void setLogLevel(const unsigned int& loglevel);

        std::vector<std::pair<std::string, chimera::runtime::IModulePathProvider*> >* getLoaders();
        unsigned int getLogLevel();
    protected:
    private:
        unsigned int _loglevel;
        std::vector<std::pair<std::string, chimera::runtime::IModulePathProvider*> >* _pathLookup;
};

#endif // CONFIGURATION_H
