// replicate the circuit from the fade example :
// https://www.arduino.cc/en/Tutorial/Fade
// then upload this sketch to your arduino

#include <ShapedOscillator.h>

#define LED_PIN 9

ShapedOscillator freqLfo(0.1, 0, -1);
ShapedOscillator shapeLfo(0.02);
ShapedOscillator osc(1, 0, -1);

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  osc.setFrequency(freqLfo.update() * 19 + 1);
  osc.setXShape(shapeLfo.update() * 2 - 1);

  analogWrite(LED_PIN, osc.update() * 255);
}
