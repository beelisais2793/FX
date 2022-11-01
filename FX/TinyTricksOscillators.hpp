// https://github.com/thomassidor/tinytricks/blob/master/src/oscillators/oscillator.cpp
struct TinyOscillator {
  private:
    float phase = 0.0f;
    float freq = 0.0f;
    float theta = 0.01f;
    float isStepEOC = false;

  public:
    enum OscillatorType {
      SIN,
      SAW,
      SQR,
      TRI
    };

    void step(float dt) {
      //phase+= freq;
      phase += freq / dt;
      if (phase >= 1.0f) {
        phase -= 1.f;
        isStepEOC = true;
      }
      else
        isStepEOC = false;
    }

    void reset() {
      phase = 0.f;
      isStepEOC = true;
    }

    void setTheta(float t) {
      theta = t;
    }

    float getSin() {
      return sinf(2.0f * M_PI * phase) * 5.0f;
    }

    float getSaw() {
      return ((1 + triInternal((2.f * phase - 1.f) / 4.f) * sqrInternal(phase / 2.f)) / 2.f) * 10.f - 5.f;
    }

    float getTri() {
      return triInternal(phase) * 5.f;
    }

    float triInternal(float x) {
      return 1.f - 2.f * (acos((1.f - theta) * sin(2.f * M_PI * x))) / M_PI;
    }

    float getSqr() {
      return sqrInternal(phase) * 5.f;
    }

    float sqrInternal(float x) {
      return 2 * atan(sin(2.0f * M_PI * x) / theta) / M_PI;
    }

    bool isEOC() {
      return isStepEOC;
    }

    void setPitch(float pitch) {
      freq = dsp::FREQ_C4 * powf(2.0f, pitch);
    }


};

struct BareboneOscillator {
  float phase = 0.0f;
  float freq = 0.0f;
  float isStepEOC = false;

  void step(float dt) {
    //phase+= freq;
    phase += freq / dt;
    if (phase >= 1.0f) {
      phase = 0.f;
      isStepEOC = true;
    }
    else
      isStepEOC = false;
  }

  void reset() {
    phase = 0.f;
    isStepEOC = true;
  }


  bool isEOC() {
    return isStepEOC;
  }

  void setPitch(float pitch) {
    freq = dsp::FREQ_C4 * powf(2.0f, pitch);
  }
};


struct LowFrequencyOscillator {
  float phase = 0.0f;
  float pw = 0.5f;
  float freq = 1.0f;
  bool offset = false;
  bool invert = false;
  rack::dsp::SchmittTrigger resetTrigger;
  LowFrequencyOscillator() {}
  void setPitch(float pitch) {
    pitch = fminf(pitch, 8.0f);
    freq = powf(2.0f, pitch);
  }
  void setPulseWidth(float pw_) {
    const float pwMin = 0.01f;
    pw = clamp(pw_, pwMin, 1.0f - pwMin);
  }
  void setReset(float reset) {
    if (resetTrigger.process(reset)) {
      phase = 0.0f;
    }
  }
  void setPhase(float p) {
    phase = p;
  }
  void step(float dt) {
    float deltaPhase = fminf(freq * dt, 0.5f);
    phase += deltaPhase;
    if (phase >= 1.0f)
      phase -= 1.0f;
  }
  float sin() {
    if (offset)
      return 1.0f - cosf(2.0f * M_PI * phase) * (invert ? -1.0f : 1.0f);
    else
      return sinf(2.0f * M_PI * phase) * (invert ? -1.0f : 1.0f);
  }
  float tri(float x) {
    return 4.0f * fabsf(x - roundf(x));
  }
  float tri() {
    if (offset)
      return tri(invert ? phase - 0.5f : phase);
    else
      return -1.0f + tri(invert ? phase - 0.25f : phase - 0.75f);
  }
  float saw(float x) {
    return 2.0f * (x - roundf(x));
  }
  float saw() {
    if (offset)
      return invert ? 2.0f * (1.0f - phase) : 2.0f * phase;
    else
      return saw(phase) * (invert ? -1.0f : 1.0f);
  }
  float sqr() {
    float sqr = (phase < pw) ^ invert ? 1.0f : -1.0f;
    return offset ? sqr + 1.0f : sqr;
  }
  float light() {
    return sinf(2.0f * M_PI * phase);
  }
};


struct SimplexOscillator {
  static const int BUFFER_LENGTH = 2048;
  float phase = 0.0f;
  float freq = 0.0f;

  unsigned int tick = 0;

  bool isStepEOC = false;

  bool reverse = false;

  float buffer[BUFFER_LENGTH] = {0};
  int bufferIndex = 0;

  float min = -1.f;
  float max = 1.f;

  float mirror = false;

  SimplexNoise simp;

  SimplexOscillator() {
    simp.init();
  }

  void setMirror(bool _mirror) {
    mirror = _mirror;
    min = -1.f;
    max = 1.f;
    tick = 0;
    reset();
  }

  void step(float dt) {
    float delta = freq / dt;

    isStepEOC =  false;
    if (mirror) {
      if (!reverse) {
        phase += delta;
        if (phase >= 0.5f)
          reverse = true;
      }
      else {
        phase -= delta;
        if (phase < 0.f) {
          reverse = false;
          //phase = 0.f;
          phase = -phase;
          isStepEOC = true;
        }
      }
    }
    else {
      phase += delta;
      if (phase >= 1.0f) {
        //phase = 0.f;//-= 1.0f;
        phase -= 1.0f;
        isStepEOC = true;
      }
    }
  }

