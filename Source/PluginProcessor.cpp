/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

enum WaveformType {
    SINE,
    SAW,
	SQUARE,
    numTypes
};

#define two_Pi (2.0 * double_Pi)

//==============================================================================
PDistortAudioProcessor::PDistortAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
	parameters(*this, nullptr, Identifier("PARAMETERS"),
		{
			std::make_unique<AudioParameterFloat> ("gain", "Gain", 0.0, 1.0, 0.15),
			std::make_unique<AudioParameterFloat>("phaseBend", "Phase Bend", 0.0, 1.0, 0.5),
            std::make_unique<AudioParameterInt>("type", "Distortion Type", 0, numTypes, 0)
		}
	)

#endif
{
	gainParameterValue = parameters.getRawParameterValue("gain");
	phaseBendParameterValue = parameters.getRawParameterValue("phaseBend");

}

PDistortAudioProcessor::~PDistortAudioProcessor()
{
}

//==============================================================================
const String PDistortAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool PDistortAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool PDistortAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool PDistortAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double PDistortAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int PDistortAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int PDistortAudioProcessor::getCurrentProgram()
{
    return 0;
}

void PDistortAudioProcessor::setCurrentProgram (int index)
{
}

const String PDistortAudioProcessor::getProgramName (int index)
{
    return {};
}

void PDistortAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void PDistortAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	curSampleRate = sampleRate;
    numSamples = samplesPerBlock;
    
    
    phase = 0.0;
    phaseIncrement = 0.1;
}

void PDistortAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool PDistortAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif



inline double getPhaseSkewed(double phase, double skew)
{
	double warpedPhase;
	

	double m1;
	double m2;
	double b2;


	double x1 = skew;
	double currentPhase = phase - (int)phase;


	if (x1 <= 0)
		x1 = 0.00001;


	m1 = 0.5 / x1;
	m2 = 0.5 / (1.0  - x1);
	b2 = 1.0 - m2;


	if (currentPhase < x1)
	{
		warpedPhase = m1 * currentPhase;
	}
	else
	{
		warpedPhase = m2 * currentPhase + b2;
	}

	return warpedPhase;
}


double getSaw(double phase, double skew)
{
	return cos(two_Pi * getPhaseSkewed(phase, skew));
}


double getSquare(double phase, double skew)
{
	double warpedPhase;

	warpedPhase = getPhaseSkewed(2.0 * phase, skew) * 0.5;

	if (phase > 0.5)
	{
		warpedPhase += 0.5;
	}


	return cos(warpedPhase * two_Pi);
}


double getModulatorTriangle(double phase, double skew)
{
	double val;
	double curPhase = phase - (int)phase;
	double A = skew;

	if (curPhase < 0.5)
	{
		val =    (4.0 * A) * curPhase - A;
	}
	else
	{
		val =  - (4.0 * A) * curPhase + (3.0 * A);
	}

	return val;
}




void PDistortAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    
    int numSamples = buffer.getNumSamples();    
    
    WaveformType type = SINE;
	int int_type = (int)*parameters.getRawParameterValue("type");
    type = (WaveformType) int_type;
    double currentSample = 0.0;
    
    
    for (int i = 0; i < numSamples ; i++)
    {
		
        switch (type) 
		{
            case SINE: {
				currentSample = sin(phase * two_Pi);
                
                break;
            }
            case SAW: {
				currentSample = getSaw(phase, *phaseBendParameterValue);
                break;
            }
			
            case SQUARE: {
				currentSample = getSquare(phase, *phaseBendParameterValue);
                break;
            }
            default:
                break;
                
        }
        
		currentSample *= *gainParameterValue;
		
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
           
            auto* channelData = buffer.getWritePointer (channel);

            channelData[i] = currentSample;
        }
        
        phase += getPhaseIncrement(110.0); // 110 hz
        
        while (phase >= 1.0)
            phase -= 1.0;
    }
    
    
}


double PDistortAudioProcessor::getPhaseIncrement(double frequency) 
{
    double inc = frequency/ curSampleRate;
    return inc;
}


//==============================================================================
bool PDistortAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* PDistortAudioProcessor::createEditor()
{
    return new PDistortAudioProcessorEditor (*this, parameters);
}

//==============================================================================
void PDistortAudioProcessor::getStateInformation (MemoryBlock& destData)
{
	auto state = parameters.copyState();
	std::unique_ptr<XmlElement> xml(state.createXml());
	copyXmlToBinary(*xml, destData);
}

void PDistortAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	std::unique_ptr<XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

	if (xmlState.get() != nullptr) {
		if (xmlState->hasTagName(parameters.state.getType())) {
			parameters.replaceState(ValueTree::fromXml(*xmlState));
		}
	}
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new PDistortAudioProcessor();
}
