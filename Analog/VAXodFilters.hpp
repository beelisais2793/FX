#pragma once

#include <iostream>

namespace Analog::Filters::XOD
{
	// *--------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass with Feedback ---* //

	class onePoleTPTFB_LP {
	public:

	protected:
		// controls
		DspFloatType fAlpha;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_LP(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		void setAlpha_LP(DspFloatType alpha);
		void doFilterStage_LP(DspFloatType xn, DspFloatType& z1fb, DspFloatType& ynLP);
	};


	// *--------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass Reference Model ---* //

	class onePoleTPT_LPRef2 {
	public:

	protected:
		// controls
		DspFloatType fAlpha;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_LPRef(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_LPRef(){return sampleRate;}
		DspFloatType getZ1regValue_LPRef(){return z1;}
		void setAlpha_LPRef(DspFloatType alpha);
		void doFilterStage_LPRef(DspFloatType xn, DspFloatType& z1fb, DspFloatType& ynLP);
	};

	const DspFloatType pi = 3.141595926536;
	const DspFloatType Fs = 48000;						// Fixed clock frequency

	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass Model ---* //

	class onePoleTPT_LP {
	public:

	protected:
		// controls
		DspFloatType G;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_LP(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_LP(){return sampleRate;}
		DspFloatType getZ1regValue_LP(){return z1;}
		void setFc_LP(DspFloatType fc);
		void doFilterStage_LP(DspFloatType xn, DspFloatType& ynLP);
	};



	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT High-Pass Model ---* //

	class onePoleTPT_HP {
	public:

	protected:
		// controls
		DspFloatType G;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_HP(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_HP(){return sampleRate;}
		DspFloatType getZ1regValue_HP(){return z1;}
		void setFc_HP(DspFloatType fc);
		void doFilterStage_HP(DspFloatType xn, DspFloatType& ynHP);
	};


	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass + High-Pass Model ---* //

	class onePoleTPT_LPHP {
	public:

	protected:
		// controls
		DspFloatType G;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_LPHP(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_LPHP(){return sampleRate;}
		DspFloatType getZ1regValue_LPHP(){return z1;}
		void setFc_LPHP(DspFloatType fc);
		void doFilterStage_LPHP(DspFloatType xn, DspFloatType& ynLP, DspFloatType& ynHP);
	};


	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT All-Pass Model ---* //

	class onePoleTPT_AP {
	public:

	protected:
		// controls
		DspFloatType G;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_AP(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_AP(){return sampleRate;}
		DspFloatType getZ1regValue_AP(){return z1;}
		void setFc_AP(DspFloatType fc);
		void doFilterStage_AP(DspFloatType xn, DspFloatType& ynAP);
	};



	// *--------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass Reference Model ---* //

	class onePoleTPT_LPRef3 {
	public:

	protected:
		// controls
		DspFloatType fAlpha;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_LPRef(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_LPRef(){return sampleRate;}
		DspFloatType getZ1regValue_LPRef(){return z1;}
		void setAlpha_LPRef(DspFloatType alpha);
		void doFilterStage_LPRef(DspFloatType xn, DspFloatType& z1fb, DspFloatType& ynLP);
	};


	// *--------------------------------------------------------* //
	// *--- Stereo Moog Ladder 4-pole Filter ---* //

	class xodMoogLadder4P : public FilterProcessor {
	public:

		xodMoogLadder4P() : FilterProcessor() {

		}
		onePoleTPTFB_LP LPF1;
		onePoleTPTFB_LP LPF2;
		onePoleTPTFB_LP LPF3;
		onePoleTPTFB_LP LPF4;

	protected:
		// controls
		DspFloatType sampleRate;	// fs

		// for loop scalar
		DspFloatType fAlpha0;

		DspFloatType fBeta1;
		DspFloatType fBeta2;
		DspFloatType fBeta3;
		DspFloatType fBeta4;

		DspFloatType fc=1000.0,res=0.5;
		DspFloatType G;
		DspFloatType K;

	public:
		void initialize(DspFloatType newSampleRate);
		void setFcAndRes(DspFloatType cutoff, DspFloatType resonance, DspFloatType sampleRate);
		void advance(DspFloatType xn, DspFloatType& yn);

