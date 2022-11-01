#pragma once

double freq_to_midi(double f) {
    return 12.0*log2(f/440.0) + 69.0;
}
double midi_to_freq(double m) {
    return pow(2.0, (m-69.0)/12.0)*440.0;
}
double semitone(double semi, double f1)
{
    double f2 = f1*pow(2.0,semi/12.0);
    return f2;
}
double octave(int octave, double f1) {
    double f2 = f1*pow(2.0,(double)octave);
    return f2;
}
double freq2cv(double f, double f0 = 261.6256)
{
    return log2(f/f0);
}
double cv2freq(double cv, double f0 = 261.6256)
{
    return f0*pow(2.0,cv);
}