/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
FmodSynthAudioProcessor::FmodSynthAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	: AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
		.withInput("Input", AudioChannelSet::stereo(), true)
#endif
		.withOutput("Output", AudioChannelSet::stereo(), true)
#endif
	),
	attackTime(0.1f),
	tree(*this, nullptr, "Parameters", createParameterLayout()/*{ std::make_unique<AudioParameterFloat>("attack", "Attack", 0.01, 8.0, 0.1),
										std::make_unique<AudioParameterFloat>("decay", "Decay", 0.01, 4.0, 0.1),
										std::make_unique<AudioParameterFloat>("sustain", "Sustain", 0.0, 1.0, 0.8),
										std::make_unique<AudioParameterFloat>("release", "Release", 0.01, 4.0, 0.5),
										std::make_unique<AudioParameterInt>("waveType", "Carrier Osc", 0, 3, 0),
										std::make_unique<AudioParameterInt>("modType", "Modulator Osc", 0, 3, 0),
										std::make_unique<AudioParameterInt>("modIndex", "Modulation Index", 0, 1500, 0),
										std::make_unique<AudioParameterInt>("filterType", "FilterType", 0, 2, 0) ,
										std::make_unique<AudioParameterInt>("filterCutoff", "FilterCutoff", 20, 10000, 660) ,
										std::make_unique<AudioParameterFloat>("filterResonance", "FilterResonance", 1.0, 5.0, 1.0),
										std::make_unique<AudioParameterInt>("lfoType", "LFO", 0, 3, 0),
										std::make_unique<AudioParameterFloat>("lfoRate", "Rate", 1, 12000, 1),
										std::make_unique<AudioParameterFloat>("lfoAmount", "Amount", 0, 1, 0),
										std::make_unique<AudioParameterFloat>("masterGain", "Gain", 0, 1, 0.1) }*/)
#endif
{

	// Envelope parameters
	//NormalisableRange<float> attackParam(0.01, 4.0, 0.01);
	//NormalisableRange<float> decayParam(0.01, 4.0, 0.01);
	//NormalisableRange<float> sustainParam(0.0, 1.0, 0.01);
	//NormalisableRange<float> releaseParam(0.01, 4.0, 0.01);
	//tree.createAndAddParameter("attack", "Attack", "", attackParam, 0.1, nullptr, nullptr);
	//tree.createAndAddParameter("decay", "Decay", "", decayParam, 0.0, nullptr, nullptr);
	//tree.createAndAddParameter("sustain", "Sustain", "", sustainParam, 1, nullptr, nullptr);
	//tree.createAndAddParameter("release", "Release", "", releaseParam, .5, nullptr, nullptr);

	// oscillator type parameter
	// NormalisableRange<float> waveTypeParam(0, 3, 1);
	// tree.createAndAddParameter("wavetype", "Oscillator", "", waveTypeParam, 1, nullptr, nullptr);

	// filter parameters
	//NormalisableRange<float> filterTypeParam(0, 2);
	//NormalisableRange<float> filterCutoffParam(20.0, 10000.0);
	//NormalisableRange<float> filterResonanceParam(1.0, 5.0);
	//tree.createAndAddParameter("filterType", "FilterType", "", filterTypeParam, 0, nullptr, nullptr);
	//tree.createAndAddParameter("filterCutoff", "FilterCutoff", "", filterCutoffParam, 660, nullptr, nullptr);
	//tree.createAndAddParameter("filterResonance", "FilterResonance", "", filterResonanceParam, 1, nullptr, nullptr);

	// addParameters creates parameters that control different attributes of the synth
	// these parameters can be controlled in any good DAW even if there is no GUI for it
	//addParameter(modulationFrequencyParameter = new AudioParameterFloat("ModFreq",
	//	"Modulation Frequency",
	//	1, 4000, 1));

	//addParameter(modulationIndexParameter = new AudioParameterFloat("ModIndex", "Modulation Index",
	//	0, 1500, 0));

	// addParameter(synthGainParameter = new AudioParameterFloat("SynthGain", "Gain", 0, 1, .5));

	// add voices
	synth.clearVoices();
	mod = new ModMatrix;
	for (int i = 0; i < 7; i++)
		synth.addVoice(new SynthVoice());

	// add synth sound
	synth.clearSounds();
	synth.addSound(new SynthSound());

}

