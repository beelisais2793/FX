# NoiseGenerator
The noise generator is a simple plugin which generates either white, pink or brown noise. In addition, there is a DC blocking filter and a smoothing filter which can be applied to the output as well. This plugin is an effect, so any inputs passing into this plugin will be mixed with the noise level at the output.

The Builds folder contains the VST as well as a standalone version for Mac OS. The Windows standalone executable is an earlier version of the program and produces only white and pink noise and no filtering.

The Plugin folder contains a header file, NoiseSource.h, which contains all of the noise generating algorithms as well as the filters. These algorithms make some use of the JUCE framework for noise generation, but otherwise need not be specific to audio applications. The outputs of the generator are in the range [-1,1], as is suitable for audio.

This program has been developed using the JUCE framework https://juce.com/
