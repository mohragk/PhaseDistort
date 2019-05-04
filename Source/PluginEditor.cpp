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
    
    addAndMakeVisible(triggerButton);
    triggerAttachment.reset(new ButtonAttachment(valueTreeState, "trigger", triggerButton));
    triggerButton.setButtonText("TRIGGER");
    triggerButton.setTriggeredOnMouseDown(true);
    triggerButton.onStateChange = [this]() { handleButtonStateChange(); };
    //triggerButton.setClickingTogglesState(true);
    
    
    

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

void PDistortAudioProcessorEditor::handleButtonStateChange()
{
    Button::ButtonState state = triggerButton.getState();
    
    if(state == Button::ButtonState::buttonDown)
        triggerButton.setToggleState(true, sendNotificationSync);
    else
        triggerButton.setToggleState(false, sendNotificationSync);
}


bool PDistortAudioProcessorEditor::keyStateChanged(bool isKeyDown)
{
    if (isKeyDown)
    {
        if(KeyPress::isKeyCurrentlyDown(KeyPress::spaceKey))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            triggerButton.setState(bDown);
        }
        
    }
    else
    {
        Button::ButtonState bDown = Button::ButtonState::buttonNormal;
        triggerButton.setState(bDown);
    }
    
    
    return true;
}


void PDistortAudioProcessorEditor::resized()
{
	int height = 40;
	Rectangle<int> layout (getLocalBounds());
	gainSlider.setBounds(layout.removeFromTop(height));
	phaseBendSlider.setBounds(layout.removeFromTop(height));
    phaseTypeSlider.setBounds(layout.removeFromTop(height));
    triggerButton.setBounds(layout.removeFromTop(height));

}
