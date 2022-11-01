
#include "al/app/al_DistributedApp.hpp"
#include "al/graphics/al_Shapes.hpp"
#include "al_ext/statedistribution/al_CuttleboneDomain.hpp"

using namespace al;

// This example shows usage of the CuttleboneDomain. Although in most cases
// you will want to use CuttleboneStateSimulationDomain to distribute state,
// and attach it to your application using enableCuttlebone(), using the
// Cuttlebone domain directly requires a little more work, but gives you the
// flexibility to sync multiple states to or from different machines

#define PACKET_SIZE1 1400
#define PORT1 61900

#define PACKET_SIZE2 1400
#define PORT2 61901

struct State1 {
  al::Color backgroundColor{1.0f, 1.0f, 1.0f, 1.0f};
  al::Pose pose;
};

struct State2 {
  al::Color color{0.0f, 0.0f, 1.0f, 1.0f};
};

struct MyDistributedApp : public DistributedApp {
  Mesh mesh;

  std::shared_ptr<CuttleboneDomain<State1, PACKET_SIZE1, PORT1>>
      cuttleboneDomain1;
  std::shared_ptr<CuttleboneDomain<State2, PACKET_SIZE2, PORT2>>
      cuttleboneDomain2;
  std::shared_ptr<State1> sharedState1{new State1};
  std::shared_ptr<State2> sharedState2{new State2};

  void onInit() override {
    // Use the static function to insert the cuttlebone domain
    // We need to do this in the constructor as domains get activated on the
    // call to start() for the app
    cuttleboneDomain1 =
        simulationDomain()
            ->newSubDomain<CuttleboneDomain<State1, PACKET_SIZE1, PORT1>>();
    cuttleboneDomain2 =
        simulationDomain()
            ->newSubDomain<CuttleboneDomain<State2, PACKET_SIZE2, PORT2>>();
    if (!cuttleboneDomain1 || !cuttleboneDomain2) {
      std::cerr << "ERROR creating cuttlebone domain. Quitting." << std::endl;
      quit();
      return;
    }

    if (isPrimary()) {
      // On the simulator machine, we add a state sender and receiver
      auto sender = cuttleboneDomain1->addStateSender("", sharedState1);
      auto receiver = cuttleboneDomain2->addStateReceiver("", sharedState2);
      assert(sender);
      assert(receiver);
    } else {
      // For the non primary instance, we will make the second cuttlebone
      // the sender and the other the receiver
      auto receiver = cuttleboneDomain1->addStateReceiver("", sharedState1);
      auto sender = cuttleboneDomain2->addStateSender("", sharedState2);
      assert(sender);
      assert(receiver);
    }

    addIcosahedron(mesh);
    mesh.primitive(Mesh::LINES);
  }

  void onAnimate(double /*dt*/) override {
    if (cuttleboneDomain1->isSender()) {
      sharedState1->backgroundColor.r = float(mouse().x()) / width();
      sharedState1->backgroundColor.g = float(mouse().y()) / height();
      sharedState1->pose = pose();
    } else {
      sharedState2->color.r = float(mouse().x()) / width();
      sharedState2->color.g = float(mouse().y()) / height();
      pose() = sharedState1->pose;
    }
  }

  void onDraw(Graphics& g) override {
    g.clear(sharedState1->backgroundColor);
    g.color(sharedState2->color);
    g.draw(mesh);
  }
};

int main() {
  MyDistributedApp app;
  app.start();
  return 0;
}
