#pragma once

// *--------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass with Feedback ---* //

class onePoleTPTFB_LP {
public:

protected:
	// controls
	float fAlpha;			// cutoff
	float sampleRate;	// fs
	float z1;			// z-1 register

public:
	inline void initialize_LP(float newSampleRate) {
		sampleRate = newSampleRate;
		z1 = 0;
	}

	void setAlpha_LP(float alpha);
	void doFilterStage_LP(float xn, float& z1fb, float& ynLP);
};


// *--------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass Reference Model ---* //

 class onePoleTPT_LPRef2 {
 public:

 protected:
 	// controls
 	float fAlpha;			// cutoff
 	float sampleRate;	// fs
 	float z1;			// z-1 register

 public:
 	inline void initialize_LPRef(float newSampleRate) {
 		sampleRate = newSampleRate;
 		z1 = 0;
 	}

 	float getSampleRate_LPRef(){return sampleRate;}
 	float getZ1regValue_LPRef(){return z1;}
    void setAlpha_LPRef(float alpha);
 	void doFilterStage_LPRef(float xn, float& z1fb, float& ynLP);
 };

const double pi = 3.141595926536;
const double Fs = 48000;						// Fixed clock frequency

// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass Model ---* //

class onePoleTPT_LP {
public:

protected:
	// controls
	float G;			// cutoff
	float sampleRate;	// fs
	float z1;			// z-1 register

public:
	inline void initialize_LP(float newSampleRate) {
		sampleRate = newSampleRate;
		z1 = 0;
	}

	float getSampleRate_LP(){return sampleRate;}
	float getZ1regValue_LP(){return z1;}
	void setFc_LP(float fc);
	void doFilterStage_LP(float xn, float& ynLP);
};



// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT High-Pass Model ---* //

class onePoleTPT_HP {
public:

protected:
	// controls
	float G;			// cutoff
	float sampleRate;	// fs
	float z1;			// z-1 register

public:
	inline void initialize_HP(float newSampleRate) {
		sampleRate = newSampleRate;
		z1 = 0;
	}

	float getSampleRate_HP(){return sampleRate;}
	float getZ1regValue_HP(){return z1;}
	void setFc_HP(float fc);
	void doFilterStage_HP(float xn, float& ynHP);
};


// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass + High-Pass Model ---* //

class onePoleTPT_LPHP {
public:

protected:
	// controls
	float G;			// cutoff
	float sampleRate;	// fs
	float z1;			// z-1 register

public:
	inline void initialize_LPHP(float newSampleRate) {
		sampleRate = newSampleRate;
		z1 = 0;
	}

	float getSampleRate_LPHP(){return sampleRate;}
	float getZ1regValue_LPHP(){return z1;}
	void setFc_LPHP(float fc);
	void doFilterStage_LPHP(float xn, float& ynLP, float& ynHP);
};


// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT All-Pass Model ---* //

class onePoleTPT_AP {
public:

protected:
	// controls
	float G;			// cutoff
	float sampleRate;	// fs
	float z1;			// z-1 register

public:
	inline void initialize_AP(float newSampleRate) {
		sampleRate = newSampleRate;
		z1 = 0;
	}

	float getSampleRate_AP(){return sampleRate;}
	float getZ1regValue_AP(){return z1;}
	void setFc_AP(float fc);
	void doFilterStage_AP(float xn, float& ynAP);
};



// *--------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass Reference Model ---* //

class onePoleTPT_LPRef3 {
public:

 protected:
 	// controls
 	float fAlpha;			// cutoff
 	float sampleRate;	// fs
 	float z1;			// z-1 register

 public:
 	inline void initialize_LPRef(float newSampleRate) {
 		sampleRate = newSampleRate;
 		z1 = 0;
 	}

 	float getSampleRate_LPRef(){return sampleRate;}
  	float getZ1regValue_LPRef(){return z1;}
 	void setAlpha_LPRef(float alpha);
 	void doFilterStage_LPRef(float xn, float& z1fb, float& ynLP);
};


// *--------------------------------------------------------* //
// *--- Stereo Moog Ladder 4-pole Filter ---* //

class xodMoogLadder4P {
public:

	onePoleTPTFB_LP LPF1;
	onePoleTPTFB_LP LPF2;
	onePoleTPTFB_LP LPF3;
	onePoleTPTFB_LP LPF4;

protected:
	// controls
	float sampleRate;	// fs

