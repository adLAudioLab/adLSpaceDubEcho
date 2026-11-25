#include "PluginEditor.h"
#include "BinaryData.h"
#include <cmath>

void TapeMachineLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h, float position, float start, float end, juce::Slider&)
{
    static const auto image = juce::ImageCache::getFromMemory(BinaryData::Knob_png, BinaryData::Knob_pngSize);
    const float cx = x + w * 0.5f, cy = y + h * 0.5f;
    const float d = (float) juce::jmin(w,h);
    juce::Graphics::ScopedSaveState save(g);
    // Fixed, radial contact shadow, independent of knob rotation.
    juce::ColourGradient shadow(juce::Colours::black.withAlpha(0.45f), cx, cy,
                                juce::Colours::transparentBlack, cx + d * 0.55f, cy, true);
    shadow.addColour(0.88, juce::Colours::black.withAlpha(0.35f));
    g.setGradientFill(shadow);
    g.fillEllipse(cx - d * 0.55f, cy - d * 0.55f, d * 1.10f, d * 1.10f);
    g.addTransform(juce::AffineTransform::rotation(start + position * (end-start), cx, cy));
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImage(image, juce::Rectangle<float>(cx-d/2,cy-d/2,d,d));
}
AdLSpaceDubEchoAudioProcessorEditor::AdLSpaceDubEchoAudioProcessorEditor (AdLSpaceDubEchoAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    auto load = [](const void* data, int size) { return juce::ImageCache::getFromMemory(data, size); };
    facadeImage = load(BinaryData::Mainframe_png, BinaryData::Mainframe_pngSize);
    stripeImage = load(BinaryData::Stripe_png, BinaryData::Stripe_pngSize);
    framesImage = load(BinaryData::Frames_png, BinaryData::Frames_pngSize);
    meterImage = load(BinaryData::Meter_png, BinaryData::Meter_pngSize);
    controlsLabelsImage = load(BinaryData::Labels_png, BinaryData::Labels_pngSize);
    graduationsImage = load(BinaryData::Graduations_png, BinaryData::Graduations_pngSize);
    feedbackBoostOverlayImage = load(BinaryData::Boost_png, BinaryData::Boost_pngSize);
    // Initial compact size (80% of the 1440 x 720 design reference).
    // The host may constrain the outer plugin window independently.
    setResizable (true, true);
    setResizeLimits (900, 450, 1920, 960);
    if (auto* windowBoundsConstrainer = getConstrainer())
        windowBoundsConstrainer->setFixedAspectRatio (2.0);
    setSize (1152, 576);
    vintageLookAndFeel.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffe59c45));
    vintageLookAndFeel.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff393e43));
    vintageLookAndFeel.setColour (juce::Slider::thumbColourId, juce::Colour (0xfff2d7a5));
    vintageLookAndFeel.setColour (juce::Slider::textBoxTextColourId, juce::Colour (0xffe5d3b0));
    vintageLookAndFeel.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour (0xff292820));
    vintageLookAndFeel.setColour (juce::Slider::textBoxOutlineColourId, juce::Colour (0xff76664d));
    vintageLookAndFeel.setColour (juce::Label::textColourId, juce::Colour (0xffe5d3b0));
    setLookAndFeel (&vintageLookAndFeel);

    title.setText ("adLSpaceDubEcho", juce::dontSendNotification);
    title.setColour (juce::Label::textColourId, juce::Colour (0xffe3cfaa));
    title.setJustificationType (juce::Justification::centred);
    title.setFont (juce::Font (juce::FontOptions (29.0f).withStyle ("Bold")));
    // The engraved title is already part of the vintage metal faceplate.
    title.setVisible (false);

    auto setupRotary = [this] (juce::Slider& slider, juce::Label& label, const juce::String& name)
    {
        slider.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
        slider.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
        slider.setTooltip (name + " : drag to adjust, double-click for default");
        slider.setColour (juce::Slider::rotarySliderFillColourId, juce::Colour (0xffe59c45));
        slider.setColour (juce::Slider::rotarySliderOutlineColourId, juce::Colour (0xff393e43));
        slider.setColour (juce::Slider::thumbColourId, juce::Colour (0xfff2d7a5));
        label.setFont (juce::Font (juce::FontOptions (11.0f).withStyle ("Bold")));
        label.setText (name, juce::dontSendNotification);
        label.setJustificationType (juce::Justification::centred);
        addAndMakeVisible (slider);
        label.setVisible (false);
    };

    setupRotary (sendToEcho, sendToEchoLabel, "SEND TO ECHO");
    sendToEcho.setSliderStyle (juce::Slider::LinearHorizontal);
    sendToEcho.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    // Custom metal fader is rendered over children. Slider remains the interactive hit target.
    sendToEcho.setAlpha (1.0f);
    sendToEcho.setTooltip ("SEND TO ECHO : input level to the delay; 100% sends the full input level");
    feedbackThrowButton.setButtonText ({});
    feedbackThrowButton.setTooltip ("FEEDBACK BOOST : hold to inject audio and raise feedback to the selected level");
    feedbackThrowButton.setClickingTogglesState (false);
    feedbackThrowButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    feedbackThrowButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (feedbackThrowButton);
    feedbackThrowButton.setAlpha (1.0f);
    feedbackThrowButton.onStateChange = [this]
    {
        const bool down = feedbackThrowButton.isDown();
        auto* param = processor.getAPVTS().getParameter ("feedbackThrow");
        if (param == nullptr) return;
        if (down && !throwGestureActive)
        {
            param->beginChangeGesture();
            throwGestureActive = true;
        }
        if ((param->getValue() > 0.5f) != down)
            param->setValueNotifyingHost (down ? 1.0f : 0.0f);
        if (!down && throwGestureActive)
        {
            param->endChangeGesture();
            throwGestureActive = false;
        }
        repaint();
    };
    // APVTS-bound selectors are kept as non-visible state bridges. The visible
    // controls are entirely custom JUCE overlay artwork, never a modified PNG.
    boostLevelSelect.addItemList (juce::StringArray { "98%", "105%", "115%" }, 1);
    boostReleaseSelect.addItemList (juce::StringArray { "50ms", "0.5s", "1s", "3s", "5s", "10s" }, 1);
    boostLevelSelect.setVisible (false);
    boostReleaseSelect.setVisible (false);
    addChildComponent (boostLevelSelect);
    addChildComponent (boostReleaseSelect);
    boostLevelSwitch.setButtonText ({});
    boostReleaseButton.setButtonText ({});
    for (auto* button : { &boostLevelSwitch, &boostReleaseButton })
    {
        button->setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
        button->setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
        button->setAlpha (1.0f); // hit target only; artwork painted in paintOverChildren
        addAndMakeVisible (*button);
    }
    boostLevelSwitch.setTooltip ("BOOST LEVEL: cycle 98%, 105%, 115%");
    boostReleaseButton.setTooltip ("BOOST RELEASE: cycle 50 ms, 500 ms, 1 s, 3 s, 5 s, 10 s");
    boostLevelSwitch.onClick = [this]
    {
        boostLevelSelect.setSelectedId (boostLevelSelect.getSelectedId() % 3 + 1,
                                        juce::sendNotificationSync);
        repaint();
    };
    boostReleaseButton.onClick = [this]
    {
        boostReleaseSelect.setSelectedId (boostReleaseSelect.getSelectedId() % 6 + 1,
                                          juce::sendNotificationSync);
        repaint();
    };
    setupRotary (time, timeLabel, "TIME");
    setupRotary (feedback, feedbackLabel, "FEEDBACK");
    setupRotary (tone, toneLabel, "TONE");
    setupRotary (drive, driveLabel, "DRIVE");
    setupRotary (modulation, modulationLabel, "MOD DEPTH");
    setupRotary (modulationRate, modulationRateLabel, "MOD RATE");
    setupRotary (stereoWidth, stereoWidthLabel, "WIDTH");
    setupRotary (stereoRate, stereoRateLabel, "PING PONG");
    setupRotary (mix, mixLabel, "DRY / WET");
    setupRotary (tapeAging, tapeAgingLabel, "TAPE AGING");
    setupRotary (oscCharacter, oscCharacterLabel, "OSC CHARACTER");
    setupRotary (lowCut, lowCutLabel, "LOW CUT");
    setupRotary (highCut, highCutLabel, "HIGH CUT");
    setupRotary (lowResonance, lowResonanceLabel, "LOW RES");
    setupRotary (highResonance, highResonanceLabel, "HIGH RES");
    setupRotary (wow, wowLabel, "WOW");
    setupRotary (flutter, flutterLabel, "FLUTTER");
    setupRotary (tapeHiss, tapeHissLabel, "TAPE HISS");
    setupRotary (inputHighPass, inputHighPassLabel, "INPUT HPF");

    dubChaos.setSliderStyle (juce::Slider::RotaryHorizontalVerticalDrag);
    dubChaos.setRotaryParameters (juce::MathConstants<float>::pi * 1.25f, juce::MathConstants<float>::pi * 2.75f, true);
    dubChaos.setTextBoxStyle (juce::Slider::NoTextBox, false, 0, 0);
    dubChaos.setTooltip ("STEREO CHAOS");
    dubChaos.setTextValueSuffix (" %");
    dubChaos.setColour (juce::Slider::thumbColourId, juce::Colour (0xffe7b35d));
    dubChaosLabel.setText ("STEREO CHAOS", juce::dontSendNotification);
    dubChaosLabel.setJustificationType (juce::Justification::centred);
    dubChaosLabel.setFont (juce::Font (juce::FontOptions (11.0f).withStyle ("Bold")));
    dubChaosLabel.setVisible (false);
    addAndMakeVisible (dubChaos);

    auto& state = processor.getAPVTS();
    dubChaosAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment> (state, "dubChaos", dubChaos);
    sendToEchoAttachment = std::make_unique<Attachment> (state, "sendToEcho", sendToEcho);
    boostLevelAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (state, "boostLevel", boostLevelSelect);
    boostReleaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment> (state, "boostRelease", boostReleaseSelect);
    timeAttachment = std::make_unique<Attachment> (state, "time", time);
    feedbackAttachment = std::make_unique<Attachment> (state, "feedback", feedback);
    toneAttachment = std::make_unique<Attachment> (state, "tone", tone);
    driveAttachment = std::make_unique<Attachment> (state, "drive", drive);
    modulationAttachment = std::make_unique<Attachment> (state, "modulation", modulation);
    modulationRateAttachment = std::make_unique<Attachment> (state, "modRate", modulationRate);
    stereoWidthAttachment = std::make_unique<Attachment> (state, "stereoWidth", stereoWidth);
    stereoRateAttachment = std::make_unique<Attachment> (state, "stereoRate", stereoRate);
    mixAttachment = std::make_unique<Attachment> (state, "mix", mix);
    tapeAgingAttachment = std::make_unique<Attachment> (state, "tapeAging", tapeAging);
    oscCharacterAttachment = std::make_unique<Attachment> (state, "oscCharacter", oscCharacter);
    lowCutAttachment = std::make_unique<Attachment> (state, "lowCut", lowCut);
    highCutAttachment = std::make_unique<Attachment> (state, "highCut", highCut);
    lowResonanceAttachment = std::make_unique<Attachment> (state, "lowResonance", lowResonance);
    highResonanceAttachment = std::make_unique<Attachment> (state, "highResonance", highResonance);
    wowAttachment = std::make_unique<Attachment> (state, "wow", wow);
    flutterAttachment = std::make_unique<Attachment> (state, "flutter", flutter);
    tapeHissAttachment = std::make_unique<Attachment> (state, "tapeHiss", tapeHiss);
    inputHighPassAttachment = std::make_unique<Attachment> (state, "inputHighPass", inputHighPass);
    resized();
    startTimerHz (30);
}

