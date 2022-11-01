/*
  ==============================================================================

    ModMatrix.cpp
    Created: 10 Dec 2016 10:18:41am
    Author:  Matthias Pueski

  ==============================================================================
*/

#include "ModMatrix.h"

ModMatrix::ModMatrix() {
    this->modSources = new std::map<int,String>();
    this->modTargets = new std::map<int,String>();	
	this->dummy = new DummyModulator();
	this->modulations = new Modulation*[6];
	for (int i = 0; i < 6; i++) {
		modulations[i] = new Modulation(nullptr,nullptr);
	}	
}

ModMatrix::~ModMatrix() {
	
    removeAllChangeListeners();
	
	for (int i = 0; i < 6; i++) {
        delete modulations[i];
    }
	
    modSources->clear();
    modTargets->clear();
    delete modSources;
    delete modTargets;
	delete config;
	delete dummy;
}

void ModMatrix::addModulator(Modulator * m)
{
	modulators.push_back(m);
}

void ModMatrix::addModTarget(ModTarget * m)
{
	targets.push_back(m);
}

void ModMatrix::registerSource(String source, int id) {
    this->modSources->insert(std::make_pair(id,source));
}

void ModMatrix::registerTarget(String target, int id) {
    this->modTargets->insert(std::make_pair(id,target));
}

map<int,String>* ModMatrix::getSources() {
    return this->modSources;
}

map<int,String>* ModMatrix::getTargets() {
    return this->modTargets;
}

double ModMatrix::getSampleRate(){
    return this->sampleRate;
}

void ModMatrix::setSampleRate(double rate) {
    this->sampleRate = rate;
}

Model* ModMatrix::getModel() {
    return this->model;
}

void ModMatrix::setModel(Model *model) {
    this->model = model;
}

void ModMatrix::setConfig(ModMatrixConfig* config)
{
	this->config = config;

	
	
	for (int i = 0; i < config->getNumConfigs();i++) {
		ModSlotConfig* msc = config->getSlotConfig(i);

		/*
		Modulation* mod = new Modulation(i);
		this->modulations.push_back(mod);
		mod->setEnabled(msc->isSlotEnabled());
		*/
		if (msc->getSourceId() == 1) {
			modulations[i]->setModulator(dummy);
		}
		// LFO 1
		else if (msc->getSourceId() == 2) {
			// modulations.at(i)->setModulator(model->getLfo1());
		}
		// LFO 2
		else if (msc->getSourceId() == 3) {
			// modulations.at(i)->setModulator(model->getLfo2());
		}
		else if (msc->getSourceId() == 4) {
			// modulations.at(i)->setModulator(model->getModEnvelopes().at(0));
		}
		else if (msc->getSourceId() == 5) {
			// modulations.at(i)->setModulator(model->getModEnvelopes().at(1));
		}
		else if (msc->getSourceId() == 6) {
			// modulations.at(i)->setModulator(model->getModEnvelopes().at(2));
		}
		else if (msc->getSourceId() == 7) {
			// modulations.at(i)->setModulator(model->getSequencer());
		}			


		// Filter envelope
		if (msc->getTargetId() == 2) {
			/*
			ModTarget* target = model->getFilter();

			if (modulations.at(i)->getTargets().size() >= 1) {
				modulations.at(i)->getTargets()[0] = target;
			}
			else {
				modulations.at(i)->addTarget(target);
			}

			target->setModulator(modulations.at(i)->getModulator());
			target->setModAmount(msc->getAmount());
			*/
		}
		// Osc 1 Pitch
		if (msc->getTargetId() == 3) {
			/*
			modulations.at(i)->getTargets().clear();

			for (int j = 0; j < model->getVoices().size(); j++) {
				model->getVoices().at(j)->setModAmount(msc->getAmount1());
				MultimodeOscillator* m = static_cast<MultimodeOscillator*>(model->getVoices().at(j)->getOscillator(0));
				m->setModulator(modulations.at(i)->getModulator());
				m->setModAmount(msc->getAmount1());
				modulations.at(i)->addTarget(m);
			}
			*/
		}
		// Osc 2 Pitch
		if (msc->getTargetId() == 4) {
			/*
			modulations.at(i)->getTargets().clear();

            for (int j = 0; j < model->getVoices().size(); j++) {
                model->getVoices().at(j)->setModAmount(msc->getAmount1());
                MultimodeOscillator* m = static_cast<MultimodeOscillator*>(model->getVoices().at(j)->getOscillator(1));
                m->setModulator(modulations.at(i)->getModulator());
                m->setModAmount(msc->getAmount1());
                modulations.at(i)->addTarget(m);
            }
			*/
            
        }
		// Osc 3 Pitch
		if (msc->getTargetId() == 5) {
			/*
			modulations.at(i)->getTargets().clear();

            for (int j = 0; j < model->getVoices().size(); j++) {
                model->getVoices().at(j)->setModAmount(msc->getAmount1());
                MultimodeOscillator* m = static_cast<MultimodeOscillator*>(model->getVoices().at(j)->getOscillator(2));
                m->setModulator(modulations.at(i)->getModulator());
                m->setModAmount(msc->getAmount1());
                modulations.at(i)->addTarget(m);
            }
			*/
            
        }

	}

	sendChangeMessage();

}

ModMatrixConfig* ModMatrix::getConfiguration()
{
	return this->config;
}

void ModMatrix::createDefaultConfig()
{
	if (config != nullptr) {
		delete config;

		for (int i = 0; i < 6; i++) {
			modulations[i]->setEnabled(false);
		}

	}

	ModMatrixConfig* mmc = new ModMatrixConfig();

	for (int i = 0; i < 6;i++) {

        ModSlotConfig* msc = new ModSlotConfig();

		msc->setAmount(0);		
		msc->setSourceId(1);
		msc->setTargetId(1);
		msc->setEnabled(false);

		mmc->addConfig(msc);

	}

	setConfig(mmc);

}
