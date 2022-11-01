#pragma once

#include "absl/status/statusor.h"
#include <portmidi.h>

#include <atomic>
#include "sigslot/signal.hpp"
#include <thread>

class MIDIReceiver {
 public:
  std::vector<std::pair<PmDeviceID, const PmDeviceInfo *>> ListDevices() const;
  const PmDeviceInfo *CurrentDeviceInfo() const;
  PmDeviceID CurrentDeviceID() const { return midi_device_; }

  absl::StatusOr<const PmDeviceInfo *> OpenDefaultDevice();
  absl::StatusOr<const PmDeviceInfo *> OpenDevice(PmDeviceID midi_device);
  absl::Status Start();
  absl::Status Stop();
  absl::Status Close();
  bool running() const { return running_; }

  sigslot::signal<PmTimestamp, uint8_t /* note */, uint8_t /* velocity */>
      NoteOnSignal;
  sigslot::signal<uint8_t> NoteOffSignal;

 private:
  void ProcessBuffer(const PmEvent *buffer, int length);

  std::atomic_bool open_;
  std::atomic_bool running_;
  std::thread receive_thread_;
  PmStream *midi_stream_;
  PmDeviceID midi_device_;
};