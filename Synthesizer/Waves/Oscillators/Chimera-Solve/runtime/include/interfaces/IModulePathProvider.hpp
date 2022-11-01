#ifndef IMODULEPATHPROVIDER_H
#define IMODULEPATHPROVIDER_H

namespace chimera {
    namespace runtime {
        class IModulePathProvider
        {
            public:
                IModulePathProvider() {}
                virtual ~IModulePathProvider() {}
                virtual const std::string getFileName(const std::string& moduleName) = 0;
            protected:
            private:
        };
    }
}
#endif // IMODULEPATHPROVIDER_H
