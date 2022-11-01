#include "Cuttlebone/Packet.hpp"
#include <iostream>
#include <stdlib.h>
using namespace std;

using namespace cuttlebone;

struct State {
  unsigned n;
  char data[12345];
  void zero() { memset(this, 0, sizeof(State)); }
};

int main() {
  State state;
  state.zero();
  state.n = 20;
  sprintf(state.data, "this is a message to the internet. fuck.");

  PacketMaker<State, Packet<1400>> original(state, 1);

  cout << sizeof(State) << endl;
  cout << PacketMaker<State, Packet<1400>>::TOTAL_PART_COUNT << endl;
  cout << PacketMaker<State, Packet<1400>>::LAST_DATA_SIZE << endl;
  cout << Packet<1400>::DATA_SIZE << endl;
  cout << 1400 << endl;

  Packet<1400> packet[PacketMaker<State, Packet<1400>>::TOTAL_PART_COUNT];
  for (unsigned i = 0; original.fill(packet[i]); ++i)
    cout << packet[i].header.partNumber << " " << packet[i].header.partSize
         << endl;

  state.zero();
  PacketTaker<State, Packet<1400>> reconstructed(state, 1);
  for (unsigned i = 0; reconstructed.take(packet[i]); ++i)
    cout << packet[i].header.partNumber << " " << packet[i].header.partSize
         << endl;
}
