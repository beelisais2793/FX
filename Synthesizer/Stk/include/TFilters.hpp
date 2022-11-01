#pragma once

// this is now in Kfr DSP-1
// KfrFilters
//#include "TIIRFilters.hpp"
//#include "TFIRFilters.hpp"


// nigel_biquad = zolzer
// rbj biquad 
// cppfilters.hpp
// massberg biquad

#include "TBiquadFilter.hpp"
#include "TRBJFilter.hpp"
#include "TSVF.hpp"


#include "TCurtisVCF.hpp"
#include "TKorg35LPF.hpp"
#include "TKorg35HPF.hpp"
#include "TDiodeLadder.hpp"
#include "TFormantFilter.hpp"
#include "THouvilainenMoog.hpp"
#include "TImprovedMoog.hpp"
#include "TKrajeskiMoog.hpp"
#include "TMicrotrackerMoog.hpp"
#include "TMoogHalfLadder.hpp"
#include "TMoogLadder.hpp"
#include "TMusicDSPMoog.hpp"
#include "TOberheim.hpp"
#include "TOberheimMoog.hpp"
#include "TRKMoog.hpp"
#include "TRKSimulationModel.hpp"
#include "TStateVariableFilter.hpp"

/////////////////////////////////////////////////////////////////
// C++ Filters
// now in cppfilters.hpp
/////////////////////////////////////////////////////////////////
#include "fo_apf.h"
#include "fo_lpf.h"
#include "fo_hpf.h"
#include "fo_shelving_high.h"
#include "fo_shelving_low.h"
#include "so_apf.h"
#include "so_bpf.h"
#include "so_bsf.h"
#include "so_lpf.h"
#include "so_hpf.h"
#include "so_butterworth_bpf.h"
#include "so_butterworth_bsf.h"
#include "so_butterworth_hpf.h"
#include "so_butterworth_lpf.h"
#include "so_linkwitz_riley_hpf.h"
#include "so_linkwitz_riley_lpf.h"
#include "so_parametric_cq_boost.h"
#include "so_parametric_cq_cut.h"
#include "so_parametric_ncq.h"
