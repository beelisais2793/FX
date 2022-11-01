/*
Allocore Example: modelshader

Description:
This demonstrates loading an OBJ and applying a shader

Author:
Graham Wakefield 2011
*/

//#include "al/core.hpp"
#include "al/app/al_App.hpp"
#include "al_ext/assets3d/al_Asset.hpp"
//#include "module/img/loadImage.hpp"
#include <algorithm>  // max
#include <cstdint>    // uint8_t
#include <vector>

using namespace al;
using namespace std;

struct MyApp : App {
  Scene *ascene{nullptr};
  Vec3f scene_min, scene_max, scene_center;
  Texture tex;
  vector<Mesh> meshes;
  float a = 0.f;  // current rotation angle

  void onCreate() {
    // load in a "scene"

    std::string fileName = "ducky.obj";
    ascene = Scene::import(fileName);
    if (!ascene) {
      printf("error reading %s\n", fileName.c_str());
      return;
    }
    ascene->getBounds(scene_min, scene_max);
    scene_center = (scene_min + scene_max) / 2.f;
    ascene->print();

    //    auto filename = searchpaths.find("hubble.jpg").filepath();

    //    auto imageData = imgModule::loadImage(filename);
    //    if (imageData.data.size() == 0) {
    //      cout << "failed to load image" << endl;
    //    }
    //    cout << "loaded image size: " << imageData.width << ", " <<
    //    imageData.height << endl;

    //    tex.create2D(imageData.width, imageData.height);
    //    tex.submit(imageData.data.data(), GL_RGBA, GL_UNSIGNED_BYTE);

    // extract meshes from scene
    meshes.resize(ascene->meshes());
    for (int i = 0; i < ascene->meshes(); i += 1) {
      ascene->mesh(i, meshes[i]);
    }
  }

  void onDraw(Graphics &g) {
    g.clear(0.1);

    gl::depthTesting(true);
    g.lighting(true);
    // g.light().dir(1.f, 1.f, 2.f);

    g.pushMatrix();

    // rotate it around the y axis
    g.rotate(a, 0.f, 1.f, 0.f);
    a += 0.5;

    // scale the whole asset to fit into our view frustum
    float tmp = scene_max[0] - scene_min[0];
    tmp = std::max(scene_max[1] - scene_min[1], tmp);
    tmp = std::max(scene_max[2] - scene_min[2], tmp);
    tmp = 2.f / tmp;
    g.scale(tmp);

    // center the model
    g.translate(-scene_center);

    //    tex.bind(0);
    //    g.texture(); // use texture to color the mesh
    // draw all the meshes in the scene
    for (auto &m : meshes) {
      g.draw(m);
    }
    //    tex.unbind(0);

    g.popMatrix();
  }
};

int main(int argc, char *const argv[]) {
  MyApp app;
  app.dimensions(640, 480);
  app.start();
  return 0;
}
