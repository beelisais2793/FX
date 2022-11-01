// https://github.com/janne808/kocmoc-rack-modules/blob/master/src/phasor.cpp
class Phasor{
public:
  // constructor/destructor
  Phasor(double initialPhase, double initialFrequency, double initialSampleRate);
  Phasor();
  ~Phasor();

  // state tick
  void Tick();
  
  // set parameters
  void SetPhase(double newPhase);
  void SetFrequency(double newFrequency);
  void SetPhaseModulation(double newPhaseModulation);
  void SetSampleRate(double newSampleRate);

  // get parameters
  double GetPhase();
  double GetPhaseIncrement();
  double GetFrequency();
  double GetPhaseModulation();
  double GetSampleRate();

private:
  double phase;
  double phaseIncrement;
  double phaseModulation;
  double frequency;
  double sampleRate;

  void ComputePhaseIncrement();
};

// constructor
Phasor::Phasor(double initialPhase, double initialFrequency, double initialSampleRate) {
  phase = initialPhase;
  frequency = initialFrequency;
  sampleRate = initialSampleRate;
  ComputePhaseIncrement();
}

// default constructor
Phasor::Phasor() {
  phase = 0.0;
  frequency = 440.0;
  sampleRate = 44100.0;
  ComputePhaseIncrement();  
}

// destructor
Phasor::~Phasor() {
}

// state tick
void Phasor::Tick() {
  // increment phase
  phase += phaseIncrement;

  // make sure phase stays in [0..2*PI]
  phase = fmod(phase, 2.0*M_PI);
}

void Phasor::SetPhase(double newPhase) {
  phase = newPhase;
}

void Phasor::SetFrequency(double newFrequency) {
  frequency = newFrequency;
  ComputePhaseIncrement();
}

void Phasor::SetPhaseModulation(double newPhaseModulation) {
  phaseModulation = newPhaseModulation;
}

void Phasor::SetSampleRate(double newSampleRate) {
  sampleRate = newSampleRate;
  ComputePhaseIncrement();
}

double Phasor::GetPhase() {
  return phase + phaseModulation;
}

double Phasor::GetPhaseIncrement() {
  return phaseIncrement;
}

double Phasor::GetFrequency() {
  return frequency;
}

double Phasor::GetSampleRate() {
  return sampleRate;
}

double Phasor::GetPhaseModulation() {
  return phaseModulation;
}

void Phasor::ComputePhaseIncrement(){
  phaseIncrement = 2.0*M_PI*frequency/sampleRate;
}