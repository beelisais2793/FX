#pragma once
#include <cmath>
#include <cstdlib>
#include <cstring>

#define KTABLE 64 // BLEP table oversampling factor

#include "Filters/OnePole.hpp"

const double minBlep_Table[2048] = {
	-0.000000,	-0.000000,	-0.000000,	-0.000000,
	-0.000000,	0.000000,	0.000000,	0.000000,
	0.000000,	0.000000,	0.000000,	0.000000,
	0.000000,	0.000000,	0.000001,	0.000001,
	0.000001,	0.000001,	0.000002,	0.000002,
	0.000003,	0.000004,	0.000005,	0.000006,
	0.000007,	0.000009,	0.000011,	0.000013,
	0.000016,	0.000019,	0.000022,	0.000027,
	0.000031,	0.000037,	0.000043,	0.000050,
	0.000058,	0.000067,	0.000078,	0.000089,
	0.000102,	0.000117,	0.000134,	0.000152,
	0.000173,	0.000195,	0.000221,	0.000249,
	0.000280,	0.000315,	0.000352,	0.000394,
	0.000440,	0.000491,	0.000546,	0.000606,
	0.000672,	0.000744,	0.000823,	0.000908,
	0.001001,	0.001102,	0.001212,	0.001330,
	0.001458,	0.001597,	0.001746,	0.001908,
	0.002081,	0.002268,	0.002469,	0.002685,
	0.002916,	0.003164,	0.003430,	0.003714,
	0.004018,	0.004342,	0.004688,	0.005057,
	0.005450,	0.005869,	0.006314,	0.006786,
	0.007288,	0.007821,	0.008386,	0.008984,
	0.009618,	0.010288,	0.010996,	0.011745,
	0.012535,	0.013368,	0.014247,	0.015172,
	0.016147,	0.017172,	0.018250,	0.019383,
	0.020573,	0.021822,	0.023131,	0.024504,
	0.025943,	0.027449,	0.029024,	0.030672,
	0.032394,	0.034193,	0.036072,	0.038031,
	0.040075,	0.042205,	0.044424,	0.046734,
	0.049138,	0.051639,	0.054238,	0.056938,
	0.059742,	0.062652,	0.065671,	0.068802,
	0.072046,	0.075406,	0.078885,	0.082485,
	0.086208,	0.090057,	0.094035,	0.098142,
	0.102383,	0.106758,	0.111270,	0.115922,
	0.120714,	0.125650,	0.130731,	0.135959,
	0.141336,	0.146864,	0.152543,	0.158376,
	0.164364,	0.170508,	0.176811,	0.183271,
	0.189892,	0.196674,	0.203617,	0.210722,
	0.217990,	0.225421,	0.233016,	0.240775,
	0.248697,	0.256782,	0.265031,	0.273443,
	0.282017,	0.290753,	0.299649,	0.308705,
	0.317919,	0.327290,	0.336816,	0.346496,
	0.356327,	0.366309,	0.376437,	0.386710,
	0.397126,	0.407681,	0.418372,	0.429197,
	0.440151,	0.451231,	0.462434,	0.473756,
	0.485192,	0.496739,	0.508391,	0.520144,
	0.531994,	0.543935,	0.555962,	0.568070,
	0.580254,	0.592507,	0.604824,	0.617199,
	0.629626,	0.642098,	0.654609,	0.667154,
	0.679724,	0.692313,	0.704914,	0.717521,
	0.730126,	0.742721,	0.755301,	0.767856,
	0.780380,	0.792865,	0.805304,	0.817688,
	0.830011,	0.842264,	0.854440,	0.866530,
	0.878527,	0.890423,	0.902211,	0.913882,
	0.925429,	0.936845,	0.948120,	0.959248,
	0.970221,	0.981032,	0.991674,	1.002138,
	1.012418,	1.022507,	1.032397,	1.042082,
	1.051556,	1.060811,	1.069841,	1.078641,
	1.087203,	1.095523,	1.103595,	1.111413,
	1.118972,	1.126267,	1.133293,	1.140047,
	1.146523,	1.152717,	1.158627,	1.164248,
	1.169578,	1.174613,	1.179352,	1.183790,
	1.187928,	1.191763,	1.195293,	1.198518,
	1.201438,	1.204050,	1.206356,	1.208356,
	1.210050,	1.211440,	1.212526,	1.213310,
	1.213794,	1.213980,	1.213872,	1.213471,
	1.212781,	1.211805,	1.210548,	1.209014,
	1.207207,	1.205132,	1.202794,	1.200198,
	1.197351,	1.194259,	1.190927,	1.187362,
	1.183572,	1.179562,	1.175342,	1.170917,
	1.166296,	1.161488,	1.156499,	1.151340,
	1.146018,	1.140542,	1.134921,	1.129165,
	1.123283,	1.117283,	1.111176,	1.104972,
	1.098679,	1.092309,	1.085870,	1.079373,
	1.072827,	1.066243,	1.059631,	1.053000,
	1.046361,	1.039723,	1.033097,	1.026492,
	1.019918,	1.013384,	1.006901,	1.000477,
	0.994122,	0.987845,	0.981656,	0.975562,
	0.969572,	0.963696,	0.957940,	0.952314,
	0.946825,	0.941480,	0.936286,	0.931252,
	0.926382,	0.921685,	0.917165,	0.912829,
	0.908682,	0.904730,	0.900976,	0.897427,
	0.894085,	0.890956,	0.888041,	0.885344,
	0.882869,	0.880617,	0.878590,	0.876790,
	0.875218,	0.873875,	0.872762,	0.871878,
	0.871224,	0.870799,	0.870601,	0.870629,
	0.870882,	0.871358,	0.872053,	0.872966,
	0.874093,	0.875430,	0.876974,	0.878720,
	0.880664,	0.882801,	0.885127,	0.887635,
	0.890320,	0.893175,	0.896196,	0.899375,
	0.902705,	0.906180,	0.909793,	0.913536,
	0.917401,	0.921381,	0.925469,	0.929655,
	0.933933,	0.938292,	0.942727,	0.947227,
	0.951784,	0.956391,	0.961037,	0.965715,
	0.970416,	0.975131,	0.979851,	0.984569,
	0.989275,	0.993962,	0.998619,	1.003241,
	1.007817,	1.012341,	1.016803,	1.021198,
	1.025516,	1.029750,	1.033894,	1.037940,
	1.041881,	1.045711,	1.049424,	1.053013,
	1.056472,	1.059797,	1.062981,	1.066020,
	1.068909,	1.071644,	1.074220,	1.076634,
	1.078882,	1.080962,	1.082870,	1.084604,
	1.086162,	1.087542,	1.088743,	1.089763,
	1.090602,	1.091260,	1.091736,	1.092031,
	1.092146,	1.092080,	1.091837,	1.091417,
	1.090822,	1.090055,	1.089118,	1.088015,
	1.086747,	1.085320,	1.083737,	1.082001,
	1.080118,	1.078092,	1.075927,	1.073629,
	1.071204,	1.068656,	1.065992,	1.063217,
	1.060338,	1.057361,	1.054293,	1.051139,
	1.047908,	1.044605,	1.041238,	1.037814,
	1.034340,	1.030823,	1.027270,	1.023688,
	1.020086,	1.016470,	1.012847,	1.009225,
	1.005611,	1.002012,	0.998435,	0.994888,
	0.991376,	0.987907,	0.984488,	0.981125,
	0.977824,	0.974592,	0.971435,	0.968358,
	0.965368,	0.962469,	0.959667,	0.956968,
	0.954376,	0.951895,	0.949529,	0.947284,
	0.945162,	0.943168,	0.941304,	0.939573,
	0.937979,	0.936522,	0.935207,	0.934034,
	0.933004,	0.932120,	0.931382,	0.930790,
	0.930345,	0.930047,	0.929896,	0.929890,
	0.930030,	0.930313,	0.930739,	0.931305,
	0.932010,	0.932851,	0.933826,	0.934931,
	0.936164,	0.937521,	0.939000,	0.940595,
	0.942303,	0.944119,	0.946040,	0.948060,
	0.950175,	0.952380,	0.954669,	0.957038,
	0.959480,	0.961991,	0.964564,	0.967194,
	0.969876,	0.972602,	0.975367,	0.978166,
	0.980991,	0.983837,	0.986698,	0.989567,
	0.992440,	0.995308,	0.998167,	1.001011,
	1.003833,	1.006628,	1.009390,	1.012113,
	1.014793,	1.017422,	1.019998,	1.022513,
	1.024963,	1.027344,	1.029650,	1.031878,
	1.034023,	1.036081,	1.038048,	1.039920,
	1.041695,	1.043369,	1.044938,	1.046401,
	1.047755,	1.048998,	1.050127,	1.051141,
	1.052038,	1.052818,	1.053479,	1.054021,
	1.054443,	1.054745,	1.054928,	1.054990,
	1.054934,	1.054760,	1.054469,	1.054062,
	1.053541,	1.052908,	1.052165,	1.051314,
	1.050358,	1.049299,	1.048140,	1.046885,
	1.045537,	1.044098,	1.042574,	1.040967,
	1.039282,	1.037523,	1.035694,	1.033799,
	1.031844,	1.029832,	1.027768,	1.025658,
	1.023505,	1.021316,	1.019095,	1.016846,
	1.014576,	1.012289,	1.009990,	1.007685,
	1.005377,	1.003074,	1.000779,	0.998497,
	0.996234,	0.993993,	0.991781,	0.989602,
	0.987459,	0.985358,	0.983304,	0.981299,
	0.979349,	0.977457,	0.975627,	0.973863,
	0.972168,	0.970545,	0.968998,	0.967530,
	0.966142,	0.964839,	0.963622,	0.962493,
	0.961454,	0.960507,	0.959654,	0.958895,
	0.958232,	0.957666,	0.957198,	0.956827,
	0.956554,	0.956379,	0.956302,	0.956323,
	0.956440,	0.956653,	0.956960,	0.957361,
	0.957854,	0.958438,	0.959109,	0.959868,
	0.960710,	0.961634,	0.962638,	0.963717,
	0.964871,	0.966095,	0.967386,	0.968742,
	0.970158,	0.971631,	0.973158,	0.974735,
	0.976357,	0.978021,	0.979724,	0.981460,
	0.983226,	0.985018,	0.986831,	0.988662,
	0.990506,	0.992359,	0.994216,	0.996074,
	0.997929,	0.999776,	1.001610,	1.003429,
	1.005228,	1.007004,	1.008751,	1.010467,
	1.012149,	1.013791,	1.015392,	1.016947,
	1.018453,	1.019908,	1.021309,	1.022652,
	1.023934,	1.025155,	1.026310,	1.027398,
	1.028417,	1.029366,	1.030241,	1.031043,
	1.031768,	1.032418,	1.032989,	1.033482,
	1.033896,	1.034231,	1.034486,	1.034661,
	1.034757,	1.034773,	1.034711,	1.034570,
	1.034352,	1.034058,	1.033688,	1.033245,
	1.032729,	1.032142,	1.031487,	1.030764,
	1.029977,	1.029127,	1.028216,	1.027248,
	1.026224,	1.025148,	1.024022,	1.022849,
	1.021632,	1.020374,	1.019079,	1.017749,
	1.016388,	1.014998,	1.013584,	1.012149,
	1.010696,	1.009229,	1.007750,	1.006265,
	1.004775,	1.003285,	1.001797,	1.000315,
	0.998844,	0.997385,	0.995942,	0.994518,
	0.993117,	0.991741,	0.990393,	0.989077,
	0.987796,	0.986551,	0.985345,	0.984181,
	0.983062,	0.981989,	0.980966,	0.979993,
	0.979072,	0.978206,	0.977396,	0.976644,
	0.975950,	0.975317,	0.974744,	0.974234,
	0.973786,	0.973402,	0.973081,	0.972825,
	0.972633,	0.972505,	0.972441,	0.972441,
	0.972504,	0.972631,	0.972819,	0.973068,
	0.973378,	0.973747,	0.974174,	0.974657,
	0.975195,	0.975786,	0.976429,	0.977121,
	0.977862,	0.978648,	0.979478,	0.980349,
	0.981259,	0.982206,	0.983187,	0.984200,
	0.985242,	0.986310,	0.987403,	0.988516,
	0.989649,	0.990796,	0.991957,	0.993128,
	0.994306,	0.995489,	0.996674,	0.997857,
	0.999037,	1.000211,	1.001375,	1.002528,
	1.003667,	1.004788,	1.005890,	1.006971,
	1.008027,	1.009057,	1.010058,	1.011028,
	1.011966,	1.012869,	1.013735,	1.014563,
	1.015351,	1.016098,	1.016801,	1.017460,
	1.018074,	1.018641,	1.019160,	1.019631,
	1.020053,	1.020425,	1.020746,	1.021017,
	1.021237,	1.021405,	1.021523,	1.021589,
	1.021605,	1.021570,	1.021485,	1.021351,
	1.021167,	1.020936,	1.020658,	1.020333,
	1.019964,	1.019550,	1.019094,	1.018597,
	1.018061,	1.017486,	1.016874,	1.016228,
	1.015549,	1.014839,	1.014099,	1.013332,
	1.012540,	1.011725,	1.010888,	1.010032,
	1.009160,	1.008272,	1.007373,	1.006462,
	1.005544,	1.004620,	1.003692,	1.002763,
	1.001834,	1.000908,	0.999988,	0.999074,
	0.998170,	0.997277,	0.996398,	0.995533,
	0.994687,	0.993859,	0.993052,	0.992268,
	0.991508,	0.990774,	0.990067,	0.989389,
	0.988742,	0.988126,	0.987543,	0.986994,
	0.986480,	0.986002,	0.985560,	0.985156,
	0.984790,	0.984463,	0.984175,	0.983927,
	0.983718,	0.983550,	0.983421,	0.983333,
	0.983284,	0.983276,	0.983306,	0.983376,
	0.983485,	0.983632,	0.983816,	0.984036,
	0.984293,	0.984585,	0.984910,	0.985269,
	0.985659,	0.986080,	0.986531,	0.987009,
	0.987515,	0.988045,	0.988600,	0.989177,
	0.989775,	0.990392,	0.991027,	0.991677,
	0.992342,	0.993020,	0.993708,	0.994406,
	0.995111,	0.995822,	0.996536,	0.997253,
	0.997971,	0.998687,	0.999400,	1.000109,
	1.000812,	1.001506,	1.002192,	1.002866,
	1.003528,	1.004176,	1.004808,	1.005424,
	1.006021,	1.006599,	1.007157,	1.007693,
	1.008206,	1.008695,	1.009159,	1.009597,
	1.010009,	1.010394,	1.010750,	1.011078,
	1.011377,	1.011646,	1.011885,	1.012093,
	1.012271,	1.012418,	1.012535,	1.012620,
	1.012675,	1.012699,	1.012692,	1.012655,
	1.012589,	1.012493,	1.012368,	1.012214,
	1.012033,	1.011824,	1.011590,	1.011329,
	1.011044,	1.010735,	1.010403,	1.010049,
	1.009674,	1.009279,	1.008866,	1.008435,
	1.007988,	1.007525,	1.007049,	1.006560,
	1.006060,	1.005549,	1.005030,	1.004504,
	1.003971,	1.003434,	1.002893,	1.002350,
	1.001807,	1.001263,	1.000722,	1.000184,
	0.999651,	0.999122,	0.998601,	0.998088,
	0.997584,	0.997091,	0.996609,	0.996139,
	0.995683,	0.995242,	0.994816,	0.994406,
	0.994013,	0.993639,	0.993283,	0.992946,
	0.992630,	0.992334,	0.992059,	0.991806,
	0.991574,	0.991366,	0.991180,	0.991016,
	0.990876,	0.990759,	0.990666,	0.990596,
	0.990549,	0.990525,	0.990524,	0.990546,
	0.990591,	0.990657,	0.990746,	0.990856,
	0.990987,	0.991138,	0.991310,	0.991500,
	0.991710,	0.991937,	0.992182,	0.992443,
	0.992720,	0.993011,	0.993317,	0.993636,
	0.993967,	0.994310,	0.994663,	0.995026,
	0.995397,	0.995775,	0.996160,	0.996551,
	0.996946,	0.997345,	0.997746,	0.998149,
	0.998552,	0.998955,	0.999356,	0.999755,
	1.000151,	1.000542,	1.000928,	1.001309,
	1.001682,	1.002047,	1.002404,	1.002752,
	1.003089,	1.003416,	1.003731,	1.004034,
	1.004324,	1.004600,	1.004863,	1.005111,
	1.005345,	1.005563,	1.005765,	1.005951,
	1.006121,	1.006275,	1.006411,	1.006531,
	1.006633,	1.006719,	1.006787,	1.006838,
	1.006871,	1.006888,	1.006888,	1.006871,
	1.006837,	1.006787,	1.006721,	1.006639,
	1.006542,	1.006430,	1.006304,	1.006163,
	1.006009,	1.005842,	1.005662,	1.005470,
	1.005267,	1.005053,	1.004829,	1.004596,
	1.004353,	1.004103,	1.003845,	1.003580,
	1.003310,	1.003034,	1.002754,	1.002469,
	1.002182,	1.001893,	1.001601,	1.001309,
	1.001017,	1.000725,	1.000435,	1.000147,
	0.999861,	0.999579,	0.999300,	0.999027,
	0.998758,	0.998496,	0.998240,	0.997991,
	0.997750,	0.997516,	0.997291,	0.997076,
	0.996869,	0.996673,	0.996487,	0.996311,
	0.996147,	0.995994,	0.995852,	0.995721,
	0.995603,	0.995497,	0.995403,	0.995321,
	0.995252,	0.995195,	0.995150,	0.995118,
	0.995098,	0.995091,	0.995095,	0.995112,
	0.995140,	0.995180,	0.995232,	0.995294,
	0.995368,	0.995452,	0.995546,	0.995650,
	0.995764,	0.995886,	0.996018,	0.996158,
	0.996305,	0.996461,	0.996623,	0.996791,
	0.996966,	0.997146,	0.997331,	0.997521,
	0.997714,	0.997911,	0.998111,	0.998313,
	0.998517,	0.998722,	0.998928,	0.999135,
	0.999341,	0.999546,	0.999750,	0.999952,
	1.000152,	1.000350,	1.000544,	1.000734,
	1.000921,	1.001103,	1.001280,	1.001452,
	1.001618,	1.001778,	1.001932,	1.002080,
	1.002220,	1.002353,	1.002479,	1.002597,
	1.002707,	1.002810,	1.002904,	1.002989,
	1.003066,	1.003135,	1.003195,	1.003246,
	1.003289,	1.003323,	1.003348,	1.003364,
	1.003372,	1.003371,	1.003362,	1.003345,
	1.003319,	1.003286,	1.003245,	1.003196,
	1.003139,	1.003076,	1.003005,	1.002928,
	1.002845,	1.002755,	1.002660,	1.002559,
	1.002453,	1.002342,	1.002226,	1.002107,
	1.001983,	1.001857,	1.001727,	1.001594,
	1.001459,	1.001322,	1.001183,	1.001043,
	1.000902,	1.000761,	1.000619,	1.000478,
	1.000337,	1.000198,	1.000059,	0.999922,
	0.999787,	0.999654,	0.999524,	0.999396,
	0.999272,	0.999151,	0.999034,	0.998921,
	0.998812,	0.998707,	0.998607,	0.998511,
	0.998421,	0.998336,	0.998256,	0.998181,
	0.998112,	0.998049,	0.997992,	0.997940,
	0.997894,	0.997854,	0.997821,	0.997793,
	0.997771,	0.997755,	0.997745,	0.997740,
	0.997742,	0.997749,	0.997762,	0.997781,
	0.997804,	0.997834,	0.997868,	0.997907,
	0.997951,	0.998000,	0.998053,	0.998111,
	0.998172,	0.998238,	0.998307,	0.998379,
	0.998455,	0.998534,	0.998615,	0.998699,
	0.998785,	0.998873,	0.998963,	0.999054,
	0.999146,	0.999240,	0.999334,	0.999429,
	0.999524,	0.999618,	0.999713,	0.999807,
	0.999900,	0.999992,	1.000083,	1.000173,
	1.000261,	1.000347,	1.000431,	1.000513,
	1.000593,	1.000670,	1.000744,	1.000815,
	1.000883,	1.000948,	1.001010,	1.001068,
	1.001123,	1.001174,	1.001222,	1.001265,
	1.001305,	1.001341,	1.001373,	1.001401,
	1.001425,	1.001445,	1.001461,	1.001473,
	1.001481,	1.001485,	1.001485,	1.001481,
	1.001474,	1.001463,	1.001448,	1.001430,
	1.001408,	1.001383,	1.001354,	1.001323,
	1.001288,	1.001251,	1.001211,	1.001168,
	1.001123,	1.001076,	1.001026,	1.000975,
	1.000921,	1.000866,	1.000810,	1.000752,
	1.000693,	1.000633,	1.000573,	1.000511,
	1.000449,	1.000387,	1.000325,	1.000262,
	1.000200,	1.000138,	1.000077,	1.000016,
	0.999957,	0.999898,	0.999840,	0.999783,
	0.999728,	0.999674,	0.999622,	0.999572,
	0.999523,	0.999477,	0.999432,	0.999390,
	0.999349,	0.999311,	0.999276,	0.999242,
	0.999211,	0.999183,	0.999157,	0.999134,
	0.999113,	0.999095,	0.999080,	0.999067,
	0.999057,	0.999049,	0.999045,	0.999042,
	0.999042,	0.999045,	0.999050,	0.999058,
	0.999068,	0.999080,	0.999095,	0.999111,
	0.999130,	0.999151,	0.999174,	0.999198,
	0.999224,	0.999252,	0.999282,	0.999313,
	0.999345,	0.999379,	0.999414,	0.999450,
	0.999486,	0.999524,	0.999562,	0.999601,
	0.999640,	0.999680,	0.999720,	0.999760,
	0.999800,	0.999841,	0.999881,	0.999920,
	0.999960,	0.999998,	1.000037,	1.000074,
	1.000111,	1.000147,	1.000182,	1.000216,
	1.000249,	1.000281,	1.000312,	1.000341,
	1.000369,	1.000396,	1.000421,	1.000445,
	1.000467,	1.000488,	1.000507,	1.000524,
	1.000540,	1.000554,	1.000566,	1.000577,
	1.000586,	1.000593,	1.000599,	1.000602,
	1.000605,	1.000605,	1.000604,	1.000602,
	1.000597,	1.000592,	1.000584,	1.000576,
	1.000566,	1.000554,	1.000542,	1.000528,
	1.000512,	1.000496,	1.000479,	1.000460,
	1.000441,	1.000421,	1.000399,	1.000378,
	1.000355,	1.000332,	1.000308,	1.000284,
	1.000260,	1.000235,	1.000210,	1.000185,
	1.000159,	1.000134,	1.000109,	1.000083,
	1.000058,	1.000033,	1.000009,	0.999985,
	0.999961,	0.999938,	0.999915,	0.999893,
	0.999871,	0.999850,	0.999830,	0.999811,
	0.999792,	0.999774,	0.999758,	0.999742,
	0.999727,	0.999713,	0.999700,	0.999688,
	0.999677,	0.999667,	0.999658,	0.999650,
	0.999644,	0.999638,	0.999634,	0.999630,
	0.999628,	0.999626,	0.999626,	0.999627,
	0.999628,	0.999631,	0.999635,	0.999639,
	0.999645,	0.999651,	0.999658,	0.999666,
	0.999675,	0.999685,	0.999695,	0.999706,
	0.999717,	0.999729,	0.999742,	0.999755,
	0.999769,	0.999783,	0.999797,	0.999812,
	0.999827,	0.999842,	0.999858,	0.999873,
	0.999889,	0.999904,	0.999920,	0.999936,
	0.999951,	0.999967,	0.999982,	0.999997,
	1.000012,	1.000027,	1.000041,	1.000055,
	1.000068,	1.000081,	1.000094,	1.000106,
	1.000118,	1.000129,	1.000140,	1.000150,
	1.000159,	1.000168,	1.000177,	1.000184,
	1.000191,	1.000198,	1.000203,	1.000208,
	1.000213,	1.000217,	1.000220,	1.000222,
	1.000224,	1.000225,	1.000225,	1.000225,
	1.000225,	1.000223,	1.000221,	1.000219,
	1.000215,	1.000212,	1.000207,	1.000203,
	1.000198,	1.000192,	1.000186,	1.000179,
	1.000172,	1.000165,	1.000157,	1.000150,
	1.000141,	1.000133,	1.000124,	1.000115,
	1.000106,	1.000097,	1.000088,	1.000078,
	1.000069,	1.000059,	1.000050,	1.000040,
	1.000031,	1.000022,	1.000013,	1.000003,
	0.999994,	0.999986,	0.999977,	0.999969,
	0.999961,	0.999953,	0.999945,	0.999938,
	0.999931,	0.999924,	0.999918,	0.999912,
	0.999906,	0.999901,	0.999896,	0.999891,
	0.999887,	0.999883,	0.999880,	0.999877,
	0.999875,	0.999872,	0.999871,	0.999869,
	0.999868,	0.999868,	0.999868,	0.999868,
	0.999868,	0.999869,	0.999871,	0.999872,
	0.999874,	0.999877,	0.999879,	0.999882,
	0.999885,	0.999889,	0.999893,	0.999897,
	0.999901,	0.999905,	0.999910,	0.999915,
	0.999920,	0.999925,	0.999930,	0.999935,
	0.999941,	0.999946,	0.999952,	0.999957,
	0.999963,	0.999969,	0.999974,	0.999980,
	0.999985,	0.999991,	0.999996,	1.000001,
	1.000006,	1.000012,	1.000017,	1.000021,
	1.000026,	1.000030,	1.000035,	1.000039,
	1.000043,	1.000047,	1.000050,	1.000053,
	1.000056,	1.000059,	1.000062,	1.000064,
	1.000067,	1.000068,	1.000070,	1.000072,
	1.000073,	1.000074,	1.000074,	1.000075,
	1.000075,	1.000075,	1.000075,	1.000075,
	1.000074,	1.000073,	1.000072,	1.000071,
	1.000069,	1.000068,	1.000066,	1.000064,
	1.000062,	1.000060,	1.000057,	1.000055,
	1.000052,	1.000050,	1.000047,	1.000044,
	1.000041,	1.000038,	1.000035,	1.000032,
	1.000028,	1.000025,	1.000022,	1.000019,
	1.000015,	1.000012,	1.000009,	1.000006,
	1.000003,	1.000000,	0.999997,	0.999994,
	0.999991,	0.999988,	0.999986,	0.999983,
	0.999981,	0.999978,	0.999976,	0.999974,
	0.999972,	0.999970,	0.999969,	0.999967,
	0.999966,	0.999964,	0.999963,	0.999962,
	0.999961,	0.999960,	0.999960,	0.999959,
	0.999959,	0.999959,	0.999959,	0.999959,
	0.999959,	0.999959,	0.999960,	0.999960,
	0.999961,	0.999962,	0.999963,	0.999964,
	0.999965,	0.999966,	0.999967,	0.999968,
	0.999970,	0.999971,	0.999973,	0.999975,
	0.999976,	0.999978,	0.999980,	0.999981,
	0.999983,	0.999985,	0.999987,	0.999988,
	0.999990,	0.999992,	0.999994,	0.999996,
	0.999997,	0.999999,	1.000001,	1.000002,
	1.000004,	1.000005,	1.000007,	1.000008,
	1.000010,	1.000011,	1.000012,	1.000013,
	1.000014,	1.000015,	1.000016,	1.000017,
	1.000018,	1.000019,	1.000019,	1.000020,
	1.000020,	1.000021,	1.000021,	1.000021,
	1.000021,	1.000022,	1.000022,	1.000021,
	1.000021,	1.000021,	1.000021,	1.000020,
	1.000020,	1.000020,	1.000019,	1.000018,
	1.000018,	1.000017,	1.000016,	1.000016,
	1.000015,	1.000014,	1.000013,	1.000012,
	1.000011,	1.000010,	1.000009,	1.000009,
	1.000008,	1.000007,	1.000006,	1.000005,
	1.000004,	1.000003,	1.000002,	1.000001,
	1.000000,	0.999999,	0.999998,	0.999998,
	0.999997,	0.999996,	0.999995,	0.999995,
	0.999994,	0.999993,	0.999993,	0.999992,
	0.999992,	0.999991,	0.999991,	0.999991,
	0.999990,	0.999990,	0.999990,	0.999990,
	0.999990,	0.999990,	0.999989,	0.999989,
	0.999990,	0.999990,	0.999990,	0.999990,
	0.999990,	0.999990,	0.999991,	0.999991,
	0.999991,	0.999992,	0.999992,	0.999992,
	0.999993,	0.999993,	0.999994,	0.999994,
	0.999994,	0.999995,	0.999995,	0.999996,
	0.999996,	0.999997,	0.999997,	0.999998,
	0.999998,	0.999999,	0.999999,	1.000000,
	1.000000,	1.000001,	1.000001,	1.000001,
	1.000002,	1.000002,	1.000002,	1.000003,
	1.000003,	1.000003,	1.000003,	1.000004,
	1.000004,	1.000004,	1.000004,	1.000004,
	1.000004,	1.000005,	1.000005,	1.000005,
	1.000005,	1.000005,	1.000005,	1.000005,
	1.000004,	1.000004,	1.000004,	1.000004,
	1.000004,	1.000004,	1.000004,	1.000003,
	1.000003,	1.000003,	1.000003,	1.000003,
	1.000002,	1.000002,	1.000002,	1.000002,
	1.000002,	1.000001,	1.000001,	1.000001,
	1.000001,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	0.999999,	0.999999,
	0.999999,	0.999999,	0.999999,	0.999999,
	0.999999,	0.999999,	0.999998,	0.999998,
	0.999998,	0.999998,	0.999998,	0.999998,
	0.999998,	0.999998,	0.999998,	0.999998,
	0.999998,	0.999998,	0.999998,	0.999998,
	0.999998,	0.999999,	0.999999,	0.999999,
	0.999999,	0.999999,	0.999999,	0.999999,
	0.999999,	0.999999,	0.999999,	0.999999,
	0.999999,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000,
	1.000000,	1.000000,	1.000000,	1.000000};

