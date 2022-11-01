#include "Cuttlebone/Configuration.hpp"
#include "Cuttlebone/Selector.hpp"
#include "Cuttlebone/Broadcaster.hpp"
#include "Cuttlebone/Timer.hpp"

/*
const char* config[] = {"192.168.10.255", "gr01", "audio", "gr02", "gr03", "gr04",
                      "gr05", "gr06",  "gr07", "gr08", "gr09",
                      "gr10", "gr11",  "gr12", "gr13", "gr14", };

BROKEN
struct App : HostRole, Selector<App>, Broadcaster, Timer, Checksum {
  unsigned char* buffer;
  unsigned char* b;
  unsigned packetSize;

  void init(unsigned packetSize, unsigned timerPeriod, unsigned selectTimeout,
            const char* ip, unsigned port) {

    HostRole::init(hostRole);

    this->packetSize = packetSize;
    buffer = new unsigned char[packetSize];
    b = new unsigned char[packetSize];

    if (isSelector) Selector<App>::start(packetSize, selectTimeout, port);
    if (isBroadcaster) {
      Broadcaster::init(packetSize, ip, port);
      Timer::start(timerPeriod);
    }
  }

  void onTimer() {
    printf("send:%03u ", b[0]);
    checksum(b, packetSize);
    print();
    send(b);
    b[0]++;
  }

  void onNewBuffer() {
    printf("recv:%03u ", buffer[0]);
    checksum(buffer, packetSize);
    print();
  }
};

int main(int argc, char* argv[]) {
  unsigned packetSize = 512, timerPeriod = 800000, selectTimeout = 900000;
  const char* ip = "127.0.0.1";
  unsigned port = 8888;
  if (argc > 1) packetSize = atoi(argv[1]);
  if (argc > 2) timerPeriod = atof(argv[2]);
  if (argc > 3) selectTimeout = atoi(argv[3]);
  if (argc > 4) ip = argv[4];
  if (argc > 5) port = atoi(argv[5]);

  printf("packetSize:%u timerPeriod:%u selectTimeout:%u ip:%s port:%u\n",
         packetSize, timerPeriod, selectTimeout, ip, port);

  App app;
  app.init(packetSize, timerPeriod, selectTimeout, ip, port);
  getchar();
}
*/

int main(){}
