/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "ADSR/ADSR.h"

#define two_Pi (2.0 * double_Pi)
#define KEYBOARD_NOTES_COUNT 9

struct oscillator_data {
    double phase;
    double phaseInc;
};

struct playing_notes {
    bool noteDown[KEYBOARD_NOTES_COUNT];
};

//==============================================================================
/**
*/
class PDistortAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    PDistortAudioProcessor();
    ~PDistortAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;
    
    double getPhaseIncrement(double frequency);

	float* gainParameterValue;
	float* phaseBendParameterValue;
    

	std::unique_ptr<EnvelopeGenerator> envelopeGenerator, envelopeGeneratorVol;
    
    oscillator_data lfoData, oscData;
    
    double getLFO(oscillator_data* data)
    {
        double val = 0.0;
        if (data != nullptr)
        {
            
            val = sin(data->phase * two_Pi);
            
            data->phase += data->phaseInc;
            while (data->phase > 1.0)
                data->phase -= 1.0;
        }
        
        return val;
        
    }
    
    playing_notes playingNotes = {};
    
    
private:
    //=============================================================================
	AudioProcessorValueTreeState parameters;
    double curSampleRate;
    int numSamples;
    
    double currentFrequency;
    double distortAmount;

    
    
	
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDistortAudioProcessor)
};
