#include <thread>
#include <unistd.h>

using namespace std;

//#define LOG_FILE ("/tmp/file.txt") // LOG to a file instead of standard out
//#define DONT_COMPILE_LOG // like LOG never existed
#include "Cuttlebone/Log.hpp"

using namespace cuttlebone;

int main(int argc, char* argv[]) {
  LOG("main|started");

  thread audio([&]() {
    LOG("audio|started");
    for (int i = 0; i < 345; i++) {
      usleep(2902);
      LOG("audio|%d", i);
    }
    LOG("audio|ended");
  });

  thread graphics([&]() {
    LOG("graphics|started");
    for (int i = 0; i < 60; i++) {
      usleep(16667);
      LOG("graphics|%d", i);
    }
    LOG("graphics|ended");
  });

  thread network([&]() {
    LOG("network|started");
    for (int i = 0; i < 10; i++) {
      usleep(100000);
      LOG("network|%d", i);
    }
    LOG("network|ended");
  });

  LOG("main|waiting for <enter>...");
  getchar();
  audio.join();
  graphics.join();
  network.join();
  LOG("main|ended");
}
