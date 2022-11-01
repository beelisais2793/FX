#pragma once

float freq_to_midi(float f) {
    return 12.0*log2(f/440.0) + 69;
}
float midi_to_freq(float m) {
    return pow(2.0, (m-69)/12)*440.0;
}
float semitone(float semi, float f1)
{
    float f2 = f1*pow(2,semi/12.0f);
    return f2;
}
float octave(int octave, float f1) {
    float f2 = f1*pow(2,octave);
    return f2;
}
float freq2cv(float f, float f0 = 261.6256)
{
    return log2(f/f0);
}
float cv2freq(float cv, float f0 = 261.6256)
{
    return f0*pow(2,cv);
}