#ifndef TYPELOOKUP_H
#define TYPELOOKUP_H

namespace chimera {
    namespace runtime {

        class TypeLookup:
            protected StateSynchrony
        {
            friend class ChimeraRuntime;
            public:
                TypeLookup(ParameterTypeSystem* typeSys);
                virtual ~TypeLookup();
                virtual const std::string getGUID() const override;
                size_t findType(size_t baseType, size_t innerType) const;
            protected:
                virtual void notifyItemAdded(StateSynchrony* sender, void* item, void const * const data) override;
                virtual void notifyItemRemoved(StateSynchrony* sender, void* item, void const * const data) override;
            private:
                ParameterTypeSystem* _typeSys;
                void addTypeLookup(size_t baseType) const;
                std::unordered_map<size_t, std::unordered_map<size_t, size_t> >* _lookup;
        };
    }
}

#endif // TYPELOOKUP_H
