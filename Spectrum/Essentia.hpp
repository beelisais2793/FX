#pragma once
#include <essentia.essentia.h>
#include <essentia/algorithmfactory.h>
#include <essentia/pool.h>

namespace Essentia
{        
    struct StandardAlgorithm
    {
        essentia::standard::Algorithm * alg;

        StandardAlgorithm() {
            alg = NULL;
        }
        StandardAlgorithm(const StandardAlgorithm & a) {
            alg = a.alg;
        }
        ~StandardAlgorithm() {

        }
        void ProcessBlock(size_t n, float * input, float * output)
        {

        }
    };

    struct AlgorithmFactory
    {
        AlgorithmFactory & factory;
        AlgorithmFactory() : factory(essentia::AlgorithmFactory::instance()) {

        }
        StandardAlgorithm CreateStandardAlgorithm(const std::string& name)
        {
            StandardAlgorithm a;
            a.alg = factory.create(name);
            return a;
        }

        
    };
}