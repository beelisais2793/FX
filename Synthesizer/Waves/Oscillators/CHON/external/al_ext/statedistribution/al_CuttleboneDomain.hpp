#ifndef CUTTLEBONEDOMAIN_H
#define CUTTLEBONEDOMAIN_H

#include <cassert>
#include <cstring>
#include <functional>
#include <iostream>
#include <memory>
#include <vector>

#include "al/app/al_App.hpp"
#include "al/app/al_ComputationDomain.hpp"
#include "al/app/al_StateDistributionDomain.hpp"
#include "al/spatial/al_Pose.hpp"

#ifdef AL_USE_CUTTLEBONE
#include "Cuttlebone/Cuttlebone.hpp"
#endif

namespace al {

template <class TSharedState, unsigned PACKET_SIZE, unsigned PORT>
class CuttleboneReceiveDomain;

template <class TSharedState, unsigned PACKET_SIZE, unsigned PORT>
class CuttleboneSendDomain;

/**
 * @brief CuttleboneDomain class
 * @ingroup App
 */
template <class TSharedState = DefaultState, unsigned PACKET_SIZE = 1400,
          unsigned PORT = 63059>
class CuttleboneDomain : public StateDistributionDomain<TSharedState> {
 public:
  std::shared_ptr<CuttleboneSendDomain<TSharedState, PACKET_SIZE, PORT>>
  addStateSender(std::string id = "",
                 std::shared_ptr<TSharedState> statePtr = nullptr);

  std::shared_ptr<CuttleboneReceiveDomain<TSharedState, PACKET_SIZE, PORT>>
  addStateReceiver(std::string id = "",
                   std::shared_ptr<TSharedState> statePtr = nullptr);

  static std::shared_ptr<CuttleboneDomain> enableCuttlebone(App *app) {
#ifdef AL_USE_CUTTLEBONE
    auto cbDomain =
        app->graphicsDomain()
            ->newSubDomain<CuttleboneDomain<TSharedState, PACKET_SIZE, PORT>>(
                true);
    cbDomain->init(app->graphicsDomain().get());
    return cbDomain;
#else
    (void)app;
    std::cout << "Cuttlebone support not available. Ignoring enableCuttlebone()"
              << std::endl;
    return nullptr;
#endif
  }

 private:
};

template <class TSharedState = DefaultState, unsigned PACKET_SIZE = 1400,
          unsigned PORT = 63059>
class CuttleboneReceiveDomain : public StateReceiveDomain<TSharedState> {
 public:
  bool init(ComputationDomain *parent = nullptr) override;

  bool tick() override {
    this->tickSubdomains(true);

    assert(this->mState);  // State must have been set at this point
#ifdef AL_USE_CUTTLEBONE
    assert(mTaker);
    this->mQueuedStates = mTaker->get(*(this->mState.get()));
    return true;
#else
    return false;
#endif
  }

  bool cleanup(ComputationDomain * /*parent*/ = nullptr) override {
    this->cleanupSubdomains(true);
#ifdef AL_USE_CUTTLEBONE
    mTaker->stop();
    mTaker = nullptr;
    this->cleanupSubdomains(false);
    return true;
#else
    //    mState = nullptr;
    return false;
#endif
  }

 private:
#ifdef AL_USE_CUTTLEBONE
  std::unique_ptr<cuttlebone::Taker<TSharedState, PACKET_SIZE, PORT>> mTaker;
#endif
};

template <class TSharedState, unsigned PACKET_SIZE, unsigned PORT>
bool CuttleboneReceiveDomain<TSharedState, PACKET_SIZE, PORT>::init(
    ComputationDomain *parent) {
  this->initializeSubdomains(true);
  assert(parent != nullptr);

#ifdef AL_USE_CUTTLEBONE
  if (!mTaker) {
    mTaker =
        std::make_unique<cuttlebone::Taker<TSharedState, PACKET_SIZE, PORT>>();
    mTaker->start();
    bool ret = this->initializeSubdomains(false);
    return ret;
  } else {
    std::cout << "CuttleboneSendDomain - already initialized" << std::endl;
    bool ret = this->initializeSubdomains(false);
    return ret;
  }
#else
  return false;
#endif
}

template <class TSharedState = DefaultState, unsigned PACKET_SIZE = 1400,
          unsigned PORT = 63059>
class CuttleboneSendDomain : public StateSendDomain<TSharedState> {
 public:
  bool init(ComputationDomain * /*parent*/ = nullptr) override {
    this->initializeSubdomains(true);

#ifdef AL_USE_CUTTLEBONE
    if (!mMaker) {
      mMaker =
          std::make_unique<cuttlebone::Maker<TSharedState, PACKET_SIZE, PORT>>(
              this->mAddress.c_str());
      mMaker->start();
      bool ret = this->initializeSubdomains(false);
      return ret;
    } else {
      std::cout << "CuttleboneSendDomain - already initialized" << std::endl;
      this->initializeSubdomains(false);
      return true;
    }
#else
    return false;
#endif
  }

  bool tick() override {
    this->tickSubdomains(true);

    assert(this->mState);  // State must have been set at this point
#ifdef AL_USE_CUTTLEBONE
    assert(mMaker);
    mMaker->set(*(this->mState.get()));
    this->tickSubdomains(false);
    return true;
#else
    return false;
#endif
  }

  bool cleanup(ComputationDomain * /*parent*/ = nullptr) override {
    this->cleanupSubdomains(true);
#ifdef AL_USE_CUTTLEBONE
    if (mMaker) {
      mMaker->stop();
      mMaker = nullptr;
    }
    this->cleanupSubdomains(false);
    return true;
#else
    //    mState = nullptr;
    return false;
#endif
  }

 private:
#ifdef AL_USE_CUTTLEBONE
  std::unique_ptr<cuttlebone::Maker<TSharedState, PACKET_SIZE, PORT>> mMaker;
#endif
};

template <class TSharedState, unsigned PACKET_SIZE, unsigned PORT>
std::shared_ptr<CuttleboneSendDomain<TSharedState, PACKET_SIZE, PORT>>
CuttleboneDomain<TSharedState, PACKET_SIZE, PORT>::addStateSender(
    std::string id, std::shared_ptr<TSharedState> statePtr) {
  auto newDomain = this->template newSubDomain<
      CuttleboneSendDomain<TSharedState, PACKET_SIZE, PORT>>(false);
  newDomain->setId(id);
  newDomain->setStatePointer(statePtr);
  this->mIsSender = true;
  return newDomain;
}

template <class TSharedState, unsigned PACKET_SIZE, unsigned PORT>
std::shared_ptr<CuttleboneReceiveDomain<TSharedState, PACKET_SIZE, PORT>>
CuttleboneDomain<TSharedState, PACKET_SIZE, PORT>::addStateReceiver(
    std::string id, std::shared_ptr<TSharedState> statePtr) {
  auto newDomain = this->template newSubDomain<
      CuttleboneReceiveDomain<TSharedState, PACKET_SIZE, PORT>>(true);
  newDomain->setId(id);
  newDomain->setStatePointer(statePtr);
  return newDomain;
}

}  // namespace al

#endif  // CUTTLEBONEDOMAIN_H
