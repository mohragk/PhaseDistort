/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
PDistortAudioProcessorEditor::PDistortAudioProcessorEditor (PDistortAudioProcessor& p, AudioProcessorValueTreeState& vst, playing_notes& notes)
    : AudioProcessorEditor (&p), processor (p), valueTreeState(vst), playingNotes(notes)
{
    
    
	addAndMakeVisible(gainSlider);
	gainAttachment.reset(new SliderAttachment(valueTreeState, "gain", gainSlider));
    addAndMakeVisible(gainLabel);
    gainLabel.setText("GAIN", dontSendNotification);
    gainLabel.attachToComponent(&gainSlider, true);
    
    
    addAndMakeVisible(phaseTypeSlider);
    phaseTypeAttachment.reset(new SliderAttachment(valueTreeState, "type", phaseTypeSlider));
    addAndMakeVisible(phaseTypeLabel);
    phaseTypeLabel.setText("OSC TYPE", dontSendNotification);
    phaseTypeLabel.attachToComponent(&phaseTypeSlider, true);
    

	addAndMakeVisible(phaseBendSlider);
	phaseBendAttachment.reset(new SliderAttachment(valueTreeState, "phaseBend", phaseBendSlider));
    addAndMakeVisible(phaseBendLabel);
    phaseBendLabel.setText("CUTOFF", dontSendNotification);
    phaseBendLabel.attachToComponent(&phaseBendSlider, true);
    
    
    
    addAndMakeVisible(pulseWidthSlider);
    pulseWidthAttachment.reset(new SliderAttachment(valueTreeState, "pulseWidth", pulseWidthSlider));
    addAndMakeVisible(pulseWidthLabel);
    pulseWidthLabel.setText("PW (type 3)", dontSendNotification);
    pulseWidthLabel.attachToComponent(&pulseWidthSlider, true);
    
   
    addAndMakeVisible(numVoicesSlider);
    numVoicesAttachment.reset(new SliderAttachment(valueTreeState, "numVoices", numVoicesSlider));
    addAndMakeVisible(numVoicesLabel);
    numVoicesLabel.setText("NUM VOICES", dontSendNotification);
    numVoicesLabel.attachToComponent(&numVoicesSlider, true);
    
    
    
    
   
    
    for (int i =0; i < KEYBOARD_NOTES_COUNT; i++)
    {
        addAndMakeVisible(keyTriggerButtons[i]);
        
        keyTriggerButtons[i].setButtonText("TRIGGER " + std::to_string(i + 1));
        keyTriggerButtons[i].setTriggeredOnMouseDown(true);
        keyTriggerButtons[i].onStateChange = [this]() { handleButtonStateChange(); };
    }
    

    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (600, 400);
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
    
    for (int i = 0 ; i < KEYBOARD_NOTES_COUNT; i++)
    {
        Button::ButtonState state = keyTriggerButtons[i].getState();
        
        if(state == Button::ButtonState::buttonDown)
        {
            playingNotes.noteDown[i] = true;
        }
        else
            playingNotes.noteDown[i] = false;
    }
}


bool PDistortAudioProcessorEditor::keyStateChanged(bool isKeyDown)
{
    if (isKeyDown)
    {
        
        
        if(KeyPress::isKeyCurrentlyDown('a'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[0].setState(bDown);
        }
        
        if(KeyPress::isKeyCurrentlyDown('s'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[1].setState(bDown);
        }
        
        if(KeyPress::isKeyCurrentlyDown('d'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[2].setState(bDown);
        }
        
        if(KeyPress::isKeyCurrentlyDown('f'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[3].setState(bDown);
        }
        if(KeyPress::isKeyCurrentlyDown('g'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[4].setState(bDown);
        }
        if(KeyPress::isKeyCurrentlyDown('h'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[5].setState(bDown);
        }
        if(KeyPress::isKeyCurrentlyDown('j'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[6].setState(bDown);
        }
        if(KeyPress::isKeyCurrentlyDown('k'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[7].setState(bDown);
        }
        if(KeyPress::isKeyCurrentlyDown('l'))
        {
            Button::ButtonState bDown = Button::ButtonState::buttonDown;
            keyTriggerButtons[8].setState(bDown);
        }
    }
    else
    {
        Button::ButtonState bNorm = Button::ButtonState::buttonNormal;
        
        if(!KeyPress::isKeyCurrentlyDown('a')) { keyTriggerButtons[0].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('s')) { keyTriggerButtons[1].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('d')) { keyTriggerButtons[2].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('f')) { keyTriggerButtons[3].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('g')) { keyTriggerButtons[4].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('h')) { keyTriggerButtons[5].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('j')) { keyTriggerButtons[6].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('k')) { keyTriggerButtons[7].setState(bNorm); }
        if(!KeyPress::isKeyCurrentlyDown('l')) { keyTriggerButtons[8].setState(bNorm); }
        
    }
    
    
    return true;
}


void PDistortAudioProcessorEditor::resized()
{
	int height = 40;
    int labelW = 80;
	Rectangle<int> layout (getLocalBounds());
    {
        Rectangle<int> sliderLayout (layout.removeFromTop(height));
        gainSlider.setBounds(sliderLayout.removeFromRight(getWidth() - labelW));
    }
    
    {
        Rectangle<int> sliderLayout (layout.removeFromTop(height));
        phaseTypeSlider.setBounds(sliderLayout.removeFromRight(getWidth() - labelW));
    }
    
    {
        Rectangle<int> sliderLayout (layout.removeFromTop(height));
        phaseBendSlider.setBounds(sliderLayout.removeFromRight(getWidth() - labelW));
    }
	
    {
        Rectangle<int> sliderLayout (layout.removeFromTop(height));
        pulseWidthSlider.setBounds(sliderLayout.removeFromRight(getWidth() - labelW));
    }

    {
        Rectangle<int> sliderLayout (layout.removeFromTop(height));
        numVoicesSlider.setBounds(sliderLayout.removeFromRight(getWidth() - labelW));
    }
    
    
    Rectangle<int> keyLayout (layout);
    int w = getWidth() / KEYBOARD_NOTES_COUNT;
    
    for (int i = 0; i < KEYBOARD_NOTES_COUNT; i++)
    {
        keyTriggerButtons[i].setBounds(keyLayout.removeFromLeft(w));
        keyTriggerButtons[i].setButtonText(std::to_string(i+1));
    }

}
