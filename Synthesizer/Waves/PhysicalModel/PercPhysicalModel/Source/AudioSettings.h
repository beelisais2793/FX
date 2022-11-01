
#ifndef SETTINGS_H_INCLUDED
#define SETTINGS_H_INCLUDED


class AudioSettings : ChangeListener
{
public:
	AudioDeviceManager& getInputDeviceManager()
    {
		return *pInputDeviceManager;
	}
    
	AudioDeviceManager& getOutputDeviceManager()
    {
		return *pOutputDeviceManager;
	}

    void changeListenerCallback (ChangeBroadcaster* source) override
    {
        
    }
    
    
	AudioSettings()
	{
		pInputDeviceManager = new AudioDeviceManager();
		pOutputDeviceManager = new AudioDeviceManager();

		pInputDeviceManager->initialise(
			0,			// numInputChannelsNeeded
			0,			// numOutputChannelsNeeded
			nullptr,	// XML
			true,		// selectDefaultDeviceOnFailure
			String(),	// preferredDefaultDeviceName
			0			// preferredSetupOptions
			);

		pOutputDeviceManager->initialise(
			0,			// numInputChannelsNeeded
			2,			// numOutputChannelsNeeded
			nullptr,	// XML
			true,		// selectDefaultDeviceOnFailure
			String(),	// preferredDefaultDeviceName
			0			// preferredSetupOptions
			);

		pOutputDeviceManager->addChangeListener(this);

		OwnedArray<AudioIODeviceType> types;
		pOutputDeviceManager->createAudioDeviceTypes(types);
		for (auto& t : types)
        {
			String typeName(t->getTypeName());				// This will be things like "DirectSound", "CoreAudio", etc.
			t->scanForDevices();							// This must be called before getting the list of devices
			StringArray deviceNames(t->getDeviceNames());	// This will now return a list of available devices of this type
		}
	}

    ~AudioSettings()
    {
        
    }

	void launchInputWindow()
    {
		launch(pInputDeviceManager, 1, 0);
	}

	void launchOutputWindow()
    {
		launch(pOutputDeviceManager, 0, 2);
	}
    
    void launch(AudioDeviceManager* pDeviceManager, int inputs, int outputs)
    {
        AudioDeviceSelectorComponent* settingsPane = new AudioDeviceSelectorComponent(*pDeviceManager,
                                                                                      inputs, inputs,	// min/max inputs
                                                                                      outputs, outputs,	// min/max outputs
                                                                                      inputs > 0,	// showMidiInputOptions
                                                                                      outputs > 0,	// showMidiOutputSelector
                                                                                      true,         // showChannelsAsStereoPairs
                                                                                      true);		// showAdvancedOptions
                                                                                      
        settingsPane->setSize(640, 480);
        
        DialogWindow::LaunchOptions options;
        
        options.content.setOwned(settingsPane);
        options.dialogTitle = "Audio Settings";
        options.dialogBackgroundColour = Colours::lightgrey;
        options.escapeKeyTriggersCloseButton = true;
        options.useNativeTitleBar = true;
        options.resizable = true;
        
        DialogWindow* dialogWindow = options.launchAsync();
        dialogWindow->centreWithSize(450, 250);
    }
    
    
private:
    ScopedPointer<AudioDeviceManager> pInputDeviceManager;
    ScopedPointer<AudioDeviceManager> pOutputDeviceManager;
    

};

#endif  // SETTINGS_H_INCLUDED
