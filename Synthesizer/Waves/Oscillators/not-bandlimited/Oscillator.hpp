#include <math.h>
namespace brettstory {
	namespace audio {
        namespace synthisize {
            enum OscillatorMode {
                OSCILLATOR_MODE_SINE = 0,
                OSCILLATOR_MODE_SAW = 1,
                OSCILLATOR_MODE_SQUARE = 2,
                OSCILLATOR_MODE_TRIANGLE = 3
            };

            class Oscillator {
            private:
                OscillatorMode oscillator_mode;
                double frequency;
                double phase;
                double sample_rate;
                double phase_increment;

                void GenerateSineWave(double* buffer, int number_of_frames);
                void GenerateSawWave(double* buffer, int number_of_frames);
                void GenerateSquareWave(double* buffer, int number_of_frames);
                void GenerateTriangleWave(double* buffer, int number_of_frames);
                void UpdateIncrement();
            public:
                void SetMode(OscillatorMode mode);
                void SetFrequency(double new_frequency);
                void SetSampleRate(double new_sample_rate);
                void Generate(double* buffer, int nFrames);
                
                Oscillator() :
                    oscillator_mode(OSCILLATOR_MODE_SINE),
                    frequency(440.0),
                    phase(0.0),
                    sample_rate(44100.0) { UpdateIncrement(); };
            };
        }
    }
}
