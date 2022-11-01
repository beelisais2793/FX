#pragma once
#include <cmath>
#include <vector>

using std::vector;


class BezierCurve{
public:

    /***************************************
     * Default contructor
     ***************************************/
    BezierCurve();
    
    /***************************************
     * Set the X and Y values for the points
     * used in the curve.  P0 is the start
     * point, P1 is the control point,
     * and P2 is the end point.
     ***************************************/
    void set(float x0, float y0, float x1, float y1, float x2, float y2);
    
    /***************************************
     * Approximates t value based on x and 
     * returns Y
     ***************************************/
    float getValueAt(float input);
    
private:
    float x[3];
    float y[3];
};


class Buffer{
public:
  Buffer();
  Buffer(size_t size);
  void initialize(size_t size);
  //Writes one sample to buffer then increments writeIndex
  void write(float sample);
  
  //Increments readIndex and returns sample at that index
  float read();
  
  float getSample(const size_t index);
  
  void setSample(float sample, size_t index);

  void shiftReadIndex(int amount);
  
  void setReadIndex(int index);
  
  void setWriteIndex(int index);

  int getWriteIndex();

  int getReadIndex();
  
  void resize(size_t size);
private:
  void increment(size_t &index);
  void decrement(size_t &index);
  float *data;
  size_t writeIndex;
  size_t readIndex;
  size_t numSamples;
};


#ifndef e
#define e 2.718281828459
#endif

class EnvelopeDetector{
public:
    
    EnvelopeDetector(bool isDigital = true);
    float getNextValue(float input);
    void setParams(float releaseTimeMs, float attackTimeMs, float sampleRate);
    
private:
    float ta; //Attack Time
    float tr; //Release Time
    float tc; //Time Constant;
    
    bool digital;
    
    float fs;
    
    float yn1; //Previous output value
};


float floatToDecibel(float);

float decibelToFloat(float);

float msToSamples(float, float);

float samplesToMs(float, float);


/*
Simple class to generate oscillators or LFOs.  Good for generating waves that wont change frequency
during playback because it can make audible pops or clicks when changing frequency.  However it is slightly more
efficient than the GordonSmithOscillator.
*/

class DirectFormOscillator {
public:
    DirectFormOscillator();
    
    /*Must be called at the beginning of processBlock() in the Audio Processor
     *This method initializes all the values neccesary to begin oscillation.*/
    void setFrequency(float frequency, float sampleRate);
    
    /*Returns the next sample and increments the delaySamples*/
    float getNextSample(float input);
	
	/*Called to reset the output buffer*/
    void flush();
private:
    float theta;
    float fo; 
    float fs;
    
    float b1, b2;
    float yn1, yn2;
    
    bool init;
};

class GordonSmithOscillator{
public:
	GordonSmithOscillator();
	/*Must be called at the beginning of processBlock() in the Audio Processor
	*This method initializes all the values neccesary to begin oscillation.*/
	void setFrequency(float frequency, float sampleRate);

	/*Returns the next sample and increments the delaySamples*/
	float getNextSample(float input);
	void flush();

private:

	float yq;
	float yq1;
	float yn;
	float yn1;
	float epsilon;

	float theta;
	float fo;
	float fs;

	bool init;

};


class ModulatedDelay{
public:
    ModulatedDelay();
    ModulatedDelay(size_t minDelay, size_t maxDelay, float fb, float wet);
    void initialize(size_t minDelay, size_t maxDelay, float fb, float wet);
    float getNextSample(float sample);
    void resize(size_t delayAmount);
    void flush();
    void setFeedback(float feedback);
    void setWet(float wet_amount);
    void setDepth(float depth);
    
    /*Must be set before processing begins*/
    void setFrequency(float frequency, float sampleRate);
    
    /*Must be set before processing begins*/
    void setDelayRange(int min, int max);

    int getWriteIndex();

    int getReadIndex();
    

private:
    Buffer buffer;
    GordonSmithOscillator osc;
    
