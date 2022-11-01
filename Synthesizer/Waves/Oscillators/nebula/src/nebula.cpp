// nebula.cpp
// 

#include "GetOpt.h"
#include "Envelope.h"
#include "OscillatorI.h"
#include "OscillatorF.h"
#include "Granulator.h"
#include "WavFile.h"

#include <stdexcept>
#include <iostream>
#include <vector>

using namespace std;
using namespace soundmath;

const int TABLEN = 4096;
const float SR = 44100;
const int MAXPOLY = 1024;

int main (int argc, char* argv[]) {
	srand (time (NULL));
	
	cout << "[nebula, ver. 1.00]" << endl << endl;
	cout << "advanced granular synthesis" << endl;
	cout << "(c) 2021 by Carmine E. Cella" << endl << endl;
	
	int bsize = 64; // default

	float* buffer1 = 0;
	float* buffer2 = 0;
	float* buffer3 = 0;

	float* obuff = 0;
	float* obuffL = 0;
	float* obuffR = 0;

	float* win = new float[TABLEN + 1];

	float* proto1 = new float[TABLEN + 1]; // guard point
	float* proto2 = new float[TABLEN + 1]; // guard point
    
	try {
		GetOpt opt ("d:D:l:L:f:F:s:K:j:J:a:b:h:H:m:g:w:p:q");
		
		double dmin = 10;
		double dmax = 10;
		double Dmin = 0;
		double Dmax = 0;		
		double lmin = 1./10;
		double lmax = 1./10;
		double Lmin = 0;
		double Lmax = 0;		
		double fmin = 220;
		double fmax = 220;
		double Fmin = 0;
		double Fmax = 0;
		double smin = 1;
		double smax = 1;
		double Gmin = 0;
		double Gmax = 0;		
		double jmin = .5;
		double jmax = .5;
		double Jmin = 1;
		double Jmax = 1;
		int polyph = MAXPOLY;
		bool olapDetect = false;
		
		double amin = .25;
		double amax = .25;

		float mmin = 0;
		float mmax = 0;

		string wtype = "hann";

		float aset = 1;
			
		std::string proto1Name = "sine";
        memset (proto1, 0, sizeof (float) * (TABLEN + 1));
		OscillatorI<float>::gen (proto1, TABLEN, &aset, 1, -.25);

		std::string proto2Name = "sine";
        memset (proto2, 0, sizeof (float) * (TABLEN + 1));
		OscillatorI<float>::gen (proto2, TABLEN, &aset, 1, -.25);

		float gletScaling = 0;
		bool relative = false;
		int sz1 = TABLEN;
		int sz2 = TABLEN;

		int c = 0;
		while ((c = opt.parse (argc, argv)) != -1) {
			switch (c) {
			case 'd': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					dmin = atof (opt.arg ());
					dmax = dmin;
				} else {
					dmin = atof (v.substr (0, sep).c_str ());
					dmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}
			case 'D': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					Dmin = atof (opt.arg ());
					Dmax = Dmin;
				} else {
					Dmin = atof (v.substr (0, sep).c_str ());
					Dmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}				
			case 'l': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					lmin = atof (opt.arg ());
					lmax = lmin;
				} else {
					lmin = atof (v.substr (0, sep).c_str ());
					lmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}	
			case 'L': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					Lmin = atof (opt.arg ());
					Lmax = Lmin;
				} else {
					Lmin = atof (v.substr (0, sep).c_str ());
					Lmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}						
			case 'f': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					fmin = atof (opt.arg ());
					fmax = fmin;
				} else {
					fmin = atof (v.substr (0, sep).c_str ());
					fmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}	
			case 'F': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					Fmin = atof (opt.arg ());
					Fmax = Fmin;
				} else {
					Fmin = atof (v.substr (0, sep).c_str ());
					Fmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}	
			case 's': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					smin = atof (opt.arg ());
					smax = smin;
				} else {
					smin = atof (v.substr (0, sep).c_str ());
					smax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}
			case 'K': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					Gmin = atof (opt.arg ());
					Gmax = Gmin;
				} else {
					Gmin = atof (v.substr (0, sep).c_str ());
					Gmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}								
			case 'j': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					jmin = atof (opt.arg ());
					jmax = jmin;
				} else {
					jmin = atof (v.substr (0, sep).c_str ());
					jmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}		
			case 'J': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					Jmin = atof (opt.arg ());
					Jmax = Jmin;
				} else {
					Jmin = atof (v.substr (0, sep).c_str ());
					Jmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}                
			case 'a': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					amin = atof (opt.arg ());
					amax = amin;
				} else {
					amin = atof (v.substr (0, sep).c_str ());
					amax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;
			}	
			case 'b':
				bsize = atoi (opt.arg ());
				break;
			case 'p':
				polyph = atoi (opt.arg ());
				break;					
			case 'h': {
				int v = atoi (opt.arg ());
				switch (v) {
				default: {
					//sz1 = readSamples (opt.arg (), proto1, bsize);

					WavInFile in (opt.arg ());

					int nchnls = in.getNumChannels ();
					float* tmp1 = new float[in.getNumSamples () * nchnls];
					sz1 = (in.getNumSamples ());

					if (proto1) delete [] proto1;
					proto1 = new float[sz1 + bsize];
					memset (proto1, 0, sizeof (float) * (sz1 + bsize));

					in.read (tmp1, in.getNumSamples () * nchnls);

					int pointer = 0;
					while (pointer <= sz1) {
						for (int i = 0; i < bsize; ++i) {
							for (int j = 0; j < nchnls; ++j) {
								proto1[(i + pointer)] += (tmp1[nchnls * (i + pointer) + j] / nchnls);
							}
						}
						pointer += bsize;
					}

					delete [] tmp1;

					relative = true;
					proto1Name = opt.arg ();
					break;
				}
						
				case 1:
					memset (proto1, 0, sizeof (float) * (TABLEN + 1));
					OscillatorI<float>::gen (proto1, TABLEN, &aset, 1, -.25);
					break;
				case 2:
					proto1Name = "triangle";
					memset (proto1, 0, sizeof (float) * (TABLEN + 1));
					OscillatorI<float>::triangle (proto1, TABLEN, 150);
					break;
				case 3:	
					proto1Name = "square";
					memset (proto1, 0, sizeof (float) * (TABLEN + 1));
					OscillatorI<float>::square (proto1, TABLEN, 150);
					break;	
				case 4:
					proto1Name = "rand";
					memset (proto1, 0, sizeof (float) * (TABLEN + 1));
					for (int i = 0; i < TABLEN; ++i) {
						proto1[i] = ((float) rand () / RAND_MAX) * 2 - 1.;
					}		
					break;
				}	
					
				break; 
			}
			case 'H': {
				int v = atoi (opt.arg ());

				switch (v) {
				default: {
					WavInFile in (opt.arg ());

					int nchnls = in.getNumChannels ();
					float* tmp1 = new float[in.getNumSamples () * nchnls];
					sz2 = (in.getNumSamples ());

					if (proto2) delete [] proto2;
					proto2 = new float[sz2 + bsize];
					memset (proto2, 0, sizeof (float) * (sz2 + bsize));

					in.read (tmp1, in.getNumSamples () * nchnls);

					int pointer = 0;
					while (pointer <= sz2) {
						for (int i = 0; i < bsize; ++i) {
							for (int j = 0; j < nchnls; ++j) {
								proto2[(i + pointer)] += (tmp1[nchnls * (i + pointer) + j] / nchnls);
							}
						}
						pointer += bsize;
					}

					delete [] tmp1;

					relative = true;
					proto1Name = opt.arg ();
					break;
				}

				case 1:
					memset (proto2, 0, sizeof (float) * (TABLEN + 1));
					OscillatorI<float>::gen (proto2, TABLEN, &aset, 1, -.25);
					break;
				case 2:
					proto2Name = "triangle";
					memset (proto2, 0, sizeof (float) * (TABLEN + 1));
					OscillatorI<float>::triangle (proto2, TABLEN, 150);
					break;
				case 3:	
					proto2Name = "square";
					memset (proto2, 0, sizeof (float) * (TABLEN + 1));
					OscillatorI<float>::square (proto2, TABLEN, 150);
					break;	
				case 4:
					proto2Name = "rand";
					memset (proto2, 0, sizeof (float) * (TABLEN + 1));
					for (int i = 0; i < TABLEN; ++i) {
						proto2[i] = ((float) rand () / RAND_MAX) * 2 - 1.;
					}		
					break;
				}	

				break;                                        
			}
			case 'm': {
				string v = opt.arg ();
				size_t sep = v.find (":");
				if (sep == string::npos) {
					mmin = atof (opt.arg ());
					mmax = mmin;
				} else {
					mmin = atof (v.substr (0, sep).c_str ());
					mmax = atof (v.substr (sep + 1, v.size () - 1).c_str ());
				}
				break;                  
			}         
			case 'g':
				gletScaling = atof (opt.arg ());
				break;				           
			case 'w': {
				wtype = opt.arg ();
				break; 
			}   
			case 'q':
				olapDetect = true;
				break;	                
            }
		}
		
		if (argc - opt.index () != 2) {
			throw runtime_error (
								 "syntax is 'nebula [options] dur out.wav'\n\n" \

								 "-d<x> <x:y>   density (grains per sec.)\n"	\
								 "-D<x> <x:y>   random density (ratio on -d)\n"	\
								 "-l<x> <x:y>   grain length (sec.)\n"\
								 "-L<x> <x:y>   random grain length (ratio on -l)\n"\
								 "-f<x> <x:y>   grain frequency (Hz)\n"	\
								 "-s<x> <x:y>   speed for sample reading (only useful for files)\n"	\
								 "-F<x> <x:y>   random grain frequency (ratio on -f)\n"	\
								 "-K<x> <x:y>   random grain glissato (ratio on -f)\n"	\
								 "-j<x> <x:y>   jittering (normalized)\n" \
								 "-J<x> <x:y>   random jittering (ratio on -j)\n" \
								 "-h<x>         prototype 1: filename.wav or\n" \
								 "              1 = sine, 2 = triangle, 3 = square, 4 = random\n" \
								 "-H<x>         prototype 1: filename.wav or \n" \
								 "              1 = sine, 2 = triangle, 3 = square, 4 = random\n" \
								 "-m<x> <x:y>   morphing coefficient (only for coherent prototypes)\n"	\
								 "-g<x>         grainlets with given scaling (-l, -L unused)\n"	\
								 "-w<x>         window type (hann, hamm, black)\n"	\
								 "-a<x> <x:y>   amplitude for each grain (normalized)\n"\
								 "-b<x>         buffer size\n" \
								 "-p<x>         polyphony limit\n" \
								 "-q            detect maximum overlap\n\n" \
    
								 "some arguments can be also in the form <x:y> where x and y are\n" \
								 "the values of the parameter at the beginning and at the end of process\n" \
								 );
		}

		float dur = atof (argv[opt.index ()]);
		
		// check params
		if (dur <= 0) {
			throw runtime_error ("invalid duration");	
		}
		if (dmin <= 0 || dmax <= 0) {
			throw runtime_error ("invalid density");
		}		
		if (Dmin < 0 || Dmax < 0 || Dmin > 1 || Dmax > 1) {
			throw runtime_error ("invalid random density");
		}
		if (lmin <= 0 || lmax <= 0) {
			throw runtime_error ("invalid length");
		}
		if (Lmin < 0 || Lmax < 0 || Lmin > 1 || Lmax > 1) {
			throw runtime_error ("invalid random length");
		}
		// fmin and fmax can be negative
		if (Fmin < 0 || Fmax < 0 || Fmin > 1 || Fmax > 1) {
			throw runtime_error ("invalid random frequency");
		}	
		if (Gmin < 0 || Gmax < 0 || Gmin > 1 || Gmax > 1) {
			throw runtime_error ("invalid glissato");
		}			
		if (jmin < 0 || jmax < 0 || jmin > 1 || jmax > 1) {
			throw runtime_error ("invalid jitter");
		}
		if (Jmin < 0 || Jmax < 0 || Jmin > 1 || Jmax > 1) {
			throw runtime_error ("invalid random jitter");
		}		
		if (amin < 0 || amax < 0 || amin > 1 || amax > 1) {
			throw runtime_error ("invalid amplitude");
		}		
		if (bsize <= 0) {
			throw runtime_error ("invalid buffer size");
		}
		if (polyph <= 0 || polyph > MAXPOLY) {
			throw runtime_error ("invalid polyphony");
		}		
		if (mmin < 0 || mmax < 0 || mmin > 1 || mmax > 1) {
			throw runtime_error ("invalid morphing");
		}	
			
		WavOutFile out (argv[opt.index () + 1], (int) SR, 16, 2);		

		buffer1 = new float[bsize];
		buffer2 = new float[bsize];
		buffer3 = new float[bsize];

		obuff = new float[2 * bsize];
		obuffL = new float[bsize];
		obuffR = new float[bsize];

		Granulator <float, OscillatorF> g (
										   SR, proto1, proto2, win,
										   sz1, sz2, TABLEN, MAXPOLY
										   );

 		int buffers = (int) (SR / bsize * dur);
		
		float dincr = (dmax - dmin) / buffers;
		float d = dmin;
		float Dincr = (Dmax - Dmin) / buffers;
		float D = Dmin;
		float lincr = (lmax - lmin) / buffers;
		float l = lmin;
		float Lincr = (Lmax - Lmin) / buffers;
		float L = Lmin;
		float fincr = (fmax - fmin) / buffers;
		float f = fmin;
		float Fincr = (Fmax - Fmin) / buffers;
		float F = Fmin;		
		float sincr = (smax - smin) / buffers;
		float s = smin;
		float Gincr = (Gmax - Gmin) / buffers;
		float G = Gmin;		
		float jincr = (jmax - jmin) / buffers;
		float j = jmin;
		float Jincr = (Jmax - Jmin) / buffers;
		float J = Jmin;
						
		float aincr = (amax - amin) / buffers;
		float a = amin;

		float mincr = (mmax - mmin) / buffers;
		float m = mmin;

		float a0, a1, a2;
		if (wtype == "hann") {
			a0 = .5;
			a1 = .5;
			a2 = 0;
		} else if (wtype == "hamm") {
			a0 = .54;
			a1 = .46;
			a2 = 0;
		} else if (wtype == "black") {
			a0 =.42;
			a1 = .5;
			a2 = .08;
		} else {
			throw runtime_error ("invalid window type");	
		}
		
		Envelope<float>::window (win, TABLEN, a0, a1, a2);
		win[TABLEN] = win[0];

		if (relative && (fmin > 3 || fmax > 3)) {
			cout << "[warning: frequency very high for sample-based granulation]" << endl << endl;
		}
		if (!relative && (fmin < 20 || fmax < 20)) {
			cout << "[warning: frequency very low for prototypal granulation]" << endl << endl;
		}

		cout << "density        = " << dmin << " -> " << dmax << " grains per sec." << endl;
		cout << "rand density   = " << Dmin << " -> " << Dmax << endl;
		if (!gletScaling) {
			cout << "length         = " << lmin << " -> " << lmax << " sec. " << endl;
			cout << "rand length    = " << Lmin << " -> " << Lmax << endl;
		} else {
			cout << "length         = grainlets (scaled by " << gletScaling << ")" << endl;
		}
		cout << "frequency      = " << fmin << " -> " << fmax << (relative ? " x" : " Hz") << endl;
		cout << "rand frequency = " << Fmin << " -> " << Fmax << endl;
		cout << "speed          = " << smin << " -> " << smax << endl;
		cout << "rand glissato  = " << Gmin << " -> " << Gmax << endl;
		cout << "jitter         = " << jmin << " -> " << jmax << endl;
		cout << "rand jitter    = " << Jmin << " -> " << Jmax << endl;		
		cout << "buffer size    = " << bsize << " frames" << endl;
		cout << "amplitude      = " << amin << " -> " << amax << endl;
		cout << "prototype 1    = " << proto1Name << endl;
		cout << "prototype 2    = " << proto2Name << endl;
		cout << "morphing       = " << mmin << " -> " << mmax << endl;
		cout << "window         = " << wtype << endl;
		cout << "total duration = " << dur << " sec." << endl << endl;

		g.relativePitch (relative);

		g.amplitude (a);
		g.length (l, L);
		g.frequency (f, F);

		g.density (d, D);
		g.jitter (j, J);
		g.morphing (m);
		g.glissato (G);
		g.grainlets (gletScaling);
		g.speed (s);

		g.schedule (0, 0, 0); // force first triggering
		g.poly (polyph); // limit polyphony
		
		cout << "processing...... "; cout.flush ();
		memset (obuff, 0, sizeof (float) * 2 * bsize);

		clock_t tic = clock ();
		int olap = 0;
		int molap = 0;

		for (int i = 0; i < buffers; ++i) {	
			memset (obuffL, 0, sizeof (float) * bsize);
 			memset (obuffR, 0, sizeof (float) * bsize);
			
			g.process (obuffL, obuffR, buffer1, buffer2, buffer3, bsize);
			
			// interleave
			for (int z = 0; z < bsize; ++z) {
				obuff[2 * z] = obuffL[z];
				obuff[2 * z + 1] = obuffR[z];
			}
			
			out.write (obuff, 2 * bsize);
		
			if (olapDetect) {
				olap = g.olap ();
				if (olap > molap) molap = olap;
			}
			
 			g.amplitude (a);
 			g.length (l, L);
 			g.frequency (f, F);
			g.speed (s);
 			g.density (d, D);
 			g.jitter (j, J);
 			g.morphing (m);
 			g.glissato (G);
	
			d += dincr;
			D += Dincr;
			l += lincr;
			L += Lincr;
			f += fincr;
			F += Fincr;
			s += sincr;
			G += Gincr;
			j += jincr;
			J += Jincr;
			
			a += aincr;
			m += mincr;
		}		
		clock_t toc = clock ();		
		cout << "done in " << (float) (toc - tic) / CLOCKS_PER_SEC << " sec. " << endl;

		if (olapDetect) {
			cout << endl << "max overlap    = " << molap << endl;
		}
	} catch (bad_alloc& memmoryAllocationException) {
		cout << "memory allocation exception: "
			 << memmoryAllocationException.what ()
			 << endl;
	} catch (exception& e) {
		cout << "Error: " << e.what () << endl;
	}
	catch (...) {
		cout << "Fatal error: unknown exception" << endl;
	}
	delete [] win;
	delete [] buffer1;
	delete [] buffer2;
	delete [] buffer3;

	delete [] obuff;
	delete [] obuffL;
	delete [] obuffR;
	delete [] proto1;
	delete [] proto2;

	return 0;
}

// EOF


