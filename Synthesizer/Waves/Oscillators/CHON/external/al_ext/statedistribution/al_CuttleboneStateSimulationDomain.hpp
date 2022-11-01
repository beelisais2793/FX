#ifndef INCLUDE_AL_CUTTLEBONEAPP_HPP
#define INCLUDE_AL_CUTTLEBONEAPP_HPP

/* Andres Cabrera, 2019, mantaraya36@gmail.com
 */

#include <iostream>
#include <map>

#include "al/app/al_DistributedApp.hpp"
#include "al/app/al_SimulationDomain.hpp"
#include "al_ext/statedistribution/al_CuttleboneDomain.hpp"

#ifdef AL_USE_CUTTLEBONE
#include "Cuttlebone/Cuttlebone.hpp"
#endif

namespace al {

static bool canUseCuttlebone() {
#ifdef AL_USE_CUTTLEBONE
  return true;
#else
  return false;
#endif
}

template <class TSharedState, unsigned PACKET_SIZE = 1400,
          unsigned PORT = 63059>
class CuttleboneStateSimulationDomain
    : public StateDistributionDomain<TSharedState> {
public:
  virtual bool init(ComputationDomain *parent = nullptr) {
    return StateDistributionDomain<TSharedState>::init(parent);
  }

  virtual std::shared_ptr<StateSendDomain<TSharedState>>
  addStateSender(std::string id = "") {
    auto newDomain = this->template newSubDomain<
        CuttleboneSendDomain<TSharedState, PACKET_SIZE, PORT>>(false);
    newDomain->setId(id);
    newDomain->setStatePointer(this->statePtr());
    this->mIsSender = true;
    return newDomain;
  }

  virtual std::shared_ptr<StateReceiveDomain<TSharedState>>
  addStateReceiver(std::string id = "") {
    auto newDomain = this->template newSubDomain<
        CuttleboneReceiveDomain<TSharedState, PACKET_SIZE, PORT>>(true);
    newDomain->setId(id);
    newDomain->setStatePointer(this->statePtr());
    return newDomain;
  }

  static std::shared_ptr<
      CuttleboneStateSimulationDomain<TSharedState, PACKET_SIZE, PORT>>
  enableCuttlebone(DistributedAppWithState<TSharedState> *app) {
    std::shared_ptr<
        CuttleboneStateSimulationDomain<TSharedState, PACKET_SIZE, PORT>>
        cbDomain = app->graphicsDomain()
                       ->template newSubDomain<CuttleboneStateSimulationDomain<
                           TSharedState, PACKET_SIZE, PORT>>(true);
    app->graphicsDomain()->removeSubDomain(app->simulationDomain());
    if (cbDomain) {
      //      cbDomain->A
      app->mSimulationDomain = cbDomain;

      cbDomain->simulationFunction =
          std::bind(&App::onAnimate, app, std::placeholders::_1);
      if (app->isPrimary()) {
        auto sender = cbDomain->addStateSender();
        if (app->additionalConfig.find("broadcastAddress") !=
            app->additionalConfig.end()) {
          sender->setAddress(app->additionalConfig["broadcastAddress"]);
        } else {
          sender->setAddress("127.0.0.1");
        }
        assert(sender);
      } else {
        auto receiver = cbDomain->addStateReceiver();
        assert(receiver);
      }
      if (!cbDomain->init(nullptr)) {
        cbDomain = nullptr;
        return nullptr;
      }

    } else {
      std::cerr << "ERROR creating cuttlebone domain" << std::endl;
    }
    return cbDomain;
  }
};

} // namespace al

#endif // INCLUDE_AL_CUTTLEBONEAPP_HPP