    //Controls the ratio of input to out. (dry = 1 - wet)
    float wet;
    
    //Feedback amount in range of 0 to 1
    float fb;
    
    //Sample Rate
    int fs;
    
    float depth;
    
	int centerDelay;
	
	int prevDelay;
	
	int scalar;
    
    //Speed of oscillation
    float oscFreq;
    
    //Number of samples that the read index can vary by.
    int delayRange;
};   


class BiQuadFilter {
public: 
	/*
	Constructor. Initializes the sample rate and center frequency.
	Creates a series of Buffers and pushes them into the vectors for each channel.
	*/
	BiQuadFilter(int numChannels, int centerFrequency, double sampleRate);
	
	/*
	Calculates and returns the next output sample based on the sample that is input.
	Channel specifies which channel is being processed.
	*/
	float nextSample(float sample, int channel);
	
	void setSampleRate(double sampleRate);
	double getSampleRate();
	
	/*
	Call any time the center frequency is changed. All coefficients are recalculated.
	*/
	virtual void update(float fc, float fs, float G = 0);
	
	int numChannels;
	double fs; //Sample Rate
	int fc; //Center Frequency

	float a0, a1, a2; //Input coefficients
	float b1, b2; //Output coefficients
	float c0, d0; //Dry/Wet coefficients
	
private:
	//Holds a number of buffers equal to the number of channels.
	//Each buffer only has two samples and represents the input buffer.
	vector<Buffer> inputBuffer;
	
	//Holds a number of buffers equal to the number of channels.
	//Each buffer only has two samples and represents the output buffer.
	vector<Buffer> outputBuffer;
};

  

class HighPassIIR : public BiQuadFilter {
public:
	HighPassIIR(int numChannels, float fc, float fs);

	void update(float fc, float fs, float G);

private:
	float thetac;
	float omegac;
	float K;
	float sigma;
};  


class LowPassIIR : public BiQuadFilter {
public:
	LowPassIIR(int numChannels, float fc, float fs);

	void update(float fc, float fs, float G);

private:
	float thetac;
	float omegac;
	float K;
	float sigma;
};

class BandPassIIR : public BiQuadFilter {
public:
  BandPassIIR(int numChannels, float fc, float fs);
  float nextSample(float sample, int channel);
  void update(float fc, float fs);
  
  void setSampleRate(double sampleRate);
  double getSampleRate();

private:
  LowPassIIR *lowpass;
  HighPassIIR *highpass;
};


class PeakIIR : public BiQuadFilter {
public:
	PeakIIR(int numChannels, float G, float fc, float Q, float fs);

	void update(float fc, float fs, float G);

private:
	float thetac;
	float mu;
	float sigma;
	float beta;
	float gamma;
	float Q; //Quality factor A.K.A bandwidth
	int G;
};


class Chorus{
public:
    Chorus();
    float getNextSample(float input);
    void setWet(float wet);
    
    /*Must be called before processing*/
    void setSpeed(float frequency, float sampleRate);
    /*Must be called before processing*/
    void setDepth(int milliseconds, float sampleRate);
private:
    ModulatedDelay modDly;
};


class Compressor{
public:
    Compressor(int numChannels, bool isDigital = true);
    void setRatio(float ratio){r = ratio;}
    void setThreshold(float threshold){th = threshold;}
    void setParams(float timeAttack, float timeRelease, float threshold, float ratio, float sampleRate);
    float getNextSample(float input, int channel);
private:
    std::vector<EnvelopeDetector*> detector;
    //attack time
    float attack;
    //release time
    float release;
    //compression ratio
    float r;
    //threshold
    float th;
	
	int numChannels;
};

class DigitalDelay{
public:
  DigitalDelay(size_t delayAmount, float fb, float wet  = 0);
  float getNextSample(float sample);
  void resize(size_t delayAmount);
  void flush();
  