FmodSynthAudioProcessor::~FmodSynthAudioProcessor()
{
}

//==============================================================================

AudioProcessorValueTreeState::ParameterLayout FmodSynthAudioProcessor::createParameterLayout()
{
	std::vector<std::unique_ptr<RangedAudioParameter>> params;

	auto attackRange = NormalisableRange<float>(0.01, 8.0, 0.01);
	attackRange.setSkewForCentre(1);
	auto decayRange = NormalisableRange<float>(0.01, 8.0, 0.01);
	decayRange.setSkewForCentre(1);
	auto releaseRange = NormalisableRange<float>(0.01, 8.0, 0.01);
	releaseRange.setSkewForCentre(1);

	// ADSR

	auto attackParam = std::make_unique<AudioParameterFloat>("attack-A", "Attack A", attackRange, 0.1);
	auto decayParam = std::make_unique<AudioParameterFloat>("decay-A", "Decay A", decayRange, 0.1);
	auto sustainParam = std::make_unique<AudioParameterFloat>("sustain-A", "Sustain A", 0.0, 1.0, 0.8);
	auto releaseParam = std::make_unique<AudioParameterFloat>("release-A", "Release A", releaseRange, 0.5);

	auto attackBParam = std::make_unique<AudioParameterFloat>("attack-B", "Attack B", attackRange, 0.1);
	auto decayBParam = std::make_unique<AudioParameterFloat>("decay-B", "Decay B", decayRange, 0.1);
	auto sustainBParam = std::make_unique<AudioParameterFloat>("sustain-B", "Sustain B", 0.0, 1.0, 0.8);
	auto releaseBParam = std::make_unique<AudioParameterFloat>("release-B", "Release B", releaseRange, 0.5);

	auto attackCParam = std::make_unique<AudioParameterFloat>("attack-C", "Attack C", attackRange, 0.1);
	auto decayCParam = std::make_unique<AudioParameterFloat>("decay-C", "Decay C", decayRange, 0.1);
	auto sustainCParam = std::make_unique<AudioParameterFloat>("sustain-C", "Sustain C", 0.0, 1.0, 0.8);
	auto releaseCParam = std::make_unique<AudioParameterFloat>("release-C", "Release C", releaseRange, 0.5);

	auto attackDParam = std::make_unique<AudioParameterFloat>("attack-D", "Attack D", attackRange, 0.1);
	auto decayDParam = std::make_unique<AudioParameterFloat>("decay-D", "Decay D", decayRange, 0.1);
	auto sustainDParam = std::make_unique<AudioParameterFloat>("sustain-D", "Sustain D", 0.0, 1.0, 0.8);
	auto releaseDParam = std::make_unique<AudioParameterFloat>("release-D", "Release D", releaseRange, 0.5);

	// FILTER
	auto filterTypeParam = std::make_unique<AudioParameterInt>("filterType", "FilterType", 0, 2, 0);
	auto filterRange = NormalisableRange<float>(20.0, 20000.0, 1);
	filterRange.setSkewForCentre(1000.0);
	auto filterCutoffParam = std::make_unique<AudioParameterFloat>("filterCutoff", "FilterCutoff", filterRange, 660, " hz");
	auto filterResParam = std::make_unique<AudioParameterFloat>("filterResonance", "FilterResonance", 1.0, 5.0, 1.0);

	// LFO
	auto lfoTypeParam = std::make_unique<AudioParameterInt>("lfoType", "LFO", 0, 4, 0);

	auto lfoRateRange = NormalisableRange<float>(1, 12000, 1);
	lfoRateRange.setSkewForCentre(1000);
	auto lfoRateParam = std::make_unique<AudioParameterFloat>("lfoRate", "Rate", lfoRateRange, 1);
	auto lfoAmountParam = std::make_unique<AudioParameterFloat>("lfoAmount", "Amount", 0, 1, 0);

	// GAIN
	auto gainParam = std::make_unique<AudioParameterFloat>("masterGain", "Gain", 0, 1, 0.4);

	// OPERATOR A
	auto oscRangeA = NormalisableRange<float>(20.0, 20000.0, 1);
	oscRangeA.setSkewForCentre(1000);
	auto detuneRange = NormalisableRange<float>(-2400.0, 2400.0, 100.0);
	detuneRange.symmetricSkew;
	auto opAoscParam		= std::make_unique<AudioParameterInt>("oscTypeA", "Oscillator A", 0, 3, 0);
	auto opAindexParam		= std::make_unique<AudioParameterInt>("modIndexA", "Index A", 0, 3000, 0);
	auto opAfreqParam		= std::make_unique<AudioParameterFloat>("modFreqA", "Frequency A", oscRangeA, 330);
	auto opAfixedFreqParam	= std::make_unique<AudioParameterFloat>("fixedFreqA", "Fixed Frequency A", 0, 1, 0);
	auto opAfreqRatioParam	= std::make_unique<AudioParameterFloat>("freqRatioA", "Ratio A", detuneRange, 0);
	
	// OPERATOR B
	auto oscRangeB = NormalisableRange<float>(20.0, 20000.0, 1);
	oscRangeB.setSkewForCentre(1000);
	auto opBoscParam = std::make_unique<AudioParameterInt>("oscTypeB", "Oscillator B", 0, 3, 0);
	auto opBindexParam = std::make_unique<AudioParameterInt>("modIndexB", "Index B", 0, 3000, 0);
	auto opBfreqParam = std::make_unique<AudioParameterFloat>("modFreqB", "Frequency B", oscRangeB, 330);
	auto opBfixedFreqParam = std::make_unique<AudioParameterFloat>("fixedFreqB", "Fixed Frequency B", 0, 1, 0);
	auto opBfreqRatioParam = std::make_unique<AudioParameterFloat>("freqRatioB", "Ratio B", detuneRange, 0);
	
	// OPERATOR C
	auto oscRangeC = NormalisableRange<float>(20.0, 20000.0, 1);
	oscRangeC.setSkewForCentre(1000);
	auto opCoscParam = std::make_unique<AudioParameterInt>("oscTypeC", "Oscillator C", 0, 3, 0);
	auto opCindexParam = std::make_unique<AudioParameterInt>("modIndexC", "Index C", 0, 3000, 0);
	auto opCfreqParam = std::make_unique<AudioParameterFloat>("modFreqC", "Frequency C", oscRangeC, 330);
	auto opCfixedFreqParam = std::make_unique<AudioParameterFloat>("fixedFreqC", "Fixed Frequency C", 0, 1, 0);
	auto opCfreqRatioParam = std::make_unique<AudioParameterFloat>("freqRatioC", "Ratio C", detuneRange, 0);
	
	// OPERATOR D
	auto oscRangeD = NormalisableRange<float>(20.0, 20000.0, 1);
	oscRangeD.setSkewForCentre(1000);
	auto opDoscParam = std::make_unique<AudioParameterInt>("oscTypeD", "Oscillator D", 0, 3, 0);
	auto opDindexParam = std::make_unique<AudioParameterInt>("modIndexD", "Index D", 0, 3000, 0);
	auto opDfreqParam = std::make_unique<AudioParameterFloat>("modFreqD", "Frequency D", oscRangeD, 330);
	auto opDfixedFreqParam = std::make_unique<AudioParameterFloat>("fixedFreqD", "Fixed Frequency D", 0, 1, 0);
	auto opDfreqRatioParam = std::make_unique<AudioParameterFloat>("freqRatioD", "Ratio D", detuneRange, 0);

	auto algorithmParam = std::make_unique<AudioParameterInt>("algorithmSelection", "Algorithm", 0, 5, 0);

	// Envelope
	params.push_back(std::move(attackParam));
	params.push_back(std::move(decayParam));
	params.push_back(std::move(sustainParam));
	params.push_back(std::move(releaseParam));

	params.push_back(std::move(attackBParam));
	params.push_back(std::move(decayBParam));
	params.push_back(std::move(sustainBParam));
	params.push_back(std::move(releaseBParam));

	params.push_back(std::move(attackCParam));
	params.push_back(std::move(decayCParam));
	params.push_back(std::move(sustainCParam));
	params.push_back(std::move(releaseCParam));

	params.push_back(std::move(attackDParam));
	params.push_back(std::move(decayDParam));
	params.push_back(std::move(sustainDParam));
	params.push_back(std::move(releaseDParam));

	// Filter
	params.push_back(std::move(filterTypeParam));
	params.push_back(std::move(filterCutoffParam));
	params.push_back(std::move(filterResParam));

	// LFO
	params.push_back(std::move(lfoTypeParam));
	params.push_back(std::move(lfoRateParam));
	params.push_back(std::move(lfoAmountParam));
	params.push_back(std::move(gainParam));

	// Operator
	params.push_back(std::move(opAoscParam));
	params.push_back(std::move(opAindexParam));
	params.push_back(std::move(opAfreqParam));
	params.push_back(std::move(opAfixedFreqParam));
	params.push_back(std::move(opAfreqRatioParam));

	params.push_back(std::move(opBoscParam));
	params.push_back(std::move(opBindexParam));
	params.push_back(std::move(opBfreqParam));
	params.push_back(std::move(opBfixedFreqParam));
	params.push_back(std::move(opBfreqRatioParam));

	params.push_back(std::move(opCoscParam));
	params.push_back(std::move(opCindexParam));
	params.push_back(std::move(opCfreqParam));
	params.push_back(std::move(opCfixedFreqParam));
	params.push_back(std::move(opCfreqRatioParam));

	params.push_back(std::move(opDoscParam));
	params.push_back(std::move(opDindexParam));
	params.push_back(std::move(opDfreqParam));
	params.push_back(std::move(opDfixedFreqParam));
	params.push_back(std::move(opDfreqRatioParam));

	params.push_back(std::move(algorithmParam));

	return { params.begin(), params.end() };

}

