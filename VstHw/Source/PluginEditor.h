/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class VstHwAudioProcessorEditor  : public juce::AudioProcessorEditor, juce::Slider::Listener
{
public:
    VstHwAudioProcessorEditor (VstHwAudioProcessor&);
    ~VstHwAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void sliderValueChanged(juce::Slider* slider) override;

private:

    juce::Slider m_gainSlider;
    juce::TextButton m_loadButton {"Load"};
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    VstHwAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (VstHwAudioProcessorEditor)
};