  void setFeedback(float feedback);
  void setWet(float wet_amount);
    
private:
    Buffer buffer;
    
    //Controls the ratio of input to out. (dry = 1 - wet)
    float wet;
    
    //Feedback amount in range of 0 to 1
    float fb;
    
    //Sample Rate
    int fs;
    
	//Number of samples to be delayed
    size_t delayAmount;
};

class Flanger{
public:
    Flanger();
    float getNextSample(float input);
    void setWet(float wet);
    
    /*Must be called before processing*/
    void setSpeed(float frequency, float sampleRate);
    /*Must be called before processing*/
    void setDepth(float milliseconds, float sampleRate);
private:
    ModulatedDelay modDly;
};


 class Vibrato{
public:
    Vibrato();
    float getNextSample(float input);
    void setWet(float wet);
    
    /*Must be called before processing*/
    void setSpeed(float frequency, float sampleRate);
    /*Must be called before processing*/
    void setDepth(int milliseconds, float sampleRate);
private:
    ModulatedDelay modDly;  
};


BezierCurve::BezierCurve(){
    
}

void BezierCurve::set(float x0, float y0, float x1, float y1, float x2, float y2){
    x[0] = x0;
    x[1] = x1;
    x[2] = x2;
    y[0] = y0;
    y[1] = y1;
    y[2] = y2;
}

float BezierCurve::getValueAt(float input){
    float t = fabs(input - x[0])/fabs(x[2] - x[0]);//approximate t from x
    float output = pow(1 - t, 2) * y[0] + 2 * (1 - t) * t * y[1] + t * t * y[2];
    return output;
}


Buffer::Buffer(){
}

Buffer::Buffer(size_t size){
  initialize(size);
}

void Buffer::initialize(size_t size){
  data = new float[size * 2];
  writeIndex = size - 1;
  readIndex = 0;
  numSamples = size;
}

void Buffer::write(float sample){
  data[writeIndex] = sample;
  increment(writeIndex);
}

float Buffer::read(){
  if(data == NULL)
    return 0;
  float val = data[readIndex];
  increment(readIndex);
  return val;
}

float Buffer::getSample(const size_t index){
  return data[index];
}

void Buffer::setSample(float sample, size_t index){
  data[index] = sample;
}

void Buffer::setReadIndex(int index){
  readIndex = index;
}
  
void Buffer::setWriteIndex(int index){
  writeIndex = index;
}

void Buffer::increment(size_t &index){
  index++;
  if(index == numSamples)
    index = 0;
}

void Buffer::decrement(size_t &index){
  index--;
  if(index < 0)
    index = numSamples - 1;
}

void Buffer::resize(size_t size){
  int difference = numSamples - size;
  shiftReadIndex(difference);
}

void Buffer::shiftReadIndex(int amount){
  if(amount < 0)
    for(int i = 0; i > amount; i--)
      decrement(readIndex);
  if(amount > 0)
    for(int i = 0; i < amount; i++)
      increment(readIndex);
}

int Buffer::getWriteIndex(){return writeIndex;}

int Buffer::getReadIndex(){return readIndex;}


EnvelopeDetector::EnvelopeDetector(bool isDigital){
    digital = isDigital;
    if(isDigital){
        tc = logf(0.01);
    }
    else{
        tc = logf(0.368);
    }
}

void EnvelopeDetector::setParams(float attackTimeMs, float releaseTimeMs, float sampleRate){
    fs = sampleRate;
    //ta = powf(e, (tc/(attackTimeMs * fs * 0.001)));
    //tr = powf(e, (tc/(releaseTimeMs * fs * 0.001)));
	ta = exp(-1 / ((fs / 1000) * attackTimeMs));
	tr = exp(-1 / ((fs / 1000) * releaseTimeMs));
}

