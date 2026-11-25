#pragma once
#include <JuceHeader.h>
#include <atomic>

class AdLSpaceDubEchoAudioProcessor final : public juce::AudioProcessor
{
public:
    AdLSpaceDubEchoAudioProcessor();
    ~AdLSpaceDubEchoAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
    void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override { return true; }
    const juce::String getName() const override { return JucePlugin_Name; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 8.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram (int) override {}
    const juce::String getProgramName (int) override { return {}; }
    void changeProgramName (int, const juce::String&) override {}
    void getStateInformation (juce::MemoryBlock&) override;
    void setStateInformation (const void*, int) override;

    float getOutputVuLevel() const noexcept { return outputVuLevel.load (std::memory_order_relaxed); }
    juce::AudioProcessorValueTreeState& getAPVTS() { return parameters; }

private:
    std::atomic<float> outputVuLevel { 0.0f };
    juce::AudioProcessorValueTreeState parameters;
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    juce::AudioBuffer<float> delayBuffer;
    int writePosition = 0;
    double currentSampleRate = 44100.0;
    // Tape-style continuous delay-time glide, in samples.
    float smoothedDelaySamples = 0.0f;
    float timeGlideCoefficient = 0.0f;
    double modulationPhase = 0.0;
    double wowPhase = 0.0, flutterPhase = 0.0;
    float wowNoise = 0.0f, flutterNoise = 0.0f;
    float wowNoiseTarget = 0.0f, flutterNoiseTarget = 0.0f;
    int wowRandomCounter = 0, flutterRandomCounter = 0;
    juce::Random wowRandom, flutterRandom;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> wowAmountSmoothed, flutterAmountSmoothed;
    float lowpassL = 0.0f, lowpassR = 0.0f;
    float agingLowpassL = 0.0f, agingLowpassR = 0.0f;
    struct BiquadCoefficients { double b0 = 1.0, b1 = 0.0, b2 = 0.0, a1 = 0.0, a2 = 0.0; };
    struct BiquadState
    {
        double z1 = 0.0, z2 = 0.0;
        float process (float input, const BiquadCoefficients& c) noexcept
        {
            const double x = input;
            const double y = c.b0 * x + z1;
            z1 = c.b1 * x - c.a1 * y + z2;
            z2 = c.b2 * x - c.a2 * y;
            return (float) y;
        }
    };
    BiquadState highpassL, highpassR, lowpassFilterL, lowpassFilterR;
    BiquadState inputHpL, inputHpR;
    float hissHpL = 0.0f, hissHpR = 0.0f, hissLpL = 0.0f, hissLpR = 0.0f;
    juce::Random hissRandom;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> hissLevelSmoothed;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> chaosBlend;
    juce::SmoothedValue<float, juce::ValueSmoothingTypes::Linear> sendSmoothed, throwSmoothed;
    bool wasBoostPressed = false;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AdLSpaceDubEchoAudioProcessor)
};
