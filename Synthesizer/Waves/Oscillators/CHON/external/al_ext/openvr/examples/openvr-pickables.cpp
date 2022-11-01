

#include "al/app/al_App.hpp"
#include "al/ui/al_Pickable.hpp"
#include "al/ui/al_PickableManager.hpp"

#include "al_ext/openvr/al_OpenVRDomain.hpp"

using namespace al;

struct MyApp : public App {

  PickableManager mPickableManager;
  VAOMesh mSphere;
  VAOMesh mBox;
  VAOMesh mController;
  Mesh mRay;
  Mesh mHit;

  std::shared_ptr<OpenVRDomain> openVRDomain;

  void onCreate() override {
    // Initialize openVR in onCreate. A graphics context is needed.
    openVRDomain = OpenVRDomain::enableVR(this);
    openVRDomain->setDrawFunction([this](Graphics &g) {
      this->drawScene(*(this->openVRDomain->mGraphics));
    });

    nav().pos(0, 0, 3);
    navControl().useMouse(false);

    addSphere(mSphere, 0.1, 8, 8);
    mSphere.primitive(Mesh::LINE_STRIP);
    mSphere.update();
    addWireBox(mBox, 0.1f);
    mBox.scale(1, 0.1, 1);
    mBox.update();
    addWireBox(mController, 0.1f);
    mController.scale(0.1, 0.1, 1);
    mController.update();
    addSphere(mHit, 0.01f, 8, 8);
    mRay.primitive(Mesh::LINES);

    // create some pickables
    for (int i = 0; i < 9; i++) {
      PickableBB *p = new PickableBB;
      p->set(mSphere);
      p->pose = Pose(Vec3f(i * 0.25f - 1, 0, 0), Quatf());
      if (i == 0)
        p->scale = 0.5f;
      if (i == 4)
        p->pose = Pose(Vec3f(i * 0.25f - 1, 0, 0),
                       Quatf().fromEuler(Vec3f(0.5, 0, 0)));
      if (i == 8)
        p->scale = 2.0f;

      PickableBB *child = new PickableBB;
      child->set(mBox);
      p->addChild((*child));
      mPickableManager << p;
    }
  }

  void onAnimate(double dt) override {

#ifdef AL_EXT_OPENVR
    // OpenVR Pickable events
    auto l = openVRDomain->mOpenVR.LeftController;
    auto r = openVRDomain->mOpenVR.RightController;

    auto ray = r.ray();

    if (r.triggerPress()) {
      mPickableManager.event(PickEvent(Pick, ray));
    } else if (r.gripPress()) {
      mPickableManager.event(PickEvent(Pick, ray));
      mPickableManager.event(PickEvent(TranslateRay, r.pose()));
    } else if (r.triggerDown()) {
      mPickableManager.event(PickEvent(Drag, ray, r.vel));
    } else if (r.gripDown()) {
      mPickableManager.event(PickEvent(RotatePose, r.pose()));
    } else if (r.triggerRelease() || r.gripRelease()) {
      mPickableManager.event(PickEvent(Unpick, ray));
    } else {
      mPickableManager.event(PickEvent(Point, ray));
    }

    // if(r.gripPress()){
    //   mPickableManager.pick(r.pos);
    // } else if(r.gripDown()){
    //   if(l.gripDown()){

    //   }else{
    //     mPickableManager.drag(r.pos, r.vel);
    //   }
    // } else if(r.gripRelease()){
    //   mPickableManager.unpick(r.pos);
    // } else {
    //   mPickableManager.point(r.pos);
    // }

    // openVR draw.
    // Draw in onAnimate, to make sure drawing happens only once per frame
    // Pass a function that takes Graphics &g argument
#endif
  }

  void drawScene(Graphics &g) {
    g.clear();
    g.depthTesting(true);

    for (auto p : mPickableManager.pickables()) {
      g.color(1, 1, 1);
      p->draw(g);
      g.color(1, 0, 0);
      //      p->drawChildren(g);
    }

    auto h1 = mPickableManager.lastPick();
    if (h1.hit) {
      g.pushMatrix();
      g.translate(h1());
      g.color(0, 0, 1);
      g.draw(mHit);
      g.popMatrix();
    }
    auto h2 = mPickableManager.lastPoint();
    if (h2.hit) {
      g.pushMatrix();
      g.translate(h2());
      g.color(0, 1, 0);
      g.draw(mHit);
      g.popMatrix();
    }

#ifdef AL_EXT_OPENVR
    // Draw markers for the controllers
    // The openVR object is available in the VRRenderer class to query the
    // controllers
    g.pushMatrix();
    g.translate(openVRDomain->mOpenVR.LeftController.pos);
    g.rotate(openVRDomain->mOpenVR.LeftController.quat);

    g.color(0, 1, 1);
    g.draw(mController);
    g.popMatrix();

    // right hand
    g.pushMatrix();
    g.translate(openVRDomain->mOpenVR.RightController.pos);
    g.rotate(openVRDomain->mOpenVR.RightController.quat);
    g.color(1, 0, 1);
    g.draw(mController);
    g.popMatrix();

    // draw controller rays
    auto r1 = openVRDomain->mOpenVR.RightController.ray();
    auto r2 = openVRDomain->mOpenVR.LeftController.ray();
    mRay.reset();
    mRay.vertices().push_back(r1.o);
    mRay.vertices().push_back(r1.o + r1.d * 5);
    mRay.vertices().push_back(r2.o);
    mRay.vertices().push_back(r2.o + r2.d * 5);
    g.draw(mRay);
#endif
  }

  void onDraw(Graphics &g) override {
    // This onDraw determines what is seen on the Desktop window.
    // In this case, the desktop nav is independent to the HMD nav
    drawScene(g);
  }

  bool onMouseMove(const Mouse &m) override {
    mPickableManager.onMouseMove(graphics(), m, width(), height());
    return true;
  }
  bool onMouseDown(const Mouse &m) override {
    mPickableManager.onMouseDown(graphics(), m, width(), height());
    return true;
  }
  bool onMouseDrag(const Mouse &m) override {
    mPickableManager.onMouseDrag(graphics(), m, width(), height());
    return true;
  }
  bool onMouseUp(const Mouse &m) override {
    mPickableManager.onMouseUp(graphics(), m, width(), height());
    return true;
  }
};

int main() {
  MyApp app;
  app.start();
  return 0;
}