		void setCutoff(DspFloatType Fc) {
			fc = Fc;
			setFcAndRes(fc,res,sampleRate);
		}
		void setResonance(DspFloatType r) {
			res = r;
			setFcAndRes(fc,res,sampleRate);
		}
		DspFloatType Tick(DspFloatType I, DspFloatType A=1, DspFloatType X=1, DspFloatType Y=1) {
			DspFloatType out = I;
			DspFloatType f   = fc;
			DspFloatType r   = res;
			setCutoff(f*fabs(X));
			setResonance(r * fabs(Y));
			advance(I,out);
			setCutoff(f);
			setResonance(r);
			return A*out;
		}
	};

	// *--------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass Reference Model ---* //

	class onePoleTPT_LPRef4 {
	public:

	protected:
		// controls
		DspFloatType fAlpha;			// cutoff
		DspFloatType sampleRate;	// fs
		DspFloatType z1;			// z-1 register

	public:
		inline void initialize_LPRef(DspFloatType newSampleRate) {
			sampleRate = newSampleRate;
			z1 = 0;
		}

		DspFloatType getSampleRate_LPRef(){return sampleRate;}
		DspFloatType getZ1regValue_LPRef(){return z1;}
		void setAlpha_LPRef(DspFloatType alpha);
		void doFilterStage_LPRef(DspFloatType xn, DspFloatType& z1fb, DspFloatType& ynLP);
	};

	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass Model ---* //

	void onePoleTPT_LP::setFc_LP(DspFloatType fc) {
		// pre-warp the cutoff - bilinear-transform filters
		DspFloatType wd = 2*pi*fc;
		DspFloatType T = 1/sampleRate;
		DspFloatType wa = (2/T)*tan(wd*T/2);
		DspFloatType g = wa*T/2;

		// calculate big G value - Zavalishin p46 (the Art of VA Design)
		// 0.0 < G < 1.0 (from simulation)
		G = g/(1.0 + g);

		std::cout<< "TPT G = "<<G<<std::endl;
	}

	void onePoleTPT_LP::doFilterStage_LP(DspFloatType xn, DspFloatType& ynLP) {
		DspFloatType v = (xn - z1)*G;
		ynLP = v + z1;
		z1 = ynLP + v;

	}


	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT High-Pass Model ---* //

	void onePoleTPT_HP::setFc_HP(DspFloatType fc) {
		// prewarp the cutoff - bilinear-transform filters
		DspFloatType wd = 2*pi*fc;
		DspFloatType T = 1/sampleRate;
		DspFloatType wa = (2/T)*tan(wd*T/2);
		DspFloatType g = wa*T/2;

		// calculate big G value - Zavalishin p46 (the Art of VA Design)
		// 0.0 < G < 1.0 (from simulation)
		G = g/(1.0 + g);

		std::cout<<"TPT G = "<<G<<std::endl;
	}

	void onePoleTPT_HP::doFilterStage_HP(DspFloatType xn, DspFloatType& ynHP) {
		DspFloatType v = (xn - z1)*G;
		DspFloatType ynLP = v + z1;
		ynHP = xn - ynLP;
		z1 = ynLP + v;

	}


	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass + High-Pass Model ---* //

	void onePoleTPT_LPHP::setFc_LPHP(DspFloatType fc) {
		// prewarp the cutoff - bilinear-transform filters
		DspFloatType wd = 2*pi*fc;
		DspFloatType T = 1/sampleRate;
		DspFloatType wa = (2/T)*tan(wd*T/2);
		DspFloatType g = wa*T/2;

		// calculate big G value - Zavalishin p46 (the Art of VA Design)
		// 0.0 < G < 1.0 (from simulation)
		G = g/(1.0 + g);

		std::cout<<"TPT G = "<<G<<std::endl;
	}

	void onePoleTPT_LPHP::doFilterStage_LPHP(DspFloatType xn, DspFloatType& ynLP, DspFloatType& ynHP) {
		DspFloatType v = (xn - z1)*G;
		ynLP = v + z1;
		ynHP = xn - ynLP;
		z1 = ynLP + v;

	}


	// *---------------------------------------------------------------------------* //
	// *--- 1-pole TPT All-Pass Model ---* //