typedef enum
{
    OT_SAW,
    OT_RSAW,
    OT_SQUARE,
    OT_TRIANGLE,
} oscwave_t;

typedef struct
{
    double f; // frequency
    double p; // phase
    double v; // output
    
    double triangle;
    double saw;
    double rsaw;
    double square;
    double sine;

    bool bSync;		// hardsync on/off
    double fSync;	// hardsync frequency
    double fPWM;		// pulse width
    double s_rate;

    oscwave_t type; // square or saw
    
    double *buffer; // circular output buffer
    int cBuffer;	// buffer size
    int iBuffer;	// current buffer position
    int nInit;		// amount of initialized entries
} osc_t;


struct minBLEP
{

    minBLEP(double sampleRate) {                
        sr = sampleRate;
        fc = 440.0f;
        lpO = osc_Create(sampleRate);
        lpO->f = fc;        
        block.setFc(10/sampleRate);
    }    
    ~minBLEP() {
         osc_Free(lpO);
        
    }
    void setFrequency(double f)
    {   
       
        lpO->f = f;       
        //lpO->triangle = 0;
    }
    void setWaveform(oscwave_t type)
    {        
        lpO->type = type;        
    }
    double Tick(double I=0,double A=1, double X=0, double Y=0)
    {
        return osc_Play(lpO);
    }
    
