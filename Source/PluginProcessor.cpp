#include "PluginProcessor.h"
#include "PluginEditor.h"

AdLSpaceDubEchoAudioProcessor::AdLSpaceDubEchoAudioProcessor()
    : AudioProcessor (BusesProperties()
        .withInput ("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput ("Output", juce::AudioChannelSet::stereo(), true)),
      parameters (*this, nullptr, "PARAMETERS", createParameterLayout())
{
}

juce::AudioProcessorValueTreeState::ParameterLayout AdLSpaceDubEchoAudioProcessor::createParameterLayout()
{
    using P = juce::AudioProcessorValueTreeState;
    P::ParameterLayout layout;
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "time", "Time", juce::NormalisableRange<float>(40.0f, 2000.0f, 1.0f), 375.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "feedback", "Feedback", juce::NormalisableRange<float>(0.0f, 1.15f, 0.01f), 0.42f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "tone", "Tone", juce::NormalisableRange<float>(0.01f, 0.98f, 0.01f), 0.35f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "drive", "Drive", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.18f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "modulation", "Modulation", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.10f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "modRate", "Modulation Rate", [] { juce::NormalisableRange<float> range (0.01f, 8.0f, 0.01f); range.setSkewForCentre (1.0f); return range; }(), 0.25f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "stereoWidth", "Stereo Width", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.50f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "stereoRate", "Ping Pong", juce::NormalisableRange<float>(0.01f, 3.0f, 0.01f), 0.20f));
    layout.add (std::make_unique<juce::AudioParameterFloat>(
        "mix", "Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.30f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("tapeAging", "Tape Aging", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("dubChaos", "Stereo Chaos", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("oscCharacter", "Osc Character", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("lowCut", "Low Cut", juce::NormalisableRange<float> (20.0f, 5000.0f, 1.0f, 0.35f), 20.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("highCut", "High Cut", juce::NormalisableRange<float> (800.0f, 20000.0f, 1.0f, 0.45f), 20000.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("lowResonance", "Low Resonance", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("highResonance", "High Resonance", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("wow", "Wow", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("flutter", "Flutter", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("tapeHiss", "Tape Hiss", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 0.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> ("inputHighPass", "Input Hi Pass", juce::NormalisableRange<float> (20.0f, 2000.0f, 1.0f, 0.40f), 20.0f));
    layout.add (std::make_unique<juce::AudioParameterFloat> (
        "sendToEcho", "Send To Echo", juce::NormalisableRange<float> (0.0f, 1.0f, 0.01f), 1.0f));
    layout.add (std::make_unique<juce::AudioParameterBool> (
        "feedbackThrow", "Feedback Throw", false));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "boostLevel", "Boost Level", juce::StringArray { "98%", "105%", "115%" }, 0));
    layout.add (std::make_unique<juce::AudioParameterChoice> (
        "boostRelease", "Boost Release", juce::StringArray { "50 ms", "500 ms", "1 s", "3 s", "5 s", "10 s" }, 2));
    return layout;
}

void AdLSpaceDubEchoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;
    delayBuffer.setSize (2, (int) std::ceil (2.1 * sampleRate) + samplesPerBlock + 4);
    delayBuffer.clear();
    writePosition = 0;
    smoothedDelaySamples = parameters.getRawParameterValue ("time")->load()
                          * 0.001f * (float) currentSampleRate;
    // 400 ms tape-speed response; calculated once, never in the audio loop.
    timeGlideCoefficient = 1.0f - std::exp (-1.0f / (0.400f * (float) currentSampleRate));
    modulationPhase = 0.0;
    wowPhase = flutterPhase = 0.0;
    wowNoise = flutterNoise = 0.0f;
    wowNoiseTarget = flutterNoiseTarget = 0.0f;
    wowRandomCounter = flutterRandomCounter = 0;
    wowAmountSmoothed.reset (sampleRate, 0.035);
    flutterAmountSmoothed.reset (sampleRate, 0.035);
    wowAmountSmoothed.setCurrentAndTargetValue (0.0f);
    flutterAmountSmoothed.setCurrentAndTargetValue (0.0f);
    lowpassL = lowpassR = 0.0f;
    agingLowpassL = agingLowpassR = 0.0f;
    highpassL = {}; highpassR = {}; lowpassFilterL = {}; lowpassFilterR = {};
    inputHpL = {}; inputHpR = {};
    hissHpL = hissHpR = hissLpL = hissLpR = 0.0f;
    hissLevelSmoothed.reset (sampleRate, 0.05);
    hissLevelSmoothed.setCurrentAndTargetValue (0.0f);
    chaosBlend.reset (sampleRate, 0.06);
    chaosBlend.setCurrentAndTargetValue (0.0f);
    sendSmoothed.reset (sampleRate, 0.025);
    sendSmoothed.setCurrentAndTargetValue (parameters.getRawParameterValue ("sendToEcho")->load());
    throwSmoothed.reset (sampleRate, 0.045);
    throwSmoothed.setCurrentAndTargetValue (0.0f);
    wasBoostPressed = false;
}

