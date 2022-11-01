#ifndef ABSTRACTSYSTEMDYNAMIC_H
#define ABSTRACTSYSTEMDYNAMIC_H

namespace chimera {
    namespace simulation {
        class AbstractSystemDynamic
        {
            public:
                AbstractSystemDynamic();
                virtual ~AbstractSystemDynamic();
                virtual std::unordered_map<std::string, size_t> getFeatures() const = 0;
                virtual const std::string getSystemName() const = 0;
            protected:
            private:
        };
    }
}

#endif // ABSTRACTSYSTEMDYNAMIC_H
