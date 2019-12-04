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
#define MAX_VOICES 8

enum class WaveformType {
    SINE  = 0 ,
    SAW,
    SQUARE,
    FANCY_SQUARE,
    numTypes
};

struct oscillator_data {
    double phase;
    double phaseInc;
    
    WaveformType type;
};

struct playing_notes {
    bool noteDown[KEYBOARD_NOTES_COUNT];
};


struct voice {
    oscillator_data osc;
    EnvelopeGenerator* eg[2];
    
    
    bool gateOn;
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
    double getSample(voice* currentVoice, AudioProcessorValueTreeState* parameters);

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

	
    

	std::unique_ptr<EnvelopeGenerator> envelopeGenerator, envelopeGeneratorVol;
    
    oscillator_data lfoData;
    
    
    voice synthVoices[MAX_VOICES] = {};
    
    double getLFO(oscillator_data* osc)
    {
        double val = 0.0;
        if (osc != nullptr)
        {
            
            val = sin(osc->phase * two_Pi);
            
            osc->phase += osc->phaseInc;
            while (osc->phase > 1.0)
                osc->phase -= 1.0;
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
