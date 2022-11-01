#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "AudioEngine/LupoSynth.h"
#include "MainUI.h"
#include <map>


class AttachmentFactory
{
public:
	AttachmentFactory(LupoAudioProcessor* processor, LupoSynth* lupo) {
		this->processor = processor;
		this->lupo = lupo;
		this->attachments = new vector<AudioProcessorValueTreeState::SliderAttachment*>();
		this->buttonAttachments = new vector<AudioProcessorValueTreeState::ButtonAttachment*>();
		this->comboAttachments = new vector<AudioProcessorValueTreeState::ComboBoxAttachment*>();
	}

	void initState() {		
		processor->getValueTreeState()->state = ValueTree(Identifier("default"));
	}

	void createParam(String name, String label, float min, float max, float defaultValue) {
		processor->getValueTreeState()->createAndAddParameter(name, name, label, NormalisableRange<float>(min, max), defaultValue, nullptr, nullptr);
		processor->getValueTreeState()->addParameterListener(name, lupo);
	}
	void  createSliderAttachment(String name,Slider* comp) {
		AudioProcessorValueTreeState::SliderAttachment* att = new AudioProcessorValueTreeState::SliderAttachment(*processor->getValueTreeState(), name, *comp);
		attachments->push_back(att);
	}

	void  createComboAttachment(String name, ComboBox* comp) {
		AudioProcessorValueTreeState::ComboBoxAttachment* att = new AudioProcessorValueTreeState::ComboBoxAttachment(*processor->getValueTreeState(), name, *comp);
		comboAttachments->push_back(att);
	}

	void  createButtonAttachment(String name, Button* comp) {
		AudioProcessorValueTreeState::ButtonAttachment* att = new AudioProcessorValueTreeState::ButtonAttachment(*processor->getValueTreeState(), name, *comp);
		buttonAttachments->push_back(att);
	}

	~AttachmentFactory() {
		for (int i = 0; i < attachments->size(); i++) {
			delete attachments->at(i);
		}
		delete attachments;
		for (int i = 0; i < buttonAttachments->size(); i++) {
			delete buttonAttachments->at(i);
		}
		delete buttonAttachments;
		for (int i = 0; i < comboAttachments->size(); i++) {
			delete comboAttachments->at(i);
		}
		delete comboAttachments;
	};

	StringArray AttachmentFactory::getSliderParams()
	{
		return {
			"cutoff1","resonance1","mainVolume","envAmt1",
			"cutoff2","resonance2","envAmt2",
			"ampAttack","ampDecay","ampSustain","ampRelease",
			"fltAttack", "fltDecay", "fltSustain", "fltRelease",
			"auxAttack1", "auxDecay1","auxSustain1","auxRelease1",
			"auxAttack2", "auxDecay2","auxSustain2","auxRelease2",
			"lfo1Shape", "lfo1Speed","lfo1Amount",
			"lfo2Shape", "lfo2Speed","lfo2Amount",
			"lfo3Shape", "lfo3Speed","lfo3Amount",
			"osc1Pitch","osc1Fine","osc1Volume","osc1Pan",
			"osc2Pitch","osc2Fine","osc2Volume","osc2Pan"
			"osc3Pitch","osc3Fine","osc3Volume","osc3Pan",
			"osc4Pitch","osc4Fine","osc4Volume","osc4Pan",
			"dlyTimeLeft", "dlyTimeRight", "dlyFeedback","dlyMix",
			"rvbRoomSize", "rvbDdamping", "rvbWetLevel", "rvbDryLevel", "rvbWidth", "rvbFreezeMode",
			"chrDelay", "chrModulation", "chrFeedback", "chrMix",
			"filterMode","cutoffLink"
		};
	}
private:

	vector<AudioProcessorValueTreeState::SliderAttachment*>* attachments;
	vector<AudioProcessorValueTreeState::ButtonAttachment*>* buttonAttachments;
	vector<AudioProcessorValueTreeState::ComboBoxAttachment*>* comboAttachments;

	class ParamDescriptor {
	public:
		ParamDescriptor(NormalisableRange<float> range, float defaultValue) {
			this->range = range;
			this->defaultValue = defaultValue;
		}
	private:
		NormalisableRange<float> range;
		float defaultValue;
	};

	/*


	MultimodeOscillator::OscMode osc1Shape;
	MultimodeOscillator::OscMode osc2Shape;
	MultimodeOscillator::OscMode osc3Shape;
	MultimodeOscillator::OscMode osc4Shape;
*/

	LupoAudioProcessor* processor;
	LupoSynth* lupo;
};

