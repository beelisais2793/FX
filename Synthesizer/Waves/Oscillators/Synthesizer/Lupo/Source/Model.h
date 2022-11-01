/*
  ==============================================================================

	Model.h
	Created: 19 Mar 2020 7:08:09pm
	Author:  mpue

  ==============================================================================
*/

#pragma once

#include "AudioEngine/MultimodeOscillator.h"

class Model {

public:

	Model() {

		cutoff1 = 18000.0f;
		resonance1 = 0.5f;
		envAmt1 = 0.5f;

		cutoff2 = 18000.0f;
		resonance2 = 0.5f;
		envAmt2 = 0.5f;

		mainVolume = 2.0f;
		osc1Pitch = 0;
		osc1Fine = 0;
		osc1Volume = 1.0f;
		osc1Pan = 0;
		osc1Shape = 0.0f;
		osc2Pitch = 0;
		osc2Fine = 0;
		osc2Volume = 1.0f;
		osc2Pan = 0;
		osc2Shape = 0.0f;
		osc3Pitch = 0;
		osc3Fine = 0;
		osc3Pan = 0;
		osc3Volume = 1.0f;
		osc3Shape = 0.0f;
		osc4Pitch = 0;
		osc4Fine = 0;
		osc4Volume = 1.0f;
		osc4Pan = 0;
		osc4Shape = 0.0f;



		dlyTimeLeft = 500;
		dlyTimeRight = 375;
		dlyMix = 0;
		dlyFeedback = 0.25;

		chrDelay = 0.1f;
		chrDelay = 0.1f;
		chrFeedback = 0.1f;
		chrMix = 0;
	}

	~Model() {}


	float cutoff1;
	float resonance1;
	float envAmt1;

	float cutoff2;
	float resonance2;
	float envAmt2;

	float mainVolume;
	float ampAttack = 0;
	float ampDecay = 3;
	float ampSustain = 0.1f;
	float ampRelease = 0.1f;

	float fltAttack = 0;
	float fltDecay = 2;
	float fltSustain = 0.1f;
	float fltRelease = 0.1f;

	float auxAttack = 0;
	float auxDecay = 1;
	float auxSustain = 0.1f;
	float auxRelease = 0.1f;

	float osc1Pitch;
	float osc1Fine;
	float osc1Volume;
	float osc1Pan;

	float osc1Shape;

	float osc2Pitch;
	float osc2Fine;
	float osc2Volume;
	float osc2Pan;
	float osc2Shape;
	bool osc1Sync = false;

	float osc3Pitch;
	float osc3Fine;
	float osc3Volume;
	float osc3Pan;
	float osc3Shape;

	float osc4Pitch;
	float osc4Fine;
	float osc4Volume;
	float osc4Pan;
	float osc4Shape;

	int dlyTimeLeft;
	int dlyTimeRight;
	float dlyFeedback;
	float dlyMix;

	float rvbRoomSize = 0.5f;     /**< Room size, 0 to 1.0, where 1.0 is big, 0 is small. */
	float rvbDdamping = 0.5f;     /**< Damping, 0 to 1.0, where 0 is not damped, 1.0 is fully damped. */
	float rvbWetLevel = 0.0f;    /**< Wet level, 0 to 1.0 */
	float rvbDryLevel = 1.0f;     /**< Dry level, 0 to 1.0 */
	float rvbWidth = 1.0f;     /**< Reverb width, 0 to 1.0, where 1.0 is very wide. */
	float rvbFreezeMode = 0.0f;
	float reverbEnabled = false;

	float chrDelay;
	float chrModulation;
	float chrFeedback;
	float chrMix;

	float distMode = 0.0f;
	float distDrive = 0.0f;
	float distMix = 0.0f;

	float modAmount[6] = { 0.0f };

	bool osc1Enabled = true;
	bool osc2Enabled = false;
	bool osc3Enabled = false;
	bool osc4Enabled = false;


	int getModSource(int source) {
		return this->modSource[source];
	}

	int getModTarget(int target) {
		return this->modTarget[target];
	}

	void setModSource(int index, int source) {
		modSource[index] = source;
	}

	void setModTarget(int index, int target) {
		modTarget[index] = target;
	}

private:

	int modSource[6] = { 0 };
	int modTarget[6] = { 0 };
};