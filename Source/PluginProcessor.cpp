/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GlobalSettings.h"

//==============================================================================
SimpleFirAudioProcessor::SimpleFirAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    delay_line = new DelayLine<float>();
    delay_line->set_delay(1);
    fir = new Fir<float>(delay_line, 0.0);
    /**********************************/
    addParameter(filter_gain = new juce::AudioParameterFloat("filter_gain", // parameterID
        "Gain", // parameter name
        min_fir_gain,   // minimum value
        max_fir_gain,   // maximum value
        1.0f)); // default value
    addParameter(filter_delay = new juce::AudioParameterInt("filter_delay", // parameterID
        "Delay", // parameter name
        min_delay_in_samples,   // minimum value
        max_delay_in_samples,   // maximum value
        min_delay_in_samples)); // default value
}

SimpleFirAudioProcessor::~SimpleFirAudioProcessor()
{
    delete delay_line;
    delete fir;
    int c = 0;
}

//==============================================================================
const juce::String SimpleFirAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool SimpleFirAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool SimpleFirAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool SimpleFirAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double SimpleFirAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int SimpleFirAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int SimpleFirAudioProcessor::getCurrentProgram()
{
    return 0;
}

void SimpleFirAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String SimpleFirAudioProcessor::getProgramName (int index)
{
    return {};
}

void SimpleFirAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void SimpleFirAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void SimpleFirAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool SimpleFirAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    juce::ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
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

void SimpleFirAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    //for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
      //  buffer.clear (i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.
    fir->set_gain(fir_gain); //out of the loop
    delay_line->set_delay(delay);

    /*DBG("fir_gain");
    DBG(fir_gain);
    DBG("\n*************\nfir_delay");
    DBG(delay);

    DBG("\n*************\ngain par");
    DBG(*filter_gain);
    DBG("\n*************\ndelay par");
    DBG(*filter_delay);
    */
    *filter_gain = fir_gain;
    *filter_delay = delay;

    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer (channel);

        // ..do something to the data...
        for (int i = 0; i < buffer.getNumSamples(); i++)
        {
            auto input = buffer.getSample(channel, i);
            channelData[i] = fir->get_fir_sample(input);
        }
    }
}

//==============================================================================
bool SimpleFirAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* SimpleFirAudioProcessor::createEditor()
{
    return new SimpleFirAudioProcessorEditor (*this);
}

//==============================================================================
void SimpleFirAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
    juce::MemoryOutputStream(destData, true).writeFloat(*filter_gain);
    juce::MemoryOutputStream(destData, true).writeInt(*filter_delay);
}

void SimpleFirAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
    *filter_gain = juce::MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readFloat();
    *filter_delay = juce::MemoryInputStream(data, static_cast<size_t> (sizeInBytes), false).readInt();
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new SimpleFirAudioProcessor();
}