AdLSpaceDubEchoAudioProcessorEditor::~AdLSpaceDubEchoAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel (nullptr);
}

void AdLSpaceDubEchoAudioProcessorEditor::paint (juce::Graphics& g)
{
    const float scale = (float)getWidth()/designWidth;
    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::scale(scale));
    const juce::Rectangle<float> full(0,0,1774,887);
    for (const auto* image : { &facadeImage, &stripeImage, &framesImage, &controlsLabelsImage, &graduationsImage, &feedbackBoostOverlayImage })
        g.drawImage(*image,full);
    g.drawImage(meterImage,juce::Rectangle<float>(1238,111,443,207));
    // Original level smoothing and -18 dBFS reference, relocated to original dial.
    const float db = juce::Decibels::gainToDecibels(juce::jmax(vuDisplay,0.000001f),-90.0f)+18.0f;
    const float t = juce::jlimit(0.0f,1.0f,(db+24.0f)/27.0f);
    const float angle = -1.40f + 2.80f*t;
    const float px=1459.0f, py=303.0f, length=120.0f;
    g.setColour(juce::Colour(0xff2b2119));
    g.drawLine(px,py,px+std::sin(angle)*length,py-std::cos(angle)*length,1.65f);
}
void AdLSpaceDubEchoAudioProcessorEditor::paintOverChildren(juce::Graphics& g)
{
    juce::Graphics::ScopedSaveState save(g);
    g.addTransform(juce::AffineTransform::scale((float)getWidth()/designWidth));
    const float x=1204.5f+89.0f*(float)sendToEcho.valueToProportionOfLength(sendToEcho.getValue());
    g.setColour(juce::Colour(0xffe8d8a6));
    g.fillRoundedRectangle(x-6.5f,394,13,31,3);
    const bool pressed=processor.getAPVTS().getRawParameterValue("feedbackThrow")->load()>0.5f;
    g.setColour(juce::Colour(0xff151512)); g.fillEllipse(1370,394,40,40);
    g.setColour(juce::Colour(pressed?0xffef3027:0xff8c231b)); g.fillEllipse(1372,396,36,36);
    auto led=[&g](float x,float y,bool on){g.setColour(juce::Colour(on?0xffe73522:0xff79764b));g.fillEllipse(x-6,y-6,12,12);};
    const int level=(int)processor.getAPVTS().getRawParameterValue("boostLevel")->load();
    const int release=(int)processor.getAPVTS().getRawParameterValue("boostRelease")->load();
    for(int i=0;i<3;++i) led(1460,387+28*i,i==level);
    for(int i=0;i<6;++i) led(1590+50*(i%3),389+46*(i/3),i==release);
}
void AdLSpaceDubEchoAudioProcessorEditor::timerCallback()
{
    const float level=processor.getOutputVuLevel();
    vuDisplay+=(level>vuDisplay?0.13f:0.055f)*(level-vuDisplay);
    repaint();
}
void AdLSpaceDubEchoAudioProcessorEditor::resized()
{
    const float scale=(float)getWidth()/designWidth;
    auto rect=[scale](float x,float y,float w,float h){return juce::Rectangle<float>(x*scale,y*scale,w*scale,h*scale).toNearestInt();};
    auto place=[&rect](juce::Slider& s,float x,float y,float d){s.setBounds(rect(x-d/2,y-d/2,d,d));};
    sendToEcho.setBounds(rect(1198,389,112,40));
    feedbackThrowButton.setBounds(rect(1370,394,40,40));
    boostLevelSwitch.setBounds(rect(1445,375,95,80));
    boostReleaseButton.setBounds(rect(1565,375,155,85));
    place(inputHighPass,125,235,94);
    place(mix,125,395,94);
    place(time,442,211,94);
    place(feedback,332,383,94);
    place(tone,552,383,94);
    place(drive,820,235,94);
    place(tapeAging,1020,235,94);
    place(wow,760,410,94);
    place(flutter,920,410,94);
    place(tapeHiss,1080,410,94);
    place(lowCut,130,625,94);
    place(lowResonance,130,785,94);
    place(highCut,330,625,94);
    place(highResonance,330,785,94);
    place(oscCharacter,587,705,154);
    place(modulation,850,651,124);
    place(modulationRate,1025,763,124);
    place(stereoWidth,1285,651,124);
    place(stereoRate,1610,651,124);
    place(dubChaos,1490,763,124);
}