    // call when main oscillator resets
    void syncSlave()
    {        
        if(lpS == NULL) return;
        /*
        double fs=lpO->f / lpO->s_rate;
		lpS->v=0.0f;        
		osc_AddBLEP(lpS, lpS->p/fs,1.0f);
        
        
        // add BLEP at sync
        if (lpS->type >=OT_SQUARE)
        {
            lpS->v=1.0f;
            lpS->triangle = 0.0f;
            osc_AddBLEP(lpS, (lpS->p-lpS->fPWM)/fs,-1.0f);
        } 
        */   
        lpS->p=0;
    }

    void setSlave(osc_t * o)
    {
        lpS = o;
    }

    float blepInit(float v)
    {
        // add BLEP buffer contents
        if (lpO->nInit)
        {
            v+=lpO->buffer[lpO->iBuffer];
            lpO->nInit--;
            if (++lpO->iBuffer>=lpO->cBuffer) lpO->iBuffer=0;
        }
        return v;
    }
    OnePole block;
    double fc,sr;
    osc_t* lpO;
    osc_t* lpS;
    
    double  osc_Play(osc_t *lpO);
    void    osc_AddBLEP(osc_t *lpO, double offset, double amp);
    osc_t*  osc_Create(double rate);
    void    osc_Free(osc_t * lp);
};