const String FmodSynthAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool FmodSynthAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool FmodSynthAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool FmodSynthAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double FmodSynthAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int FmodSynthAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int FmodSynthAudioProcessor::getCurrentProgram()
{
    return 0;
}

void FmodSynthAudioProcessor::setCurrentProgram (int index)
{
}

const String FmodSynthAudioProcessor::getProgramName (int index)
{
    //return {};
	return String();
}

void FmodSynthAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void FmodSynthAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

	ignoreUnused(samplesPerBlock);
	lastSampleRate = sampleRate;
	synth.setCurrentPlaybackSampleRate(lastSampleRate);
	maxiSettings::setup(lastSampleRate, getTotalNumOutputChannels(), samplesPerBlock);

	dsp::ProcessSpec spec;
	spec.sampleRate = sampleRate;
	spec.maximumBlockSize = samplesPerBlock;
	spec.numChannels = getTotalNumOutputChannels();

	stateVariableFilter.reset(); 
	stateVariableFilter.prepare(spec);
	updateFilter();

	//lfoValue = LowFrequencyOscillator->setLfo();
}

void FmodSynthAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool FmodSynthAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

void FmodSynthAudioProcessor::updateFilter()
{
	int menuChoice = *tree.getRawParameterValue("filterType");
	float cutoff = *tree.getRawParameterValue("filterCutoff");
	float res = *tree.getRawParameterValue("filterResonance");
	cutoff += (lfoValue * 200);
	if (cutoff < 20)
		cutoff = 20;
	else if (cutoff > 20000)
		cutoff = 20000;

	switch (menuChoice)
	{
	case 0:
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::lowPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);
		break;
	case 1:
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::highPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);
		break;
	case 2:
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::bandPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);
		break;
	default:
		stateVariableFilter.state->type = dsp::StateVariableFilter::Parameters<float>::Type::bandPass;
		stateVariableFilter.state->setCutOffFrequency(lastSampleRate, cutoff, res);
		break;
	}
}

