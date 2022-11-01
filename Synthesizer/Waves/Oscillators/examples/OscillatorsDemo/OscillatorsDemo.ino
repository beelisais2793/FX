// upload this sketch on any board and open the serial plotter

#include <BlinkOscillator.h>
#include <NoiseOscillator.h>
#include <SequenceOscillator.h>
#include <ShapedOscillator.h>

BlinkOscillator blink;
NoiseOscillator noiz;
SequenceOscillator seq;
ShapedOscillator tri;

// blink an S.O.S :
static const uint32_t sequence[] = {
  100, 100, 100, 100, 100, 200,
  200, 100, 200, 100, 200, 200,
  100, 100, 100, 100, 100, 600  
};

size_t sequenceLength = sizeof(sequence) / sizeof(*sequence);

void setup() {
  blink.setFrequency(0.5);
  blink.setOnOffRatio(0.3);

  noiz.setFrequency(10);

  seq.setSequence((uint32_t *) sequence, sequenceLength);

  tri.setFrequency(2);
  tri.setXShape(-1);
  tri.setYShape(1);

  Serial.begin(115200);
}

void loop() {
  Serial.print(blink.update() ? 1 : 0);
  Serial.print(" ");
  Serial.print(noiz.update());
  Serial.print(" ");
  Serial.print(seq.update() ? 1 : 0);
  Serial.print(" ");
  Serial.println(tri.update());

  delay(10);
}