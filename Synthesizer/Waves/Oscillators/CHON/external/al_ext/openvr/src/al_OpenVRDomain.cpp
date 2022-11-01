#include "al_ext/openvr/al_OpenVRDomain.hpp"

using namespace al;

bool OpenVRDomain::init(ComputationDomain *parent) {
#ifdef AL_EXT_OPENVR
  // A graphics context is needed to initialize OpenVR

  if (!mGraphics) {
    mGraphics = std::make_unique<Graphics>();
  }

  std::cerr << "Initializing OpenVR domain" << std::endl;
  if (!mOpenVR.init()) {
    std::cerr << "ERROR: OpenVR init returned error" << std::endl;
    return false;
  }

  mGraphics->init();
  return true;
#else
  std::cerr << "Not building wiht OpenVR support" << std::endl;
  return false;
#endif
}

bool OpenVRDomain::tick() {
#ifdef AL_EXT_OPENVR
  // Update traking and controller data;
  mOpenVR.update();
  if (drawSceneFunc) {
    mOpenVR.draw(drawSceneFunc, *mGraphics);
  }
#endif
  return true;
}

bool OpenVRDomain::cleanup(ComputationDomain *parent) {
#ifdef AL_EXT_OPENVR
  mOpenVR.close();
#endif
  return true;
}