void FmodSynthAudioProcessor::updateParameters()
{
	// update parameters for processes
}

void FmodSynthAudioProcessor::process(dsp::ProcessContextReplacing<float> context)
{
	// do processing here and output
	
}


void FmodSynthAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	/*
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
    }
	*/
	mod->getLfoParams(tree.getRawParameterValue("lfoType"),
					  tree.getRawParameterValue("lfoRate"),
					  tree.getRawParameterValue("lfoAmount"));
	lfoValue = mod->setLfo();
	// for each voice
	for (int i = synth.getNumVoices(); --i >= 0;)
	{
		// if it casts successfully as a synth voice
		if ((voice = dynamic_cast<SynthVoice*>(synth.getVoice(i))))
		{
			//	voice->setParameters(double(modulationFrequencyParameter->get()),	// setParameters() is located in SynthVoice
			//		double(modulationIndexParameter->get()));

			voice->setADSRSampleRate(lastSampleRate);

			// Operator
			voice->getOperatorAParams(tree.getRawParameterValue("oscTypeA"),
									 tree.getRawParameterValue("modIndexA"), 
									 tree.getRawParameterValue("modFreqA"), 
									 tree.getRawParameterValue("fixedFreqA"), 
									 tree.getRawParameterValue("freqRatioA"));

			voice->getOperatorBParams(tree.getRawParameterValue("oscTypeB"),
									 tree.getRawParameterValue("modIndexB"),
									 tree.getRawParameterValue("modFreqB"),
									 tree.getRawParameterValue("fixedFreqB"),
									 tree.getRawParameterValue("freqRatioB"));

			voice->getOperatorCParams(tree.getRawParameterValue("oscTypeC"),
									  tree.getRawParameterValue("modIndexC"),
									  tree.getRawParameterValue("modFreqC"),
									  tree.getRawParameterValue("fixedFreqC"),
									  tree.getRawParameterValue("freqRatioC"));

			voice->getOperatorDParams(tree.getRawParameterValue("oscTypeD"),
									  tree.getRawParameterValue("modIndexD"),
									  tree.getRawParameterValue("modFreqD"),
									  tree.getRawParameterValue("fixedFreqD"),
									  tree.getRawParameterValue("freqRatioD"));
			// Envelope
			voice->getEnvelopeParams(tree.getRawParameterValue("attack-A"),			// set envelope parameters
									 tree.getRawParameterValue("decay-A"),
									 tree.getRawParameterValue("sustain-A"),
									 tree.getRawParameterValue("release-A"));

			voice->getEnvelopeBParams(tree.getRawParameterValue("attack-B"),			// set envelope parameters
									  tree.getRawParameterValue("decay-B"),
									  tree.getRawParameterValue("sustain-B"),
									  tree.getRawParameterValue("release-B"));

			voice->getEnvelopeCParams(tree.getRawParameterValue("attack-C"),			// set envelope parameters
									  tree.getRawParameterValue("decay-C"),
									  tree.getRawParameterValue("sustain-C"),
									  tree.getRawParameterValue("release-C"));

			voice->getEnvelopeDParams(tree.getRawParameterValue("attack-D"),			// set envelope parameters
									  tree.getRawParameterValue("decay-D"),
									  tree.getRawParameterValue("sustain-D"),
									  tree.getRawParameterValue("release-D"));

			voice->getAlgorithm(tree.getRawParameterValue("algorithmSelection"));

			//voice->getFilterParams(tree.getRawParameterValue("filterType"),
			//					   tree.getRawParameterValue("filterCutoff"),
			//					   tree.getRawParameterValue("filterResonance"));

			voice->getGain(tree.getRawParameterValue("masterGain"));

		}
	}

	buffer.clear();
	synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());

	updateFilter();
	dsp::AudioBlock<float> block(buffer);
	stateVariableFilter.process(dsp::ProcessContextReplacing<float>(block));	// route buffer through filter

}

//==============================================================================
bool FmodSynthAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* FmodSynthAudioProcessor::createEditor()
{
    return new FmodSynthAudioProcessorEditor (*this);
}

//==============================================================================
void FmodSynthAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
	MemoryOutputStream stream(destData, true);
}

void FmodSynthAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new FmodSynthAudioProcessor();
}