float EnvelopeDetector::getNextValue(float input){
	input = fabs(input);
    if(input > yn1){
        //yn1 = ta * (yn1 - input) + input;
		yn1 *= ta;
		yn1 += (1 - ta) * input;
    }
    else{
        //yn1 = tr * (yn1 - input) + input;
		yn1 *= tr;
		yn1 += (1 - tr) * input;
    }
    return yn1;
}


float floatToDecibel(float value){
  return 20 * logf(value);
}

float decibelToFloat(float value){
  return pow(10, (value/20));
}

float msToSamples(float ms, float sampleRate){
  return ms * (sampleRate / 1000);
}

float samplesToMs(float samples, float sampleRate){
  return samples/(sampleRate/1000);
}


Chorus::Chorus() : modDly(){}

float Chorus::getNextSample(float input){
    return modDly.getNextSample(input);
}

void Chorus::setSpeed(float frequency, float sampleRate){
    modDly.setFrequency(frequency, sampleRate);
}

void Chorus::setDepth(int milliseconds, float sampleRate){
    modDly.initialize(msToSamples(7, sampleRate), msToSamples(milliseconds, sampleRate), 0.99, 0.5);
}

Compressor::Compressor(int numChannels, bool isDigital){
    for(int i = 0; i < numChannels; i++){
		detector.push_back(new EnvelopeDetector(isDigital));
	}
	Compressor::numChannels = numChannels;
}

void Compressor::setParams(float timeAttack, float timeRelease, float threshold, float ratio, float sampleRate){
    for(int i = 0; i < numChannels; i++)
		detector[i]->setParams(timeAttack, timeRelease, sampleRate);
    setRatio(ratio);
    setThreshold(threshold);
}

float Compressor::getNextSample(float input, int channel){
	float level = detector[channel]->getNextValue(input);
    float compLevel;
    if(level <= th){
        return input;
    }
    else{
        //Calculate new level after compression then apply to input
        compLevel = (1/r) * (level - th) + th;
		
        return input * compLevel / level;
    }
}

DigitalDelay::DigitalDelay(size_t delayAmount, float fb, float wet) : buffer(){
	this->delayAmount = delayAmount;
	this->fb = fb;
	this->wet = wet;
    buffer.initialize(delayAmount);
}

float DigitalDelay::getNextSample(float sample){
    float yn = buffer.read();
    buffer.write(sample + fb * yn);
    return wet * yn + sample * (1 - wet);
}

void DigitalDelay::resize(size_t delayAmount){
    buffer.resize(delayAmount);
}

void DigitalDelay::setFeedback(float feedback){ fb = feedback; }

void DigitalDelay::setWet(float wet_amount){ wet = wet_amount; }

void DigitalDelay::flush(){
    
}


Flanger::Flanger() : modDly(){}

void Flanger::setDepth(float milliseconds, float sampleRate){
    modDly.initialize(0, msToSamples(milliseconds, sampleRate), 0.99, 0.5);
}

float Flanger::getNextSample(float input){
    return modDly.getNextSample(input);
}

void Flanger::setSpeed(float frequency, float sampleRate){
    modDly.setFrequency(frequency, sampleRate);
}


ModulatedDelay::ModulatedDelay(): buffer(), osc(){}
ModulatedDelay::ModulatedDelay(size_t minDelay, size_t maxDelay, float fb, float wet) : buffer(maxDelay), osc(){
    this->fb = fb;
    this->wet = wet;
    depth = 1;
    
    this->scalar = (maxDelay - minDelay) / 2;
    
    this->centerDelay = minDelay + scalar;

    buffer.initialize(maxDelay);
    buffer.setReadIndex(minDelay);
    buffer.setWriteIndex(maxDelay);
}
void ModulatedDelay::initialize(size_t minDelay, size_t maxDelay, float fb, float wet){
    this->fb = fb;
    this->wet = wet;
    depth = 1;
    
    this->scalar = (maxDelay - minDelay) / 2;
    
    this->centerDelay = minDelay + scalar;

    buffer.initialize(maxDelay);
    buffer.setReadIndex(minDelay);
    buffer.setWriteIndex(maxDelay);
}