  void reset() {
    phase = 0.f;
    reverse = false;
  }

  bool isEOC() {
    return isStepEOC;
  }

  void setPitch(float pitch) {
    freq = dsp::FREQ_C4 * powf(2.0f, pitch);
  }

  float getValue(float detaillevel, float x, float y, float z, float scale) {
    return simp.SumOctaveSmooth(detaillevel, x + phase, y, z, scale);
  }

  float getOsc(float detaillevel, float x, float y, float z, float scale) {
    float value = getValue(detaillevel, x, y, z, scale);
    return value * 5.f;
  }


  float getNormalizedOsc(float detaillevel, float x, float y, float z, float scale) {

    float value = getValue(detaillevel, x, y, z, scale);

    if (bufferIndex >= BUFFER_LENGTH) {
      bufferIndex = 0;
    }

    buffer[bufferIndex] = value;
    bufferIndex++;


    if (tick % 256 == 0) {
      auto result = std::minmax_element(begin(buffer), end(buffer));
      min = *result.first;
      max = *result.second;
    }
    tick++;

    value = clamp(rescale(value, min, max, -1.f, 1.f), -1.f, 1.f);
    return value * 5.f;;

  }
};

struct WaveTable {
  static const int WAVEFORM_COUNT = 3;
  static const int MAX_SAMPLE_COUNT = 2048;
  int WAVETABLE_SIZE = MAX_SAMPLE_COUNT;
  float lookuptable[WAVEFORM_COUNT][MAX_SAMPLE_COUNT] = {{0}};
  int recordingIndex = 0;

  float getSample(float y, float x) {

    //Getting indexes for current place in table
    int index0 = (int) x;
    int index1 = index0 == (WAVETABLE_SIZE - 1) ? (int) 0 : index0 + 1;

    // How far are we from the index
    float indexFrac = x - (float) index0;

    //Getting indexes for the levels based on y
    float frac = y * (WAVEFORM_COUNT - 1);
    int level0 = floor(frac);
    int level1 = ceil(frac);
    float levelFrac = frac - (float) level0;

    //Getting the four samples in the table
    float Level0Value0 = lookuptable[level0][index0];
    float Level0Value1 = lookuptable[level0][index1];

    float Level1Value0 = lookuptable[level1][index0];
    float Level1Value1 = lookuptable[level1][index1];

    //Interpolating between the two
    float interpolatedValueForLevel0 = Level0Value0 + indexFrac * (Level0Value1 - Level0Value0);
    float interpolatedValueForLevel1 = Level1Value0 + indexFrac * (Level1Value1 - Level1Value0);

    float finalValue = interpolatedValueForLevel0 + levelFrac * (interpolatedValueForLevel1 - interpolatedValueForLevel0);

    return finalValue;
  }

  void reset() {
    recordingIndex = 0.f;
  }

  void startCapture() {
    reset();
  }

  void endCapture() {
    WAVETABLE_SIZE = recordingIndex;
    reset();
  }

  void addSampleToFrame(float sampleValue, int waveId) {
    lookuptable[waveId][recordingIndex] = sampleValue;
  }

  void endFrame() {
    recordingIndex = recordingIndex + 1;
  }
};

struct WaveTableOscillator {
  WaveTable *waveTable;

  float currentIndex = 0.f;
  float tableDelta = 0.f;

  bool isStepEOC = false;

  bool mirror = false;
  bool reverse = false;

  float prevPitch = 90000.f;

  float phase = 0.f;
  float freq = 0.f;

  WaveTableOscillator() {
    waveTable = new  WaveTable();
  }

  float getSample(float y) {
    if (waveTable == nullptr)
      return 0.f;
    else
      return waveTable->getSample(y, currentIndex);
  }

  void step() {
    if (waveTable == nullptr)
      return;

    isStepEOC = false;

    if (mirror) {
      if (!reverse) {
        currentIndex += tableDelta;
        if (currentIndex >= waveTable->WAVETABLE_SIZE / 2.f)
          reverse = true;
      }
      else {
        currentIndex -= tableDelta;
        if (currentIndex < 0.f) {
          reverse = false;
          currentIndex = 0.f;
          isStepEOC = true;
        }
      }
    }
    else {
      currentIndex += tableDelta;
      if (currentIndex >= waveTable->WAVETABLE_SIZE) {
        currentIndex -= waveTable->WAVETABLE_SIZE;
        isStepEOC = true;
      }
    }
  }

  bool isEOC() {
    return isStepEOC;
  }

  void setPitch(float pitch, float sampleRate) {
    if (waveTable == nullptr)
      return;

    if (pitch != prevPitch) {
      float frequency = dsp::FREQ_C4 * powf(2.0f, pitch);
      auto tableSizeOverSampleRate = waveTable->WAVETABLE_SIZE / sampleRate;
      tableDelta = frequency * tableSizeOverSampleRate;
      prevPitch = pitch;
    }
  }

  void setMirror(bool _mirror) {
    mirror = _mirror;
    reset();
  }


  void reset() {
    currentIndex = 0.f;
  }
};