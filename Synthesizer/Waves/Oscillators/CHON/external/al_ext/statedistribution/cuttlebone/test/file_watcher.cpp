#include "Cuttlebone/FileWatcher.hpp"
using namespace cuttlebone;

#include <iostream>
#include <thread>
using namespace std;

struct Foo : FileWatcher {
  virtual void onModify(const char* filePath) {
    cout << filePath << " was changed" << endl;
  }
  virtual void start() {
    thread t([this]() { this->FileWatcher::start(); });
    getchar();
  }
};

int main() {
  Foo foo;
  foo.watch("/tmp/foo");
  foo.start();
  foo.stop();
}
