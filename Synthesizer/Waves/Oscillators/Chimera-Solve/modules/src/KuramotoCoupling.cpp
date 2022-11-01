#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <list>
#include <stdlib.h>
#include <boost/numeric/ublas/vector.hpp>

#include "def.hpp"
#include "Naming.hpp"
#include "ExtensionNaming.hpp"
#include "StateSynchrony.hpp"
#include "interfaces/ILogger.hpp"
#include "LoggingSystem.hpp"
#include "ParameterValue.hpp"
#include "ParameterType.hpp"
#include "extendTypes.hpp"
#include "types/LuaFunctionWrapper.hpp"
#include "ParameterTypeSystem.hpp"
#include "ParameterValueCollection.hpp"
#include "Module.hpp"
#include "EntryPoint.hpp"
#include "EntryPointSystem.hpp"
#include "ChimeraSystem.hpp"
#include "interfaces/IEventListener.hpp"
#include "interfaces/IEventListenerProvider.hpp"
//#include "event/Observer.hpp"
//#include "event/StateEventListener.hpp"
//#include "NotificationManager.hpp"
#include "EntryPointBase/AbstractSystemDynamic.hpp"
#include "EntryPointBase/TemplateOdeSystem.hpp"
#include "Network/INetwork.hpp"
#include "Network/AbstractCoupling.hpp"
#include "Network/TemplateCoupling.hpp"
#include "Network/CouplingModule.hpp"
#include "KuramotoCoupling.hpp"

extern "C"
{
    chimera::Module* LoadModule()
    {
        return new KuramotoCouplingModule();
    }

    void UnloadModule(chimera::Module* reg)
    {
        delete (KuramotoCouplingModule*)reg;
    }
}

KuramotoCouplingModule::KuramotoCouplingModule()
{
    //ctor
}

KuramotoCouplingModule::~KuramotoCouplingModule()
{
    //dtor
}

const std::string KuramotoCouplingModule::getGUID() const
{
    return "Kuramoto-Coupling";
}

AbstractCoupling* KuramotoCouplingModule::getCoupling(chimera::vec_t_LuaItem& parameters) const
{
    double sigma;
    double alpha;
    double beta;
    double epsilon;

    if(parameters.size() == 1 && parameters[0].getType() == chimera::systemtypes::PID_TABLE)
    {
        bool inSigma = false;
        bool inAlpha = false;
        bool inBeta = false;
        bool inEpsilon = false;
        chimera::map_t_LuaItem* paramMap = (chimera::map_t_LuaItem*)parameters[0].getValue();
        for(auto p : *paramMap)
        {
            if(p.first == "sigma" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inSigma = true;
                sigma = p.second;
            }
            if(p.first == "alpha" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inAlpha = true;
                alpha = p.second;
            }
            if(p.first == "beta" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inBeta = true;
                beta = p.second;
            }
            if(p.first == "epsilon" && p.second.getType() == chimera::systemtypes::PID_NUMBER)
            {
                inEpsilon = true;
                epsilon = p.second;
            }
        }
        if(!(inSigma && inAlpha && inBeta && inEpsilon))
        {
            return nullptr;
        }
        return new KuramotoCoupling(sigma, alpha, beta, epsilon);
    }
    else if(parameters.size() >= 4 &&
            parameters[0].getType() == chimera::systemtypes::PID_NUMBER &&
            parameters[1].getType() == chimera::systemtypes::PID_NUMBER &&
            parameters[2].getType() == chimera::systemtypes::PID_NUMBER &&
            parameters[3].getType() == chimera::systemtypes::PID_NUMBER)
    {
        sigma = parameters[0];
        alpha = parameters[1];
        beta = parameters[2];
        epsilon = parameters[3];
        return new KuramotoCoupling(sigma, alpha, beta, epsilon);
    }

    return nullptr;
}

KuramotoCoupling::KuramotoCoupling(double sigma, double alpha, double beta, double epsilon):
    _sigma(sigma),
    _alpha(alpha),
    _beta(beta),
    _epsilon(epsilon)
{
    _sysCache = nullptr;
    _sinA = sin(alpha);
    _cosA = cos(alpha);
    _sinB = sin(beta);
    _cosB = cos(beta);
}

KuramotoCoupling::~KuramotoCoupling()
{

}

void KuramotoCoupling::recalcSinCos(const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const int& N)
{
    for(int i = 0; i < N; i++)
    {
        _sinCache[i] = sin(system[i][0]);
        _cosCache[i] = cos(system[i][0]);
    }
}

void KuramotoCoupling::operator()(const INetwork<double, boost::numeric::ublas::vector<double> >* network, const boost::numeric::ublas::vector<boost::numeric::ublas::vector<double> >& system, const double& t, const int& i, boost::numeric::ublas::vector<double>& out)
{
std::cout.precision(17);
    static int N;
    static void* sysPtr;
    static double Ksins;
    static double Kcoss;
    static double sini;
    static double cosi;
    static double sinj;
    static double cosj;

    sysPtr = (void*)&system;
    N = system.size();

    if(sysPtr != _sysCache || t != _timeCache)
    {
        _sinCache.resize(N);
        _cosCache.resize(N);
        recalcSinCos(system, N);
    }
/*
    Ksins = 0.0;
    Kcoss = 0.0;
    for(int i2 = 0; i2 < N; i2++)
    {
    std::cout << "KSINS[ " << i2 << "] = " << _sinCache[i2] << std::endl;
        Ksins += system[i][i2 + 1] * _sinCache[i];
        Kcoss += system[i][i2 + 1] * _cosCache[i];
    }
    std::cout << "Ksins: " << Ksins << "  Kcoss: " << Kcoss << std::endl;
*/
    sini = _sinCache[i];
    cosi = _cosCache[i];
    for(int j = 0; j < N; j++)
    {
        sinj = _sinCache[j];
        //std::cout << "phi(" << j << ")\t"<< system[j][0] << std::endl;
        cosj = _cosCache[j];

        out[0] = out[0] - system[i][j + 1] * (_cosA * (sini * cosj - cosi * sinj) + _sinA * (cosi * cosj + sini * sinj));
        //out[0] = -system[i][j + 1] * (sinj * (sini * _sinA - cosi * _cosA) + cosj * (sini * _cosA + cosi * _sinA));
        //out[0] = -system[i][j + 1] * (sini * cosj * _cosA + sinj*_cosA*cosi + _sinA*cosj*cosi -sini*sinj*_sinA);
        //out[0] = -system[i][j + 1] * (_cosA * (sini * cosj - cosi * sinj) + _sinA * (cosi * cosj + sini * sinj));

        out[j + 1] = - _epsilon * ((_cosB * (sini * cosj - cosi * sinj) + _sinB * (cosi * cosj + sini * sinj)) + system[i][j + 1]);
        //out[j + 1] = - _epsilon * ((sini * (sinj * _sinB - cosj * _cosB) + cosi * (sinj * _cosB + cosj * _sinB)) + system[i][j + 1]);
        //out[j + 1] = - _epsilon * (sini*sinj*_cosB + sinj*_cosB*cosi + _sinB*cosj*cosi  - sini*sinj*_sinB + system[i][j + 1]);
    }
    //int x;
    //std::cin >> x;
    out[0] = _sigma / N * out[0];

/*
    for(int j = 0; j <= N; j++)
    {
        std::cout << t << "\t" << i << " " << j << "\t" << out[j] << std::endl;
    }
    int age;
std::cin >> age;
*/

}
