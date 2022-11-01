#ifndef FILTER_BANK_H
#define FILTER_BANK_H

namespace FX::Filters::FilterBank
{
  enum WindowType { HAMMING, BLACKMAN, GAUSSIAN, KAISER, ERROR };

  class FilterBank
  {
    public:

    // This constructor designs a new filter with the specified parameters, and initializes the weights array.
    FilterBank(bool verbose, unsigned channels, unsigned taps, WindowType windowType);

    unsigned getNrTaps();
    unsigned getNrChannels();
    
    DspFloatType *getWeights(unsigned channel);

    // Reverse the order of the taps. This can be useful, because FIR implementatons might run over the taps backwards for efficiency.
    // This is the case for LOFAR, for example.
    void reverseTaps();

    // Print the weights array in the natural order, in a format that can be read by gnuplot.
    void printWeights();

    static WindowType getWindowTypeFromString(char* s);

  private:
    // Hamming window function
    void hamming(unsigned n, DspFloatType d[]);

    // Blackman window function
    void blackman(unsigned n, DspFloatType d[]);

    // Gaussian window function
    void gaussian(int n, DspFloatType a, DspFloatType d[]);

    // Kaiser window function
    void kaiser(int n, DspFloatType beta, DspFloatType d[]);

    // helper functions
    DspFloatType besselI0(DspFloatType x);
    void interpolate(const DspFloatType x[], const DspFloatType y[], unsigned xlen, unsigned n, DspFloatType result[]);
    void generate_fir_filter(unsigned n, DspFloatType w, const DspFloatType window[], DspFloatType result[]);
    void generate_filter();

    // Returns the first power of two higher than n.
    unsigned nextPowerOfTwo(unsigned n);

    // The window used for generating the filter, default is KAISER.
    WindowType itsWindowType;

    const unsigned itsNrTaps;
    const unsigned itsNrChannels;
    const bool itsVerbose;

    DspFloatType* weights; // [nrChannels][taps];
  };
}
#endif // FILTER_BANK_H