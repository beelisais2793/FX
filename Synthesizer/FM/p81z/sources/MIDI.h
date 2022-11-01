#pragma once

enum {
    kStatusNoteOn = 0x90,
    kStatusNoteOff = 0x80,
    kStatusControllerChange = 0xb0,
    kStatusPitchBend = 0xe0,
};

enum {
    kCcDataMsb = 6,
    kCcVolumeMsb = 7,
    kCcPanMsb = 10,
    kCcExpressionMsb = 11,
    kCcDataLsb = 38,
    kCcVolumeLsb = 39,
    kCcPanLsb = 42,
    kCcExpressionLsb = 43,
    kCcVelocityPrefix = 88,
    kCcNrpnLsb = 98,
    kCcNrpnMsb = 99,
    kCcRpnLsb = 100,
    kCcRpnMsb = 101,
    kCcSoundOff = 120,
    kCcResetControllers = 121,
    kCcNotesOff = 123,
    kCcOmniOff = 124,
    kCcOmniOn = 125,
    kCcMonoOn = 126,
    kCcPolyOn = 127,
};

struct RpnIdentifier {
    unsigned registered : 1;
    unsigned msb : 7;
    unsigned lsb : 7;
};
