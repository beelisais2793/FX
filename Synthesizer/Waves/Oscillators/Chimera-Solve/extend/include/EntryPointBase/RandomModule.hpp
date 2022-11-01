#ifndef RANDOMMODULE_H
#define RANDOMMODULE_H

namespace chimera {
    namespace simulation {
        class RandomModule:
            public chimera::Module
        {
            public:
                RandomModule();
                virtual ~RandomModule();
                virtual const size_t getType() const = 0;
                virtual void* getInstance(EntryPoint const * const entrypoint, chimera::vec_t_LuaItem& parameters) const override;
                // TODO (kekstoaster#3#): Replace std::default_random_engin with generic engine
                virtual AbstractRandom* getRandomInstance(AbstractRandomGenerator* generator, chimera::vec_t_LuaItem& parameters) const = 0;
                virtual bool load(EntryPoint const * const entryPoint, void const * const params) override;
            protected:
            private:
                AbstractRandomGenerator* _generator;
        };

        class DefaultRandomGenerator:
            public AbstractRandomGenerator
        {
            public:
                DefaultRandomGenerator();
                virtual ~DefaultRandomGenerator();
                virtual unsigned int min() override;
                virtual unsigned int max() override;
                virtual unsigned int operator()() override;
            private:
                std::default_random_engine* _engine;
        };
    }
}

#endif // RANDOMMODULE_H