inline void minBLEP::osc_Free(osc_t * lp)
{
    if(lp->buffer) free(lp->buffer);
    free(lp);
}

inline osc_t* minBLEP::osc_Create(double rate)
{
	osc_t *lpO=(osc_t*)malloc(sizeof(osc_t));

	// frequency
	lpO->f=440;
	lpO->p=0;
	lpO->v=0;

	// stuff
	lpO->bSync=false;
	lpO->fPWM=0.5;
	lpO->type=OT_SAW;
	lpO->s_rate = rate;

	// buffer
	lpO->cBuffer = 1024; //gMinBLEP.c/KTABLE;
	lpO->buffer=(double*)malloc(sizeof(double)*lpO->cBuffer);
	lpO->iBuffer=0;
	lpO->nInit=0;

	return lpO;
}

#define LERP(A,B,F) ((B-A)*F+A)

//----------------------------------------------------------

// add impulse into buffer
void minBLEP::osc_AddBLEP(osc_t *lpO, double offset, double amp)
{
    int i;
    double *lpOut=lpO->buffer+lpO->iBuffer;
    const double *lpIn=minBlep_Table+(uint32_t)(KTABLE*offset);
    double  frac=fmod(KTABLE*offset,1.0);
    int     cBLEP=(2048/KTABLE)-1;
    double *lpBufferEnd=lpO->buffer+lpO->cBuffer;
    double f;

	// add
	for (i=0; i<lpO->nInit; i++, lpIn+=KTABLE, lpOut++)
	{
		if (lpOut>=lpBufferEnd) lpOut=lpO->buffer;
		f=LERP(lpIn[0],lpIn[1],frac);
		*lpOut+=amp*(1-f);
	}

	// copy
	for (; i<cBLEP; i++, lpIn+=KTABLE, lpOut++)
	{
		if (lpOut>=lpBufferEnd) lpOut=lpO->buffer;
		f=LERP(lpIn[0],lpIn[1],frac);
		*lpOut=amp*(1-f);
	}

	lpO->nInit=cBLEP;
}

