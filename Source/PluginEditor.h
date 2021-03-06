/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class PDistortAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    PDistortAudioProcessorEditor (PDistortAudioProcessor&, AudioProcessorValueTreeState&, playing_notes&);
    ~PDistortAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    void handleButtonStateChange();
    
    bool keyStateChanged(bool isKeyDown) override;
    

private:
    PDistortAudioProcessor& processor;
	AudioProcessorValueTreeState& valueTreeState;
    playing_notes& playingNotes;

	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

    Label gainLabel;
	Slider gainSlider;
	std::unique_ptr<SliderAttachment> gainAttachment;

    Label phaseBendLabel;
	Slider phaseBendSlider;
	std::unique_ptr<SliderAttachment> phaseBendAttachment;
    
    Label phaseTypeLabel;
    Slider phaseTypeSlider;
    std::unique_ptr<SliderAttachment> phaseTypeAttachment;
    
    Label pulseWidthLabel;
    Slider pulseWidthSlider;
    std::unique_ptr<SliderAttachment> pulseWidthAttachment;
    
    Label numVoicesLabel;
    Slider numVoicesSlider;
    std::unique_ptr<SliderAttachment> numVoicesAttachment;
    
   
    
    
    
    TextButton keyTriggerButtons[KEYBOARD_NOTES_COUNT];

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDistortAudioProcessorEditor)
};
