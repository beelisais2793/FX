#include "Oscillator.hpp"
#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif // !_USE_MATH_DEFINES
#include <cmath>

namespace brettstory {
	namespace audio {
        namespace synthisize {
            const double two_pi = 2 * M_PI;

            void Oscillator::Generate(double* buffer, int number_of_frames) {
                switch (oscillator_mode) {
                    case OSCILLATOR_MODE_SINE:
                        GenerateSineWave(buffer, number_of_frames);
                        break;

                    case OSCILLATOR_MODE_SAW:
                        GenerateSawWave(buffer, number_of_frames);
                        break;

                    case OSCILLATOR_MODE_SQUARE:
                        GenerateSquareWave(buffer, number_of_frames);
                        break;

                    case OSCILLATOR_MODE_TRIANGLE:
                        GenerateTriangleWave(buffer, number_of_frames);
                        break;
                }
            }

            void Oscillator::SetMode(OscillatorMode mode) {
                oscillator_mode = mode;
            }

            void Oscillator::SetFrequency(double new_frequency) {
                frequency = new_frequency;
                UpdateIncrement();
            }

            void Oscillator::SetSampleRate(double new_sample_rate) {
                sample_rate = fmax(1.0, new_sample_rate);
                UpdateIncrement();
            }

            void Oscillator::GenerateSawWave(double* buffer, int number_of_frames) {
                for (int i = 0; i < number_of_frames; i++) {
                    buffer[i] = 1.0 - (2.0 * phase / two_pi);
                    phase += phase_increment;

                    while (phase >= two_pi) {
                        phase -= two_pi;
                    }
                }
            }

            void Oscillator::GenerateSineWave(double* buffer, int number_of_frames) {
                for (int i = 0; i < number_of_frames; i++) {
                    buffer[i] = sin(phase);
                    phase += phase_increment;

                    while (phase >= two_pi) {
                        phase -= two_pi;
                    }
                }
            }

            void Oscillator::GenerateSquareWave(double* buffer, int number_of_frames) {
                for (int i = 0; i < number_of_frames; i++) {
                    if (phase <= M_PI) {
                        buffer[i] = 1.0;
                    }
                    else {
                        buffer[i] = -1.0;
                    }

                    phase += phase_increment;
                    while (phase >= two_pi) {
                        phase -= two_pi;
                    }
                }
            }

            void Oscillator::GenerateTriangleWave(double* buffer, int number_of_frames) {
                    for (int i = 0; i < number_of_frames; i++) {
                        double value = -1.0 + (2.0 * phase / two_pi);
                        buffer[i] = 2.0 * (fabs(value) - 0.5);
                        phase += phase_increment;

                        while (phase >= two_pi) {
                            phase -= two_pi;
                        }
                    }
            }

            void Oscillator::UpdateIncrement() {
                phase_increment = frequency * two_pi / sample_rate;
            }
        }
    }
}