/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PDistortAudioProcessorEditor::PDistortAudioProcessorEditor (PDistortAudioProcessor& p, AudioProcessorValueTreeState& vst)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vst)
{
	addAndMakeVisible(gainSlider);
	gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));

	addAndMakeVisible(phaseBendSlider);
	phaseBendAttachment.reset(new SliderAttachment(valueTreeState, "phaseBend", phaseBendSlider));
    
    addAndMakeVisible(phaseTypeSlider);
    phaseTypeAttachment.reset(new SliderAttachment(valueTreeState, "type", phaseTypeSlider));

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
    
}

PDistortAudioProcessorEditor::~PDistortAudioProcessorEditor()
{
}

//==============================================================================
void PDistortAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

  
}

void PDistortAudioProcessorEditor::resized()
{
	int height = 40;
	Rectangle<int> layout (getLocalBounds());
	gainSlider.setBounds(layout.removeFromTop(height));
	phaseBendSlider.setBounds(layout.removeFromTop(height));
    phaseTypeSlider.setBounds(layout.removeFromTop(height));

}
