
#pragma once


class WDF
{
public:
    virtual ~WDF() {}

    WDF(DspFloatType Rp) : R(Rp), G(1 / Rp), a(0), b(0) {}
    //----------------------------------------------------------------------
    virtual std::string getLabel() const = 0;
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected() = 0;
    virtual inline void incident(DspFloatType value) = 0;
    //----------------------------------------------------------------------
    DspFloatType voltage() // v
    {
        return (a + b) / 2.f;
    }
    //----------------------------------------------------------------------
    DspFloatType current() // i
    {
        return (a - b) / 2.f * R;
    }
    //----------------------------------------------------------------------
    DspFloatType R; // the WDF port resistance
    DspFloatType G; // the inverse port resistance
    DspFloatType a; // incident wave (incoming wave)
    DspFloatType b; // reflected wave (outgoing wave)
    //----------------------------------------------------------------------
};
//==============================================================================
class Adaptor : public WDF
{
public:
    WDF *left;  // WDF element connected at the left port
    WDF *right; // WDF element connected at the right port
    //----------------------------------------------------------------------
    //    Adaptor (WDF *l, WDF *r, DspFloatType R)
    //        : left (l), right (r), WDF (R)
    Adaptor(DspFloatType R, WDF *l, WDF *r)
        : WDF(R), left(l), right(r)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const = 0;
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected() = 0;
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType wave)
    {
        // set the waves to the children according to the scattering rules
        left->incident(left->b - (left->R / R) * (wave + left->b + right->b));
        right->incident(right->b - (right->R / R) * (wave + left->b + right->b));
        a = wave;
    }
    //----------------------------------------------------------------------
};
//==============================================================================
class Serie : public Adaptor
{
public:
    //    Serie (WDF *l, WDF *r)
    //        : Adaptor (l, r, (l->R + r->R))
    Serie(WDF *l, WDF *r)
        : Adaptor((l->R + r->R), l, r)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "Serie"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = -(left->reflected() + right->reflected());
        return b;
    }
    //----------------------------------------------------------------------
};
//==============================================================================
class Parallel : public Adaptor
{
public:
    //    Parallel (WDF *l, WDF *r)
    //        : Adaptor (l, r, (l->R * r->R / (l->R + r->R)))
    Parallel(WDF *l, WDF *r)
        : Adaptor((l->R * r->R / (l->R + r->R)), l, r)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "Parallel"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = (left->G / G) * left->reflected() + (right->G / G) * right->reflected();
        return b;
    }
    //----------------------------------------------------------------------
};
//==============================================================================
class Resistor : public WDF
{
public:
    Resistor(DspFloatType R)
        : WDF(R)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "R"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = 0;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
    }
    //----------------------------------------------------------------------
};
//==============================================================================
class Capacitor : public WDF
{
public:
    Capacitor(DspFloatType C, DspFloatType T)
        : WDF(T / (2.0f * C)),
          state(0)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "C"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = state;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
        state = value;
    }
    //----------------------------------------------------------------------
    DspFloatType state;
    //----------------------------------------------------------------------
};
//==============================================================================
class Inductor : public WDF
{
public:
    Inductor(DspFloatType L, DspFloatType T)
        : WDF(2.0f * L / T),
          state(0)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "L"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = -state;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
        state = value;
    }
    //----------------------------------------------------------------------
    DspFloatType state;
    //----------------------------------------------------------------------
};
//==============================================================================
class ShortCircuit : public WDF
{
public:
    ShortCircuit(DspFloatType R)
        : WDF(R)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "Short Circuit"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = -a;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
    }
    //----------------------------------------------------------------------
};
//==============================================================================
class OpenCircuit : public WDF
{
public:
    OpenCircuit(DspFloatType R)
        : WDF(R)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "Open Circuit"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = a;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
    }
    //----------------------------------------------------------------------
};
//==============================================================================
class VoltageSource : public WDF
{
public:
    //    VoltageSource (DspFloatType V, DspFloatType R)
    //        : Vs (V),
    //          WDF (R)
    VoltageSource(DspFloatType R, DspFloatType V)
        : WDF(R), Vs(V)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "Vs"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = -a + 2.0f * Vs;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
    }
    //----------------------------------------------------------------------
    DspFloatType Vs;
    //----------------------------------------------------------------------
};
//==============================================================================
class CurrentSource : public WDF
{
public:
    //    CurrentSource (DspFloatType I, DspFloatType R)
    //        : Is (I),
    //          WDF (R)
    CurrentSource(DspFloatType R, DspFloatType I)
        : WDF(R), Is(I)
    {
    }
    //----------------------------------------------------------------------
    virtual std::string getLabel() const { return "Is"; }
    //----------------------------------------------------------------------
    virtual inline DspFloatType reflected()
    {
        b = a + 2.0f * R * Is;
        return b;
    }
    //----------------------------------------------------------------------
    virtual inline void incident(DspFloatType value)
    {
        a = value;
    }
    //----------------------------------------------------------------------
    DspFloatType Is;
    //----------------------------------------------------------------------
};
//==============================================================================
#endif // __WDF_H_974CDD6D__
//==============================================================================


