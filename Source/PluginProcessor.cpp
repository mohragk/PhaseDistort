/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"





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
			std::make_unique<AudioParameterFloat>("phaseBend", "Phase Bend", 0.0, 1.0, 1.0),
            std::make_unique<AudioParameterFloat>("pulseWidth", "Pulse Width", 0.0, 1.0, 0.5),
            std::make_unique<AudioParameterInt>("type", "Distortion Type", 0, numTypes - 1, 1),
            std::make_unique<AudioParameterInt>("numVoices", "Number of Voices", 1, MAX_VOICES, 1)
		}
	)

#endif
{
	
	envelopeGenerator.reset( new EnvelopeGenerator() );
    envelopeGeneratorVol.reset( new EnvelopeGenerator() );
    
    
    // set correct EG for voices
    for(int i = 0; i < MAX_VOICES ; i++)
    {
        synthVoices[i].eg[0] = envelopeGenerator.get();
        synthVoices[i].eg[1] = envelopeGeneratorVol.get();
    }
    

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
    
    lfoData.phase = 0.0;
    lfoData.phaseInc = getPhaseIncrement(1.5);
    
    
    for (int i = 0; i < MAX_VOICES; i++)
    {
        synthVoices[i].osc.phase = 0.0;
        synthVoices[i].osc.phaseInc = getPhaseIncrement(110.0);
    }
    
    envelopeGenerator.get()->prepareToPlay(sampleRate);
    envelopeGenerator.get()->setSustainLevel(0.3);
    envelopeGenerator.get()->setDecayRate(12.5);
    
    envelopeGeneratorVol.get()->prepareToPlay(sampleRate);
    
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

dsp::LookupTableTransform<double> sinLUT { [](double x) { return sin(x); }, 0.0, two_Pi, 256 };
dsp::LookupTableTransform<double> cosLUT { [](double x) { return cos(x); }, 0.0, two_Pi, 256 };

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
	return cosLUT(two_Pi * getPhaseSkewed(phase, skew));
}


double getSquare(double phase, double skew)
{
	double warpedPhase;

	warpedPhase = getPhaseSkewed(2.0 * phase, skew) * 0.5;

	if (phase > 0.5)
	{
		warpedPhase += 0.5;
	}


	return cosLUT(warpedPhase * two_Pi);
}


inline double getPhaseSkewedForSteps(double phase, double skew, int numSteps)
{
    double warpedPhase;
    double fract = 1.0 / (double) numSteps;
    
    int step = (int)(phase / fract);
      
    
    if (step % 2 == 1)
        warpedPhase = getPhaseSkewed(phase * numSteps, 1.0 - skew) * fract;
    else
        warpedPhase = getPhaseSkewed(phase * numSteps, skew ) * fract;
    
    warpedPhase += (fract * step);

    return warpedPhase;
}

double getFancySquare(double phase, double skew)
{
    double warpedPhase;
    
    int numSteps = 4;
    warpedPhase = getPhaseSkewedForSteps(phase, skew, numSteps);
    
    return sinLUT(warpedPhase * two_Pi);
}


inline double getModulatorTriangle(double phase, double skew)
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
    
    int numVoices = *parameters.getRawParameterValue("numVoices");
    
    
    // set the oscillator type
    for(int i = 0; i < numVoices; i++)
    {
        WaveformType type = SINE;
        int int_type = (int)*parameters.getRawParameterValue("type");
        type = (WaveformType) int_type;
        synthVoices[i].osc.type = type;
    }
    
    
    
    for (int i = 0; i < numSamples ; i++)
    {
        int on = false;
        
        // CHECK KEYBOARD NOTES
        for(int i = 0; i < KEYBOARD_NOTES_COUNT; i++)
        {
            if (playingNotes.noteDown[i])
            {
                int baseNote = 42;
                int currentMidiNote = i + baseNote;
                for (int v = 0; v < numVoices; v++)
                {
                    double freq = MidiMessage::getMidiNoteInHertz(currentMidiNote);
                    synthVoices[v].osc.phaseInc = getPhaseIncrement(freq + v);
                    on = true;
                }
                
                break; //biased towards lowest keys, but we don't care
            }
            else
            {
                on = false;
            }
        }
        
        
        envelopeGenerator.get()->gate(on);
        envelopeGeneratorVol.get()->gate(on);
        
        envelopeGenerator.get()->process();
        envelopeGeneratorVol.get()->process();
        
        double currentSample[2] = {0.0, 0.0};
        
        for(int voi = 0; voi < numVoices; voi++)
        {
            double s = getSample(&synthVoices[voi], &parameters) * (1.0 / numVoices);
            
            if (numVoices == 1 )
            {
                currentSample[0] = s * 0.5;
                currentSample[1] = s * 0.5;
                break;
            }
            
            if (voi % 2 == 0)
                currentSample[0] += s;
            else
                currentSample[1] += s;
        }
        
        
        
        for (int channel = 0; channel < totalNumOutputChannels; ++channel)
        {
            auto* channelData = buffer.getWritePointer (channel);
            channelData[i] = currentSample[channel];
        }
        
       
    }
 
}


double PDistortAudioProcessor::getSample(voice* currentVoice, AudioProcessorValueTreeState* parameters)
{
    double currentSample = 0.0;
    double currentPhase = currentVoice->osc.phase;
    
    float* phaseBend = parameters->getRawParameterValue("phaseBend");
    float* pulseWidth = parameters->getRawParameterValue("pulseWidth");
    float* gain = parameters->getRawParameterValue("gain");
    
    
    // PROCESS EG SEPERATELY
    
    double egVal = currentVoice->eg[0]->getOutput() * *phaseBend;
    double egValInverted = 1.0 - egVal;
    egValInverted *= 0.5;
    
    double modAmount = egValInverted;
    
    WaveformType currentType = currentVoice->osc.type;
    switch (currentType)
    {
        case SINE: {
            currentSample = sinLUT(currentPhase * two_Pi);
            
            break;
        }
        case SAW: {
            currentSample = getSaw(currentPhase, modAmount);
            break;
        }
            
        case SQUARE: {
            currentSample = getSquare(currentPhase, modAmount);
            break;
        }
        case FANCY_SQUARE: {
            double pw = (*pulseWidth + 1.0) * 0.5;
            double warpedPhase = getPhaseSkewed(currentPhase, pw);
            currentSample = getFancySquare(warpedPhase, modAmount);
            break;
        }
        default:
            break;
            
    }
    double egValVolume = currentVoice->eg[1]->getOutput();
    currentSample *= *gain * egValVolume;
    
    currentVoice->osc.phase += currentVoice->osc.phaseInc;
    
    while (currentVoice->osc.phase >= 1.0)
        currentVoice->osc.phase -= 1.0;
    
    return currentSample;
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
    return new PDistortAudioProcessorEditor (*this, parameters, playingNotes );
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
