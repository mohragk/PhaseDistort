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
    PDistortAudioProcessorEditor (PDistortAudioProcessor&, AudioProcessorValueTreeState&);
    ~PDistortAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;

private:
    PDistortAudioProcessor& processor;
	AudioProcessorValueTreeState& valueTreeState;

	typedef AudioProcessorValueTreeState::SliderAttachment SliderAttachment;
	//typedef AudioProcessorValueTreeState::ButtonAttachment ButtonAttachment;

	Slider gainSlider;
	std::unique_ptr<SliderAttachment> gainAttachment;

	Slider phaseBendSlider;
	std::unique_ptr<SliderAttachment> phaseBendAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PDistortAudioProcessorEditor)
};