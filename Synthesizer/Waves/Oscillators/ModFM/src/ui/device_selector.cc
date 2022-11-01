#include "ui/device_selector.h"

#include <absl/strings/str_format.h>
#include <glog/logging.h>

#include "midi.h"

MIDIDeviceSelector::MIDIDeviceSelector(nanogui::Widget *parent,
                                       MIDIReceiver *midi_receiver)
    : midi_receiver_(midi_receiver), nanogui::ComboBox(parent) {
  auto device_list = midi_receiver_->ListDevices();
  std::vector<std::string> device_names;
  for (int i = 0; i < device_list.size(); i++) {
    const auto &device = device_list[i];
    int chosen_device = midi_receiver->CurrentDeviceID();
    if (device.first == chosen_device) set_selected_index(i);
    device_names.emplace_back(device.second->name);
  }
  set_items(device_names);

  set_callback(std::bind(std::mem_fn(&MIDIDeviceSelector::PickDevice), this,
                         std::placeholders::_1));
}

void MIDIDeviceSelector::PickDevice(int index) {
  auto device_list = midi_receiver_->ListDevices();

  int chosen_device = -1;
  for (int i = 0; i < device_list.size(); i++) {
    if (i == index) {
      chosen_device = device_list[i].first;
      auto stop_result = midi_receiver_->Stop();
      if (!stop_result.ok()) {
        LOG(ERROR) << "Unable to stop MIDI device: " << stop_result;
        return;
      }
      auto close_result = midi_receiver_->Close();
      if (!close_result.ok()) {
        LOG(ERROR) << "Unable to close MIDI device: " << close_result;
        return;
      }
      auto change_result = midi_receiver_->OpenDevice(chosen_device);
      if (!change_result.ok()) {
        LOG(ERROR) << "Unable to open MIDI device: " << change_result.status();
        return;
      }
      auto start_result = midi_receiver_->Start();
      if (!start_result.ok()) {
        LOG(ERROR) << "Unable to start MIDI device: " << start_result;
      }

      break;
    }
  }
}
