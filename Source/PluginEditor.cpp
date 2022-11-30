/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include "GlobalSettings.h"

//==============================================================================
SimpleFirAudioProcessorEditor::SimpleFirAudioProcessorEditor (SimpleFirAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 400);
    gain_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    gain_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    gain_slider.setTopLeftPosition(-100, 0);
    gain_slider.setRange(min_fir_gain, max_fir_gain, 0.05);
    gain_slider.addListener(this);
    addAndMakeVisible(gain_slider);
    /*Delay slider section*/
    delay_slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
    delay_slider.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 100, 25);
    delay_slider.setTopLeftPosition(100, 0);
    delay_slider.setRange(1, max_delay_in_samples, 1);
    delay_slider.addListener(this);
    addAndMakeVisible(delay_slider);
}

SimpleFirAudioProcessorEditor::~SimpleFirAudioProcessorEditor()
{
}

//==============================================================================
void SimpleFirAudioProcessorEditor::paint (juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));
}

void SimpleFirAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    gain_slider.setBounds(getLocalBounds());
    delay_slider.setBounds(getLocalBounds());
}


void SimpleFirAudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &gain_slider)
    {
        audioProcessor.fir_gain = gain_slider.getValue();
    }
    else if (slider == &delay_slider)
    {
        audioProcessor.delay = delay_slider.getValue();
    }
}
