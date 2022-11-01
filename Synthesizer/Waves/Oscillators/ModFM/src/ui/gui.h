#pragma once

#include <atomic>
#include <thread>

class Patch;
class MIDIReceiver;
class GUI {
 public:
  GUI(Patch *patch, MIDIReceiver *midi_receiver);
  ~GUI();
  void Start();
  void Stop();
  bool running() const;

 private:
  void StartUI();

  Patch *patch_;
  MIDIReceiver *midi_receiver_;
};