float ModulatedDelay::getNextSample(float sample){
    float oscVal = osc.getNextSample(0);
    int currentDelay = centerDelay + (oscVal * scalar);
    float yn = buffer.read();
    buffer.shiftReadIndex(prevDelay - currentDelay);
    prevDelay = currentDelay;
    buffer.write(sample + fb * yn);
    return wet * yn + sample * (1 - wet);
    return 0;
}

void ModulatedDelay::resize(size_t delayAmount){
    buffer.resize(delayAmount);
}

void ModulatedDelay::flush(){}

void ModulatedDelay::setDepth(float depth){
    this->depth = depth;
}

void ModulatedDelay::setFeedback(float feedback){
    fb = feedback;
}

void ModulatedDelay::setWet(float wet_amount){
    wet = wet_amount;
}

void ModulatedDelay::setFrequency(float frequency, float sampleRate){
    osc.setFrequency(frequency, sampleRate);
    oscFreq = frequency;
}

int ModulatedDelay::getWriteIndex(){
    return buffer.getWriteIndex();
}

int ModulatedDelay::getReadIndex(){
    return buffer.getReadIndex();
}


Vibrato::Vibrato() : modDly(){}

float Vibrato::getNextSample(float input){
    return modDly.getNextSample(input);
}

void Vibrato::setSpeed(float frequency, float sampleRate){
    modDly.setFrequency(frequency, sampleRate);
}

void Vibrato::setDepth(int milliseconds, float sampleRate){
    modDly.initialize(0, msToSamples(milliseconds, sampleRate), 0, 1);
}


BandPassIIR::BandPassIIR(int numChannels, float fc, float fs) : BiQuadFilter(numChannels, fc, fs){
    lowpass = new LowPassIIR(numChannels, fc, fs);
    highpass = new HighPassIIR(numChannels, fc, fs);
}
float BandPassIIR::nextSample(float sample, int channel){
    return highpass->nextSample(lowpass->nextSample(sample, channel), channel);
}
void BandPassIIR::update(float fc, float fs){
    lowpass->update(fc, fs, 1);
    highpass->update(fc, fs, 1);
}

void BandPassIIR::setSampleRate(double sampleRate) {
    lowpass->setSampleRate(sampleRate);
    highpass->setSampleRate(sampleRate);
}

double BandPassIIR::getSampleRate(){
    return lowpass->getSampleRate();
}

void BiQuadFilter::setSampleRate(double sampleRate) { fs = sampleRate; }
double BiQuadFilter::getSampleRate() { return fs; }
BiQuadFilter::BiQuadFilter(int numChannels, int centerFrequency, double sampleRate){
	fc = centerFrequency;
	BiQuadFilter::numChannels = numChannels;
	fs = sampleRate;
	for (int i = 0; i < numChannels; i++){
		Buffer *b = new Buffer(2);
		inputBuffer.push_back(*b);
		b = new Buffer(2);
		outputBuffer.push_back(*b);
	}
}

float BiQuadFilter::nextSample(float sample, int channel){
	float output = a0 * sample + a1 * inputBuffer[channel].getSample(0) + a2 * inputBuffer[channel].getSample(1)
				 - b1 * outputBuffer[channel].getSample(0) - b2 * outputBuffer[channel].getSample(1);
				 
	outputBuffer[channel].setSample( outputBuffer[channel].getSample(0),1);
	outputBuffer[channel].setSample(output, 0);
	inputBuffer[channel].setSample(inputBuffer[channel].getSample(0), 1);
	inputBuffer[channel].setSample(sample, 0);
	
	output = 0.5 * c0 * output + 0.5 * d0 * sample;	
	return output;
}

void BiQuadFilter::update(float fc, float fs, float G){}


HighPassIIR::HighPassIIR(int numChannels, float fc, float fs) : BiQuadFilter(numChannels, fc, fs) {
		update(fc, fs, 0);
}