	void onePoleTPT_AP::setFc_AP(DspFloatType fc) {
		// prewarp the cutoff - bilinear-transform filters
		DspFloatType wd = 2*pi*fc;
		DspFloatType T = 1/sampleRate;
		DspFloatType wa = (2/T)*tan(wd*T/2);
		DspFloatType g = wa*T/2;

		// calculate big G value - Zavalishin p46 (the Art of VA Design)
		// 0.0 < G < 1.0 (from simulation)
		G = g/(1.0 + g);

		std::cout<<"TPT G = "<<G<<std::endl;
	}

	void onePoleTPT_AP::doFilterStage_AP(DspFloatType xn, DspFloatType& ynAP) {
		DspFloatType v = (xn - z1)*G;
		DspFloatType LP = v + z1;
		DspFloatType HP = xn - LP;
		ynAP = LP - HP;
		z1 = LP + v;

	}


	// *--------------------------------------------------------* //




	// *--------------------------------------------------------* //
	// *--- 1-pole TPT Low-Pass with Feedback ---* //

	void onePoleTPTFB_LP::setAlpha_LP(DspFloatType alpha) {
		fAlpha = alpha;
	}

	// TPT 1-pole filter - Zavalishin p46 (the Art of VA Design)
	void onePoleTPTFB_LP::doFilterStage_LP(DspFloatType xn, DspFloatType& z1fb, DspFloatType& ynLP) {
		DspFloatType v = (xn - z1)*fAlpha;
		ynLP = v + z1;
		z1 = ynLP + v;
		z1fb = z1;

		//std::cout<<"xn_LPR = "<<xn<<",      v_LPR = "<<v<<",      LP_LPR = "<<ynLP<<",      z1_LPR = "<<z1<<std::endl;

	}



	// *--------------------------------------------------------* //
	// *--- Stereo TPT Moog Half Ladder Low-Pass filter ---* //


	void xodMoogLadder4P::initialize(DspFloatType newSampleRate) {

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

	void xodMoogLadder4P::setFcAndRes(DspFloatType cutoff, DspFloatType resonance, DspFloatType sampleRate) {

		// prewarp for BZT
		DspFloatType wd = 2*pi*cutoff;
		DspFloatType T  = 1/(DspFloatType)sampleRate;
		DspFloatType wa = (2/T)*tan(wd*T/2);
		DspFloatType g  = wa*T/2;

		// G - the feedforward coeff in the VA One Pole
		DspFloatType G = g/(1.0 + g);

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


	void xodMoogLadder4P::advance(DspFloatType xn, DspFloatType& yn) {

		static DspFloatType SM = 0;
		//static DspFloatType SM_R = 0;

		static DspFloatType yn_LP1;
		static DspFloatType yn_LP2;
		static DspFloatType yn_LP3;
	//	static DspFloatType ynR_LP2;

		static DspFloatType z1fb_1;
		static DspFloatType z1fb_2;
		static DspFloatType z1fb_3;
		static DspFloatType z1fb_4;

	//	static DspFloatType z1fb_R1;
	//	static DspFloatType z1fb_R2;
	//	static DspFloatType z1fb_R3;

		SM = fBeta1*z1fb_1 + fBeta2*z1fb_2 + fBeta3*z1fb_3 + fBeta4*z1fb_4;		// sum 4 internal Z1 states

		DspFloatType un = fAlpha0*(xn - K*SM);

		//std::cout<<"xnL = "<<xnL<<",	fb1 = "<<z1fb_L1<<",	fb2 = "<<z1fb_L2<<",	fb3 = "<<z1fb_L3<<",	fAlpha0 = "<<fAlpha0<<",	SM_L = "<<SM_L<<",	unL_REF = "<<unL<<std::endl;

		LPF1.doFilterStage_LP(un, z1fb_1, yn_LP1);
		LPF2.doFilterStage_LP(yn_LP1, z1fb_2, yn_LP2);
		LPF3.doFilterStage_LP(yn_LP2, z1fb_3, yn_LP3);
		LPF4.doFilterStage_LP(yn_LP3, z1fb_4, yn);


		//std::cout<<"SM_REF = "<<SM_L<<",        K_REF = "<<K<<",        unL_REF = "<<unL<<std::endl;

	}
}