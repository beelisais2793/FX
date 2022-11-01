#pragma once

class HardClipper {

// THIS CLASS IMPLEMENTS AN ANTIALIASED HARD CLIPPING FUNCTION.
// THIS CLASS USES THE FIRST-ORDER ANTIDERIVATIVE METHOD.

private:

	DspFloatType output = 0.0;

	DspFloatType xn1 = 0.0;
	DspFloatType Fn = 0.0;
	DspFloatType Fn1 = 0.0;

	const DspFloatType thresh = 10.0e-2;
	const DspFloatType oneTwelfth = 1.0/12.0;

public:

	HardClipper() {}
	~HardClipper() {}

	void process(DspFloatType input) {
		output = antialiasedHardClipN1(input);
	}

	DspFloatType signum(DspFloatType x) {
		return (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);
	}

	DspFloatType hardClipN0(DspFloatType x) {
		// Hard clipping function
		return 0.5f*(signum(x+1.0f)*(x+1.0f) - signum(x-1.0f)*(x-1.0f)); 
	}

	DspFloatType hardClipN1(DspFloatType x) {
		// First antiderivative of hardClipN0
		return 0.25f*(signum(x+1.0f)*(x+1.0f)*(x+1.0f) - signum(x-1.0f)*(x-1.0f)*(x-1.0f) - 2.0f); 
	}

	DspFloatType hardClipN2(DspFloatType x) {
		// second antiderivative of hardClipN0
		return oneTwelfth*(signum(x+1.0f)*(x+1.0f)*(x+1.0f)*(x+1.0f) - signum(x-1.0f)*(x-1.0f)*(x-1.0f)*(x-1.0f) - 6.0f*x); 
	}

	DspFloatType antialiasedHardClipN1(DspFloatType x) {

		// Hard clipping with 1st-order antialiasing
		Fn = hardClipN1(x);
		DspFloatType tmp = 0.0;
		if (std::abs(x - xn1) < thresh) {
			tmp = hardClipN0(0.5f * (x + xn1));
		}
		else {
			tmp = (Fn - Fn1)/(x - xn1);
		}

		// Update states
		xn1 = x;
		Fn1 = Fn;

		return tmp;
	}

	DspFloatType getClippedOutput() {
		return output;
	}

};

class Wavefolder {

// THIS CLASS IMPLEMENTS A FOLDING FUNCTION, SOMETIMES KNOWN AS A MATHEMATICAL FOLDER DUE TO ITS
// SHARP EDGES. THE SOUND PRODUCED IS SIMILAR TO THAT OF THE BUCHLA 259'S TIMBRE SECTION. 

private:

	DspFloatType output = 0.0;

	// Antialiasing state variables
	DspFloatType xn1 = 0.0;
	DspFloatType xn2 = 0.0;
	DspFloatType Fn = 0.0;
	DspFloatType Fn1 = 0.0;
	DspFloatType Gn = 0.0;
	DspFloatType Gn1 = 0.0;

	// Ill-conditioning threshold
	const DspFloatType thresh = 10.0e-2;

	const DspFloatType oneSixth = 1.0/6.0;

	HardClipper hardClipper;

public:

	Wavefolder() {}
	~Wavefolder() {}

	void process(DspFloatType input) {
		output = antialiasedFoldN2(input);
	}

	DspFloatType foldFunctionN0(DspFloatType x) {
		// Folding function
		return (2.0f*hardClipper.hardClipN0(x) - x);
	}

	DspFloatType foldFunctionN1(DspFloatType x) {	
		// First antiderivative of the folding function
		return (2.0f*hardClipper.hardClipN1(x) - 0.5f*x*x);
	}

	DspFloatType foldFunctionN2(DspFloatType x) {	
		// Second antiderivative of the folding function
		return (2.0f*hardClipper.hardClipN2(x) - oneSixth*(x*x*x));
	}

	DspFloatType antialiasedFoldN1(DspFloatType x) {

		// Folding with 1st-order antialiasing (not recommended)
		Fn = foldFunctionN1(x);
		DspFloatType tmp = 0.0;
		if (std::abs(x - xn1) < thresh) {
			tmp = foldFunctionN0(0.5f * (x + xn1));
		}
		else {
			tmp = (Fn - Fn1)/(x - xn1);
		}

		// Update states
		xn1 = x;
		Fn1 = Fn;

		return tmp;
	}

	DspFloatType antialiasedFoldN2(DspFloatType x) {

		// Folding with 2nd-order antialiasing
		Fn = foldFunctionN2(x);
		DspFloatType tmp = 0.0;
		if (std::abs(x - xn1) < thresh) {
			// First-order escape rule
			Gn = foldFunctionN1(0.5f * (x + xn1));
		}
		else {
			Gn = (Fn - Fn1) / (x - xn1);
		}

		if (std::abs(x - xn2) < thresh) {
			// Second-order escape
			DspFloatType delta = 0.5f * (x - 2.0f*xn1 + xn2);
			if (std::abs(delta) < thresh) {
				tmp = foldFunctionN0(0.25f * (x + 2.0f*xn1 + xn2));
			}
			else {
				DspFloatType tmp1 = foldFunctionN1(0.5f * (x + xn2));
				DspFloatType tmp2 = foldFunctionN2(0.5f * (x + xn2));
				tmp = (2.0f/delta)*(tmp1 + (Fn1 - tmp2)/delta);
			}
		}
		else {
			tmp = 2.0f * (Gn - Gn1)/(x - xn2);
		}

		 // Update state variables
		Fn1 = Fn;
		Gn1 = Gn;
		xn2 = xn1;
		xn1 = x;

		return tmp;
	}

	DspFloatType getFoldedOutput() {
		return output;
	}

};

class SoftClipper {

// THIS CLASS IMPLEMENTS A PIECEWISE SOFT SATURATOR WITH 
// FIRST-ORDER ANTIDERIVATIVE ANTIALIASING.

private:

	DspFloatType output = 0.0;

	// Antialiasing variables
	DspFloatType xn1 = 0.0;
	DspFloatType Fn = 0.0;
	DspFloatType Fn1 = 0.0;
	const DspFloatType thresh = 10.0e-2;

public:
	SoftClipper() {}
	~SoftClipper() {}

	void process(DspFloatType input) {
		output = antialiasedSoftClipN1(input);
	}

	DspFloatType signum(DspFloatType x) {
		return (x > 0.0f) ? 1.0f : ((x < 0.0f) ? -1.0f : 0.0f);
	}

	DspFloatType softClipN0(DspFloatType x) {
		return (std::abs(x)<1) ? std::sin(0.5f*M_PI*x) : signum(x); 
	}

	DspFloatType softClipN1(DspFloatType x) {
		
		return (std::abs(x)<1) ? 1.0f - (2.0f/M_PI)*cos(x*0.5f*M_PI) : signum(x)*x;
	}

	DspFloatType antialiasedSoftClipN1(DspFloatType x) {

		Fn = softClipN1(x);
		DspFloatType tmp = 0.0;
		if (std::abs(x - xn1) < thresh) {
			tmp = softClipN0(0.5f * (x + xn1));
		}
		else {
			tmp = (Fn - Fn1)/(x - xn1);
		}

		// Update states
		xn1 = x;
		Fn1 = Fn;

		return tmp;
	}

	DspFloatType getClippedOutput() {
		return output;
	}
};
