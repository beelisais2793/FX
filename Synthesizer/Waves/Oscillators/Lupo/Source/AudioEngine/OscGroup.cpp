/*
  ==============================================================================

    OscGroup.cpp
    Created: 14 Mar 2020 11:53:07am
    Author:  mpue

  ==============================================================================
*/

#include "OscGroup.h"

OscGroup::OscGroup() {
}

OscGroup::~OscGroup() {
}

void OscGroup::setModulator(Modulator * mod)
{
	for (int i = 0; i < targets.size();i++) {
		targets.at(i)->setModulator(mod);
	}
}

void OscGroup::addTarget(ModTarget * target)
{
	targets.push_back(target);
}

std::vector<ModTarget*> OscGroup::getTargets()
{
	return targets;
}