	// for loop scalar
	float fAlpha0;

	float fBeta1;
	float fBeta2;
	float fBeta3;
	float fBeta4;

	float G;

	float K;

public:
	void initialize(float newSampleRate);
	void setFcAndRes(float cutoff, float resonance, float sampleRate);
	void advance(float xn, float& yn);
};

// *--------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass Reference Model ---* //

 class onePoleTPT_LPRef4 {
 public:

 protected:
 	// controls
 	float fAlpha;			// cutoff
 	float sampleRate;	// fs
 	float z1;			// z-1 register

 public:
 	inline void initialize_LPRef(float newSampleRate) {
 		sampleRate = newSampleRate;
 		z1 = 0;
 	}

 	float getSampleRate_LPRef(){return sampleRate;}
 	float getZ1regValue_LPRef(){return z1;}
 	void setAlpha_LPRef(float alpha);
 	void doFilterStage_LPRef(float xn, float& z1fb, float& ynLP);
 };

// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass Model ---* //

void onePoleTPT_LP::setFc_LP(float fc) {
	// pre-warp the cutoff - bilinear-transform filters
	float wd = 2*pi*fc;
	float T = 1/sampleRate;
	float wa = (2/T)*tan(wd*T/2);
	float g = wa*T/2;

	// calculate big G value - Zavalishin p46 (the Art of VA Design)
	// 0.0 < G < 1.0 (from simulation)
	G = g/(1.0 + g);

	std::cout<<"TPT G = "<<G<<std::endl;
}

void onePoleTPT_LP::doFilterStage_LP(float xn, float& ynLP) {
	float v = (xn - z1)*G;
	ynLP = v + z1;
	z1 = ynLP + v;

}


// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT High-Pass Model ---* //

void onePoleTPT_HP::setFc_HP(float fc) {
	// prewarp the cutoff - bilinear-transform filters
	float wd = 2*pi*fc;
	float T = 1/sampleRate;
	float wa = (2/T)*tan(wd*T/2);
	float g = wa*T/2;

	// calculate big G value - Zavalishin p46 (the Art of VA Design)
	// 0.0 < G < 1.0 (from simulation)
	G = g/(1.0 + g);

	std::cout<<"TPT G = "<<G<<std::endl;
}

void onePoleTPT_HP::doFilterStage_HP(float xn, float& ynHP) {
	float v = (xn - z1)*G;
	float ynLP = v + z1;
	ynHP = xn - ynLP;
	z1 = ynLP + v;

}


// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass + High-Pass Model ---* //

void onePoleTPT_LPHP::setFc_LPHP(float fc) {
	// prewarp the cutoff - bilinear-transform filters
	float wd = 2*pi*fc;
	float T = 1/sampleRate;
	float wa = (2/T)*tan(wd*T/2);
	float g = wa*T/2;

	// calculate big G value - Zavalishin p46 (the Art of VA Design)
	// 0.0 < G < 1.0 (from simulation)
	G = g/(1.0 + g);

	std::cout<<"TPT G = "<<G<<std::endl;
}

void onePoleTPT_LPHP::doFilterStage_LPHP(float xn, float& ynLP, float& ynHP) {
	float v = (xn - z1)*G;
	ynLP = v + z1;
	ynHP = xn - ynLP;
	z1 = ynLP + v;

}


// *---------------------------------------------------------------------------* //
// *--- 1-pole TPT All-Pass Model ---* //

void onePoleTPT_AP::setFc_AP(float fc) {
	// prewarp the cutoff - bilinear-transform filters
	float wd = 2*pi*fc;
	float T = 1/sampleRate;
	float wa = (2/T)*tan(wd*T/2);
	float g = wa*T/2;

	// calculate big G value - Zavalishin p46 (the Art of VA Design)
	// 0.0 < G < 1.0 (from simulation)
	G = g/(1.0 + g);

	std::cout<<"TPT G = "<<G<<std::endl;
}

void onePoleTPT_AP::doFilterStage_AP(float xn, float& ynAP) {
	float v = (xn - z1)*G;
	float LP = v + z1;
	float HP = xn - LP;
	ynAP = LP - HP;
	z1 = LP + v;

}


// *--------------------------------------------------------* //




// *--------------------------------------------------------* //
// *--- 1-pole TPT Low-Pass with Feedback ---* //

void onePoleTPTFB_LP::setAlpha_LP(float alpha) {
	fAlpha = alpha;
}

