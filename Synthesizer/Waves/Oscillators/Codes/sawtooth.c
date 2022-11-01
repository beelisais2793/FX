Sawtooth

p = period = 1/fs = f/fs
t = t - abs(t)
t = t mod 1 
t = 2*(t/p - (1/2 + t/p))
t = (2*a)/pi * atan(cot((pi*x)/p)

float scaleBetween(float unscaledNum, float minAllowed, float maxAllowed, float min, float max) { //scaling function
    return (maxAllowed - minAllowed) * (unscaledNum - min) / (max - min) + minAllowed;
}

//const int16_t sampleRate = 44100;
const int16_t nyquist = 21000;

const int tableSize = 256; // the size of your wavetable
const int numberOfTables = 45;
//const int notesInTables = nyquist/numberOfTables;
const float PI = 3.141592653589793238462643;
//float phase = PI; //Uncoment this if you want to flip the wave phase
float phase = 0;
float angularFreq = (2* PI)/tableSize;
float table[numberOfTables][tableSize] = {{0},{0}};

float output;
int16_t scaledOutput;

float maxAmp = 1;
int numberOfHarmonics; //those will be calculated in the loop

float minValue = 0; //those will be used for normalizing the scale
float maxValue = 0;

int16_t resolution = 32767; //how big the biggest value will be

const float midiFrequencies[136] = {8.66196, 9.17702, 9.72272, 10.3009, 10.9134, 11.5623, 12.2499, 12.9783, 13.75, 14.5676, 15.4339, 16.3516, 17.3239, 18.354, 19.4454, 20.6017, 21.8268, 23.1247, 24.4997, 25.9565, 27.5, 29.1352, 30.8677, 32.7032, 34.6478, 36.7081, 38.8909, 41.2034, 43.6535, 46.2493, 48.9994, 51.9131, 55, 58.2705, 61.7354, 65.4064, 69.2957, 73.4162, 77.7817, 82.4069, 87.3071, 92.4986, 97.9989, 103.826, 110, 116.541, 123.471, 130.813, 138.591, 146.832, 155.563, 164.814, 174.614, 184.997, 195.998, 207.652, 220, 233.082, 246.942, 261.626, 277.183, 293.665, 311.127, 329.628, 349.228, 369.994, 391.995, 415.305, 440, 466.164, 493.883, 523.251, 554.365, 587.33, 622.254, 659.255, 698.456, 739.989, 783.991, 830.609, 880, 932.328, 987.767, 1046.5, 1108.73, 1174.66, 1244.51, 1318.51, 1396.91, 1479.98, 1567.98, 1661.22, 1760, 1864.65, 1975.53, 2093, 2217.46, 2349.32, 2489.02, 2637.02, 2793.83, 2959.96, 3135.96, 3322.44, 3520, 3729.31, 3951.07, 4186.01, 4434.92, 4698.64, 4978.03, 5274.04, 5587.65, 5919.91, 6271.93, 6644.88, 7040, 7458.62, 7902.13, 8372.02, 8869.84, 9397.27, 9956.06, 10548.1, 11175.3, 11839.8, 12543.9, 13289.7, 14080, 14917.2, 15804.3, 16744, 17739.7, 18794.5, 19912.1, 21096.2};

int z = 0;
for (int t=0; t<numberOfTables; t++) {
        
        numberOfHarmonics = round(nyquist/midiFrequencies[z]); //skips every three notes
        
        for (int n=0; n<tableSize; n++) { //creates a Sawtooth wavetable
            
            for(int m=0; m<numberOfHarmonics; m++) {
                
                table[t][n] = (table[t][n] + ( (maxAmp/(m+1) * sin((angularFreq*(m+1))*n + (phase))))); //sawtooth formula
            }
            
            if (table[t][n] > maxValue) { //checks highest value
                maxValue = table[t][n];
            }
            if (table[t][n] < minValue) { //checks lowest value
                minValue = table[t][n];
            }
            
        }
        z = z+3;
    }


   