void AdLSpaceDubEchoAudioProcessor::releaseResources()
{
    delayBuffer.setSize (0, 0);
}

bool AdLSpaceDubEchoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    return layouts.getMainInputChannelSet() == juce::AudioChannelSet::stereo()
        && layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void AdLSpaceDubEchoAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer&)
{
    juce::ScopedNoDenormals noDenormals;
    if (buffer.getNumChannels() < 2 || delayBuffer.getNumSamples() < 4)
        return;

    const float timeMs = parameters.getRawParameterValue("time")->load();
    const float feedback = parameters.getRawParameterValue("feedback")->load();
    sendSmoothed.setTargetValue (parameters.getRawParameterValue ("sendToEcho")->load());
    const bool boostPressed = parameters.getRawParameterValue ("feedbackThrow")->load() > 0.5f;
    // Different linear attack and release times. Preserve the current smoothed
    // value when changing direction, including a retrigger during release.
    static constexpr double releaseSeconds[] = { 0.050, 0.500, 1.000, 3.000, 5.000, 10.000 };
    const int releaseIndex = juce::jlimit (0, 5, (int) parameters.getRawParameterValue ("boostRelease")->load());
    const float boostTargets[] = { 0.98f, 1.05f, 1.15f };
    const int levelIndex = juce::jlimit (0, 2, (int) parameters.getRawParameterValue ("boostLevel")->load());
    const float boostTarget = boostTargets[levelIndex];
    if (boostPressed != wasBoostPressed)
    {
        throwSmoothed.reset (currentSampleRate, boostPressed ? 0.005 : releaseSeconds[releaseIndex]);
        wasBoostPressed = boostPressed;
    }
    throwSmoothed.setTargetValue (boostPressed ? 1.0f : 0.0f);
    // While held, BOOST continuously injects incoming audio independently of SEND.
    const float tone = parameters.getRawParameterValue("tone")->load();
    const float drive = parameters.getRawParameterValue("drive")->load();
    const float lowCutHz = parameters.getRawParameterValue("lowCut")->load();
    const float highCutHz = parameters.getRawParameterValue("highCut")->load();
    const bool enableLowCut = lowCutHz > 20.5f;
    const bool enableHighCut = highCutHz < 19999.0f;
    const float lowResonance = parameters.getRawParameterValue("lowResonance")->load();
    const float highResonance = parameters.getRawParameterValue("highResonance")->load();
    // Resonant 2-pole biquads, 12 dB/oct. Coefficients calculated once per block.
    // Q = 0.707 (Butterworth) at zero resonance, rising to 2.0.
    auto makeFilter = [this] (float cutoff, float resonance, bool highpass)
    {
        const double fs = currentSampleRate;
        const double hz = juce::jlimit (10.0, 0.45 * fs, (double) cutoff);
        const double w = juce::MathConstants<double>::twoPi * hz / fs;
        const double cs = std::cos (w), sn = std::sin (w);
        const double q = 0.7071067811865476 + 1.2928932188134524 * (double) resonance;
        const double alpha = sn / (2.0 * q);
        const double a0 = 1.0 + alpha;
        BiquadCoefficients c;
        c.b0 = ((highpass ? 1.0 + cs : 1.0 - cs) * 0.5) / a0;
        c.b1 = (highpass ? -(1.0 + cs) : (1.0 - cs)) / a0;
        c.b2 = c.b0;
        c.a1 = (-2.0 * cs) / a0;
        c.a2 = (1.0 - alpha) / a0;
        return c;
    };
    const auto hp = makeFilter (lowCutHz, lowResonance, true);
    const auto lp = makeFilter (highCutHz, highResonance, false);
    const float inputHighPassHz = parameters.getRawParameterValue ("inputHighPass")->load();
    const bool inputHpEnabled = inputHighPassHz > 20.5f;
    const auto inputHpCoeffs = makeFilter (inputHighPassHz, 0.0f, true);
    hissLevelSmoothed.setTargetValue (parameters.getRawParameterValue ("tapeHiss")->load());
    const float hissHpCoeff = std::exp (-juce::MathConstants<float>::twoPi * 180.0f / (float) currentSampleRate);
    const float hissLpCoeff = std::exp (-juce::MathConstants<float>::twoPi * juce::jmin (8500.0f, 0.4f * (float) currentSampleRate) / (float) currentSampleRate);
    const float oscCharacter = parameters.getRawParameterValue("oscCharacter")->load();
    const float tapeAging = parameters.getRawParameterValue("tapeAging")->load();
    const float modulation = parameters.getRawParameterValue("modulation")->load();
    const float modulationRate = parameters.getRawParameterValue("modRate")->load();
    wowAmountSmoothed.setTargetValue (parameters.getRawParameterValue ("wow")->load());
    flutterAmountSmoothed.setTargetValue (parameters.getRawParameterValue ("flutter")->load());
    const float stereoWidth = parameters.getRawParameterValue("stereoWidth")->load();
    // Keep the existing parameter ID and range for saved REAPER projects.
    // It now controls cross-feedback intensity, not an LFO frequency.
    const float pingPong = juce::jlimit (0.0f, 1.0f,
        (parameters.getRawParameterValue("stereoRate")->load() - 0.01f) / 2.99f);
    const float mix = parameters.getRawParameterValue("mix")->load();
    const float chaosAmount = juce::jlimit (0.0f, 1.0f, parameters.getRawParameterValue("dubChaos")->load());
    chaosBlend.setTargetValue (chaosAmount);

    const int n = delayBuffer.getNumSamples();
    const float targetDelaySamples = timeMs * 0.001f * (float) currentSampleRate;
    // Increased from +/-20 ms to +/-30 ms at maximum modulation.
    const float modulationDepthSamples = modulation * 0.030f * (float) currentSampleRate;
    const float coeff = juce::jlimit (0.005f, 0.98f, tone);
    const float dry = 1.0f - mix;
    // Tape Aging adds a separate, per-repeat magnetic loss in the feedback path.
    // At zero the DSP is identical to the preceding release.
    const float agingCutoffHz = 15000.0f - tapeAging * 14200.0f;
    const float agingPole = std::exp (-juce::MathConstants<float>::twoPi * agingCutoffHz / (float) currentSampleRate);
    const float agingMix = tapeAging;
    const float agingDrive = 1.0f + 3.0f * tapeAging;

    auto* left = buffer.getWritePointer (0);
    auto* right = buffer.getWritePointer (1);
    auto* delayLeft = delayBuffer.getWritePointer (0);
    auto* delayRight = delayBuffer.getWritePointer (1);

    const double phaseIncrement = juce::MathConstants<double>::twoPi
                                * (double) modulationRate / currentSampleRate;

    double blockVuEnergy = 0.0;
    for (int i = 0; i < buffer.getNumSamples(); ++i)
    {
        const float originalInputL = left[i];
        const float originalInputR = right[i];
        // Input HPF is the first DSP stage, before injection, delay and feedback.
        // The dry path remains untouched, as is conventional for an effect send.
        const float inputL = inputHpEnabled ? inputHpL.process (originalInputL, inputHpCoeffs) : originalInputL;
        const float inputR = inputHpEnabled ? inputHpR.process (originalInputR, inputHpCoeffs) : originalInputR;
        // Mod Rate is the ONLY oscillator affecting delay read positions.
        // Two bounded, smooth tape-speed irregularities. They affect only delay read
        // positions, never the recirculation gain. Independent from Mod Rate.
        // Slowly interpolated random targets prevent abrupt delay-time jumps.
        if (--wowRandomCounter <= 0)
        {
            wowRandomCounter = juce::jmax (1, (int) (currentSampleRate * 0.65));
            wowNoiseTarget = wowRandom.nextFloat() * 2.0f - 1.0f;
        }
        if (--flutterRandomCounter <= 0)
        {
            flutterRandomCounter = juce::jmax (1, (int) (currentSampleRate * 0.075));
            flutterNoiseTarget = flutterRandom.nextFloat() * 2.0f - 1.0f;
        }
        const float wowSmooth = 1.0f - std::exp (-1.0f / (0.20f * (float) currentSampleRate));
        const float flutterSmooth = 1.0f - std::exp (-1.0f / (0.018f * (float) currentSampleRate));
        wowNoise += wowSmooth * (wowNoiseTarget - wowNoise);
        flutterNoise += flutterSmooth * (flutterNoiseTarget - flutterNoise);
        const float wowAmount = wowAmountSmoothed.getNextValue();
        const float flutterAmount = flutterAmountSmoothed.getNextValue();
        // Maximum wow +/- ~18 ms (previously 6 ms); flutter +/- ~0.8 ms.
        const float wowOffset = wowAmount * (0.012f * static_cast<float> (std::sin (wowPhase)) + 0.006f * wowNoise)
                              * (float) currentSampleRate;
        const float flutterOffset = flutterAmount * (0.0005f * static_cast<float> (std::sin (flutterPhase)) + 0.0003f * flutterNoise)
                                  * (float) currentSampleRate;
        const float modOffset = (float) std::sin (modulationPhase) * modulationDepthSamples
                              + wowOffset + flutterOffset;
        // Move the read head continuously: pitch bends like a tape echo,
        // instead of jumping between unrelated samples when TIME changes.
        smoothedDelaySamples += timeGlideCoefficient * (targetDelaySamples - smoothedDelaySamples);
        const float delaySamplesL = juce::jlimit (1.0f, (float) n - 3.0f,
                                                  smoothedDelaySamples + modOffset);
        const float delaySamplesR = delaySamplesL;

        auto readEcho = [n, this] (const float* channel, float delaySamples)
        {
            float pos = (float) writePosition - delaySamples;
            if (pos < 0.0f) pos += (float) n;
            const int a = (int) pos;
            const int b = (a + 1) % n;
            return channel[a] + (pos - (float) a) * (channel[b] - channel[a]);
        };
        const float rawEchoL = readEcho (delayLeft, delaySamplesL);
        const float rawEchoR = readEcho (delayRight, delaySamplesR);

        // Low-pass filtering makes successive repeats progressively darker.
        lowpassL += coeff * (rawEchoL - lowpassL);
        lowpassR += coeff * (rawEchoR - lowpassR);

        // Drive controls saturation directly; no separate Drive Strength parameter.
        const float driveGain = 1.0f + drive * 7.0f;
        const float clippedL = std::tanh (lowpassL * driveGain) / driveGain;
        const float clippedR = std::tanh (lowpassR * driveGain) / driveGain;
        float echoL = lowpassL * (1.0f - drive) + clippedL * drive;
        float echoR = lowpassR * (1.0f - drive) + clippedR * drive;

        // Preserve the un-widened echoes for the feedback loop.
        // Stereo Width must NEVER increase feedback gain.
        const float feedbackEchoL = echoL;
        const float feedbackEchoR = echoR;

        // Stereo width is applied to the output ONLY, after feedback is computed.
        // Normalize the mid/side matrix to avoid boosting peak output at high width.
        const float mid = 0.5f * (echoL + echoR);
        const float side = 0.5f * (echoL - echoR);
        const float widthMultiplier = stereoWidth * 2.0f;
        const float widthGain = 1.0f / juce::jmax (1.0f, widthMultiplier);
        const float outputEchoL = (mid + side * widthMultiplier) * widthGain;
        const float outputEchoR = (mid - side * widthMultiplier) * widthGain;

        const float chaos = chaosBlend.getNextValue();
        // The special mode deliberately couples stereo widening and cross-feedback.
        // At maximum width/ping-pong it introduces stronger, deliberately unstable cross-channel loop gain.
        // The normal mode is unchanged.
        const float chaosInteraction = chaos * pingPong * stereoWidth;
        const float chaosFeedbackL = feedbackEchoL + chaosInteraction * 1.20f * (feedbackEchoL - feedbackEchoR);
        const float chaosFeedbackR = feedbackEchoR + chaosInteraction * 1.20f * (feedbackEchoR - feedbackEchoL);

        // Cross-feedback produces genuine alternating L/R repeats.
        // At maximum Ping Pong a mono input enters the left delay only,
        // then each repeat feeds the opposite channel.
        constexpr float feedbackSaturation = 1.65f;
        const float saturatedFeedbackL = std::tanh (chaosFeedbackL * feedbackSaturation) / feedbackSaturation;
        const float saturatedFeedbackR = std::tanh (chaosFeedbackR * feedbackSaturation) / feedbackSaturation;
        const float monoInput = 0.5f * (inputL + inputR);
        const float manualSend = sendSmoothed.getNextValue();
        // A held BOOST keeps the input open at full level for its entire duration.
        // Once released, SEND TO ECHO regains control of the incoming signal.
        const float sendLevel = boostPressed ? 1.0f : manualSend;
        const float injectionL = sendLevel * ((1.0f - pingPong) * inputL + pingPong * monoInput);
        const float injectionR = sendLevel * ((1.0f - pingPong) * inputR);
        const float feedbackL = (1.0f - pingPong) * saturatedFeedbackL
                              + pingPong * saturatedFeedbackR;
        const float feedbackR = (1.0f - pingPong) * saturatedFeedbackR
                              + pingPong * saturatedFeedbackL;
        // Soft saturation bounds the internal signal even when the loop is driven hard.
        // This ceiling is active only in Stereo Chaos mode, leaving the normal DSP intact.
        // Filter and saturate the recirculating repeats, not the dry input.
        // This makes each new echo progressively darker and more textured.
        agingLowpassL = (1.0f - agingPole) * feedbackL + agingPole * agingLowpassL;
        agingLowpassR = (1.0f - agingPole) * feedbackR + agingPole * agingLowpassR;
        const float agedL = std::tanh (agingLowpassL * agingDrive) / agingDrive;
        const float agedR = std::tanh (agingLowpassR * agingDrive) / agingDrive;
        const float tapeFeedbackL = feedbackL + agingMix * (agedL - feedbackL);
        const float tapeFeedbackR = feedbackR + agingMix * (agedR - feedbackR);
        // Analog-style feedback saturation. Zero is bit-for-bit the former path.
        // The normalized tanh curve never amplifies small signals: Feedback remains
        // the sole source of loop gain, while Character controls the saturation knee.
        const float oscDrive = 1.0f + 9.0f * oscCharacter;
        const float oscSoftL = std::tanh (tapeFeedbackL * oscDrive) / oscDrive;
        const float oscSoftR = std::tanh (tapeFeedbackR * oscDrive) / oscDrive;
        const float oscLoopL = tapeFeedbackL + oscCharacter * (oscSoftL - tapeFeedbackL);
        const float oscLoopR = tapeFeedbackR + oscCharacter * (oscSoftR - tapeFeedbackR);
        // Dedicated frequency shaping INSIDE the recirculation path.
        // The extreme positions bypass exactly to preserve legacy sound.
        // A one-pole HP followed by a one-pole LP creates cumulative loss per repeat.
        const float hpL = enableLowCut ? highpassL.process (oscLoopL, hp) : oscLoopL;
        const float hpR = enableLowCut ? highpassR.process (oscLoopR, hp) : oscLoopR;
        const float filteredLoopL = enableHighCut ? lowpassFilterL.process (hpL, lp) : hpL;
        const float filteredLoopR = enableHighCut ? lowpassFilterR.process (hpR, lp) : hpR;
        // Gentle protection only when resonance is raised; bypass stays transparent.
        // Keeps extreme feedback experiments bounded without changing the default sound.
        const float resonanceAmount = juce::jmax (enableLowCut ? lowResonance : 0.0f,
                                                 enableHighCut ? highResonance : 0.0f);
        const float protectedL = filteredLoopL + resonanceAmount *
            (std::tanh (filteredLoopL * 1.5f) / 1.5f - filteredLoopL);
        const float protectedR = filteredLoopR + resonanceAmount *
            (std::tanh (filteredLoopR * 1.5f) / 1.5f - filteredLoopR);
        // Momentary throw reaches at least 0.98 without lowering a higher user setting.
        const float throwAmount = throwSmoothed.getNextValue();
        const float effectiveFeedback = feedback + throwAmount * (juce::jmax (feedback, boostTarget) - feedback);
        const float writeL = injectionL + protectedL * effectiveFeedback;
        const float writeR = injectionR + protectedR * effectiveFeedback;
        const auto safeChaosWrite = [] (float x) { return 4.0f * std::tanh (x / 4.0f); };
        delayLeft[writePosition] = writeL + chaos * (safeChaosWrite (writeL) - writeL);
        delayRight[writePosition] = writeR + chaos * (safeChaosWrite (writeR) - writeR);

        // Band-limited independent tape hiss, added only to the wet output.
        // Never feeds the delay loop. At 0% it contributes exactly zero.
        const float hissLevel = hissLevelSmoothed.getNextValue();
        const float hissNoiseL = hissRandom.nextFloat() * 2.0f - 1.0f;
        const float hissNoiseR = hissRandom.nextFloat() * 2.0f - 1.0f;
        hissHpL = hissHpCoeff * hissHpL + (1.0f - hissHpCoeff) * hissNoiseL;
        hissHpR = hissHpCoeff * hissHpR + (1.0f - hissHpCoeff) * hissNoiseR;
        hissLpL = hissLpCoeff * hissLpL + (1.0f - hissLpCoeff) * (hissNoiseL - hissHpL);
        hissLpR = hissLpCoeff * hissLpR + (1.0f - hissLpCoeff) * (hissNoiseR - hissHpR);
        const float hissGain = 0.018f * hissLevel * hissLevel * mix;
        const float outputL = dry * originalInputL + mix * outputEchoL + hissGain * hissLpL;
        const float outputR = dry * originalInputR + mix * outputEchoR + hissGain * hissLpR;
        // Protect against extreme wet peaks in chaos mode; not a true-peak limiter.
        left[i] = outputL + chaos * (2.0f * std::tanh (outputL / 2.0f) - outputL);
        right[i] = outputR + chaos * (2.0f * std::tanh (outputR / 2.0f) - outputR);

        // Mono equivalent RMS power for a genuine output VU, after wet/dry and hiss.
        const float meterL = left[i], meterR = right[i];
        blockVuEnergy += 0.5 * (double(meterL) * meterL + double(meterR) * meterR);
        if (++writePosition >= n)
            writePosition = 0;

        wowPhase += juce::MathConstants<double>::twoPi * 0.38 / currentSampleRate;
        flutterPhase += juce::MathConstants<double>::twoPi * 7.2 / currentSampleRate;
        if (wowPhase >= juce::MathConstants<double>::twoPi) wowPhase -= juce::MathConstants<double>::twoPi;
        if (flutterPhase >= juce::MathConstants<double>::twoPi) flutterPhase -= juce::MathConstants<double>::twoPi;
        modulationPhase += phaseIncrement;
        if (modulationPhase >= juce::MathConstants<double>::twoPi)
            modulationPhase -= juce::MathConstants<double>::twoPi;
    }
    const float blockRms = (float) std::sqrt (blockVuEnergy / (double) juce::jmax (1, buffer.getNumSamples()));
    // Atomic handoff: audio thread never accesses UI objects.
    outputVuLevel.store (juce::jlimit (0.0f, 2.0f, blockRms), std::memory_order_relaxed);

}

juce::AudioProcessorEditor* AdLSpaceDubEchoAudioProcessor::createEditor()
{
    return new AdLSpaceDubEchoAudioProcessorEditor (*this);
}

void AdLSpaceDubEchoAudioProcessor::getStateInformation (juce::MemoryBlock& dest)
{
    auto state = parameters.copyState();
    std::unique_ptr<juce::XmlElement> xml (state.createXml());
    copyXmlToBinary (*xml, dest);
}

void AdLSpaceDubEchoAudioProcessor::setStateInformation (const void* data, int size)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, size));
    if (xml && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AdLSpaceDubEchoAudioProcessor();
}