// TPT 1-pole filter - Zavalishin p46 (the Art of VA Design)
void onePoleTPTFB_LP::doFilterStage_LP(float xn, float& z1fb, float& ynLP) {
	float v = (xn - z1)*fAlpha;
	ynLP = v + z1;
	z1 = ynLP + v;
	z1fb = z1;

	//std::cout<<"xn_LPR = "<<xn<<",      v_LPR = "<<v<<",      LP_LPR = "<<ynLP<<",      z1_LPR = "<<z1<<std::endl;

}



// *--------------------------------------------------------* //
// *--- Stereo TPT Moog Half Ladder Low-Pass filter ---* //


void xodMoogLadder4P::initialize(float newSampleRate) {

	fAlpha0 = 0;
	fBeta1 = 0;
	fBeta2 = 0;
	fBeta3 = 0;
	fBeta4 = 0;
	G = 0;
	K = 0;

	LPF1.initialize_LP(newSampleRate);
	LPF2.initialize_LP(newSampleRate);
	LPF3.initialize_LP(newSampleRate);
	LPF4.initialize_LP(newSampleRate);

}

void xodMoogLadder4P::setFcAndRes(float cutoff, float resonance, float sampleRate) {

	// prewarp for BZT
	double wd = 2*pi*cutoff;
	double T  = 1/(double)sampleRate;
	double wa = (2/T)*tan(wd*T/2);
	double g  = wa*T/2;

	// G - the feedforward coeff in the VA One Pole
	float G = g/(1.0 + g);

	LPF1.setAlpha_LP(G);
	LPF2.setAlpha_LP(G);
	LPF3.setAlpha_LP(G);
	LPF4.setAlpha_LP(G);


	fBeta1 = (G*G*G) / (1.0 + g);
	fBeta2 = (G*G) / (1.0 + g);
	fBeta3 = G / (1.0 + g);
	fBeta4 = 1 / (1.0 + g);


	// calculate alpha0
	// for 2nd order, K = 2 is max so limit it there
	// ** fixed-point implementation, K=2 requires many integer bits to prevent overflow
	// (future enhancement -> use internal data type to handle bit-growth)
	// currently limit K resonance to less than 2.0 to prevent overflow:
	K = resonance;
	if(K > 2.0)
		K = 2.0;

	fAlpha0 = 1.0 / (1.0 + K*G*G*G*G);


	std::cout<<std::endl<<"TB_G: "<<G<<std::endl;
	std::cout<<"TB_K: "<<K<<std::endl;
	//std::cout<<"fAlpha0 = "<<fAlpha0<<std::endl;
	//std::cout<<"setFcAndRes_Ref: g = "<<g<<",        G = "<<G<<",        K = "<<K<<",        beta1 = "<<fBeta1<<",        beta2 = "<<fBeta2<<",        beta3 = "<<fBeta3<<std::endl;

}


void xodMoogLadder4P::advance(float xn, float& yn) {

	static float SM = 0;
	//static float SM_R = 0;

	static float yn_LP1;
	static float yn_LP2;
	static float yn_LP3;
//	static float ynR_LP2;

	static float z1fb_1;
	static float z1fb_2;
	static float z1fb_3;
	static float z1fb_4;

//	static float z1fb_R1;
//	static float z1fb_R2;
//	static float z1fb_R3;

	SM = fBeta1*z1fb_1 + fBeta2*z1fb_2 + fBeta3*z1fb_3 + fBeta4*z1fb_4;		// sum 4 internal Z1 states

	float un = fAlpha0*(xn - K*SM);

	//std::cout<<"xnL = "<<xnL<<",	fb1 = "<<z1fb_L1<<",	fb2 = "<<z1fb_L2<<",	fb3 = "<<z1fb_L3<<",	fAlpha0 = "<<fAlpha0<<",	SM_L = "<<SM_L<<",	unL_REF = "<<unL<<std::endl;

	LPF1.doFilterStage_LP(un, z1fb_1, yn_LP1);
	LPF2.doFilterStage_LP(yn_LP1, z1fb_2, yn_LP2);
	LPF3.doFilterStage_LP(yn_LP2, z1fb_3, yn_LP3);
	LPF4.doFilterStage_LP(yn_LP3, z1fb_4, yn);


	//std::cout<<"SM_REF = "<<SM_L<<",        K_REF = "<<K<<",        unL_REF = "<<unL<<std::endl;

}
