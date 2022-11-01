#include "midi.h"

#include "absl/strings/str_format.h"
#include "glog/logging.h"

#include "player.h"

std::vector<std::pair<PmDeviceID, const PmDeviceInfo *>>
MIDIReceiver::ListDevices() const {
  std::vector<std::pair<PmDeviceID, const PmDeviceInfo *>> devices;
  for (int i = 0; i < Pm_CountDevices(); i++) {
    const auto *info = Pm_GetDeviceInfo(i);
    if (info->input) {
      devices.emplace_back(std::make_pair(i, info));
    }
  }
  return devices;
}

absl::StatusOr<const PmDeviceInfo *> MIDIReceiver::OpenDevice(
    PmDeviceID midi_device) {
  if (open_) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        "Device already open. Close first.");
  }
  if (running_) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        "MIDI receiver is running. Stop and close first.");
  }

  auto *device_info = Pm_GetDeviceInfo(midi_device);
  if (device_info == nullptr) {
    return absl::Status(
        absl::StatusCode::kNotFound,
        absl::StrFormat("Unable to get device info for MIDI device %d",
                        midi_device));
  }

  auto midi_err = Pm_OpenInput(&midi_stream_, midi_device, nullptr,
                               100 /* input buffer size */,
                               nullptr /* time proc */, nullptr /* time info */
  );
  if (midi_err != pmNoError) {
    return absl::Status(absl::StatusCode::kNotFound,
                        absl::StrFormat("Unable to open MIDI device: %s",
                                        Pm_GetErrorText(midi_err)));
  }

  midi_device_ = midi_device;
  open_ = true;

  LOG(INFO) << "Opened device:" << device_info->name;

  return device_info;
}

absl::StatusOr<const PmDeviceInfo *> MIDIReceiver::OpenDefaultDevice() {
  PmDeviceID default_midi_device = Pm_GetDefaultInputDeviceID();
  return OpenDevice(default_midi_device);
}

absl::Status MIDIReceiver::Start() {
  if (running_) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        "Already running");
  }
  if (!open_) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        "Device not open");
  }

  LOG(INFO) << "Starting MIDI receiver...";

  running_ = true;
  receive_thread_ = std::thread([this] {
    /* empty buffer before starting */
    PmEvent buffer[256];
    while (Pm_Poll(midi_stream_)) {
      Pm_Read(midi_stream_, buffer, 256);
    }

    while (running_) {
      int length = Pm_Read(midi_stream_, buffer, 256);
      ProcessBuffer(buffer, length);
      usleep(50);
    }
  });

  LOG(INFO) << "Started MIDI receiver";

  return absl::OkStatus();
}

void MIDIReceiver::ProcessBuffer(const PmEvent *buffer, int length) {
  for (int i = 0; i < length; i++) {
    PmMessage status = Pm_MessageStatus(buffer[i].message);
    PmMessage data1 = Pm_MessageData1(buffer[i].message);
    PmMessage data2 = Pm_MessageData2(buffer[i].message);
    auto event_masked = status & 0xf0;
    if (event_masked == 0x90) {
      auto channel = status & 0xf;
      auto note = data1 & 0x7f;
      auto velocity = data2 & 0x7f;
      NoteOnSignal(buffer[0].timestamp, velocity, note);
    } else if (event_masked == 0x80) {
      auto channel = status & 0xf;
      auto note = data1 & 0x7f;
      NoteOffSignal(note);
    }
  }
}

absl::Status MIDIReceiver::Stop() {
  running_ = false;
  if (receive_thread_.joinable()) receive_thread_.join();

  LOG(INFO) << "Stopped MIDI receiver";

  return absl::OkStatus();
}

absl::Status MIDIReceiver::Close() {
  if (running_) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        "Currently running. Stop before closing.");
  }
  if (!open_) {
    return absl::Status(absl::StatusCode::kFailedPrecondition,
                        "Device not open");
  }

  auto pm_err = Pm_Close(midi_stream_);
  if (pm_err != pmNoError) {
    return absl::Status(
        absl::StatusCode::kInternal,
        absl::StrFormat("Unable to close device: %s", Pm_GetErrorText(pm_err)));
  }

  LOG(INFO) << "Closed MIDI device: " << midi_device_;

  open_ = false;
  return absl::OkStatus();
}

const PmDeviceInfo *MIDIReceiver::CurrentDeviceInfo() const {
  if (!open_) return nullptr;
  return Pm_GetDeviceInfo(midi_device_);
}
