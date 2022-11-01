import("stdfaust.lib");

freq = hslider("freq",100,20,600,0.01);
gate = button("gate");
nextAlgo = button("Next algorithm");

gAttack1 	= 	vslider("h:Operator1/h:[0]Envelope gain/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
gDecay1 	= 	vslider("h:Operator1/h:[0]Envelope gain/[1]Decay 	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
gSustain1 	= 	vslider("h:Operator1/h:[0]Envelope gain/[2]Sustain	[style:knob]",	0.7,	0,		1,		0.01) : si.smoo;
gRelease1 	= 	vslider("h:Operator1/h:[0]Envelope gain/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
gOffset1 	= 	vslider("h:Operator1/h:[0]Envelope gain/[4]Offset	[style:knob]",	0,		0,		1,		0.01) : si.smoo;
fAttack1 	= 	vslider("h:Operator1/h:[1]Envelope freq/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
fDecay1 	= 	vslider("h:Operator1/h:[1]Envelope freq/[1]Decay	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
fSustain1 	= 	vslider("h:Operator1/h:[1]Envelope freq/[2]Sustain	[style:knob]",	1,	0,		1,		0.01) : si.smoo;
fRelease1 	= 	vslider("h:Operator1/h:[1]Envelope freq/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
fOffset1 	= 	vslider("h:Operator1/h:[1]Envelope freq/[4]Offset	[style:knob]",	0.5,	0,		1,		0.01) : si.smoo;

gAttack2 	= 	vslider("h:Operator2/h:[0]Envelope gain/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
gDecay2 	= 	vslider("h:Operator2/h:[0]Envelope gain/[1]Decay 	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
gSustain2 	= 	vslider("h:Operator2/h:[0]Envelope gain/[2]Sustain	[style:knob]",	0.7,	0,		1,		0.01) : si.smoo;
gRelease2 	= 	vslider("h:Operator2/h:[0]Envelope gain/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
gOffset2 	= 	vslider("h:Operator2/h:[0]Envelope gain/[4]Offset	[style:knob]",	0,		0,		1,		0.01) : si.smoo;
fAttack2 	= 	vslider("h:Operator2/h:[1]Envelope freq/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
fDecay2 	= 	vslider("h:Operator2/h:[1]Envelope freq/[1]Decay	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
fSustain2 	= 	vslider("h:Operator2/h:[1]Envelope freq/[2]Sustain	[style:knob]",	1,	0,		1,		0.01) : si.smoo;
fRelease2 	= 	vslider("h:Operator2/h:[1]Envelope freq/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
fOffset2 	= 	vslider("h:Operator2/h:[1]Envelope freq/[4]Offset	[style:knob]",	0.5,	0,		1,		0.01) : si.smoo;
Ratio2 		= 	vslider("h:Operator2/h:[2]General/FreqRatio			[style:knob]",	2,		0,		4,		0.01) : si.smoo;
Gain2 		= 	vslider("h:Operator2/h:[2]General/Gain				[style:knob]",	1,		0,		100,	0.01) : si.smoo;

gAttack3 	= 	vslider("h:Operator3/h:[0]Envelope gain/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
gDecay3 	= 	vslider("h:Operator3/h:[0]Envelope gain/[1]Decay 	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
gSustain3 	= 	vslider("h:Operator3/h:[0]Envelope gain/[2]Sustain	[style:knob]",	0.7,	0,		1,		0.01) : si.smoo;
gRelease3 	= 	vslider("h:Operator3/h:[0]Envelope gain/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
gOffset3 	= 	vslider("h:Operator3/h:[0]Envelope gain/[4]Offset	[style:knob]",	0,		0,		1,		0.01) : si.smoo;
fAttack3 	= 	vslider("h:Operator3/h:[1]Envelope freq/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
fDecay3 	= 	vslider("h:Operator3/h:[1]Envelope freq/[1]Decay	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
fSustain3 	= 	vslider("h:Operator3/h:[1]Envelope freq/[2]Sustain	[style:knob]",	1,	0,		1,		0.01) : si.smoo;
fRelease3 	= 	vslider("h:Operator3/h:[1]Envelope freq/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
fOffset3 	= 	vslider("h:Operator3/h:[1]Envelope freq/[4]Offset	[style:knob]",	0.5,	0,		1,		0.01) : si.smoo;
Ratio3 		= 	vslider("h:Operator3/h:[2]General/FreqRatio			[style:knob]",	2,		0,		4,		0.01) : si.smoo;
Gain3 		= 	vslider("h:Operator3/h:[2]General/Gain				[style:knob]",	1,		0,		100,	0.01) : si.smoo;

gAttack4 	= 	vslider("h:Operator4/h:[0]Envelope gain/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
gDecay4 	= 	vslider("h:Operator4/h:[0]Envelope gain/[1]Decay 	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
gSustain4 	= 	vslider("h:Operator4/h:[0]Envelope gain/[2]Sustain	[style:knob]",	0.7,	0,		1,		0.01) : si.smoo;
gRelease4 	= 	vslider("h:Operator4/h:[0]Envelope gain/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
gOffset4 	= 	vslider("h:Operator4/h:[0]Envelope gain/[4]Offset	[style:knob]",	0,		0,		1,		0.01) : si.smoo;
fAttack4 	= 	vslider("h:Operator4/h:[1]Envelope freq/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
fDecay4 	= 	vslider("h:Operator4/h:[1]Envelope freq/[1]Decay	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
fSustain4 	= 	vslider("h:Operator4/h:[1]Envelope freq/[2]Sustain	[style:knob]",	1,	0,		1,		0.01) : si.smoo;
fRelease4 	= 	vslider("h:Operator4/h:[1]Envelope freq/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
fOffset4 	= 	vslider("h:Operator4/h:[1]Envelope freq/[4]Offset	[style:knob]",	0.5,	0,		1,		0.01) : si.smoo;
Ratio4 		= 	vslider("h:Operator4/h:[2]General/FreqRatio			[style:knob]",	2,		0,		4,		0.01) : si.smoo;
Gain4 		= 	vslider("h:Operator4/h:[2]General/Gain				[style:knob]",	1,		0,		100,	0.01) : si.smoo;

gAttack5 	= 	vslider("h:Operator5/h:[0]Envelope gain/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
gDecay5 	= 	vslider("h:Operator5/h:[0]Envelope gain/[1]Decay 	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
gSustain5 	= 	vslider("h:Operator5/h:[0]Envelope gain/[2]Sustain	[style:knob]",	0.7,	0,		1,		0.01) : si.smoo;
gRelease5 	= 	vslider("h:Operator5/h:[0]Envelope gain/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
gOffset5 	= 	vslider("h:Operator5/h:[0]Envelope gain/[4]Offset	[style:knob]",	0,		0,		1,		0.01) : si.smoo;
fAttack5 	= 	vslider("h:Operator5/h:[1]Envelope freq/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
fDecay5 	= 	vslider("h:Operator5/h:[1]Envelope freq/[1]Decay	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
fSustain5 	= 	vslider("h:Operator5/h:[1]Envelope freq/[2]Sustain	[style:knob]",	1,	0,		1,		0.01) : si.smoo;
fRelease5 	= 	vslider("h:Operator5/h:[1]Envelope freq/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
fOffset5 	= 	vslider("h:Operator5/h:[1]Envelope freq/[4]Offset	[style:knob]",	0.5,	0,		1,		0.01) : si.smoo;
Ratio5 		= 	vslider("h:Operator5/h:[2]General/FreqRatio			[style:knob]",	2,		0,		4,		0.01) : si.smoo;
Gain5 		= 	vslider("h:Operator5/h:[2]General/Gain				[style:knob]",	1,		0,		100,	0.01) : si.smoo;

gAttack6 	= 	vslider("h:Operator6/h:[0]Envelope gain/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
gDecay6 	= 	vslider("h:Operator6/h:[0]Envelope gain/[1]Decay 	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
gSustain6 	= 	vslider("h:Operator6/h:[0]Envelope gain/[2]Sustain	[style:knob]",	0.7,	0,		1,		0.01) : si.smoo;
gRelease6 	= 	vslider("h:Operator6/h:[0]Envelope gain/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
gOffset6 	= 	vslider("h:Operator6/h:[0]Envelope gain/[4]Offset	[style:knob]",	0,		0,		1,		0.01) : si.smoo;
fAttack6 	= 	vslider("h:Operator6/h:[1]Envelope freq/[0]Attack	[style:knob]",	0.01,	0.01,	0.5,	0.01) : si.smoo;
fDecay6 	= 	vslider("h:Operator6/h:[1]Envelope freq/[1]Decay	[style:knob]",	0.1,	0.01,	0.5,	0.01) : si.smoo;
fSustain6 	= 	vslider("h:Operator6/h:[1]Envelope freq/[2]Sustain	[style:knob]",	1,	0,		1,		0.01) : si.smoo;
fRelease6 	= 	vslider("h:Operator6/h:[1]Envelope freq/[3]Release	[style:knob]",	0.5,	0.01,	2,		0.01) : si.smoo;
fOffset6 	= 	vslider("h:Operator6/h:[1]Envelope freq/[4]Offset	[style:knob]",	0.5,	0,		1,		0.01) : si.smoo;
Ratio6 		= 	vslider("h:Operator6/h:[2]General/FreqRatio			[style:knob]",	2,		0,		4,		0.01) : si.smoo;
Gain6 		= 	vslider("h:Operator6/h:[2]General/Gain				[style:knob]",	1,		0,		100,	0.01) : si.smoo;


opperator(ga,gd,gs,gr,go,fa,fd,fs,fr,fo,ratio,gain,mod) = genv*os.osc(fenv+mod)
	with{
		genv = gain * (en.adsr(ga,gd,gs,gr,gate)*(1-go)+go);
  		fenv = freq * ratio * (en.adsr(fa,fd,fs,fr,gate)*(1-fo)+fo); 
	};

//Algorithm 1
algorithm(0,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 2
algorithm(1,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) = 
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,_)~_;
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 3
algorithm(2,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) = 
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2);
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 4
algorithm(3,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + (opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4)~_) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2);
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_);
	};

//Algorithm 5
algorithm(4,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =

  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5) / 3
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 6
algorithm(5,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5) / 3
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 7
algorithm(6,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 8
algorithm(7,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = (opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,_)~_) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 9
algorithm(8,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,_)~_;
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 10
algorithm(9,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2);
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,_)~_;
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 11
algorithm(10,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2);
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + (opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_);
	};

//Algorithm 12
algorithm(11,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,_)~_;
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 13
algorithm(12,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + (opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_);
	};

//Algorithm 14
algorithm(13,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + (opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_);
	};

//Algorithm 15
algorithm(14,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) / 2
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,_)~_;
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 16
algorithm(15,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1)
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0);
		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 17
algorithm(16,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1)
	with{
  		m1 = (opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,_)~_) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0);
		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 18
algorithm(17,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1)
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + (opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,_)~_) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5);
		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 19
algorithm(18,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m4) / 3
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2);
		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0);
  		m4 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
		
	};

//Algorithm 20
algorithm(19,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m1) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) / 3
	with{
  		m1 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,_)~_;
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 21
algorithm(20,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m1) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m4) / 4
	with{
  		m1 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,_)~_;
  		m4 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 22
algorithm(21,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m3) / 4
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 23
algorithm(22,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m4) / 4
	with{
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0);
  		m4 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 24
algorithm(23,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m3) / 5
	with{
  		m3 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 25
algorithm(24,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m4) / 5
	with{
  		m4 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 26
algorithm(25,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) / 3
	with{
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0);
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + (opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_);
	};

//Algorithm 27
algorithm(26,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,m2) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4) / 3
	with{
  		m2 = opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,_)~_;
  		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0);
	};

//Algorithm 28
algorithm(27,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,m1) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0) / 3
	with{
  		m1 = opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0);
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,_)~_;
	};

//Algorithm 29
algorithm(28,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5) / 4
	with{
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0);
		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 30
algorithm(29,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,m3) + opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,0) / 4
	with{
  		m3 = opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,m4);
		m4 = opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,_)~_;
	};

//Algorithm 31
algorithm(30,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,m5) / 5
	with{
		m5 = opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_;
	};

//Algorithm 32
algorithm(31,ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,
			ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,
			ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,
			ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,
			ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,
			ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6) =
  
  opperator(ga1,gd1,gs1,gr1,go1,fa1,fd1,fs1,fr1,fo1,1,1,0) + opperator(ga2,gd2,gs2,gr2,go2,fa2,fd2,fs2,fr2,fo2,ratio2,gain2,0) + opperator(ga3,gd3,gs3,gr3,go3,fa3,fd3,fs3,fr3,fo3,ratio3,gain3,0) + opperator(ga4,gd4,gs4,gr4,go4,fa4,fd4,fs4,fr4,fo4,ratio4,gain4,0) + opperator(ga5,gd5,gs5,gr5,go5,fa5,fd5,fs5,fr5,fo5,ratio5,gain5,0) + (opperator(ga6,gd6,gs6,gr6,go6,fa6,fd6,fs6,fr6,fo6,ratio6,gain6,_)~_) / 6;


algo(x) = algorithm(x,gAttack1,gDecay1,gSustain1,gRelease1,gOffset1,fAttack1,fDecay1,fSustain1,fRelease1,fOffset1,
					gAttack2,gDecay2,gSustain2,gRelease2,gOffset2,fAttack2,fDecay2,fSustain2,fRelease2,fOffset2,Ratio2,Gain2,
				    gAttack3,gDecay3,gSustain3,gRelease3,gOffset3,fAttack3,fDecay3,fSustain3,fRelease3,fOffset3,Ratio3,Gain3,
				    gAttack4,gDecay4,gSustain4,gRelease4,gOffset4,fAttack4,fDecay4,fSustain4,fRelease4,fOffset4,Ratio4,Gain4,
				    gAttack5,gDecay5,gSustain5,gRelease5,gOffset5,fAttack5,fDecay5,fSustain5,fRelease5,fOffset5,Ratio5,Gain5,
				    gAttack6,gDecay6,gSustain6,gRelease6,gOffset6,fAttack6,fDecay6,fSustain6,fRelease6,fOffset6,Ratio6,Gain6);

dx7(x) = par(i,32,algo(i)) : ba.selectn(32,x);
v = hslider("chooser",0,0,31,1);

process = dx7(v) / 3;