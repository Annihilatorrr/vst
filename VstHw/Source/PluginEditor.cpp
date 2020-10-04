/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
VstHwAudioProcessorEditor::VstHwAudioProcessorEditor (VstHwAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    m_gainSlider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    m_gainSlider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 50, 20);
    m_gainSlider.setRange(-60.0f, 0.0f, 0.01f);
    m_gainSlider.setValue(-20.f);
    m_gainSlider.addListener(this);
    addAndMakeVisible(m_gainSlider);
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (400, 300);
}

VstHwAudioProcessorEditor::~VstHwAudioProcessorEditor()
{
}

//==============================================================================
void VstHwAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));

    //g.setColour (juce::Colours::white);
    //g.setFont (15.0f);
    //g.drawFittedText ("Hello World!", getLocalBounds(), juce::Justification::centred, 1);
}

void VstHwAudioProcessorEditor::resized()
{
    m_gainSlider.setBounds((getWidth() >> 1) - 50, getHeight() >> 1, 100, 150);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void VstHwAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &m_gainSlider)
    {
        static_cast<VstHwAudioProcessor*>(&processor)->setGain(slider->getValue());
    }
}
