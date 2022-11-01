sptbl["adsr"] = {

    files = {
        module = "adsr.c",
        header = "adsr.h",
        example = "ex_adsr.c",
    },

    func = {
        create = "sp_adsr_create",
        destroy = "sp_adsr_destroy",
        init = "sp_adsr_init",
        compute = "sp_adsr_compute",
    },

    params = {
        optional = {
            {
                name = "atk",
                type = "SPFLOAT",
                description = "Attack time (in seconds)",
                default = 0.1
            },
            {
                name = "dec",
                type = "SPFLOAT",
                description ="Decay time (in seconds)",
                default = 0.1
            },
            {
                name = "sus",
                type = "SPFLOAT",
                description ="Sustain (in range 0-1)",
                default = 0.5
            },
            {
                name = "rel",
                type = "SPFLOAT",
                description ="Release time (in seconds)",
                default = 0.3
            },
        }
    },

    modtype = "module",

    description = [[ADSR generator
This is an ADSR generator whose curves are created using a one-pole low pass filter.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "gate",
            description = "Gate signal. This should be a steady state signal that jumps from 0 to 1."
        },
    },

    outputs = {
        {
            name = "out",
            description = "ADSR envelope."
        },
    }

}
sptbl["allpass"] = {

    files = {
        module = "allpass.c",
        header = "allpass.h",
        example = "ex_allpass.c",
    },

    func = {
        create = "sp_allpass_create",
        destroy = "sp_allpass_destroy",
        init = "sp_allpass_init",
        compute = "sp_allpass_compute",
    },

    params = {
        mandatory = {
            {
                name = "looptime",
                type = "SPFLOAT",
                description = "The loop time of the filter, in seconds. This can also be thought of as the delay time.",
                default = 0.1
            }
        },

        optional = {
            {
                name = "revtime",
                type = "SPFLOAT",
                description = "The reverberation time, in seconds (RT-60).",
                default = 3.5
            },
        }
    },

    modtype = "module",

    description = [[Allpass filter

    Often used for the creation of reverb modules.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["atone"] = {

	files = {
	    module = "atone.c",
	    header = "atone.h",
	    example = "ex_atone.c",
	},

	func = {
	    create = "sp_atone_create",
	    destroy = "sp_atone_destroy",
	    init = "sp_atone_init",
	    compute = "sp_atone_compute",
	},

	params = {
	    optional = {
	        {
	            name = "hp",
	            type = "SPFLOAT",
	            description = "The response curve's half power point (cutoff frequency).",
	            default = "1000"
	        },
	    },
	},

	modtype = "module",

	description = [[First-order recursive highpass filter

	This is the complement to the tone module.]],

	ninputs = 1,
	noutputs = 1,

	inputs = {
	    {
	        name = "in",
	        description = "Audio signal input."
	    },
	},

	outputs = {
	    {
	        name = "out",
	        description = "Audio signal output."
	    },
	}

}
sptbl["autowah"] = {

    files = {
        module = "autowah.c",
        header = "autowah.h",
        example = "ex_autowah.c",
    },

    func = {
        create = "sp_autowah_create",
        destroy = "sp_autowah_destroy",
        init = "sp_autowah_init",
        compute = "sp_autowah_compute",
    },

    params = {
        optional = {
            {
                name = "level",
                type = "SPFLOAT*",
                description = "Overall level (between 0 and 1)",
                default = 0.1
            },
            {
                name = "wah",
                type = "SPFLOAT*",
                description ="wah amount",
                default = 0
            },
            {
                name = "mix",
                type = "SPFLOAT*",
                description ="Wet/dry amount (100 = wet, 0 = dry)",
                default = 100
            },
        }
    },

    modtype = "module",

    description = [[Automatic wah pedal
	An automatic wah effect, ported from Guitarix via Faust. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Audio input"
        },
    },
    
    outputs = {
        {
            name = "output",
            description = "Audio output."
        },
    }

}
sptbl["bal"] = {

    files = {
        module = "bal.c",
        header = "bal.h",
        example = "ex_bal.c",
    },

    func = {
        create = "sp_bal_create",
        destroy = "sp_bal_destroy",
        init = "sp_bal_init",
        compute = "sp_bal_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Balance the gain of one signal based on another signal

    This is often used to restore gain lost in the output of a filter.

In the source code, the value "ihp" is set to a static 10hz. This is the default value in Csound, and should not often need to be changed.
]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "sig",
            description = "Input signal whose gain will be changed."
        },
        {
            name = "comp",
            description = "Secondary signal to compare the input signal to."
        },
    },

    outputs = {
        {
            name = "out",
            description = "The output signal with applied gain."
        },
    }

}
sptbl["bar"] = {

    files = {
        module = "bar.c",
        header = "bar.h",
        example = "ex_bar.c",
    },

    func = {
        create = "sp_bar_create",
        destroy = "sp_bar_destroy",
        init = "sp_bar_init",
        compute = "sp_bar_compute",
    },

    params = {
        mandatory = {
            {
                name = "iK",
                type = "SPFLOAT",
                description = "Dimensionless stiffness parameter",
                default = 3
            },
            {
                name = "ib",
                type = "SPFLOAT",
                description = "High-frequency loss parameter. Keep this small",
                default = 0.001
            }
        },

        optional = {
            {
                name = "bcL",
                type = "SPFLOAT",
                description = "Boundary condition at left end of bar. 1 = clamped, 2 = pivoting, 3 = free",
                default = 1
            },
            {
                name = "bcR",
                type = "SPFLOAT",
                description = "Boundary condition at right end of bar. 1 = clamped, 2 = pivoting, 3 = free",
                default = 1
            },
            {
                name = "T30",
                type = "SPFLOAT",
                description ="30db decay time (in seconds).",
                default = 3
            },
            {
                name = "scan",
                type = "SPFLOAT",
                description ="Speed of scanning the output location.",
                default = 0.25
            },
            {
                name = "pos",
                type = "SPFLOAT",
                description ="Position along bar that strike occurs.",
                default = 0.2
            },
            {
                name = "vel",
                type = "SPFLOAT",
                description ="Normalized strike velocity",
                default = 500
            },
            {
                name = "wid",
                type = "SPFLOAT",
                description ="Spatial width of strike.",
                default = 0.05
            },
        }
    },

    modtype = "module",

    description = [[Physical model approximating the sound of a struck metal bar]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Non-zero values will strike the bar."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["biquad"] = {

    files = {
        module = "biquad.c",
        header = "biquad.h",
        --example = "ex_tone.c",
    },

    func = {
        create = "sp_biquad_create",
        destroy = "sp_biquad_destroy",
        init = "sp_biquad_init",
        compute = "sp_biquad_compute",
    },

    params = {
        optional = {
            {
                name = "b0",
                type = "SPFLOAT",
                description = "biquad coefficient.",
                default = 0
            },
            {
                name = "b1",
                type = "SPFLOAT",
                description = "biquad coefficient.",
                default = 0
            },
            {
                name = "b2",
                type = "SPFLOAT",
                description = "biquad coefficient.",
                default = 0
            },
            {
                name = "a0",
                type = "SPFLOAT",
                description = "biquad coefficient.",
                default = 0
            },
            {
                name = "a1",
                type = "SPFLOAT",
                description = "biquad coefficient.",
                default = 0
            },
            {
                name = "a2",
                type = "SPFLOAT",
                description = "biquad coefficient.",
                default = 0
            }
        }
    },

    modtype = "module",

    description = [[A sweepable biquadratic general purpose filter

    ]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["biscale"] = {

    files = {
        module = "biscale.c",
        header = "biscale.h",
        example = "ex_biscale.c",
    },

    func = {
        create = "sp_biscale_create",
        destroy = "sp_biscale_destroy",
        init = "sp_biscale_init",
        compute = "sp_biscale_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum value to scale to.",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum value to scale to.",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[Bipolar Scale

    This module scales from bipolar [-1, 1] to another range defined by min and max.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal to be scaled."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Scaled signal out"
        },
    }

}
sptbl["bitcrush"] = {

    files = {
        module = "bitcrush.c",
        header = "bitcrush.h",
        example = "ex_bitcrush.c",
    },

    func = {
        create = "sp_bitcrush_create",
        destroy = "sp_bitcrush_destroy",
        init = "sp_bitcrush_init",
        compute = "sp_bitcrush_compute",
    },

    params = {
        optional = {
            {
                name = "bitdepth",
                type = "SPFLOAT",
                description = "Bit depth. Expects an integer in the range of 1-16. Fractional values will be truncated.",
                default = 8
            },
            {
                name = "srate",
                type = "SPFLOAT",
                description ="Sampling rate.",
                default = 10000
            },
        }
    },

    modtype = "module",

    description = [[Digital signal degradation

    Bitcrusher will digitally degrade a signal by altering the bitdepth and sampling-rate. ]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output"
        },
    }

}
sptbl["blsaw"] = {

    files = {
        module = "blsaw.c",
        header = "blsaw.h",
        example = "ex_blsaw.c",
    },

    func = {
        create = "sp_blsaw_create",
        destroy = "sp_blsaw_destroy",
        init = "sp_blsaw_init",
        compute = "sp_blsaw_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT*",
                description = "Frequency, (range 0-20000)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT*",
                description ="Amplitude (range 0-1).",
		default = 1.0
            },
        }
    },

    modtype = "module",

    description = [[Bandlimited sawtooth oscillator
This is a bandlimited sawtooth oscillator ported from the "sawtooth" function from the Faust
programming language.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["blsquare"] = {

    files = {
        module = "blsquare.c",
        header = "blsquare.h",
        example = "ex_blsquare.c",
    },

    func = {
        create = "sp_blsquare_create",
        destroy = "sp_blsquare_destroy",
        init = "sp_blsquare_init",
        compute = "sp_blsquare_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT*",
                description = "Frequency, (range 0-20000)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT*",
                description ="Amplitude (range 0-1).",
		default = 1.0
            },
            {
                name = "width",
                type = "SPFLOAT*",
                description ="Duty cycle width (range 0-1).",
		default = 0.5
            },
        }
    },

    modtype = "module",

    description = [[Bandlimited square oscillator
This is a bandlimited square oscillator ported from the "squaretooth" function from the Faust
programming language.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["bltriangle"] = {

    files = {
        module = "bltriangle.c",
        header = "bltriangle.h",
        example = "ex_bltriangle.c",
    },

    func = {
        create = "sp_bltriangle_create",
        destroy = "sp_bltriangle_destroy",
        init = "sp_bltriangle_init",
        compute = "sp_bltriangle_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT*",
                description = "Frequency, (range 0-20000)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT*",
                description ="Amplitude (range 0-1).",
		default = 1.0
            },
        }
    },

    modtype = "module",

    description = [[Bandlimited triangle oscillator
This is a bandlimited triangle oscillator ported from the "triangletooth" function from the Faust
programming language.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["brown"] = {

    files = {
        module = "brown.c",
        header = "brown.h",
        example = "ex_brown.c",
    },

    func = {
        create = "sp_brown_create",
        destroy = "sp_brown_destroy",
        init = "sp_brown_init",
        compute = "sp_brown_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Brownian noise generator.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Brownian noise output."
        },
    }

}
sptbl["butbp"] = {

    files = {
        module = "butbp.c",
        header = "butbp.h",
        example = "ex_butbp.c",
    },

    func = {
        create = "sp_butbp_create",
        destroy = "sp_butbp_destroy",
        init = "sp_butbp_init",
        compute = "sp_butbp_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Center Frequency. (in Hertz)",
                default = 1000
            },
            {
                name = "bw",
                type = "SPFLOAT",
                description = "Bandwidth. (in Hertz)",
                default = 10
            },
        }
    },

    modtype = "module",

    description = [[Bandpass Butterworth filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Input signal that the filter should be applied to."
        },
    },

    outputs = {
        {
            name = "output",
            description = "Output signal (input modified by the filter)."
        },
    }

}
sptbl["butbr"] = {

    files = {
        module = "butbr.c",
        header = "butbr.h",
        example = "ex_butbr.c",
    },

    func = {
        create = "sp_butbr_create",
        destroy = "sp_butbr_destroy",
        init = "sp_butbr_init",
        compute = "sp_butbr_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Center Frequency. (in Hertz)",
                default = 1000
            },
            {
                name = "bw",
                type = "SPFLOAT",
                description = "Bandwidth. (in Hertz)",
                default = 1000
            },
        }
    },

    modtype = "module",

    description = [[Band-reject Butterworth filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "output",
            description = "Signal output."
        },
    }

}
sptbl["buthp"] = {

    files = {
        module = "buthp.c",
        header = "buthp.h",
        example = "ex_buthp.c",
    },

    func = {
        create = "sp_buthp_create",
        destroy = "sp_buthp_destroy",
        init = "sp_buthp_init",
        compute = "sp_buthp_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Cutoff Frequency.",
                default = 1000
            },
        }
    },

    modtype = "module",

    description = [[Highpass Butterworth filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "output",
            description = "Signal output."
        },
    }

}
sptbl["butlp"] = {

    files = {
        module = "butlp.c",
        header = "butlp.h",
        example = "ex_butlp.c",
    },

    func = {
        create = "sp_butlp_create",
        destroy = "sp_butlp_destroy",
        init = "sp_butlp_init",
        compute = "sp_butlp_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Cutoff Frequency.",
                default = 1000
            },
        }
    },

    modtype = "module",

    description = [[Lowpass Butterworth filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "output",
            description = "Signal output."
        },
    }

}
sptbl["clip"] = {

    files = {
        module = "clip.c",
        header = "clip.h",
        example = "ex_clip.c",
    },

    func = {
        create = "sp_clip_create",
        destroy = "sp_clip_destroy",
        init = "sp_clip_init",
        compute = "sp_clip_compute",
    },

    params = {
        optional = {
            {
                name = "lim",
                type = "SPFLOAT",
                description = "threshold / limiting value.",
                default = 1.0
            },
        }
    },

    modtype = "module",

    description = [[Applies clip-limiting to a signal]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Input signal."
        }
    },

    outputs = {
        {
            name = "out",
            description = "Output signal."
        }
    }

}
sptbl["clock"] = {

    files = {
        module = "clock.c",
        header = "clock.h",
        example = "ex_clock.c",
    },

    func = {
        create = "sp_clock_create",
        destroy = "sp_clock_destroy",
        init = "sp_clock_init",
        compute = "sp_clock_compute",
    },

    params = {

        optional = {
            {
                name = "bpm",
                type = "SPFLOAT",
                description = "Clock tempo, in beats per minute.",
                default = 120
            },
            {
                name = "subdiv",
                type = "SPFLOAT",
                description ="Clock subdivision. 2 = eighths, 4 = 16ths, etc.",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[Resettable clock with subdivisions
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When non-zero, will reset clock"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Clock output."
        },
    }

}
sptbl["comb"] = {

    files = {
        module = "comb.c",
        header = "comb.h",
        example = "ex_comb.c",
    },

    func = {
        create = "sp_comb_create",
        destroy = "sp_comb_destroy",
        init = "sp_comb_init",
        compute = "sp_comb_compute",
    },

    params = {
        mandatory = {
            {
                name = "looptime",
                type = "SPFLOAT",
                description = "The loop time of the filter, in seconds. This can also be thought of as the delay time.",
                default = 0.1
            }
        },

        optional = {
            {
                name = "revtime",
                type = "SPFLOAT",
                description = "Reverberation time, in seconds (RT-60).",
                default = 3.5
            },
        }
    },

    modtype = "module",

    description = [[Comb filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["compressor"] = {

    files = {
        module = "compressor.c",
        header = "compressor.h",
        example = "ex_compressor.c",
    },

    func = {
        create = "sp_compressor_create",
        destroy = "sp_compressor_destroy",
        init = "sp_compressor_init",
        compute = "sp_compressor_compute",
    },

    params = {
        mandatory = {
        },

        optional = {
            {
                name = "ratio",
                type = "SPFLOAT*",
                description = "Ratio to compress with, a value > 1 will compress",
                default = 1
            },
            {
                name = "thresh",
                type = "SPFLOAT*",
                description ="Threshold (in dB) 0 = max",
                default = 0
            },
            {
                name = "atk",
                type = "SPFLOAT*",
                description ="Compressor attack",
                default = 0.1
            },
            {
                name = "rel",
                type = "SPFLOAT*",
                description ="Compressor release",
                default = 0.1
            },
        }
    },

    modtype = "module",

    description = [[Compressor]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["conv"] = {

    files = {
        module = "conv.c",
        header = "conv.h",
        example = "ex_conv.c",
    },

    func = {
        create = "sp_conv_create",
        destroy = "sp_conv_destroy",
        init = "sp_conv_init",
        compute = "sp_conv_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "Ftable used as the impulse response. ",
                default = "N/A"
            },
            {
                name = "iPartLen",
                type = "SPFLOAT",
                description = [[Partition length (in samples). 
Must be a power of 2. Lower values will add less latency, at the cost 
of requiring more CPU power. 
]],
                default = 2048
            }
        },

    },

    modtype = "module",

    description = [[Partioned convolution. 
This module will perform partitioned convolution on an input signal using
an ftable as an impulse response.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input to be convolved."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["count"] = {

    files = {
        module = "count.c",
        header = "count.h",
        example = "ex_count.c",
    },

    func = {
        create = "sp_count_create",
        destroy = "sp_count_destroy",
        init = "sp_count_init",
        compute = "sp_count_compute",
    },

    params = {
        optional = {
            {
                name = "count",
                type = "SPFLOAT",
                description = "Number to count up to (count - 1). Decimal points will be truncated.",
                default = 4
            },
            {
                name = "mode",
                type = "SPFLOAT",
                description = "Counting mode. 0 = wrap-around, 1 = count up to N -1, then stop and spit out -1",
                default = 0
            },
        },
    },

    modtype = "module",

    description = [[Trigger-based fixed counter

    The signal output will count from 0 to [N-1], and then
repeat itself. Count will start when it has been triggered, otherwise it will be -1.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When non-zero, this value will increment."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["crossfade"] = {

    files = {
        module = "crossfade.c",
        header = "crossfade.h",
        example = "ex_crossfade.c",
    },

    func = {
        create = "sp_crossfade_create",
        destroy = "sp_crossfade_destroy",
        init = "sp_crossfade_init",
        compute = "sp_crossfade_compute",
    },

    params = {
        mandatory = {
        },

        optional = {
            {
                name = "pos",
                type = "SPFLOAT",
                description = "Crossfade position. 0 = all signal 1, 1 = all signal 2",
                default = 0.5
            },
        }
    },

    modtype = "module",

    description = [[Crossfade two signals. 
This module will perform a linear crossfade between two input signals.
]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "in1",
            description = "Input signal 1."
        },
        {
            name = "in2",
            description = "Input signal 2."
        },
    },

    outputs = {
        {
            name = "out",
            description = "signal output."
        },
    }

}
sptbl["dcblock"] = {

    files = {
        module = "dcblock.c",
        header = "dcblock.h",
    },

    func = {
        create = "sp_dcblock_create",
        destroy = "sp_dcblock_destroy",
        init = "sp_dcblock_init",
        compute = "sp_dcblock_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[A simple DC block filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output"
        },
    }

}
sptbl["delay"] = {

    files = {
        module = "delay.c",
        header = "delay.h",
        example = "ex_delay.c",
    },

    func = {
        create = "sp_delay_create",
        destroy = "sp_delay_destroy",
        init = "sp_delay_init",
        compute = "sp_delay_compute",
    },

    params = {
        mandatory = {
            {
                name = "time",
                type = "SPFLOAT",
                description = "Delay time, in seconds.",
                default = 1.0
            }
        },

        optional = {
            {
                name = "feedback",
                type = "SPFLOAT",
                description = "Feedback amount. Should be a value between 0-1.",
                default = 0.0
            }
        }

    },

    modtype = "module",

    description = [[Adds a delay to an incoming signal with optional feedback.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["diode"] = {

    files = {
        module = "diode.c",
        header = "diode.h",
        example = "ex_diode.c",
    },

    func = {
        create = "sp_diode_create",
        destroy = "sp_diode_destroy",
        init = "sp_diode_init",
        compute = "sp_diode_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "",
                default = 1000
            },
            {
                name = "res",
                type = "SPFLOAT",
                description ="",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Diode-ladder virtual analogue low-pass filter
This is a diode-ladder filter designed by Will Pirkle. 
More information can be found in this paper here: 
http://www.willpirkle.com/Downloads/AN-6DiodeLadderFilter.pdf
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out_left",
            description = "Stereo left output for diode."
        },
        {
            name = "out_right",
            description = "Stereo right output for diode."
        },
    }

}
sptbl["diskin"] = {

    files = {
        module = "diskin.c",
        header = "diskin.h",
        example = "ex_diskin.c",
    },

    func = {
        create = "sp_diskin_create",
        destroy = "sp_diskin_destroy",
        init = "sp_diskin_init",
        compute = "sp_diskin_compute",
    },

    params = {
        mandatory = {
            {
                name = "filename",
                type = "const char *",
                description = "Filename of the audio file.",
                default = "N/A"
            },
        },
    },

    modtype = "module",

    description = [[Read from an audio file

    Expects a 1-channel file matching the project samplerate. Diskin should be able to read any file format that libsndfile supports.]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["dist"] = {

    files = {
        module = "dist.c",
        header = "dist.h",
        example = "ex_dist.c",
    },

    func = {
        create = "sp_dist_create",
        destroy = "sp_dist_destroy",
        init = "sp_dist_init",
        compute = "sp_dist_compute",
    },

    params = {
        optional = {
            {
                name = "pregain",
                type = "SPFLOAT",
                description ="Gain applied before waveshaping.",
                default = 2.0
            },
            {
                name = "postgain",
                type = "SPFLOAT",
                description ="Gain applied after waveshaping",
                default = 0.5
            },
            {
                name = "shape1",
                type = "SPFLOAT",
                description ="Shape of the positive part of the signal. A value of 0 gets a flat clip.",
                default = 0
            },
            {
                name = "shape2",
                type = "SPFLOAT",
                description ="Like shape1, only for the negative part of the signal.",
                default = 0
            },

        }
    },

    modtype = "module",

    description = [[Distortion using a modified hyperbolic tangent function]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        }
    }

}
sptbl["dmetro"] = {

    files = {
        module = "dmetro.c",
        header = "dmetro.h",
        example = "ex_dmetro.c",
    },

    func = {
        create = "sp_dmetro_create",
        destroy = "sp_dmetro_destroy",
        init = "sp_dmetro_init",
        compute = "sp_dmetro_compute",
    },

    params = {
        optional = {
            {
                name = "time",
                type = "SPFLOAT",
                description ="Time between triggers (in seconds). This will update at the start of each trigger.",
                default = 1.0
            },
        }
    },

    modtype = "module",

    description = [[Delta Metro

    Produce a set of triggers spaced apart by time.]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "this doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Trigger output."
        },
    }

}
sptbl["drip"] = {

    files = {
        module = "drip.c",
        header = "drip.h",
        example = "ex_drip.c",
    },

    func = {
        create = "sp_drip_create",
        destroy = "sp_drip_destroy",
        init = "sp_drip_init",
        compute = "sp_drip_compute",
    },

    params = {
        mandatory = {
            {
                name = "dettack",
                type = "SPFLOAT",
                description = "Period of time over which all sound is stopped.",
                default = 0.09
            },
        },

        optional = {
            {
                name = "num_tubes",
                type = "SPFLOAT",
                description = "Number of units.",
                default = 10
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description = "Amplitude.",
                default = 0.3
            },
            {
                name = "damp",
                type = "SPFLOAT",
                description ="The damping factor. Maximum value is 2.0.",
                default = 0.2
            },
            {
                name = "shake_max",
                type = "SPFLOAT",
                description = "The amount of energy to add back into the system.",
                default = 0
            },
            {
                name = "freq",
                type = "SPFLOAT",
                description ="Main resonant frequency.",
                default = 450
            },
            {
                name = "freq1",
                type = "SPFLOAT",
                description ="The first resonant frequency.",
                default = 600
            },
            {
                name = "freq2",
                type = "SPFLOAT",
                description ="The second resonant frequency.",
                default = 750
            },
        }
    },

    modtype = "module",

    description = [[Water drop physical model

    Physical model of the sound of dripping water. When triggered, it will produce a droplet of water.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Trigger value. When non-zero, it will re-init the drip and create a drip sound."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Stereo left output for drip."
        },
    }

}
sptbl["dtrig"] = {

    files = {
        module = "dtrig.c",
        header = "dtrig.h",
        example = "ex_dtrig.c",
    },

    func = {
        create = "sp_dtrig_create",
        destroy = "sp_dtrig_destroy",
        init = "sp_dtrig_init",
        compute = "sp_dtrig_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "An ftable containing times in seconds.",
                default = "N/A"
            }
        },

        optional = {
            {
                name = "loop",
                type = "int",
                description = "When set to 1, dtrig will wrap around and loop again.",
                default = 0
            },
            {
                name = "delay",
                type = "SPFLOAT",
                description = "This sets a delay (in seconds) on the triggered output when it is initially triggered. This is useful for rhythmic sequences with rests in the beginnings.",
                default = 0
            },
            {
                name = "scale",
                type = "SPFLOAT",
                description = "Scales the timing signal. A scale of 1 is normal, a scale of 2 will double the duration, and a scale of 0.5 will halve it.",
                default = 0
            }
        }
    },

    modtype = "module",

    description = [[ Delta trigger

    This is able to create spaced out triggers. It is set off by a single trigger.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "trigger input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "These are the triggered outputs."
        },
    }

}
sptbl["dust"] = {

    files = {
        module = "dust.c",
        header = "dust.h",
        example = "ex_dust.c",
    },

    func = {
        create = "sp_dust_create",
        destroy = "sp_dust_destroy",
        init = "sp_dust_init",
        compute = "sp_dust_compute",
    },

    params = {
        optional = {
            {
                name = "amp",
                type = "SPFLOAT",
                description = "",
                default = 0.3
            },
            {
                name = "density",
                type = "SPFLOAT",
                description = "",
                default = 10
            },
            {
                name = "bipolar",
                type = "int",
                description = "Bipolar flag. A non-zero makes the signal bipolar as opposed to unipolar. ",
                default = 0
            }
        }
    },

    modtype = "module",

    description = [[A series of random impulses]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["eqfil"] = {

    files = {
        module = "eqfil.c",
        header = "eqfil.h",
        example = "ex_eqfil.c",
    },

    func = {
        create = "sp_eqfil_create",
        destroy = "sp_eqfil_destroy",
        init = "sp_eqfil_init",
        compute = "sp_eqfil_compute",
    },

    params = {

        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "The center frequency of the filter",
                default = 1000
            },
            {
                name = "bw",
                type = "SPFLOAT",
                description ="The peak/notch bandwidth in Hertz",
                default = 125
            },
            {
                name = "gain",
                type = "SPFLOAT",
                description ="The peak/notch gain",
                default = 2
            },
        }
    },

    modtype = "module",

    description = [[2nd order tunable equalization filter

    This provides a peak/notch filter for building parametric/graphic equalizers. With gain above 1, there will be a peak at the center frequency with a width dependent on bw. If gain is less than 1, a notch is formed around the center frequency (freq).
    ]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "output",
            description = "Signal output."
        },
    }

}
sptbl["expon"] = {

    files = {
        module = "expon.c",
        header = "expon.h",
        example = "ex_expon.c",
    },

    func = {
        create = "sp_expon_create",
        destroy = "sp_expon_destroy",
        init = "sp_expon_init",
        compute = "sp_expon_compute",
    },

    params = {
        optional = {
            {
                name = "a",
                type = "SPFLOAT",
                description = "Inital point.",
                default = 1.0
            },
            {
                name = "dur",
                type = "SPFLOAT",
                description = "Duration (in seconds)",
                default = 1.0
            },
            {
                name = "b",
                type = "SPFLOAT",
                description = "End point",
                default = 1.0
            }
        },

    },

    modtype = "module",

    description = [[Produce a line segment with exponential slope
This will generate a line from value A to value B in given amount of time. 
When it reaches it's target, it will stay at that value. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When nonzero, will retrigger line segment"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["fof"] = {

    files = {
        module = "fof.c",
        header = "fof.h",
        example = "ex_fof.c",
    },

    func = {
        create = "sp_fof_create",
        destroy = "sp_fof_destroy",
        init = "sp_fof_init",
        compute = "sp_fof_compute",
    },

    params = {
        mandatory = {
            {
                name = "sine",
                type = "sp_ftbl *",
                description = "ftable for sine wave.",
                default = "N/A"
            },
            {
                name = "win",
                type = "sp_ftbl *",
                description = "Ftable for envelope function (use either gen_line or gen_sinecomp)",
                default = "N/A"
            },
            {
                name = "iolaps",
                type = "int",
                description = "Maximum number of foflet overlaps.",
                default = 100 
            },
            {
                name = "iphs",
                type = "SPFLOAT",
                description = "Phase",
                default = 0
            }
        },

        optional = {
            {
                name = "amp",
                type = "SPFLOAT",
                description = "Overall amplitude",
                default = 0.5
            },
            {
                name = "fund",
                type = "SPFLOAT",
                description = "Fundamental frequency",
                default = 100
            },
            {
                name = "form",
                type = "SPFLOAT",
                description = "Formant frequency.",
                default = 500
            },
            {
                name = "oct",
                type = "SPFLOAT",
                description = "Octaviation index, if greater than zero, lowers the effective fund frequency by attenuating odd-numbered sine bursts. whole numbers are full octaves. fractions transpositional.",
                default = 0
            },
            {
                name = "band",
                type = "SPFLOAT",
                description = "Bandwidth (in -6db) expressed in Hz. The bandwidth determines the rate of exponential decay throughout the sineburst, before the enveloping is applied.",
                default = 50
            },
            {
                name = "ris",
                type = "SPFLOAT",
                description = "Rise of sinusoid burst (in seconds)",
                default = 0.003
            },
            {
                name = "dec",
                type = "SPFLOAT",
                description = "Decay of the sinusoid burst (in seconds).",
                default = 0.0007
            },
            {
                name = "dur",
                type = "SPFLOAT",
                description = "OVerall duration of sinusoid burst (in seconds).",
                default = 0.02
            },
        }
    },

    modtype = "module",

    description = [[Produces sinusoid bursts for granular and formant synthesis

]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["fofilt"] = {

    files = {
        module = "fofilt.c",
        header = "fofilt.h",
        example = "ex_fofilt.c",
    },

    func = {
        create = "sp_fofilt_create",
        destroy = "sp_fofilt_destroy",
        init = "sp_fofilt_init",
        compute = "sp_fofilt_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Center frequency.",
                default = 1000
            },
            {
                name = "atk",
                type = "SPFLOAT",
                description ="Impulse response attack time (in seconds).",
                default = 0.007
            },
            {
                name = "dec",
                type = "SPFLOAT",
                description ="Impulse reponse decay time (in seconds)",
                default = 0.04
            },
        }
    },

    modtype = "module",

    description = [[Formant filter
    When fed with a pulse train, it will generate a series of overlapping grains. Overlapping will occur when 1/freq < dec, but there is no upper limit on the number of overlaps. (cited from www.csounds.com/manual/html/fofilter.html)]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output"
        },
    }

}
sptbl["fog"] = {

    files = {
        module = "fog.c",
        header = "fog.h",
        example = "ex_fog.c",
    },

    func = {
        create = "sp_fog_create",
        destroy = "sp_fog_destroy",
        init = "sp_fog_init",
        compute = "sp_fog_compute",
    },

    params = {
        mandatory = {
            {
                name = "wav",
                type = "sp_ftbl *",
                description = "ftable for sample.",
                default = "N/A"
            },
            {
                name = "win",
                type = "sp_ftbl *",
                description = "Ftable for envelope function (use either gen_line or gen_sinecomp)",
                default = "N/A"
            },
            {
                name = "iolaps",
                type = "int",
                description = "Maximum number of foglet overlaps.",
                default = 100 
            },
            {
                name = "iphs",
                type = "SPFLOAT",
                description = "Phase",
                default = 0
            }
        },

        optional = {
            {
                name = "amp",
                type = "SPFLOAT",
                description = "Overall amplitude",
                default = 0.5
            },
            {
                name = "dens",
                type = "SPFLOAT",
                description = "Density. The frequency of grains/second.",
                default = 80 
            },
            {
                name = "trans",
                type = "SPFLOAT",
                description = "Transposition, in terms of playback speed",
                default = 1
            },
            {
                name = "spd",
                type = "SPFLOAT",
                description = "Starting sample index, normalized 0-1.",
                default = 0
            },
            {
                name = "oct",
                type = "SPFLOAT",
                description = "Octaviation index, if greater than zero, lowers the effective fund frequency by attenuating odd-numbered sine bursts. whole numbers are full octaves. fractions transpositional.",
                default = 0
            },
            {
                name = "band",
                type = "SPFLOAT",
                description = "Bandwidth (in -6db) expressed in Hz. The bandwidth determines the rate of exponential decay throughout the sineburst, before the enveloping is applied.",
                default = 50
            },
            {
                name = "ris",
                type = "SPFLOAT",
                description = "Rise of sinusoid burst (in seconds)",
                default = 0.003
            },
            {
                name = "dec",
                type = "SPFLOAT",
                description = "Decay of the sinusoid burst (in seconds).",
                default = 0.0007
            },
            {
                name = "dur",
                type = "SPFLOAT",
                description = "OVerall duration of sinusoid burst (in seconds).",
                default = 0.02
            },
        }
    },

    modtype = "module",

    description = [[Sucession of grains from data in a stored function table]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["fold"] = {

    files = {
        module = "fold.c",
        header = "fold.h",
        example = "ex_fold.c",
    },

    func = {
        create = "sp_fold_create",
        destroy = "sp_fold_destroy",
        init = "sp_fold_init",
        compute = "sp_fold_compute",
    },

    params = {
        optional = {
            {
                name = "incr",
                type = "SPFLOAT",
                description = "Increment",
                default = 1000
            },
        }
    },

    modtype = "module",

    description = [[Adds artificial foldover to an audio signal

    Primarily created for use with Decimator.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["foo"] = {

    files = {
        module = "foo.c",
        header = "foo.h",
        example = "ex_foo.c",
    },

    func = {
        create = "sp_foo_create",
        destroy = "sp_foo_destroy",
        init = "sp_foo_init",
        compute = "sp_foo_compute",
        other = {
            sp_foo_set = {
                description = "foo_set description goes here.",
                args = {
                    {
                        name = "var1",
                        type = "SPFLOAT",
                        description = "This is the first parameter",
                        default = 1.5
                    },
                    {
                        name = "var2",
                        type = "SPFLOAT",
                        description = "This is the second parameter",
                        default = 1.5
                    }
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "bar",
                type = "sp_ftbl *",
                description = "This is a mandatory table value. It does not have a default value, so we set it to 'N/A'. Any that does not or cannot have a default value should set this default value to 'N/A'.",
                default = "N/A"
            },
            {
                name = "bar2",
                type = "SPFLOAT",
                description = "This is a mandatory parameter. In soundpipe, users will always need to specify this value, but a default value has been giving in case it is needed to write more complicated engines in the future.",
                default = 123
            }
        },

        optional = {
            {
                name = "blah_1",
                type = "SPFLOAT",
                description = "This is an optional parameter. These values are always set to a value by default, and can be set after the init function has been called.",
                default = 1.5
            },
            {
                name = "blah_2",
                type = "SPFLOAT",
                description ="This is yet another optional parameter...",
                default = 456.7
            },
        }
    },

    modtype = "module",

    description = [[A short title describing the module

    This is a description of the entire module.  This is not a real module. This description should be a comprehensive sumary of what this function does.

Inside the Lua table, this is expressed as a multiline string, however it does not adhere to the tradtional 80 column rule found in programming.

Write as much text as needed here...
]],

    ninputs = 2,
    noutputs = 2,

    inputs = {
        {
            name = "clock",
            description = "this is the clock source for a made up plugin."
        },
        {
            name = "input",
            description = "this is the audio input for a made up plugin."
        },
    },

    outputs = {
        {
            name = "out_left",
            description = "Stereo left output for foo."
        },
        {
            name = "out_right",
            description = "Stereo right output for foo."
        },
    }

}
sptbl["fosc"] = {

    files = {
        module = "fosc.c",
        header = "fosc.h",
        example = "ex_fosc.c",
    },

    func = {
        create = "sp_fosc_create",
        destroy = "sp_fosc_destroy",
        init = "sp_fosc_init",
        compute = "sp_fosc_compute",
    },

    params = {
        mandatory = {
            {
                name = "tbl",
                type = "sp_ftbl *",
                description = "Wavetable to read from. Note: the size of this table must be a power of 2.",
                default = "N/A"
            },
        },
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency (in Hz)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude (typically a value between 0 and 1).",
                default = 0.4
            },
            {
                name = "car",
                type = "SPFLOAT",
                description ="Carrier frequency, expressed as a ratio number in C:M ratio. Typically an integer.",
                default = 1
            },
            {
                name = "mod",
                type = "SPFLOAT",
                description ="Modulator frequency, expressed as a ratio number in C:M ratio. Typically an integer.",
                default = 1
            },
            {
                name = "indx",
                type = "SPFLOAT",
                description ="Modulation index. Most commercial synthesizers have a range from 0-8, but there's nothing stopping you from going beyond that.",
                default = 8
            },
        }
    },

    modtype = "module",

    description = [[FM oscilator pair with linear interpolation]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This does nothing."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["gbuzz"] = {

    files = {
        module = "gbuzz.c",
        header = "gbuzz.h",
        example = "ex_gbuzz.c",
    },

    func = {
        create = "sp_gbuzz_create",
        destroy = "sp_gbuzz_destroy",
        init = "sp_gbuzz_init",
        compute = "sp_gbuzz_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "Soundpipe function table used internally. This should be a sine wave.",
                default = "N/A"
            },

            {
                name = "iphs",
                type = "SPFLOAT",
                description ="Phase to start on (in the range 0-1)",
                default = 0
            },
        },

        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency, in Hertz.",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude (Typically a value between 0 and 1).",
                default = 0.4
            },
            {
                name = "nharm",
                type = "SPFLOAT",
                description ="Number of harmonics.",
                default = 4
            },
            {
                name = "lharm",
                type = "SPFLOAT",
                description ="Lowest harmonic present. This should be a whole number integer.",
                default = 0
            },
            {
                name = "mul",
                type = "SPFLOAT",
                description ="Multiplier. This determines the relative strength of each harmonic.",
                default = 0.1
            },
        }
    },

    modtype = "module",

    description = [[Series of partials from the harmonic series

    GBuzz comes from the "buzz" family of Csound opcodes, and is capable of producing a rich spectrum of harmonic content, useful for subtractive synthesis implementation.]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output"
        },
    }

}
sptbl["gen_composite"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_composite.c",
    },

    func = {
        name = "sp_gen_composite",
    },

    params = {
        {
            name = "argstring",
            type = "char *",
            description = [[
a string of space-separated parameters, in groups of four:

arg 1 is the partial number. must be positive, but it doesn't need to be a whole number.

arg 2 is the strength.

arg 3 is the initial phase (expressed in degrees)

arg 4 is the dc offset. A dc offset of 2 will put a 2-strength sinusoid in the range
from (-2,2) to (0, 4)
]],
            default = "0.5 0.5 270 0.5"
        },
    },

    modtype = "gen",

    description = [[Generate a composite waveform of sinusoids]],

}
sptbl["gen_file"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        --example = "ex_gen_file.c",
    },

    func = {
        name = "sp_gen_file",
    },

    params = {
        {
            name = "filename",
            type = "const char *",
            description = [[filename]],
            default = "file.wav"
        },
    },

    modtype = "gen",

    description = [[Reads from a wav file

    This will only load as many samples as the length of the ftable.]],

}
sptbl["gen_gauss"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_gauss.c",
    },

    func = {
        name = "sp_gen_gauss",
    },

    params = {
        {
            name = "scale",
            type = "SPFLOAT",
            description = [[The scale of the distribution, in the range of -/+scale]],
            default = 123456
        },
        {
            name = "seed",
            type = "uint32_t",
            description = [[Random seed.]],
            default = 123456
        },
    },

    modtype = "gen",

    description = [[Gaussian distribution]],

}
sptbl["gen_line"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_line.c",
    },

    func = {
        name = "sp_gen_line",
    },

    params = {
        {
            name = "argstring",
            type = "char *",
            description = [[A list of ordered xy pairs. X expects whole number integers, as they correlate to index positions in the ftable.]],
            default = "0 0 4096 1"
        },
    },

    modtype = "gen",

    description = [[A series of line segments]],

}
sptbl["gen_padsynth"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "extra/ex_padsynth.c",
    },

    func = {
        name = "sp_gen_padsynth",
    },

    params = {
        {
            name = "amps",
            type = "sp_ftbl *",
            description = [[ftable of amplitudes to use]],
            default = "N/A"
        },
        {
            name = "f",
            type = "SPFLOAT",
            description = [[Base frequency.]],
            default = 440.0
        },
        {
            name = "bw",
            type = "SPFLOAT",
            description = [[Bandwidth.]],
            default = 40.0
        },
    },

    modtype = "gen",

    description = [[An implementation of the Padsynth Algorithm by Paul Nasca. 

This is a basic implemenation of the padsynth algorithm. More information can be found here:

http://zynaddsubfx.sourceforge.net/doc/PADsynth/PADsynth.htm

This gen routine requires libfftw, and is not compiled by default. See config.mk for more info.
    
    ]],

}
sptbl["gen_rand"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_rand.c",
    },

    func = {
        name = "sp_gen_rand",
    },

    params = {
        {
            name = "argstring",
            type = "char *",
            description = [[A string of value pairs. The first value is the value, then the probability.]],
            default = "1 0.5 2 0.5"
        },
    },

    modtype = "gen",

    description = [[Generates a user defined random number distribution.]],

}
sptbl["gen_scrambler"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_scrambler.c",
    },

    func = {
        name = "sp_gen_scrambler",
    },
    params = {
        {
            name = "dest",
            type = "&sp_ftbl *",
            description = [[destination ftable]],
            default = 123456
        },
    },

    modtype = "gen",

    description = [[Scrambles phase of ftable.
This gen routine will copy the ftable, apply an FFT, apply
a random phase, and then do an inverse FFT. This effect 
is ideal for creating pad-like sounds. 
]],

}
sptbl["gen_sine"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_sine.c",
    },

    func = {
        name = "sp_gen_sine",
    },

    modtype = "gen",

    description = [[generates a sampled sinusoid]],

}
sptbl["gen_sinesum"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_sinesum.c",
    },

    func = {
        name = "sp_gen_sinesum",
    },

    params = {
        {
            name = "argstring",
            type = "char *",
            description = [[A list of amplitudes, in the range 0-1, separated by spaces.Each position coordinates to their partial number. Position 1 is the fundamental amplitude (1 * freq). Position 2 is the first overtone (2 * freq), 3 is the second (3 * freq), etc...]],
            default = "1 0.5 0.25"
        },
    },

    modtype = "gen",

    description = [[Waveform as a sum of harmonically related sine waves ]],

}
sptbl["gen_triangle"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_triangle.c",
    },

    func = {
        name = "sp_gen_triangle",
    },

    modtype = "gen",

    description = [[generates a sampled triangle wave]],

}
sptbl["gen_xline"] = {

    files = {
        module = "ftbl.c",
        header = "ftbl.h",
        example = "ex_gen_xline.c",
    },

    func = {
        name = "sp_gen_xline",
    },

    params = {
        {
            name = "argstring",
            type = "char *",
            description = [[A list of ordered xy pairs. X expects whole number integers, as they correlate to index positions in the ftable.]],
            default = "0 0 4096 1"
        },
    },

    modtype = "gen",

    description = [[A series of exponential segments]],

}
sptbl["hilbert"] = {

    files = {
        module = "hilbert.c",
        header = "hilbert.h",
        example = "ex_hilbert.c",
    },

    func = {
        create = "sp_hilbert_create",
        destroy = "sp_hilbert_destroy",
        init = "sp_hilbert_init",
        compute = "sp_hilbert_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Hilbert transform

]],

    ninputs = 1,
    noutputs = 2,

    inputs = {
        {
            name = "input",
            description = "Input signal."
        },
    },

    outputs = {
        {
            name = "out1",
            description = "Sine output."
        },
        {
            name = "out2",
            description = "Cosine output."
        },
    }

}
sptbl["in"] = {

    files = {
        module = "in.c",
        header = "in.h",
        example = "ex_in.c",
    },

    func = {
        create = "sp_in_create",
        destroy = "sp_in_destroy",
        init = "sp_in_init",
        compute = "sp_in_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Reads from standard input.

    Expects type of SPFLOAT, which by default is a float. If the input data is larger than the number of samples, you will get a complaint about a broken pipe (but it will still work). If there is no input data from STDIN, it will hang.




The expected use case of sp_in is to utilize pipes from the commandline, like so:




cat /dev/urandom | ./my_program




Assuming my_program is using sp_in, this will write /dev/urandom (essentially white noise) to an audio file.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["incr"] = {

    files = {
        module = "incr.c",
        header = "incr.h",
        example = "ex_incr.c",
    },

    func = {
        create = "sp_incr_create",
        destroy = "sp_incr_destroy",
        init = "sp_incr_init",
        compute = "sp_incr_compute",
    },

    params = {
        mandatory = {
            {
                name = "val",
                type = "SPFLOAT",
                description = "Initial value",
                default = 0.5
            },
        },
        optional = {
            {
                name = "step",
                type = "SPFLOAT",
                description = "Step value to increment by.",
                default = 0.1
            },
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum value",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description = "Maximum value",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[Trigger-based Incrementor
When triggered, this module will increment and decrement a value bounded between a min
and max. Initially, this was designed for the specific use case of interfacing with the
griffin knob. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Trigger signal. When positive, the value will increase. When negative, the value will decrease."
        },
    },

    outputs = {
        {
            name = "out",
            description = "incr output."
        },
    }

}
sptbl["jcrev"] = {

    files = {
        module = "jcrev.c",
        header = "jcrev.h",
        example = "ex_jcrev.c",
    },

    func = {
        create = "sp_jcrev_create",
        destroy = "sp_jcrev_destroy",
        init = "sp_jcrev_init",
        compute = "sp_jcrev_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[John Chowning reverberator

    This is was built using the JC reverb implentation found in FAUST. According to the source code, the specifications for
this implementation were found on an old SAIL DART backup tape.

  This class is derived from the CLM JCRev function, which is based on the use of
  networks of simple allpass and comb delay filters.  This class implements three series
  allpass units, followed by four parallel comb filters, and two decorrelation delay lines in
  parallel at the output.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal Output."
        },
    }

}
sptbl["jitter"] = {

    files = {
        module = "jitter.c",
        header = "jitter.h",
        example = "ex_jitter.c",
    },

    func = {
        create = "sp_jitter_create",
        destroy = "sp_jitter_destroy",
        init = "sp_jitter_init",
        compute = "sp_jitter_compute",
    },

    params = {
        optional = {
            {
                name = "amp",
                type = "SPFLOAT",
                description ="The amplitude of the line. Will produce values in the range of (+/-)amp.",
                default = 0.5
            },
            {
                name = "cpsMin",
                type = "SPFLOAT",
                description = "The minimum frequency of change in Hz.",
                default = 0.5
            },
            {
                name = "cpsMax",
                type = "SPFLOAT",
                description ="The maximum frequency of change in Hz.",
                default = 4
            },
        }
    },

    modtype = "module",

    description = [[A signal with random fluctuations

     This is useful for emulating jitter found in analogue equipment. ]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["line"] = {

    files = {
        module = "line.c",
        header = "line.h",
        example = "ex_line.c",
    },

    func = {
        create = "sp_line_create",
        destroy = "sp_line_destroy",
        init = "sp_line_init",
        compute = "sp_line_compute",
    },

    params = {
        optional = {
            {
                name = "a",
                type = "SPFLOAT",
                description = "Inital point.",
                default = 0.0
            },
            {
                name = "dur",
                type = "SPFLOAT",
                description = "Duration (in seconds)",
                default = 0.5
            },
            {
                name = "b",
                type = "SPFLOAT",
                description = "End point",
                default = 1.0
            }
        },

    },

    modtype = "module",

    description = [[Produce a line segment with linear slope
This will generate a line from value A to value B in given amount of time. 
When it reaches it's target, it will stay at that value. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When non-zero, will retrigger the line slope."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["lpc"] = {

    files = {
        module = "lpc.c",
        header = "lpc.h",
        example = "ex_lpc.c",
    },

    func = {
        create = "sp_lpc_create",
        destroy = "sp_lpc_destroy",
        init = "sp_lpc_init",
        compute = "sp_lpc_compute",
        other = {
            sp_lpc_synth = {
                description = [[Toggle synth mode. 
                Instead of reading an input, manipulate the parameters in  
                a scaled ftable.]],
                args = { 
                    {
                        name = "ft",
                        type = "sp_ftbl *",
                        description = "ftable of size 7",
                        default = "N/A"
                    }
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "framesize",
                type = "int",
                description = "Sets the frame size for the encoder.",
                default = 512
            },
        },

        optional = {
        }
    },

    modtype = "module",

    description = [[A linear predictive coding filter.
This module is a wrapper for the open source library openlpc, which implements
the LPC10 audio codec optimized for speech signals. This module takes in an
input signal, downsamples it, and produces a decoded LPC10 audio signal, which
has a similar sound to that of a speak and spell. In this context, the LPC
signal is meant to be more of a audio effect rather than a utility for
communication. 

Because the LPC10 encoder
relies on frames for encoding, the output signal has a few milliseconds of
delay. The delay can be calculated in seconds as (framesize * 4) / samplerate.

In addition to using the LPC as a decoder/encoder, this module can also be 
set to synth mode. Instead of reading from an input signal, the LPC can
instead read from parameters set directly in a scaled ftable. 

]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Input signal to be processed with LPC."
        },
    },

    outputs = {
        {
            name = "output",
            description = "LPC encoded signal."
        },
    }

}
sptbl["lpf18"] = {

    files = {
        module = "lpf18.c",
        header = "lpf18.h",
        example = "ex_lpf18.c",
    },

    func = {
        create = "sp_lpf18_create",
        destroy = "sp_lpf18_destroy",
        init = "sp_lpf18_init",
        compute = "sp_lpf18_compute",
    },

    params = {
        optional = {
            {
                name = "cutoff",
                type = "SPFLOAT",
                description = "Filter cutoff frequency, in Hertz",
                default = 1000
            },
            {
                name = "res",
                type = "SPFLOAT",
                description ="Resonance. Expects a value in the range 0-1. A value of 1.0 will self oscillate at the cutoff frequency.",
                default = 0.8
            },
            {
                name = "dist",
                type = "SPFLOAT",
                description ="Distortion amount.",
                default = 2
            },
        }
    },

    modtype = "module",

    description = [[3-pole (18 db/oct slope) Low-Pass filter with resonance and tanh distortion]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["maygate"] = {

    files = {
        module = "maygate.c",
        header = "maygate.h",
        example = "ex_maygate.c",
    },

    func = {
        create = "sp_maygate_create",
        destroy = "sp_maygate_destroy",
        init = "sp_maygate_init",
        compute = "sp_maygate_compute",
    },

    params = {
        optional = {
            {
                name = "prob",
                type = "SPFLOAT",
                description = "Probability of maygate. This is a value between 0-1. The closer to 1, the more likely the maygate will let a signal through.",
                default = 0
            },

            {
                name = "mode",
                type = "int",
                description = "If mode is nonzero, maygate will output one sample triggers instead of a gate signal.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[A randomly open or closed "maybe gate"

    It takes in a trigger, and then it will randomly decide to turn the gate on or not. One particular application for maygate is to arbitrarily turn on/off sends to effects. One specific example of this could be a randomized reverb throw on a snare.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "This expects a trigger signal."
        }
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        }
    }

}
sptbl["metro"] = {

    files = {
        module = "metro.c",
        header = "metro.h",
        --example = "ex_tone.c",
    },

    func = {
        create = "sp_metro_create",
        destroy = "sp_metro_destroy",
        init = "sp_metro_init",
        compute = "sp_metro_compute",
    },

    params = {
        optional = {
             {
                name = "freq",
                type = "SPFLOAT",
                description = "The frequency to repeat.",
                default = 2.0
            },
        }
    },

    modtype = "module",

    description = [[Metronome

    Metro produces a series of 1-sample ticks at a regular rate. Typically, this is used alongside trigger-driven modules.]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This input doesn't do anything"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["mincer"] = {

    files = {
        module = "mincer.c",
        header = "mincer.h",
        example = "ex_mincer.c",
    },

    func = {
        create = "sp_mincer_create",
        destroy = "sp_mincer_destroy",
        init = "sp_mincer_init",
        compute = "sp_mincer_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "ftable containing an audio file.",
                default = "N/A"
            },
            {
                name = "winsize",
                type = "int",
                description = "FFT window size. Should be a power of 2.",
                default = "2048"
            }
        },

        optional = {
            {
                name = "time",
                type = "SPFLOAT",
                description = "Position in time. When non-changing it will do a spectral freeze of a the current point in time.",
                default = 0
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude.",
                default = 1
            },
            {
                name = "pitch",
                type = "SPFLOAT",
                description ="Pitch ratio. A value of. 1  normal, 2 is double speed, 0.5 is halfspeed, etc.",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[Phase-locked vocoder.

    Mincer is a phase-locked vocoder. It has the ability to play back an audio 
file loaded into an ftable like a sampler would. Unlike a typical sampler, mincer allows
time and pitch to be controlled separately. 
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["mode"] = {

    files = {
        module = "mode.c",
        header = "mode.h",
        example = "ex_mode.c",
    },

    func = {
        create = "sp_mode_create",
        destroy = "sp_mode_destroy",
        init = "sp_mode_init",
        compute = "sp_mode_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Resonant frequency of the filter.",
                default = 500
            },
            {
                name = "q",
                type = "SPFLOAT",
                description ="Quality factor of the filter. Roughly equal to q/freq.",
                default = 50
            },
        }
    },

    modtype = "module",

    description = [[Resonance filter used for modal synthesis

    Plucked and bell sounds can be created by passing an impulse through a combination of modal filters. ]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["moogladder"] = {

    files = {
        module = "moogladder.c",
        header = "moogladder.h",
        example = "ex_moogladder.c",
    },

    func = {
        create = "sp_moogladder_create",
        destroy = "sp_moogladder_destroy",
        init = "sp_moogladder_init",
        compute = "sp_moogladder_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Filter cutoff frequency.",
                default = 1000
            },
            {
                name = "res",
                type = "SPFLOAT",
                description ="Filter resonance",
                default = 0.4
            },
        }
    },

    modtype = "module",

    description = [[Low pass resonant filter based on the Moogladder filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        { name = "input",
            description = "The clock source for a made-up plugin."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Stereo left output for moogladder."
        },
    }

}
sptbl["noise"] = {

    files = {
        module = "noise.c",
        header = "noise.h",
        example = "ex_noise.c",
    },

    func = {
        create = "sp_noise_create",
        destroy = "sp_noise_destroy",
        init = "sp_noise_init",
        compute = "sp_noise_compute",
    },

    params = {
        optional = {
            {
                name = "amp",
                type = "SPFLOAT",
                description = "Amplitude. (Value between 0-1).",
                default = 1.0
            },
        }
    },

    modtype = "module",

    description = [[White noise generator]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["nsmp"] = {

    files = {
        module = "nsmp.c",
        header = "nsmp.h",
        example = "ex_nsmp.c",
    },

    func = {
        create = "sp_nsmp_create",
        destroy = "sp_nsmp_destroy",
        init = "sp_nsmp_init",
        compute = "sp_nsmp_compute",
        other = {
            sp_nsmp_print_index = {
                description = "Prints the index and their corresponding keyword",
                args = {
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl*",
                description = "ftbl of the audio file. It should be mono.",
                default = "N/A"
            },
            {
                name = "sr",
                type = "int",
                description = "samplerate.",
                default = "N/A"
            },
            {
                name = "init",
                type = "const char*",
                description = "ini file.",
                default = "N/A"
            }
        },

        optional = {
            {
                name = "index",
                type = "uint32_t",
                description = "This is an optional parameter. These values are always set to a value by default, and can be set after the init function has been called.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Nanosamp: a tiny sampler built for Soundpipe

    A nanosamp file is comprised of a mono audio file and an ini file. Nanosamp is geared towards percussive and found sound sample players, and is intended to be combined with soundpipe modules.

The ini file contains mappings that correspond to the audio file. Such an entry would look like this:



[keyword]

pos = 2.3

size = 3


</pre>
In this instance, an entry called "keyword" has been made, starting at 2.3 seconds in the
audio file, with a length of 3 seconds. An example file oneart.ini has been created in the
examples folder.

The SoundPipe implementation of nanosamp will automatically index the entries
in the order they appear in the INI file and must be selected this way by changing the index
parameter. Soundpipe will only select the new entry when the trigger input is a non-zero value.

]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trigger",
            description = "Trigger input."
        },
    },

    outputs = {
        {
            name = "signal out",
            description = "Signal output."
        },
    }

}
sptbl["osc"] = {

    files = {
        module = "osc.c",
        header = "osc.h",
        example = "ex_osc.c",
    },

    func = {
        create = "sp_osc_create",
        destroy = "sp_osc_destroy",
        init = "sp_osc_init",
        compute = "sp_osc_compute",
    },

    params = {
        mandatory = {
            {
                name = "tbl",
                type = "sp_ftbl *",
                description = "Wavetable to read from. Note: the size of this table must be a power of 2.",
                default = "N/A"
            },
            {
                name = "phase",
                type = "SPFLOAT",
                description ="Initial phase of waveform, expects a value 0-1",
                default = 0
            }
        },
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency (in Hz)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude (typically a value between 0 and 1).",
                default = 0.2
            },
        }
    },

    modtype = "module",

    description = [[ Table-lookup oscilator with linear interpolation]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This does nothing."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["oscmorph"] = {

    files = {
        module = "oscmorph.c",
        header = "oscmorph.h",
        example = "ex_oscmorph.c",
    },

    func = {
        create = "sp_oscmorph_create",
        destroy = "sp_oscmorph_destroy",
        init = "sp_oscmorph_init",
        compute = "sp_oscmorph_compute",
    },

    params = {
        mandatory = {
            {
                name = "tbl",
                type = "sp_ftbl **",
                description = "An array of ftables to read from. Note: the size of these tables must be a power of 2 (and the same size as well).",
                default = "N/A"
            },
            {
                name = "nft",
                type = "int",
                description ="Number of ftbls",
                default = 2
            },
            {
                name = "phase",
                type = "SPFLOAT",
                description ="Initial phase of waveform, expects a value 0-1",
                default = 0
            }
        },
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency (in Hz)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude (typically a value between 0 and 1).",
                default = 0.2
            },
            {
                name = "wtpos",
                type = "SPFLOAT",
                description ="Wavetable position. (scaled 0-1)",
                default = 0.0
            },
        }
    },

    modtype = "module",

    description = [[Wavetable morphing oscillator

This is an oscillator with linear interpolation that is capable of morphing 
between an arbitrary number of wavetables. 
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "This does nothing."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["pan2"] = {

    files = {
        module = "pan2.c",
        header = "pan2.h",
        example = "ex_pan2.c",
    },

    func = {
        create = "sp_pan2_create",
        destroy = "sp_pan2_destroy",
        init = "sp_pan2_init",
        compute = "sp_pan2_compute",
    },

    params = {
        mandatory = {
        },

        optional = {
            {
                name = "type",
                type = "uint32_t",
                description = [[Panning type. 0 = equal power, 1 = square root, 2 = linear,
3 = alternative equal power. Values outside this range will wrap. ]],
                default = 0
            },
            {
                name = "pan",
                type = "SPFLOAT",
                description ="Panning. A value of -1 is hard left, and a value of 1 is hard right, and 0 is center.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Panner]],

    ninputs = 1,
    noutputs = 2,
    
    inputs = {
        {
            name = "in",
            description = "Mono input signal to be panned."
        },
    },

    outputs = {
        {
            name = "out_left",
            description = "Stereo left output."
        },
        {
            name = "out_right",
            description = "Stereo right output."
        },
    }

}
sptbl["panst"] = {

    files = {
        module = "panst.c",
        header = "panst.h",
        example = "ex_panst.c",
    },

    func = {
        create = "sp_panst_create",
        destroy = "sp_panst_destroy",
        init = "sp_panst_init",
        compute = "sp_panst_compute",
    },

    params = {
        mandatory = {
        },

        optional = {
            {
                name = "type",
                type = "uint32_t",
                description = [[Panning type. 0 = equal power, 1 = square root, 2 = linear,
3 = alternative equal power. Values outside this range will wrap. ]],
                default = 0
            },
            {
                name = "pan",
                type = "SPFLOAT",
                description ="Panning. A value of -1 is hard left, and a value of 1 is hard right.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Stereo Panner]],

    ninputs = 2,
    noutputs = 2,

    inputs = {
        {
            name = "in2",
            description = "Stereo left input signal."
        },
        {
            name = "in2",
            description = "Stereo right input signal."
        },

    },

    outputs = {
        {
            name = "out_left",
            description = "Stereo left output."
        },
        {
            name = "out_right",
            description = "Stereo right output."
        },
    }

}
sptbl["pareq"] = {

    files = {
        module = "pareq.c",
        header = "pareq.h",
        example = "ex_pareq.c",
    },

    func = {
        create = "sp_pareq_create",
        destroy = "sp_pareq_destroy",
        init = "sp_pareq_init",
        compute = "sp_pareq_compute",
    },

    params = {
        optional = {
            {
                name = "fc",
                type = "SPFLOAT",
                description = "Center frequency in peak mode, corner frequency in shelving mode.",
                default = 1000
            },
            {
                name = "v",
                type = "SPFLOAT",
                description ="Amount at which the center frequency value shall be increased or decreased. A value of 1 is a flat response.",
                default = 1
            },
            {
                name = "q",
                type = "SPFLOAT",
                description ="Q of the filter. sqrt(0.5) is no resonance.",
                default = 0.707
            },
            {
                name = "mode",
                type = "SPFLOAT",
                description ="EQ mode. 0 = peak, 1 = low shelving, 2 = high shelving",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Parametric Equalizer
This is an implementation of Zoelzer's parametric equalizer filter.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["paulstretch"] = {

    files = {
        module = "paulstretch.c",
        header = "paulstretch.h",
        example = "ex_paulstretch.c",
    },

    func = {
        create = "sp_paulstretch_create",
        destroy = "sp_paulstretch_destroy",
        init = "sp_paulstretch_init",
        compute = "sp_paulstretch_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "ftable containing audio data",
                default = "N/A"
            },
            {
                name = "windowsize",
                type = "SPFLOAT",
                description = "Window size, in seconds.",
                default = 10
            },
            {
                name = "stretch",
                type = "SPFLOAT",
                description = "Stretch factor, 1.0 is no stretch.",
                default = 10
            },
        },

        optional = {
        }
    },

    modtype = "module",

    description = [[An extreme time-stretching algorithm by Paul Nasca Octavian
This is an implementation of the popular paulstretch algorithm used for time
stretching an audio signal to create ambient textures. Ideally, this algorithm 
is best used for stretching signals by very very long amounts. 

This version of paulstretch will take an ftable and loop through it, make
it an ideal means for creating sustained pads. 
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["pdhalf"] = {

    files = {
        module = "pdhalf.c",
        header = "pdhalf.h",
        example = "ex_pdhalf.c",
    },

    func = {
        create = "sp_pdhalf_create",
        destroy = "sp_pdhalf_destroy",
        init = "sp_pdhalf_init",
        compute = "sp_pdhalf_compute",
    },

    params = {
        optional = {
            {
                name = "amount",
                type = "SPFLOAT",
                description = "Amount of distortion, within the range [-1, 1]. 0 is no distortion.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Casio-style phase distortion with "pivot point" on the X axis
This module is designed to emulate the classic phase distortion synthesis technique.
From the mid 90's. The technique reads the first and second halves of the ftbl
at different rates in order to warp the waveform. For example, pdhalf can 
smoothly transition a sinewave into something approximating a sawtooth wave.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "sig",
            description = "Input signal, typically a phasor normalize 0-1."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["peaklim"] = {

    files = {
        module = "peaklim.c",
        header = "peaklim.h",
        example = "ex_peaklim.c",
    },

    func = {
        create = "sp_peaklim_create",
        destroy = "sp_peaklim_destroy",
        init = "sp_peaklim_init",
        compute = "sp_peaklim_compute",
    },

    params = {
        optional = {
            {
                name = "atk",
                type = "SPFLOAT",
                description = "Attack time, in seconds",
                default = 0.01
            },
            {
                name = "rel",
                type = "SPFLOAT",
                description ="Release time, in seconds",
                default = 0.1
            },
            {
                name = "thresh",
                type = "SPFLOAT",
                description ="Threshold, in dB",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Peak limiter 
This is a simple peak limiting algorithm, based off code from the Stanford
Music-424 class.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Input signal."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Output signal."
        },
    }

}
sptbl["phaser"] = {

    files = {
        module = "phaser.c",
        header = "phaser.h",
        example = "ex_phaser.c",
    },

    func = {
        create = "sp_phaser_create",
        destroy = "sp_phaser_destroy",
        init = "sp_phaser_init",
        compute = "sp_phaser_compute",
    },

    params = {
        optional = {
            {
                name = "MaxNotch1Freq",
                type = "SPFLOAT*",
                description = "Between 20 and 10000",
                default = 800 
            },
            {
                name = "MinNotch1Freq",
                type = "SPFLOAT*",
                description ="Between 20 and 5000",
                default = 100
            },
            {
                name = "Notch_width",
                type = "SPFLOAT*",
                description ="Between 10 and 5000",
                default = 1000
            },
            {
                name = "NotchFreq",
                type = "SPFLOAT*",
                description ="Between 1.1 and 4",
                default = 1.5 
            },
            {
                name = "VibratoMode",
                type = "SPFLOAT*",
                description ="1 or 0",
                default = 1
            },
            {
                name = "depth",
                type = "SPFLOAT*",
                description ="Between 0 and 1",
                default = 1
            },
            {
                name = "feedback_gain",
                type = "SPFLOAT*",
                description ="Between 0 and 1",
                default = 0
            },
            {
                name = "invert",
                type = "SPFLOAT*",
                description ="1 or 0",
                default = 0
            },
            {
                name = "level",
                type = "SPFLOAT*",
                description ="-60 to 10 dB",
                default = 0
            },
            {
                name = "lfobpm",
                type = "SPFLOAT*",
                description ="Between 24 and 360",
                default = 30
            },
        }
    },

    modtype = "module",

    description = [[A stereo phaser

	This is a stereo phaser, generated from Faust code taken from the 
Guitarix project.
]],

    ninputs = 2,
    noutputs = 2,

    inputs = {
        {
            name = "input1",
            description = "Left audio input signal."
        },
        {
            name = "input2",
            description = "Right audio input signal."
        },
    },

    outputs = {
        {
            name = "out_left",
            description = "Stereo left output for phaser."
        },
        {
            name = "out_right",
            description = "Stereo right output for phaser."
        },
    }

}
sptbl["phasor"] = {

    files = {
        module = "phasor.c",
        header = "phasor.h",
        example = "ex_phasor.c",
    },

    func = {
        create = "sp_phasor_create",
        destroy = "sp_phasor_destroy",
        init = "sp_phasor_init",
        compute = "sp_phasor_compute",
    },

    params = {
        mandatory = {
            {
                name = "iphs",
                type = "SPFLOAT",
                description = "initial phase",
                default = 0
            },
        },

        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency.",
                default = 1.5
            },
        }
    },

    modtype = "module",

    description = [[Normalized sawtooth wave from 0 to 1

    Phasors are often used when building table-lookup oscillators.]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "this doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["pinknoise"] = {

    files = {
        module = "pinknoise.c",
        header = "pinknoise.h",
        example = "ex_pinknoise.c",
    },

    func = {
        create = "sp_pinknoise_create",
        destroy = "sp_pinknoise_destroy",
        init = "sp_pinknoise_init",
        compute = "sp_pinknoise_compute",
    },

    params = {
        optional = {
            {
                name = "amp",
                type = "SPFLOAT",
                description = "Amplitude. (Value between 0-1).",
                default = 1.0
            },
        }
    },

    modtype = "module",

    description = [[Pink pinknoise generator]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["pitchamdf"] = {

    files = {
        module = "pitchamdf.c",
        header = "pitchamdf.h",
        example = "ex_pitchamdf.c",
    },

    func = {
        create = "sp_pitchamdf_create",
        destroy = "sp_pitchamdf_destroy",
        init = "sp_pitchamdf_init",
        compute = "sp_pitchamdf_compute",
    },

    params = {
        mandatory = {
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum expected frequency to detect",
                default = 200
            },
            {
                name = "max",
                type = "SPFLOAT",
                description = "Maximum expected frequency to detect",
                default = 500
            }
        }
    },

    modtype = "module",

    description = [[Pitch detection using AMDF method.
Pitchamdf tracks the pitch of signal using the AMDF (Average Magnitude Difference Function) method of 
Pitch following. 
]],

    ninputs = 1,
    noutputs = 2,

    inputs = {
        {
            name = "input",
            description = "Signal input"
        },
    },

    outputs = {
        {
            name = "freq",
            description = "Detected frequency."
        },
        {
            name = "rms",
            description = "Average amplitude."
        },
    }

}
sptbl["pluck"] = {

    files = { 
        module = "pluck.c",
        header = "pluck.h",
        example = "ex_pluck.c",
    },
    
    func = {
        create = "sp_pluck_create",
        destroy = "sp_pluck_destroy",
        init = "sp_pluck_init",
        compute = "sp_pluck_compute",
    },
    
    params = {
        mandatory = {
            {
                name = "ifreq",
                type = "SPFLOAT",
                description = "Sets the initial frequency. This frequency is used to allocate all the buffers needed for the delay. This should be the lowest frequency you plan on using.",
                default = 110
            }
        },
    
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = [[Variable frequency. Values less than the initial 
frequency (ifreq) will be doubled until it is greater than or equal to ifreq.]],
                default = "ifreq"
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude",
                default = 0.5
            },
        }
    },
    
    modtype = "module",
    
    description = [[Karplus-Strong plucked string instrument.]], 
    
    ninputs = 1,
    noutputs = 1,
    
    inputs = { 
        {
            name = "trigger",
            description = "Trigger input. When non-zero, will reinitialize and pluck."
        },
    },
    
    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }
}
sptbl["port"] = {

    files = {
        module = "port.c",
        header = "port.h",
        example = "ex_port.c",
    },

    func = {
        create = "sp_port_create",
        destroy = "sp_port_destroy",
        init = "sp_port_init",
        compute = "sp_port_compute",
        other = {
            sp_port_reset = {
                description = "Resets internal buffers, snapping to input value instead of ramping to it.",
                args = {
                    {
                        name = "input",
                        type = "SPFLOAT *",
                        description = "input value to snap to.",
                        default = 0.0
                    },
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "htime",
                type = "SPFLOAT",
                description = "",
                default = 0.02
            },
        },
        optional = {
            {
                name = "htime",
                type = "SPFLOAT",
                description = "",
                default = 0.02
            },
        },
    },

    modtype = "module",

    description = [[ Portamento-style control signal smoothing

    Useful for smoothing out low-resolution signals and applying glissando to filters.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["posc3"] = {

    files = {
        module = "posc3.c",
        header = "posc3.h",
        example = "ex_posc3.c",
    },

    func = {
        create = "sp_posc3_create",
        destroy = "sp_posc3_destroy",
        init = "sp_posc3_init",
        compute = "sp_posc3_compute",
    },

    params = {
        mandatory = {
            {
                name = "tbl",
                type = "sp_ftbl *",
                description = "Wavetable to read from. Note: the size of this table must be a power of 2.",
                default = "N/A"
            },
        },
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency (in Hz)",
                default = 440
            },
            {
                name = "amp",
                type = "SPFLOAT",
                description ="Amplitude (typically a value between 0 and 1).",
                default = 0.2
            },
        }
    },

    modtype = "module",

    description = [[ High-precision table-lookup posc3ilator with cubic interpolation]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This does nothing."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["progress"] = {

    files = {
        module = "progress.c",
        header = "progress.h",
        example = "ex_progress.c",
    },

    func = {
        create = "sp_progress_create",
        destroy = "sp_progress_destroy",
        init = "sp_progress_init",
        compute = "sp_progress_compute",
    },

    params = {
        optional = {
            {
                name = "nbars",
                type = "int",
                description = "",
                default = 30
            },
            {
                name = "skip",
                type = "int",
                description ="How many samples to skip before checking",
                default = 1000
            },
        }
    },

    modtype = "module",

    description = [[A simple progress bar for the commandline

    Mostly ideal for offline renderings and programs with finite length. Escape characters are used to show/hide the cursor. Interruption before finishing may cause the cursor to disappear.]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    }

}
sptbl["prop"] = {

    files = {
        module = "prop.c",
        header = "prop.h",
        example = "ex_prop.c",
    },

    func = {
        create = "sp_prop_create",
        destroy = "sp_prop_destroy",
        init = "sp_prop_init",
        compute = "sp_prop_compute",
        other = {
            sp_prop_reset = {
                description = "Resets prop back to starting position.",
                args = {
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "str",
                type = "const char *",
                description = "Prop string to be parsed.",
                default = "N/A"
            },
        },

        optional = {
            {
                name = "bpm",
                type = "SPFLOAT",
                description = "Beats per minute of the prop string.",
                default = 60
            },
        }
    },

    modtype = "module",

    description = [[Simple rhythmic notation gate generator

    Creates a gate using a simple rhythmic notation system called prop. When it reaches the end of the prop string, it will loop back to the beginning.

Prop has a few basic rules:

1. A '+' denotes a on. A '-' denotes an off (rest). They each have an equal duration of a quarter note.

2. On and off values can be strung together to create equally spaced gates: +-+--

3. When notes are enclosed in parantheses '()' following a positive integer N, their duration is reduced N times: ++2(+-)

4. When notes are enclosed in brackets '[]' following a positive integer N, their duration is scaled by a factor of N: ++2[++]

5. Parenthesis and brackets can be nested: +- 2[3(+2(++)+)]2(++)

]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This is doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Gate output."
        },
    }

}
sptbl["pshift"] = {

    files = {
        module = "pshift.c",
        header = "pshift.h",
        example = "ex_pshift.c",
    },

    func = {
        create = "sp_pshift_create",
        destroy = "sp_pshift_destroy",
        init = "sp_pshift_init",
        compute = "sp_pshift_compute",
    },

    params = {
        optional = {
            {
                name = "shift",
                type = "SPFLOAT*",
                description = "Pitch shift (in semitones), range -24/24.",
                default = 0
            },
            {
                name = "window",
                type = "SPFLOAT*",
                description ="Window size (in samples), max 10000",
                default = 1000
            },
            {
                name = "xfade",
                type = "SPFLOAT*",
                description ="Crossfade (in samples), max 10000",
                default = 10
            },
        }
    },

    modtype = "module",

    description = [[Time-domain pitch shifter.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["ptrack"] = {

    files = {
        module = "ptrack.c",
        header = "ptrack.h",
        example = "ex_ptrack.c",
    },

    func = {
        create = "sp_ptrack_create",
        destroy = "sp_ptrack_destroy",
        init = "sp_ptrack_init",
        compute = "sp_ptrack_compute",
    },

    params = {
        mandatory = {
            {
                name = "ihopsize",
                type = "int",
                description = "hop size.",
                default = 512
            },
            {
                name = "ipeaks",
                type = "int",
                description = "Number of peaks.",
                default = 20
            }
        },

        optional = {
        }
    },

    modtype = "module",

    description = [[Track pitch of a signal.
Ptrack is a pitch tracker, based on an algorithm originally created by
Miller Puckette.
]],

    ninputs = 1,
    noutputs = 2,

    inputs = {
        {
            name = "in",
            description = "Input signal."
        },
    },

    outputs = {
        {
            name = "freq",
            description = "Guessed frequency (in Hz)."
        },
        {
            name = "amp",
            description = "Guessed amplitude."
        },
    }

}
sptbl["randh"] = {

    files = {
        module = "randh.c",
        header = "randh.h",
        example = "ex_randh.c",
    },

    func = {
        create = "sp_randh_create",
        destroy = "sp_randh_destroy",
        init = "sp_randh_init",
        compute = "sp_randh_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description ="Minimum value to use.",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum value to use.",
                default = 1
            },
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Frequency of randomization (in Hz)",
                default = 10
            },
        }
    },

    modtype = "module",

    description = [[Random number generator with hold time.
Randh is loosely based off of the Csound opcode randomh. The design is equivalent
to scaled noise sent through a classic sample and hold module.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["randi"] = {

    files = {
        module = "randi.c",
        header = "randi.h",
        example = "ex_randi.c",
    },

    func = {
        create = "sp_randi_create",
        destroy = "sp_randi_destroy",
        init = "sp_randi_init",
        compute = "sp_randi_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum value",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum value",
                default = 1
            },
            {
                name = "cps",
                type = "SPFLOAT",
                description ="Frequency to change values.",
                default = 3
            },
            {
                name = "mode",
                type = "SPFLOAT",
                description = "Randi mode (not yet implemented yet.)",
                default = 0,
                irate = true
            },
        }
    },

    modtype = "module",

    description = [[Line segments between random values within a range]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "This doesn't do anything."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["random"] = {

    files = {
        module = "random.c",
        header = "random.h",
        example = "ex_random.c",
    },

    func = {
        create = "sp_random_create",
        destroy = "sp_random_destroy",
        init = "sp_random_init",
        compute = "sp_random_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum value.",
                default = -0.2
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum value.",
                default = 0.2
            },
        }
    },

    modtype = "module",

    description = [[Random values within a range]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummmy",
            description = "This does nothing."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["reson"] = {

    files = { 
        module = "reson.c",
        header = "reson.h",
        example = "ex_reson.c",
    },
    
    func = {
        create = "sp_reson_create",
        destroy = "sp_reson_destroy",
        init = "sp_reson_init",
        compute = "sp_reson_compute",
    },
    
    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Center frequency of the filter, or frequency position of the peak response.",
                default = 4000
            },
            {
                name = "bw",
                type = "SPFLOAT",
                description = "Bandwidth of the filter.",
                default = 1000
            }
        },
    
    },
    
    modtype = "module",
    
    description = [[A second-order resonant filter. NOTE: The output for reson appears to be very hot, so take caution when using this module.]], 
    
    ninputs = 1,
    noutputs = 1,
    
    inputs = { 
        {
            name = "input",
            description = "Signal Input."
        },
    },
    
    outputs = {
        {
            name = "output",
            description = "Signal output."
        },
    }

}
sptbl["reverse"] = {

	files = {
	    module = "reverse.c",
	    header = "reverse.h",
	    example = "ex_reverse.c",
	},

	func = {
	    create = "sp_reverse_create",
	    destroy = "sp_reverse_destroy",
	    init = "sp_reverse_init",
	    compute = "sp_reverse_compute",
	},

	params = {
	    mandatory = {
	        {
	            name = "delay",
	            type = "SPFLOAT",
	            description = "Delay time in seconds.",
	            default = "1.0"
	        }
	    }
	},

	modtype = "module",

	description = [[Signal reverser

	Reverse will store a signal inside a buffer and play it back reversed.]],

	ninputs = 1,
	noutputs = 1,

	inputs = {
	    {
	        name = "input",
	        description = "Signal input."
	    }
	},

	outputs = {
	    {
	        name = "out",
	        description = "Signal output."
	    }
	}

}
sptbl["revsc"] = {

    files = {
        module = "revsc.c",
        header = "revsc.h",
        example = "ex_revsc.c",
    },

    func = {
        create = "sp_revsc_create",
        destroy = "sp_revsc_destroy",
        init = "sp_revsc_init",
        compute = "sp_revsc_compute",
    },

    params = {
        optional = {
            {
                name = "feedback",
                type = "SPFLOAT",
                description = "Value between 0-1 that sets feedback value. The larger the value, the longer the decay.",
                default = 0.97
            },
            {
                name = "lpfreq",
                type = "SPFLOAT",
                description ="low pass cutoff frequency.",
                default = 10000
            },
        }
    },

    modtype = "module",

    description = [[ 8 FDN stereo reverb]],

    ninputs = 2,
    noutputs = 2,

    inputs = {
        {
            name = "input_1",
            description = "First input."
        },
        {
            name = "input_2",
            description = "Second input."
        },
    },

    outputs = {
        {
            name = "out_1",
            description = "Channel 1 output. Most likely for the left channel."
        },
        {
            name = "out_2",
            description = "Channel 2 output. Mose likely for the right channel."
        },
    }

}
sptbl["rms"] = {

    files = {
        module = "rms.c",
        header = "rms.h",
    },

    func = {
        create = "sp_rms_create",
        destroy = "sp_rms_destroy",
        init = "sp_rms_init",
        compute = "sp_rms_compute",
    },

    params = {
        optional = {
            {
                name = "ihp",
                type = "SPFLOAT",
                description = "Half-power point (in Hz) of internal lowpass filter. This parameter is fixed at 10Hz and is not yet mutable.",
                default = 10
            },
        }
    },

    modtype = "module",

    description = [[RMS-averaged signal amplitude

    Perform "root-mean-square" on a signal to get overall amplitude of a signal. The output signal looks similar to that of a classic VU meter.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Input signal."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Output signal."
        },
    }

}
sptbl["rpt"] = {

    files = {
        module = "rpt.c",
        header = "rpt.h",
        example = "ex_rpt.c",
    },

    func = {
        create = "sp_rpt_create",
        destroy = "sp_rpt_destroy",
        init = "sp_rpt_init",
        compute = "sp_rpt_compute",
    },

    params = {
        mandatory = {
            {
                name = "maxdur",
                type = "SPFLOAT",
                description = "Maximum delay duration in seconds. This will set the buffer size.",
                default = "0.7"
            }
        },
    },

    modtype = "module",

    description = [[Trigger based beat-repeat stuttering effect

    When the input is a non-zero value, rpt will load up the buffer and loop a certain number of times. Speed and repeat amounts can be set with the sp_rpt_set function.]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When this value is non-zero, it will start the repeater."
        },
        {
            name = "input",
            description = "The signal to be repeated."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out. This is passive unless explicity triggered in the input."
        },
    }

}
sptbl["rspline"] = {

    files = {
        module = "rspline.c",
        header = "rspline.h",
        example = "ex_rspline.c",
    },

    func = {
        create = "sp_rspline_create",
        destroy = "sp_rspline_destroy",
        init = "sp_rspline_init",
        compute = "sp_rspline_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum range.",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum range",
                default = 1 
            },
            {
                name = "cps_min",
                type = "SPFLOAT",
                description = "",
                default = 0.1
            },
            {
                name = "cps_max",
                type = "SPFLOAT",
                description ="",
                default = 3 
            },
        }
    },

    modtype = "module",

    description = [[Random spline curve generator
This is a function that generates random spline curves. This signal generator
is ideal for control signals. Curves are quite smooth when cps_min is not
too different from cps_max. When the range of these two is big, 
some discontinuity may occur. Due to the interpolation, the output 
may be greater than the range values. 
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "dummy",
            description = "Dummy input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["samphold"] = {

    files = {
        module = "samphold.c",
        header = "samphold.h",
        example = "ex_samphold.c",
    },

    func = {
        create = "sp_samphold_create",
        destroy = "sp_samphold_destroy",
        init = "sp_samphold_init",
        compute = "sp_samphold_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Classic sample and hold]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Will hold the current input value when non-zero."
        },
        {
            name = "input",
            description = "Audio input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        }
    }

}
sptbl["saturator"] = {

    files = {
        module = "saturator.c",
        header = "saturator.h",
        example = "ex_saturator.c",
    },

    func = {
        create = "sp_saturator_create",
        destroy = "sp_saturator_destroy",
        init = "sp_saturator_init",
        compute = "sp_saturator_compute",
    },

    params = {
        optional = {
            {
                name = "drive",
                type = "SPFLOAT",
                description ="Input gain into the distortion section, in decibels. Controls overall amount of distortion.",
                default = 1.0
            },
            {
                name = "dcoffset",
                type = "SPFLOAT",
                description = "Constant linear offset applied to the signal. A small offset will introduce odd harmonics into the distoration spectrum, whereas a zero offset will have only even harmonics.",
                default = 0.0
            },
        }
    },

    modtype = "module",

    description = [[Soft clip saturating distortion, based on examples from Abel/Berners' Music 424 course at Stanford.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "output."
        },
    }

}
sptbl["scale"] = {

    files = {
        module = "scale.c",
        header = "scale.h",
        example = "ex_scale.c",
    },

    func = {
        create = "sp_scale_create",
        destroy = "sp_scale_destroy",
        init = "sp_scale_init",
        compute = "sp_scale_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description = "Minimum value to scale to.",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum value to scale to.",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[Bipolar Scale

    This module scales from unipolar [0, 1] to another range defined by min and max.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal to be scaled."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Scaled signal out"
        },
    }

}
sptbl["sdelay"] = {

    files = {
        module = "sdelay.c",
        header = "sdelay.h",
        example = "ex_sdelay.c",
    },

    func = {
        create = "sp_sdelay_create",
        destroy = "sp_sdelay_destroy",
        init = "sp_sdelay_init",
        compute = "sp_sdelay_compute",
    },

    params = {
        mandatory = {
            {
                name = "size",
                type = "SPFLOAT",
                description = "Size of delay (in samples)",
                default = "128"
            },
        }

    },

    modtype = "module",

    description = [[Delays a signal by a number of samples.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Input signal to be delayed."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output for sdelay."
        },
    }

}
sptbl["slice"] = {

    files = {
        module = "slice.c",
        header = "slice.h",
        example = "ex_slice.c",
    },

    func = {
        create = "sp_slice_create",
        destroy = "sp_slice_destroy",
        init = "sp_slice_init",
        compute = "sp_slice_compute",
    },

    params = {
        mandatory = {
            {
                name = "vals",
                type = "sp_ftbl *",
                description = "A table containing slice points, in samples",
                default = "N/A"
            },
            {
                name = "buf",
                type = "sp_ftbl *",
                description = "The buffer containing the audio samples.",
                default = "N/A"
            }
        },

        optional = {
            {
                name = "id",
                type = "SPFLOAT",
                description = "Value position.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Yet another slice-based sample player.
This module takes in an audio buffer and a table with slice points. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When non-zero, (re)triggers samples"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
        {
            name = "out_right",
            description = "Stereo right output for slice."
        },
    }

}
sptbl["smoothdelay"] = {

    files = {
        module = "smoothdelay.c",
        header = "smoothdelay.h",
        example = "ex_smoothdelay.c",
    },

    func = {
        create = "sp_smoothdelay_create",
        destroy = "sp_smoothdelay_destroy",
        init = "sp_smoothdelay_init",
        compute = "sp_smoothdelay_compute",
    },

    params = {
        mandatory = {
            {
                name = "maxdel",
                type = "SPFLOAT",
                description = "Maximum delay time (in seconds)",
                default = 1.0
            },
            {
                name = "interp",
                type = "uint32_t",
                description = "interpolation time (in samples)",
                default = 1024
            }
        },

        optional = {
            {
                name = "feedback",
                type = "SPFLOAT",
                description ="",
                default = 0.1
            },
            {
                name = "del",
                type = "SPFLOAT",
                description = "",
                default = 0.5
            },
        }
    },

    modtype = "module",

    description = [[Smooth variable delay line without varispeed pitch.
Smooth delay is based off the sdelay module in Faust. The smooth delay 
algorithm involves a double delay line. Any time the delay time changes, 
the delay time of buffer not heard changes, then is crossfaded to 
that buffer.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["spa"] = {

    files = {
        module = "spa.c",
        header = "spa.h",
        example = "ex_spa.c",
    },

    func = {
        create = "sp_spa_create",
        destroy = "sp_spa_destroy",
        init = "sp_spa_init",
        compute = "sp_spa_compute",
    },

    params = {
        mandatory = {
            {
                name = "filename",
                type = "const char *",
                description = "Filename of SPA file",
                default = "N/A"
            },
        },

    },

    modtype = "module",

    description = [[Stream a Soundpipe Audio File
Similar to sp_diskin, sp_spa will stream a file in the internal soundpipe
audio format. Such a format is useful for instances where you need to read
audio files, but can't use libsndfile. 
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Output to spa."
        },
    }

}
sptbl["sparec"] = {

    files = {
        module = "sparec.c",
        header = "sparec.h",
        example = "ex_sparec.c",
    },

    func = {
        create = "sp_sparec_create",
        destroy = "sp_sparec_destroy",
        init = "sp_sparec_init",
        compute = "sp_sparec_compute",
        other = {
            sp_sparec_close = {
                description = "Close spa file and writes the rest of the data in the buffer.",
                args = {
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "filename",
                type = "const char *",
                description = "Filename to write to",
                default = "N/A"
            },
        },

    },

    modtype = "module",

    description = [[Writes signal to spa file.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Input signal."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Copy of input signal."
        }
    }

}
sptbl["streson"] = {

    files = {
        module = "streson.c",
        header = "streson.h",
        example = "ex_streson.c",
    },

    func = {
        create = "sp_streson_create",
        destroy = "sp_streson_destroy",
        init = "sp_streson_init",
        compute = "sp_streson_compute",
    },

    params = {
        optional = {
            {
                name = "freq",
                type = "SPFLOAT",
                description = "Fundamental frequency of string.",
                default = 440.0
            },
            {
                name = "fdbgain",
                type = "SPFLOAT",
                description = "Feedback amount (value between 0-1)",
                default = 0.8
            },
        }
    },

    modtype = "module",

    description = [[String resonator filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "output",
            description = "Signal output."
        },
    }

}
sptbl["switch"] = {

    files = {
        module = "switch.c",
        header = "switch.h",
        example = "ex_switch.c",
    },

    func = {
        create = "sp_switch_create",
        destroy = "sp_switch_destroy",
        init = "sp_switch_init",
        compute = "sp_switch_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Switch between two signals

    By default, the incoming first signal is selected. When triggered, the output signal will switch to the other signal.]],

    ninputs = 3,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When non-zero, will switch to the other signal."
        },
        {
            name = "input_1",
            description = "Signal input 1. This is the default signal."
        },
        {
            name = "input_2",
            description = "Signal input 2."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["tabread"] = {
    files = {
        module = "tabread.c",
        header = "tabread.h",
        example = "ex_tabread.c",
    },

    func = {
        create = "sp_tabread_create",
        destroy = "sp_tabread_destroy",
        init = "sp_tabread_init",
        compute = "sp_tabread_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "A properly allocated table (using a function like sp_gen_file).",
                default = "N/A"
            },
            {
                name = "mode",
                type = "SPFLOAT",
                description ="1 = scaled index, 0 = unscaled index",
                default = 1.0
            },
        },

        optional = {
            {
                name = "index",
                type = "SPFLOAT",
                description ="index position, either scaled or unscaled with mode parameter",
                default = 0
            },
            {
                name = "offset",
                type = "SPFLOAT",
                description ="Offset from beginning of ftable. If the mode is scaled, then it is in range 0-1, other wise it is the index position.",
                default = 1.0
            },
            {
                name = "wrap",
                type = "SPFLOAT",
                description ="Enable wraparound. 1 = on; 0 = 0ff.",
                default = 1.0
            }
        }
    },

    modtype = "module",

    description = [[Table 
        Read through an sp_ftbl with linear interpolation.
    ]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal in."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }
}
sptbl["tadsr"] = {

    files = {
        module = "tadsr.c",
        header = "tadsr.h",
        example = "ex_tadsr.c",
    },

    func = {
        create = "sp_tadsr_create",
        destroy = "sp_tadsr_destroy",
        init = "sp_tadsr_init",
        compute = "sp_tadsr_compute",
    },

    params = {
        optional = {
            {
                name = "atk",
                type = "SPFLOAT",
                description = "Attack time",
                default = 0.1
            },
            {
                name = "dec",
                type = "SPFLOAT",
                description ="Decay time",
                default = 0.1
            },
            {
                name = "sus",
                type = "SPFLOAT",
                description ="Sustain Level",
                default = 0.5
            },
            {
                name = "rel",
                type = "SPFLOAT",
                description ="release",
                default = 0.5
            },
        }
    },

    modtype = "module",

    description = [[Triggerable classic ADSR envelope]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Expects trigger signal."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Envelope signal output."
        },
    }

}
sptbl["talkbox"] = {

    files = {
        module = "talkbox.c",
        header = "talkbox.h",
        example = "ex_talkbox.c",
    },

    func = {
        create = "sp_talkbox_create",
        destroy = "sp_talkbox_destroy",
        init = "sp_talkbox_init",
        compute = "sp_talkbox_compute",
    },

    params = {
        optional = {
            {
                name = "quality",
                type = "SPFLOAT",
                description = "Quality of the talkbox sound. 0=lowest fidelity. 1=highest fidelity",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[A high talkbox emulation, simular to a vocoder.
This is the talkbox plugin ported from the MDA plugin suite. In many ways,
this Talkbox functions like a vocoder: it takes in a source signal (usually
speech), which then excites an excitation signal 
(usually a harmonically rich signal like a saw wave). This particular algorithm
uses linear-predictive coding (LPC), making speech intelligibility better 
than most vocoder implementations.
]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "source",
            description = "Input signal that shapes the excitation. Also known as the modulator."
        },
        {
            name = "excitation",
            description = "The signal to be excited. Also known as the carrier."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Talkbox signal output."
        },
    }

}
sptbl["tblrec"] = {

    files = {
        module = "tblrec.c",
        header = "tblrec.h",
        example = "ex_tblrec.c",
    },

    func = {
        create = "sp_tblrec_create",
        destroy = "sp_tblrec_destroy",
        init = "sp_tblrec_init",
        compute = "sp_tblrec_compute",
    },

    params = {
        mandatory = {
            {
                name = "bar",
                type = "sp_ftbl *",
                description = "",
                default = "N/A"
            },
        },
    },

    modtype = "module",

    description = [[Record a signal to an ftable.
This module will write audio-rate signals to a preallocated soundpipe ftable. 
Every time record is enabled, it will got back to index 0 overwrite any
previous information that was on it. 
]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Input signal."
        },
        {
            name = "trig",
            description = "When non-zero, will toggle recording on/off. Recording will set the playhead back to zero"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Copy of the input signal."
        }
    }

}
sptbl["tbvcf"] = {

    files = {
        module = "tbvcf.c",
        header = "tbvcf.h",
        example = "ex_tbvcf.c",
    },

    func = {
        create = "sp_tbvcf_create",
        destroy = "sp_tbvcf_destroy",
        init = "sp_tbvcf_init",
        compute = "sp_tbvcf_compute",
    },

    params = {
        optional = {
            {
                name = "fco",
                type = "SPFLOAT",
                description = "Filter cutoff frequency",
                default = 500
            },
            {
                name = "res",
                type = "SPFLOAT",
                description ="Resonance",
                default = 0.8
            },
            {
                name = "dist",
                type = "SPFLOAT",
                description ="Distortion. Value is typically 2.0; deviation from this can cause stability issues. ",
                default = 2.0
            },
            {
                name = "asym",
                type = "SPFLOAT",
                description ="Asymmetry of resonance. Value is between 0-1",
                default = 0.5
            },
        }
    },

    modtype = "module",

    description = [[Emulation of the Roland TB-303 filter]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["tdiv"] = {

    files = {
        module = "tdiv.c",
        header = "tdiv.h",
        example = "ex_tdiv.c",
    },

    func = {
        create = "sp_tdiv_create",
        destroy = "sp_tdiv_destroy",
        init = "sp_tdiv_init",
    },

    params = {
        optional = {
            {
                name = "num",
                type = "SPFLOAT",
                description = "Triggers every N times.",
                default = 2
            },
            {
                name = "offset",
                type = "SPFLOAT",
                description = "Offset amoutn.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Trigger divider.
This module will take in a trigger signal, and output a trigger signal
every N times.

For instance, when N = 3:

in: * * * * * * * * *

out *     *     *   

]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trigger",
            description = "Trigger. Should be a non-zero value"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["tenv"] = {

    files = {
        module = "tenv.c",
        header = "tenv.h",
        example = "ex_tenv.c",
    },

    func = {
        create = "sp_tenv_create",
        destroy = "sp_tenv_destroy",
        init = "sp_tenv_init",
        compute = "sp_tenv_compute",
    },

    params = {
        optional = {
            {
                name = "atk",
                type = "SPFLOAT",
                description = "Attack time, in seconds.",
                default = 0.1
            },
            {
                name = "hold",
                type = "SPFLOAT",
                description = "Hold time, in seconds.",
                default = 0.3
            },
            {
                name = "rel",
                type = "SPFLOAT",
                description = "Release time, in seconds.",
                default = 0.2
            },
        }
    },

    modtype = "module",

    description = [[Trigger based linear AHD envelope generator]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Trigger input. When non-zero, the envelope will (re)trigger."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output"
        },
    }

}
sptbl["tenv2"] = {

    files = {
        module = "tenv2.c",
        header = "tenv2.h",
        example = "ex_tenv2.c",
    },

    func = {
        create = "sp_tenv2_create",
        destroy = "sp_tenv2_destroy",
        init = "sp_tenv2_init",
        compute = "sp_tenv2_compute",
    },

    params = {
        optional = {
            {
                name = "atk",
                type = "SPFLOAT",
                description = "Attack time (in seconds).",
                default = 0.1
            },
            {
                name = "rel",
                type = "SPFLOAT",
                description ="Release time (in seconds).",
                default = 0.1
            },
        }
    },

    modtype = "module",

    description = [[Linear 2-stage Attack/Release envelope generator

    This envelope takes 2 triggers. When triggered once,
the envelope will rise to 1 according to the attack time. When triggered again, it will decay to 0 according to
the decay time.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Expects a trigger signal."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out (a unipolar envelope)."
        },
    }

}
sptbl["tenvx"] = {

    files = {
        module = "tenvx.c",
        header = "tenvx.h",
        example = "ex_tenvx.c",
    },

    func = {
        create = "sp_tenvx_create",
        destroy = "sp_tenvx_destroy",
        init = "sp_tenvx_init",
        compute = "sp_tenvx_compute",
    },

    params = {
        optional = {
            {
                name = "atk",
                type = "SPFLOAT",
                description = "Tau attack time, in seconds. Must be non-zero.",
                default = 0.1
            },
            {
                name = "hold",
                type = "SPFLOAT",
                description = "Hold time, in seconds. The duration of the gate signal.",
                default = 0.3
            },
            {
                name = "rel",
                type = "SPFLOAT",
                description = "Tau release time, in seconds. Must be non-zero.",
                default = 0.2
            },
        }
    },

    modtype = "module",

    description = [[Trigger based exponential AHD envelope generator.
    This envelope generator emulates the exponential behavior of analogue 
envelope generators by passing a gate signal (whose duration is specified via
the hold parameter) through a one-pole filter, whose filter coefficeints are
calculated in terms of tau.  
    ]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Trigger input. When non-zero, the envelope will (re)trigger."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output"
        },
    }

}
sptbl["tgate"] = {

    files = {
        module = "tgate.c",
        header = "tgate.h",
        example = "ex_tgate.c",
    },

    func = {
        create = "sp_tgate_create",
        destroy = "sp_tgate_destroy",
        init = "sp_tgate_init",
        compute = "sp_tgate_compute",
    },

    params = {
        optional = {
            {
                name = "time",
                type = "SPFLOAT",
                description = "Duration of the gate (in seconds)",
                default = 0.5
            },
        }
    },

    modtype = "module",

    description = [[A triggerable gate.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trigger",
            description = "When non-zero, will (re)trigger gate."
        },
    },

    outputs = {
        {
            name = "gate",
            description = "Gate output."
        },
    }

}
sptbl["thresh"] = {

    files = {
        module = "thresh.c",
        header = "thresh.h",
        example = "ex_thresh.c",
    },

    func = {
        create = "sp_thresh_create",
        destroy = "sp_thresh_destroy",
        init = "sp_thresh_init",
        compute = "sp_thresh_compute",
    },

    params = {
        optional = {
            {
                name = "thresh",
                type = "SPFLOAT",
                description = "Threshold level",
                default = 1.5
            },
            {
                name = "mode",
                type = "int",
                description ="Sets when to detect theshold crossings. 0 = from below. 1 = from above. 2 = from above/below",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Trigger generator for signals that cross a given threshold. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Input signal."
        },
    },

    outputs = {
        {
            name = "trig",
            description = "Produces a trigger when the input signal crosses the set threshold."
        },
    }

}
sptbl["timer"] = {

    files = {
        module = "timer.c",
        header = "timer.h",
        example = "ex_timer.c",
    },

    func = {
        create = "sp_timer_create",
        destroy = "sp_timer_destroy",
        init = "sp_timer_init",
        compute = "sp_timer_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[Tap-tempo like timer

When triggered, timer will begin an internal stopwatch until it is triggered again.
The output of the timer will be the time elapsed in seconds.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "clock",
            description = "When non-zero, will start/stop the timer."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Outputs the time of the last timer (in seconds)."
        },
    }

}
sptbl["tin"] = {

    files = { 
        module = "tin.c",
        header = "tin.h",
        example = "ex_tin.c",
    },
    
    func = {
        create = "sp_tin_create",
        destroy = "sp_tin_destroy",
        init = "sp_tin_init",
        compute = "sp_tin_compute",
    },
    
    params = {
    },
    
    modtype = "module",
    
    description = [[Similar to in, tin reads SPFLOATs (by default, this is a 4 byte binary float) from standard input every time it is triggered. behaves like a sample and hold, retaining the previous value (initial set to 0) until triggered. ]], 
    
    ninputs = 1,
    noutputs = 1,
    
    inputs = { 
        {
            name = "trig",
            description = "When non-zero, reads a single SPFLOAT from standard input."
        },
    },
    
    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["tone"] = {

	files = {
	    module = "tone.c",
	    header = "tone.h",
	    example = "ex_tone.c",
	},

	func = {
	    create = "sp_tone_create",
	    destroy = "sp_tone_destroy",
	    init = "sp_tone_init",
	    compute = "sp_tone_compute",
	},

	params = {
	    optional = {
	        {
	            name = "hp",
	            type = "SPFLOAT",
	            description = "The response curve's half power point (aka cutoff frequency).",
	            default = "1000"
	        },
	    },
	},

	modtype = "module",

	description = [[First-order recursive lowpass filter]],

	ninputs = 1,
	noutputs = 1,

	inputs = {
	    {
	        name = "in",
	        description = "Signal input."
	    },
	},

	outputs = {
	    {
	        name = "out",
	        description = "Signal Output."
	    },
	}

}
sptbl["trand"] = {

    files = {
        module = "trand.c",
        header = "trand.h",
        example = "ex_trand.c",
    },

    func = {
        create = "sp_trand_create",
        destroy = "sp_trand_destroy",
        init = "sp_trand_init",
        compute = "sp_trand_compute",
    },

    params = {
        optional = {
            {
                name = "min",
                type = "SPFLOAT",
                description ="Minimum value to use.",
                default = 0
            },
            {
                name = "max",
                type = "SPFLOAT",
                description ="Maximum value to use.",
                default = 1
            },
        }
    },

    modtype = "module",

    description = [[Triggered random number generator.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "When non-zero, will create a new random value."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["tseg"] = {

    files = {
        module = "tseg.c",
        header = "tseg.h",
        example = "ex_tseg.c",
    },

    func = {
        create = "sp_tseg_create",
        destroy = "sp_tseg_destroy",
        init = "sp_tseg_init",
        compute = "sp_tseg_compute",
    },

    params = {
        mandatory = {
            {
                name = "ibeg",
                type = "SPFLOAT",
                description = "Beginning value.",
                default = 0.001; 
            },
        },

        optional = {
            {
                name = "end",
                type = "SPFLOAT",
                description = "End parameter to go to.",
                default = 1.0
            },
            {
                name = "dur",
                type = "SPFLOAT",
                description = "duration to rise to (in seconds).",
                default = 1.0
            },
            {
                name = "type",
                type = "SPFLOAT",
                description ="The type of line, which determines slope of line",
                default = -1.0
            },
        }
    },

    modtype = "module",

    description = [[This module creates a series of line segments. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "trigger."
        },
    },

    outputs = {
        {
            name = "out",
            description = "tseg output."
        },
    }

}
sptbl["tseq"] = {

    files = {
        module = "tseq.c",
        header = "tseq.h",
        example = "ex_tseq.c",
    },

    func = {
        create = "sp_tseq_create",
        destroy = "sp_tseq_destroy",
        init = "sp_tseq_init",
        compute = "sp_tseq_compute",
    },

    params = {
        mandatory = {
            {
                name = "ft",
                type = "sp_ftbl *",
                description = "An ftable of values",
                default = "N/A"
            },
        },

        optional = {
            {
                name = "shuf",
                type = "int",
                description = "When shuf is non-zero, randomly pick a value rather than cycle through sequentially.",
                default = 0
            },
        }
    },

    modtype = "module",

    description = [[Function table looper

    TSeq runs through values in an ftable. It will change values when the trigger input is a non-zero value, and wrap around when it reaches the end.]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "trig",
            description = "Trigger."
        },
    },

    outputs = {
        {
            name = "val",
            description = "Value from current position in the ftable."
        },
    }

}
sptbl["vdelay"] = {

    files = {
        module = "vdelay.c",
        header = "vdelay.h",
        example = "ex_vdelay.c",
    },

    func = {
        create = "sp_vdelay_create",
        destroy = "sp_vdelay_destroy",
        init = "sp_vdelay_init",
        compute = "sp_vdelay_compute",
        other = {
            sp_vdelay_reset = {
                description = "Resets delay buffer and delay pointer.",
                args = {
                }
            }
        }
    },

    params = {
        mandatory = {
            {
                name = "maxdel",
                type = "SPFLOAT",
                description = "The maximum delay time, in seconds.",
                default = 1.0,
                irate = true
            },
        },

        optional = {
            {
                name = "del",
                type = "SPFLOAT",
                description = "Delay time (in seconds) that can be changed during performance. This value must not exceed the maximum delay time.",
                default = "maxdel * 0.5"
            },
            {
                name = "feedback",
                type = "SPFLOAT",
                description = "Amount of feedback in delay line. Should be in range 0-1.",
                default = "0"
            },
        }
    },

    modtype = "module",

    description = [[Delay line with cubic interpolation]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "in",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal out."
        },
    }

}
sptbl["voc"] = {

    files = {
        module = "voc.c",
        header = "voc.h",
        example = "ex_voc.c",
    },

    func = {
        create = "sp_voc_create",
        destroy = "sp_voc_destroy",
        init = "sp_voc_init",
        compute = "sp_voc_compute",
    },

    params = {
    },

    modtype = "module",

    description = [[A vocal tract physical model.

Based on the Pink Trombone algorithm by Neil Thapen, Voc implements a physical 
model of the vocal tract glottal pulse wave. The tract model is based on the 
classic Kelly-Lochbaum
segmented cylindrical 1d waveguide model, and the glottal pulse wave is a
LF glottal pulse model. 

The soundpipe source code for Voc is generated via ctangle, one half of the
literate documentation system known CWEB. The CWEB are maintained in a 
separate repository. They are hosted on github here: 
http://www.github.com/paulbatchelor/voc

This documentation is a stub. For a full overview on proper usage, consult
the "Top-level functions" section of the documented code, a copy of which
can be found at the Voc project page pbat.ch/proj/voc, or generate the PDF
from the github page described above.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "Voc output."
        },
    }

}
sptbl["vocoder"] = {

    files = {
        module = "vocoder.c",
        header = "vocoder.h",
        example = "ex_vocoder.c",
    },

    func = {
        create = "sp_vocoder_create",
        destroy = "sp_vocoder_destroy",
        init = "sp_vocoder_init",
        compute = "sp_vocoder_compute",
    },

    params = {
        mandatory = {
        },

        optional = {
            {
                name = "atk",
                type = "SPFLOAT*",
                description = "Attack time . (Range 0.001 and 0.5 seconds)",
                default = 0.1
            },
            {
                name = "rel",
                type = "SPFLOAT*",
                description = "Release time",
                default = 0.1
            },
            {
                name = "bwratio",
                type = "SPFLOAT*",
                description = "Coeffecient to adjust the bandwidth of each band (0.1 - 2)",
                default = 0.5
            },
            
        }
    },

    modtype = "module",

    description = [[16-band channel vocoder]],

    ninputs = 2,
    noutputs = 1,

    inputs = {
        {
            name = "source",
            description = "Source signal (also known as carrier)."
        },
        {
            name = "excite",
            description = "Excitation signal (also known as modulator)"
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["waveset"] = {

    files = {
        module = "waveset.c",
        header = "waveset.h",
        example = "ex_waveset.c",
    },

    func = {
        create = "sp_waveset_create",
        destroy = "sp_waveset_destroy",
        init = "sp_waveset_init",
        compute = "sp_waveset_compute",
    },

    params = {
        mandatory = {
            {
                name = "ilen",
                type = "SPFLOAT",
                description = "Length of buffer (in seconds).",
                default = 1.0 
            }
        },

        optional = {
            {
                name = "rep",
                type = "SPFLOAT",
                description = "Number of repeats.",
                default = 1.5
            },
        }
    },

    modtype = "module",

    description = [[Simple Time-stretching from repeating wavecyles
This module looks for zero-crossings and repeats them by a integer factor.
While a crude means for time stretching, it is a very aesthetically pleasing
effect to use on sounds and often produces a "wet" sound.

The waveset algorithm was originally created by Trevor Wishart for the Composer
Desktop Project (CDP), and was then ported to Csound. 
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Signal input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "signal output."
        },
    }

}
sptbl["wavin"] = {

    files = {
        module = "wavin.c",
        header = "wavin.h",
        example = "ex_wavin.c",
    },

    func = {
        create = "sp_wavin_create",
        destroy = "sp_wavin_destroy",
        init = "sp_wavin_init",
        compute = "sp_wavin_compute",
    },

    params = {
        mandatory = {
            {
                name = "filename",
                type = "const char *",
                description = "Filename",
                default = "N/A"
            },
        },

    },

    modtype = "module",

    description = [[Reads a mono WAV file.

This module reads a mono WAV file from disk. It uses the public-domain 
dr_wav library for decoding, so it can be a good substitute for libsndfile.
]],

    ninputs = 0,
    noutputs = 1,

    inputs = {
    },

    outputs = {
        {
            name = "out",
            description = "output signal."
        },
    }

}
sptbl["wavout"] = {

    files = {
        module = "wavout.c",
        header = "wavout.h",
        example = "ex_wavout.c",
    },

    func = {
        create = "sp_wavout_create",
        destroy = "sp_wavout_destroy",
        init = "sp_wavout_init",
        compute = "sp_wavout_compute",
    },

    params = {
        mandatory = {
            {
                name = "filename",
                type = "const char*",
                description = "The filename of the output file.",
                default = "N/A"
            }
        },
    },

    modtype = "module",

    description = [[Writes a mono signal to a WAV file.
This module uses the public-domain dr_wav library to write WAV files
to disk. This module is ideal for instances where GPL-licensed libsndfile 
cannot be used for legal reasons.
]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Mono input signal."
        },
    },

    outputs = {
        {
            name = "out",
            description = "A passthrough signal: a copy of the input signal."
        },
    }

}
sptbl["wpkorg35"] = {

    files = {
        module = "wpkorg35.c",
        header = "wpkorg35.h",
        example = "ex_wpkorg35.c",
    },

    func = {
        create = "sp_wpkorg35_create",
        destroy = "sp_wpkorg35_destroy",
        init = "sp_wpkorg35_init",
        compute = "sp_wpkorg35_compute",
    },

    params = {
        optional = {
            {
                name = "cutoff",
                type = "SPFLOAT",
                description = "Filter cutoff",
                default = 1000
            },
            {
                name = "res",
                type = "SPFLOAT",
                description ="Filter resonance (should be between 0-2)",
                default = 1.0
            },
            {
                name = "saturation",
                type = "SPFLOAT",
                description ="Filter saturation.",
                default = 0.0
            },
        }
    },

    modtype = "module",

    description = [[ Analogue model of the Korg 35 Lowpass Filter

Original port done by Will Pirkle:
http://www.willpirkle.com/Downloads/AN-5Korg35_V3.pdf

]],

    ninputs = 1,
    noutputs = 1,

    inputs = {
        {
            name = "input",
            description = "Audio input."
        },
    },

    outputs = {
        {
            name = "out",
            description = "Signal output."
        },
    }

}
sptbl["zitarev"] = {

    files = {
        module = "zitarev.c",
        header = "zitarev.h",
        example = "ex_zitarev.c",
    },

    func = {
        create = "sp_zitarev_create",
        destroy = "sp_zitarev_destroy",
        init = "sp_zitarev_init",
        compute = "sp_zitarev_compute",
    },

    params = {
        optional = {
            {
                name = "in_delay",
                type = "SPFLOAT*",
                description = "Delay in ms before reverberation begins.",
                default = 60,
                min = 10,
                max = 100 
                
            },
            {
                name = "lf_x",
                type = "SPFLOAT*",
                description ="Crossover frequency separating low and middle frequencies (Hz).",
                default = 200,
                min = 200,
                max = 1000
            },
            {
                name = "rt60_low",
                type = "SPFLOAT*",
                description ="Time (in seconds) to decay 60db in low-frequency band.",
                default = 3.0,
                min = 1,
                max = 8 
            },
            {
                name = "rt60_mid",
                type = "SPFLOAT*",
                description ="Time (in seconds) to decay 60db in mid-frequency band.",
                default = 2.0,
                min = 1,
                max = 8
            },
            {
                name = "hf_damping",
                type = "SPFLOAT*",
                description ="Frequency (Hz) at which the high-frequency T60 is half the middle-band's T60.",
                default = 6000.0,
                min = 1500,
                max = 0.5 * 48000
            },
            {
                name = "eq1_freq",
                type = "SPFLOAT*",
                description ="Center frequency of second-order Regalia Mitra peaking equalizer section 1.",
                default = 315.0,
                min = 40,
                max = 2500
            },
            {
                name = "eq1_level",
                type = "SPFLOAT*",
                description ="Peak level in dB of second-order Regalia-Mitra peaking equalizer section 1",
                default = 0,
                min = -15,
                max = 15
            },
            {
                name = "eq2_freq",
                type = "SPFLOAT*",
                description ="Center frequency of second-order Regalia Mitra peaking equalizer section 2.",
                default = 1500.0,
                min = 160,
                max = 10000
            },
            {
                name = "eq2_level",
                type = "SPFLOAT*",
                description ="Peak level in dB of second-order Regalia-Mitra peaking equalizer section 2",
                default = 0 ,
                min = -15,
                max = 15
            },
            {
                name = "mix",
                type = "SPFLOAT*",
                description ="0 = all dry, 1 = all wet",
                default = 1,
                min = 0,
                max = 1
            },
            {
                name = "level",
                type = "SPFLOAT*",
                description ="Output scale factor (in dB).",
                default = -20,
                min = -70,
                max = 40
            }
        }
    },

    modtype = "module",

    description = [[8 FDN stereo zitareverb algorithm, imported from Faust.]],

    ninputs = 2,
    noutputs = 2,

    inputs = {
        {
            name = "input_1",
            description = "First input."
        },
        {
            name = "input_2",
            description = "Second input."
        },
    },

    outputs = {
        {
            name = "out_1",
            description = "Channel 1 output. Most likely for the left channel."
        },
        {
            name = "out_2",
            description = "Channel 2 output. Mose likely for the right channel."
        },
    }

}
