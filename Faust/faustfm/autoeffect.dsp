    adapt(1,1) = _;
    adapt(2,2) = _,_;
    adapt(1,2) = _ <: _,_;
    adapt(2,1) = _,_ :> _;
    adaptor(F,G) = adapt(outputs(F),inputs(G));
    process = adaptor(library("FM_Synthesizer.dsp").process, library("FM_Synthesizer.dsp").effect) : library("FM_Synthesizer.dsp").effect;