//----------------------------------------------------------
/*
double sign(double x) {
    if(x < 0) return -1;
    return 1;
}
*/

// play waveform
double minBLEP::osc_Play(osc_t *lpO)
{
    double v;
    double fs=lpO->f / lpO->s_rate;
    static double phase = 0;

	// create waveform
	lpO->p=lpO->p+fs;
    	
    // add BLEP at end of waveform
	if (lpO->p>=1)
	{        
		lpO->p=lpO->p-1.0;
		lpO->v=0.0f;        
		osc_AddBLEP(lpO, lpO->p/fs,1.0f);
        syncSlave();
	}
    
	// add BLEP in middle of wavefor for squarewave
	if (!lpO->v && lpO->p>lpO->fPWM && lpO->type >=OT_SQUARE)
	{
		lpO->v=1.0f;
        //lpO->triangle = 0.0f;
		osc_AddBLEP(lpO, (lpO->p-lpO->fPWM)/fs,-1.0f);
	}

    // these fruities are only positive 
    lpO->saw    = lpO->p;     
    lpO->rsaw   = 1.0-lpO->saw;
    lpO->square = lpO->v;
    

	// sample value
	if (lpO->type==OT_SAW)
	{
		v = lpO->saw;    		
	}
    else if(lpO->type == OT_RSAW)
	{
		v = lpO->rsaw;		
	}
	else
	{
		v = lpO->square;		
	}

    v=blepInit(v);

    // it's not entirely stable
    lpO->triangle += (2*v-1)*fs/4.0;
    lpO->triangle -= block.process(lpO->triangle);
    if (lpO->type==OT_TRIANGLE)
        return 8*(lpO->triangle);

	return 2*v-1;
}

