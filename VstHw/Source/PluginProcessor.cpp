/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VstHwAudioProcessor::VstHwAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
    m_formatManager.registerBasicFormats();
    for (int i = 0; i < m_voices; ++i)
    {
        m_synthesiser.addVoice(new juce::SamplerVoice());
    }
}

VstHwAudioProcessor::~VstHwAudioProcessor()
{
    delete m_formatReader;
}

//==============================================================================
const juce::String VstHwAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool VstHwAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}
void VstHwAudioProcessor::loadFile()
{
    juce::FileChooser fileChooser("Please load file",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory), "*.wav; *.mp3");
    if (fileChooser.browseForFileToOpen())
    {
        auto file = fileChooser.getResult();
        std::unique_ptr<juce::AudioFormatReader> formatReader(m_formatManager.createReaderFor(file));
        if (formatReader)
        {
            juce::BigInteger range;
            range.setRange(0, 128, true);
            m_synthesiser.addSound(new juce::SamplerSound("Sample", *formatReader, range, 60, 0.0, 0.001, 10.0));
        }
    }
}
bool VstHwAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool VstHwAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double VstHwAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int VstHwAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int VstHwAudioProcessor::getCurrentProgram()
{
    return 0;
}

void VstHwAudioProcessor::setCurrentProgram(int index)
{
}

const juce::String VstHwAudioProcessor::getProgramName(int index)
{
    return {};
}

void VstHwAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void VstHwAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    m_synthesiser.setCurrentPlaybackSampleRate(sampleRate);
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void VstHwAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool VstHwAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
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

void VstHwAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    const auto totalNumInputChannels = getTotalNumInputChannels();
    const auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
    {
        buffer.clear(i, 0, buffer.getNumSamples());
    }
    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    const auto numberOfSamples = buffer.getNumSamples();
    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        auto* channelData = buffer.getWritePointer(channel);
        for (int sample = 0; sample < numberOfSamples; ++sample)
        {
            channelData[sample] *= static_cast<float>(juce::Decibels::decibelsToGain(m_gain));
        }
    }

    m_synthesiser.renderNextBlock(buffer, midiMessages, 0, numberOfSamples);
}

//==============================================================================
bool VstHwAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* VstHwAudioProcessor::createEditor()
{
    return new VstHwAudioProcessorEditor(*this);
}

//==============================================================================
void VstHwAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void VstHwAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new VstHwAudioProcessor();
}
