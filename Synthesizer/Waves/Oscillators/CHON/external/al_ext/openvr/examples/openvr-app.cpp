
// These includes are all that's needed to turn a regular App
// into a VRApp
// You will also need to specify the location of OpenVR in the
// flags.cmake file

#include "al/app/al_App.hpp"

#include "al/graphics/al_Shapes.hpp"

#include "al_ext/openvr/al_OpenVRDomain.hpp"

using namespace al;

struct MyApp : public App {
  std::shared_ptr<OpenVRDomain> openVRDomain;
  VAOMesh mCube;

  void onCreate() override {
    addCube(mCube);
    mCube.primitive(Mesh::LINE_STRIP);
    mCube.update();
    openVRDomain = OpenVRDomain::enableVR(this);
    // The openVR domain will use the app's onDraw function by default to draw
    // to the HMD
  }

  // In the VRApp the draw function only draws to the HMD
  void onDraw(Graphics &g) override {
    g.clear();
    // Draw a cube in the scene
    g.color(1);
    g.draw(mCube);

    // Draw markers for the controllers
    // The openVR object is available in the VRRenderer class to query the
    // controllers

#ifdef AL_EXT_OPENVR
    g.pushMatrix();
    g.translate(openVRDomain->mOpenVR.LeftController.pose().pos());
    g.rotate(openVRDomain->mOpenVR.LeftController.pose().quat());
    g.scale(0.1);
    g.color(0, 1, 0);
    gl::polygonLine();
    g.draw(mCube);
    g.popMatrix();

    // right hand
    g.pushMatrix();
    g.translate(openVRDomain->mOpenVR.RightController.pose().pos());
    // std::cout << openVR->RightController.pos.x <<
    // openVR->RightController.pos.y << openVR->RightController.pos.z <<
    // std::endl;
    g.rotate(openVRDomain->mOpenVR.RightController.pose().quat());
    g.scale(0.1);
    g.color(0, 0, 1);
    gl::polygonLine();
    g.draw(mCube);
    g.popMatrix();

    g.pushMatrix();
    g.translate(openVRDomain->mOpenVR.HMDPos);
    g.rotate(openVRDomain->mOpenVR.HMDQuat);
    g.color(1, 0, 0);
    gl::polygonLine();
    g.draw(mCube);
    g.popMatrix();

#endif
  }
};

int main(int argc, char *argv[]) {
  MyApp app;

  app.start();
  return 0;
}