void HighPassIIR::update(float fc, float fs, float G) {
	HighPassIIR::fc = fc;
	HighPassIIR::fs = fs;

	thetac = M_PI * fc / fs;
	omegac = M_PI * fc;
	K = omegac / tanf(thetac);
	sigma = K * K + omegac * omegac + 2 * K * omegac;

	a0 = K * K / sigma;
	a1 = -2 * a0;
	a2 = a0;
	b1 = (-2 * K * K + 2 * omegac * omegac) / sigma;
	b2 = (-2 * K * omegac + K * K + omegac * omegac) / sigma;
	c0 = 1;
	d0 = 0;
}


LowPassIIR::LowPassIIR(int numChannels, float fc, float fs) : BiQuadFilter(numChannels, fc, fs) {
	update(fc, fs, 0);
}

void LowPassIIR::update(float fc, float fs, float G) {
	LowPassIIR::fc = fc;
	LowPassIIR::fs = fs;
	thetac = M_PI * fc / fs;
	omegac = M_PI * fc;
	K = omegac / tanf(thetac);
	sigma = K * K + omegac * omegac + 2 * K * omegac;
	a0 = omegac * omegac / sigma;
	a1 = 2 * a0;
	a2 = a0;
	b1 = (-2 * K * K + 2 * omegac * omegac) / sigma;
	b2 = (-2 * K * omegac + K * K + omegac * omegac) / sigma;
	c0 = 1;
	d0 = 0;
}


PeakIIR::PeakIIR(int numChannels, float G, float fc, float Q, float fs) : BiQuadFilter(numChannels, fc, fs){
	PeakIIR::Q = Q;
	update(fc, fs, G);
}

void PeakIIR::update(float fc, float fs, float G) {
	PeakIIR::fc = fc;
	PeakIIR::fs = fs;
	PeakIIR::G = G;
	thetac = 2 * M_PI * fc / fs;
	mu = powf(10, G / 20);
	sigma = 4 / (1 + mu);
	beta = 0.5 * ((1 - sigma * tanf(thetac / (2 * Q))) / (1 + sigma * tanf(thetac / (2 * Q))));
	gamma = (0.5 + beta) * cosf(thetac);
	a0 = 0.5 - beta;
	a1 = 0.0f;
	a2 = -a0;
	b1 = -2 * gamma;
	b2 = 2 * beta;
	c0 = mu - 1.0;
	d0 = 1.0;
}


DirectFormOscillator::DirectFormOscillator(){
    init = false;
}

void DirectFormOscillator::setFrequency(float frequency, float sampleRate){
    if(fo != frequency){
        flush();
        fo = frequency;
    }
    fs = sampleRate;
	theta = 2 * M_PI * fo / fs;
    b1 = -2 * cosf(theta);
    b2 = 1.0f;
    if(!init){
        yn1 = sinf(-1 * theta);
        yn2 = sinf(-2 * theta);
        init = true;
    }
}

float DirectFormOscillator::getNextSample(float input){
    float output = - (b1 * yn1) - (b2 * yn2);
    yn2 = yn1;
    yn1 = output;
    return output;
}

void DirectFormOscillator::flush(){
    yn1 = yn2 = 0;
    init = false;
}

GordonSmithOscillator::GordonSmithOscillator() {
	init = false;
}

void GordonSmithOscillator::setFrequency(float frequency, float sampleRate) {
	fo = frequency;
	fs = sampleRate;

	theta = 2 * M_PI * fo / fs;
	epsilon = 2 * sinf(theta / 2);

	if (!init) {
		yn1 = sinf(-1 * theta);
		yq1 = cosf(-1 * theta);
		init = true;
	}

}

float GordonSmithOscillator::getNextSample(float input) {
	yq = yq1 - epsilon * yn1;
	yn = epsilon * yq + yn1;

	yq1 = yq;
	yn1 = yn;
	return yn;
}

void GordonSmithOscillator::flush() {

}