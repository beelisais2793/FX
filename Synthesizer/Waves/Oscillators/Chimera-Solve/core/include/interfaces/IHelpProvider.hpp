#ifndef IHELPPROVIDER_H
#define IHELPPROVIDER_H

namespace chimera {
    class IHelpProvider
    {
        public:
            virtual const std::string getHelp() const = 0;
            virtual const std::string getHelp(const std::string& selector) const = 0;
    };
}

#endif // IHELPPROVIDER_H