/**
 *  reference from https://forum.juce.com/t/wave-digital-filter-wdf-with-juce/11227
	The main simulation based on the DAFX-11 matlab example.
	The electronic circuit is write in five lines :
		- 3 WDF Elements (Resistor/Capacitor/Voltage)
		- 2 WDF Adaptors (Serie)
	For a simply use, all the classes overrides the WDF 
	base class. Basically, a WDF Element is a leaf, a WDF
	Adaptor is a node.
	The WDF class provide two global methods to get the 
	voltage and the current on each point of the circuit.
	The simulation produce same result than LTSPICE, 
	so the WDF class can be use to write simple electronic
	circuits with serie/parallel 3-ports adaptors (T-Shape).
	The limitation of a WDF binary tree :
		- only one nonlinear element by tree
		- nonlinear element at top of the tree (root)
		- simple serie/parallel circuits
	But it's a perfect way to produce cool blackbox with 
	correct electronic parameters. A WDF binary tree use
	the Kirchhoff's circuit laws (KCL).
	For more informations about this example :
		
		DAFX: Digital Audio Effects, Second Edition
		ISBN : 978-0-470-66599-2
		Chapter : 12.3.4 / Virtual analog effects
			    : Diode circuit model using wave digital filters
*/
//==============================================================================
/**
    diodeClipper
    ------------
    
    This is a blackbox of the GZ34 Vaccuum-Diode clipper. The simulation use
    is own internal WDF tree. For a better integration in other circuit, this
    code need to be rewrite as a class that inherit of WDF base class, allowing
    to connect a WDF Diode Element (this subcircuit) in other circuit.
    
    In this case, the some precomputed LUT (LookUp Table) can be used in place.
    This method is a simple theorical example without optimization, but you can
    easily replace the Array<double> in input by a AudioSampleBuffer and test
    this code with a real audio sample.
    
    The Fs parameter is the sampling rate of the datas inside the input buffer.
    
	Return a buffer with the output voltage of the circuit with the same size 
	of the input buffer.
*/
//==============================================================================
static inline DspFloatType diodeClipper(juce::Array<DspFloatType> &input, DspFloatType Fs,
								 DspFloatType Vdiode, VoltageSource &Vin, Serie &root, Resistor &R1)
{
	/*
    // 1 ohm is the Ri voltage source resistor
    DspFloatType Ri = 1; 
    // Internal circuit parameters 
    VoltageSource Vin (0.0, Ri); // initialize voltage at 0V
    Resistor R1 (80.0); 
    Capacitor C1 (3.5e-5, Fs); // Capacitor & Inductor need sampling rate (Fs) in constructor
    
    // create WDF circuit
	Serie RC (&R1, &C1);
    Serie root (&Vin, &RC); 
	*/

	// Resistor R1(80.0);

	// accurate simulation of GZ34 valve diode.
	DspFloatType Is = 125.56f; // reverse saturation current
	DspFloatType Vt = 0.036f;  // thermal voltage

	// initial value for the voltage over the diode (n-1 memory)
	// DspFloatType Vdiode = 0.0f;

	// for simulation
	DspFloatType b, r, Rdiode;
	std::array<DspFloatType> output;

	// the simulation loop
	int n = 0;
	int max = input.size();

	for (; n < max; ++n)
	{
		Vin.Vs = input[n] * 13;			 // read the input signal for the voltage source
		b = root.reflected();			 // get the waves up to the root
										 // ** VALVE RESISTOR **
		Rdiode = Is * exp(-Vt * Vdiode); // the nonlinear resistance of the diode
		r = (Rdiode - root.R)			 // update scattering coefficient (KCL)
			/ (Rdiode + root.R);
		root.incident(r * b);	  // evaluate the wave leaving the diode (root element)
								  // ** UPDATE **
		Vdiode = root.voltage();  // update the diode voltage for next time sample
		output.add(R1.voltage()); // the output is the voltage over the resistor R1
	}
	input = output;
	return Vdiode;
}

