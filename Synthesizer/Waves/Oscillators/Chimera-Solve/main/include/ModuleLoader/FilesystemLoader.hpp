#ifndef FILESYSTEMLOADER_H
#define FILESYSTEMLOADER_H


class FilesystemLoader:
    public chimera::runtime::IModulePathProvider
{
    public:
        FilesystemLoader(const std::string& directory);
        virtual ~FilesystemLoader();
        virtual const std::string getFileName(const std::string& moduleName) override;
    protected:
    private:
        std::string _directory;
};

#endif // FILESYSTEMLOADER_H
