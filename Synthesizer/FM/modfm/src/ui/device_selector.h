#pragma once

#include <nanogui/combobox.h>

class MIDIReceiver;
class MIDIDeviceSelector : public nanogui::ComboBox {
 public:
  MIDIDeviceSelector(Widget *parent, MIDIReceiver *midi_receiver);
  void PickDevice(int index);

 private:
  MIDIReceiver *midi_receiver_;
};
