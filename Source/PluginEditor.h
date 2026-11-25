#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class TapeMachineLookAndFeel final : public juce::LookAndFeel_V4
{
public:
    void drawRotarySlider (juce::Graphics&, int, int, int, int, float,
                           float, float, juce::Slider&) override;
};

class AdLSpaceDubEchoAudioProcessorEditor final : public juce::AudioProcessorEditor, private juce::Timer
{
public:
    explicit AdLSpaceDubEchoAudioProcessorEditor (AdLSpaceDubEchoAudioProcessor&);
    ~AdLSpaceDubEchoAudioProcessorEditor() override;
    void paint (juce::Graphics&) override;
    void paintOverChildren (juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    AdLSpaceDubEchoAudioProcessor& processor;
    juce::Label title;
    juce::Image facadeImage;
    juce::Image controlsLabelsImage;
    juce::Image redLinesImage;
    juce::Image vuLabelsImage;
    juce::Image feedbackBoostOverlayImage;
    static constexpr int designWidth = 1440;
    static constexpr int designHeight = 720;
    float vuDisplay = 0.0f;
    TapeMachineLookAndFeel vintageLookAndFeel;
    juce::Slider dubChaos;
    juce::Slider sendToEcho;
    juce::Label sendToEchoLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> sendToEchoAttachment;
    juce::TextButton feedbackThrowButton;
    juce::ComboBox boostLevelSelect, boostReleaseSelect;
    juce::TextButton boostLevelSwitch, boostReleaseButton;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> boostLevelAttachment, boostReleaseAttachment;
    bool throwGestureActive = false;
    juce::Label dubChaosLabel;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> dubChaosAttachment;
    juce::Slider time, feedback, tone, drive, modulation, modulationRate, stereoWidth, stereoRate, mix, tapeAging, oscCharacter, lowCut, highCut, lowResonance, highResonance, wow, flutter, tapeHiss, inputHighPass;
    juce::Label timeLabel, feedbackLabel, toneLabel, driveLabel;
    juce::Label modulationLabel, modulationRateLabel, stereoWidthLabel, stereoRateLabel, mixLabel, tapeAgingLabel, oscCharacterLabel, lowCutLabel, highCutLabel, lowResonanceLabel, highResonanceLabel, wowLabel, flutterLabel, tapeHissLabel, inputHighPassLabel;

    using Attachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<Attachment> timeAttachment, feedbackAttachment, toneAttachment;
    std::unique_ptr<Attachment> driveAttachment, modulationAttachment, modulationRateAttachment;
    std::unique_ptr<Attachment> stereoWidthAttachment, stereoRateAttachment, mixAttachment, tapeAgingAttachment, oscCharacterAttachment, lowCutAttachment, highCutAttachment, lowResonanceAttachment, highResonanceAttachment, wowAttachment, flutterAttachment, tapeHissAttachment, inputHighPassAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdLSpaceDubEchoAudioProcessorEditor)
};
