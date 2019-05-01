/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

enum DistortionType {
    SINE,
    TRIANGLE,
    SKEW,
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
    this->sampleRate = sampleRate;
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



inline double getPhaseSaw(double phase, double skew, int factor = 1)
{
	double warpedPhase;
	
	double division = (double)factor;
	if (division < 1.0)
		division = 1.0;

	double m1;
	double m2;
	double b2;


	double x1 = skew / division;
	double currentPhase = phase / division;


	if (x1 <= 0)
		x1 = 0.00001;


	m1 = (0.5 / division) / x1;
	m2 = (0.5 / division) / ((1.0 / division) - x1);
	b2 = (1.0 / division) - (m2 / division);


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






inline double getTriangle(double phase)
{
	double val;
	double curPhase = phase - (int)phase;
	double A = 1.0;

	if (curPhase < 0.5)
	{
		val =  -A + (4 * A) * curPhase;
	}
	else
	{
		val = 3*A - (4 * A) * curPhase;
	}

	return val;
}

double getPhaseTriangle(double phase, double skew)
{
    double phaseShift = 0.25;
    double modulator = getTriangle(phase + phaseShift) * skew;
    double warpedPhase = phase + modulator;
    return warpedPhase;
}

double getPhaseSine(double phase, double skew)
{
    double modulator = sin(phase * two_Pi) * skew;
    double warpedPhase = phase + modulator;
    return warpedPhase;
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
    
    DistortionType type = SINE;
    type = (DistortionType) *parameters.getRawParameterValue("type");
    double warpedPhase;
    
    
    for (int i = 0; i < numSamples ; i++)
    {
		
        switch (type) {
            case SINE: {
                warpedPhase = getPhaseSine(phase, *phaseBendParameterValue);
                
                break;
            }
            case TRIANGLE: {
                warpedPhase = getPhaseTriangle(phase, *phaseBendParameterValue);
                break;
            }
            case SKEW: {
                warpedPhase = getPhaseSaw(phase, *phaseBendParameterValue);
                break;
            }
            default:
                break;
                
        }
        
		
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
           
            auto* channelData = buffer.getWritePointer (channel);

            channelData[i] = cos(warpedPhase * two_Pi) * *gainParameterValue;
        }
        
        phase += getPhaseIncrement(110.0); // 110 hz
        
        while (phase >= 1.0)
            phase -= 1.0;
    }
    
    
}


double PDistortAudioProcessor::getPhaseIncrement(double frequency) 
{
    double inc = frequency/sampleRate;
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
