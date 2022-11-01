#include "al/app/al_App.hpp"
#include "al/graphics/al_Shapes.hpp"

// The AL_EXT_OPENVR macro is set if OpenVR is found.

#include "al_ext/openvr/al_OpenVRWrapper.hpp"

using namespace al;

class MyApp : public App {
public:
  void onCreate() override {
    if (!mOpenVR.init()) {
      std::cerr << "ERROR: OpenVR init returned error" << std::endl;
    }
    addCube(mCube);
    mCube.primitive(Mesh::LINE_STRIP);
    mCube.update();
  }

  void onAnimate(double dt) override {
    // Update traking and controller data;
    mOpenVR.update();

    // openVR draw.
    // Draw in onAnimate, to make sure drawing happens only once per frame
    // Pass a function that takes Graphics &g argument
    mOpenVR.draw(std::bind(&MyApp::drawScene, this, std::placeholders::_1),
                 graphics());
  }

  void drawScene(Graphics &g) {

#ifdef AL_EXT_OPENVR
    g.clear();
    // Draw a cube in the scene
    g.draw(mCube);
    // Draw markers for the controllers
    // The openVR object is available in the VRRenderer class to query the
    // controllers
    g.pushMatrix();
    g.translate(mOpenVR.LeftController.pos);
    g.rotate(mOpenVR.LeftController.quat);
    g.scale(0.1);
    g.color(1);
    g.polygonMode(Graphics::LINE);
    g.draw(mCube);
    g.popMatrix();

    // right hand
    g.pushMatrix();
    g.translate(mOpenVR.RightController.pos);
    // std::cout << openVR->RightController.pos.x <<
    // openVR->RightController.pos.y << openVR->RightController.pos.z <<
    // std::endl;
    g.rotate(mOpenVR.RightController.quat);
    g.scale(0.1);
    g.color(1);
    g.polygonMode(Graphics::LINE);
    g.draw(mCube);
    g.popMatrix();
#else

    g.clear(1.0, 0.0, 0.0); // Draw red if not building for OpenVR
#endif
  }

  void onDraw(Graphics &g) override {
    // This onDraw determines what is seen on the Desktop window.
    // In this case, the desktop nav is independent to the HMD nav
    drawScene(g);
  }

  void onExit() override { mOpenVR.close(); }

private:
  VAOMesh mCube;
  OpenVRWrapper mOpenVR;
};

int main(int argc, char *argv[]) {
  MyApp app;

  app.start();
  return 0;
}
