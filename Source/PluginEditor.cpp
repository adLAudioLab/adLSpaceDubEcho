#include "PluginEditor.h"
#include "FacadeImage.h"
#include "ControlsLabelsImage.h"
#include "RedLinesImage.h"
#include "VuLabelsImage.h"
#include "FeedbackBoostOverlayImage.h"
#include <cmath>

void TapeMachineLookAndFeel::drawRotarySlider (juce::Graphics& g, int x, int y, int w, int h,
                                                float position, float start, float end, juce::Slider&)
{
    const float cx = x + w * 0.5f;
    const float cy = y + h * 0.5f;
    const float r = juce::jmin (w, h) * 0.315f; // V11: smaller visible knob, same interactive bounds
    const float angle = start + position * (end - start);

    // All drawing is local to the slider; the shadow cannot be cut by its bounds.
    for (int i = 5; i >= 1; --i)
    {
        const float expand = (float)i * 0.65f;
        g.setColour (juce::Colour (0xff24190f).withAlpha (0.038f));
        g.fillEllipse (cx-r-expand, cy-r-expand+1.0f,
                       (r+expand)*2.0f, (r+expand)*2.0f);
    }

    // Deep black housing like the reference hardware, with an oxidised rim.
    g.setColour (juce::Colour (0xff191612));
    g.fillEllipse (cx-r, cy-r, 2.0f*r, 2.0f*r);
    g.setColour (juce::Colour (0xff4c3827));
    g.drawEllipse (cx-r+1.0f, cy-r+1.0f, 2.0f*r-2.0f, 2.0f*r-2.0f, 2.0f);
    const float faceR = r * 0.78f;
    juce::ColourGradient brushed (juce::Colour (0xffe0bd87), cx-faceR, cy-faceR,
                                  juce::Colour (0xff74533a), cx+faceR, cy+faceR, false);
    brushed.addColour (0.23, juce::Colour (0xffac8a62));
    brushed.addColour (0.48, juce::Colour (0xffd4b389));
    brushed.addColour (0.78, juce::Colour (0xff826447));
    g.setGradientFill (brushed);
    g.fillEllipse (cx-faceR, cy-faceR, 2.0f*faceR, 2.0f*faceR);

    // Fine concentric machining grooves: clearly visible, not a flat grey disk.
    for (int i = 0; i < 22; ++i)
    {
        const float rr = faceR * (0.20f + 0.033f * (float)i);
        g.setColour (juce::Colour (i % 4 == 0 ? 0x75553b25 : 0x38533b26));
        g.drawEllipse (cx-rr, cy-rr, rr*2.0f, rr*2.0f, 0.55f);
    }

    // Deterministic small pitting, scratches and aged varnish.
    for (int i = 0; i < 55; ++i)
    {
        const float theta = (float)i * 2.39996323f;
        const float rr = faceR * (0.15f + (float)((i * 37) % 75) / 100.0f);
        const float px = cx + std::cos(theta)*rr;
        const float py = cy + std::sin(theta)*rr;
        g.setColour (juce::Colour (i%5 == 0 ? 0xa45a3721 : 0x6b4e3724));
        g.fillEllipse (px, py, (i%3 == 0 ? 1.8f : 0.9f), (i%4 == 0 ? 1.7f : 0.9f));
    }
    g.setColour (juce::Colour (0xff443121));
    g.drawEllipse (cx-faceR, cy-faceR, faceR*2.0f, faceR*2.0f, 1.2f);

    // A worn black engraved indicator. Rotate ONLY the indicator and its wear.
    {
        juce::Graphics::ScopedSaveState save (g);
        g.addTransform (juce::AffineTransform::rotation (angle, cx, cy));
        g.setColour (juce::Colour (0xff261a12));
        g.drawLine (cx, cy-faceR*0.12f, cx, cy-faceR*0.91f, 3.3f);
        g.setColour (juce::Colour (0x998d7253));
        g.drawLine (cx+2.2f, cy-faceR*0.16f, cx+2.2f, cy-faceR*0.78f, 0.75f);
        g.setColour (juce::Colour (0x88b5976b));
        g.drawLine (cx-0.5f, cy-faceR*0.55f, cx-0.5f, cy-faceR*0.61f, 0.8f);
    }
}

AdLSpaceDubEchoAudioProcessorEditor::AdLSpaceDubEchoAudioProcessorEditor (AdLSpaceDubEchoAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    facadeImage = juce::ImageCache::getFromMemory (facadePng, facadePngSize);
    controlsLabelsImage = juce::ImageCache::getFromMemory (controlsLabelsPng, controlsLabelsPngSize);
    redLinesImage = juce::ImageCache::getFromMemory (redLinesPng, redLinesPngSize);
    vuLabelsImage = juce::ImageCache::getFromMemory (vuLabelsPng, vuLabelsPngSize);
    feedbackBoostOverlayImage = juce::ImageCache::getFromMemory (feedbackBoostOverlayPng, feedbackBoostOverlayPngSize);
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
    sendToEcho.setAlpha (0.0f);
    sendToEcho.setTooltip ("SEND TO ECHO : input level to the delay; 100% preserves v1.0");
    feedbackThrowButton.setButtonText ({});
    feedbackThrowButton.setTooltip ("FEEDBACK BOOST : hold to inject audio and raise feedback to the selected level");
    feedbackThrowButton.setClickingTogglesState (false);
    feedbackThrowButton.setColour (juce::TextButton::buttonColourId, juce::Colours::transparentBlack);
    feedbackThrowButton.setColour (juce::TextButton::buttonOnColourId, juce::Colours::transparentBlack);
    addAndMakeVisible (feedbackThrowButton);
    feedbackThrowButton.setAlpha (0.0f);
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
        button->setAlpha (0.0f); // hit target only; artwork painted in paintOverChildren
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
    // Original static artwork, composited as separate PNG layers in its validated order.
    const float scale = (float) getWidth() / (float) designWidth;
    juce::Graphics::ScopedSaveState save (g);
    g.addTransform (juce::AffineTransform::scale (scale));
    const juce::Rectangle<float> fullPanel (0.0f, 0.0f, (float) designWidth, (float) designHeight);
    g.drawImage (facadeImage, fullPanel);
    g.drawImage (controlsLabelsImage, fullPanel);
    g.drawImage (redLinesImage, fullPanel);
    g.drawImage (vuLabelsImage, fullPanel);

    // Only the needle is drawn dynamically, directly over the preserved aged dial.
    // V15: calibrate the visual travel against the dial. 0 VU = -18 dBFS.
    // Full-left at -24 VU or below; full-right at +3 VU or above.
    // The smoothing/inertia in timerCallback is unchanged.
    const float dBvu = juce::Decibels::gainToDecibels (juce::jmax (vuDisplay, 0.000001f), -90.0f) + 18.0f;
    const float t = juce::jlimit (0.0f, 1.0f, (dBvu + 24.0f) / 27.0f);
    // V12: pivot aligned with the horizontal centre of the V8 meter window.
    // Coordinates are in the 1440x720 design space, not PNG pixel coordinates.
    const float pivotX = 1182.0f;
    const float pivotY = 299.0f;
    const float needleLength = 110.0f;
    // V16: symmetric +5.7 degrees at both ends compared with V15; preserve audio mapping and inertia.
    const float angleMin = -1.40f;
    const float angleMax =  1.40f;
    const float angle = angleMin + (angleMax - angleMin) * t;
    const float tipX = pivotX + std::sin (angle) * needleLength;
    const float tipY = pivotY - std::cos (angle) * needleLength;

    // V14: a constant-width, fine steel needle. The fixed mechanical hood
    // is part of VintagePanelImage.h; do not draw any pivot screw here.
    g.setColour (juce::Colour (0x50251a13));
    g.drawLine (pivotX + 0.65f, pivotY + 0.7f,
                tipX + 0.65f, tipY + 0.7f, 1.55f);
    g.setColour (juce::Colour (0xff2b2119));
    g.drawLine (pivotX, pivotY, tipX, tipY, 1.35f);

}


void AdLSpaceDubEchoAudioProcessorEditor::paintOverChildren (juce::Graphics& g)
{
    // Scale overlay coordinates back into the original 1440 x 720 design space.
    // The real Slider components use proportionally scaled hit regions below.
    const float scale = (float) getWidth() / (float) designWidth;
    juce::Graphics::ScopedSaveState save (g);
    g.addTransform (juce::AffineTransform::scale (scale));
    // Draw the 20 knobs on TOP of the child sliders, regardless of host/slider
    // look-and-feel painting. The actual Slider components remain interactive.
    auto draw = [this, &g] (juce::Slider& s)
    {
        const auto b = s.getBounds().toFloat();
        const float invScale = (float) designWidth / (float) getWidth();
        const double lo = s.getMinimum(), hi = s.getMaximum();
        const float normalized = hi > lo ? (float) juce::jlimit (0.0, 1.0,
                                                   (s.getValue() - lo) / (hi - lo)) : 0.0f;
        const float begin = juce::MathConstants<float>::pi * 1.2f;
        const float finish = juce::MathConstants<float>::pi * 2.8f;
        vintageLookAndFeel.drawRotarySlider (g,
                                            juce::roundToInt (b.getX() * invScale),
                                            juce::roundToInt (b.getY() * invScale),
                                            juce::roundToInt (b.getWidth() * invScale),
                                            juce::roundToInt (b.getHeight() * invScale),
                                            normalized, begin, finish, s);
    };
    draw (inputHighPass);
    draw (mix);
    draw (time);
    draw (feedback);
    draw (tone);
    draw (drive);
    draw (tapeAging);
    draw (wow);
    draw (flutter);
    draw (tapeHiss);
    draw (lowCut);
    draw (lowResonance);
    draw (highCut);
    draw (highResonance);
    draw (oscCharacter);
    draw (modulation);
    draw (modulationRate);
    draw (stereoWidth);
    draw (stereoRate);
    draw (dubChaos);
    // Original Feedback Boost overlay, still above the knob drawings as before.
    // Original 527x265 reference crop (90,137)-(409,194) maps to design (1056,337).
    g.drawImage (feedbackBoostOverlayImage, juce::Rectangle<float> (0.0f, 0.0f, 1440.0f, 720.0f));

    // SEND: one dynamic handle only. All coordinates below are measured in the
    // original 1774 x 887 overlay image, then mapped to the 1440 x 720 JUCE canvas.
    // The static handle has been removed from DubControlOverlay.png.
    constexpr float sx = 1440.0f / 1774.0f, sy = 720.0f / 887.0f;
    const float sendNorm = juce::jlimit (0.0f, 1.0f,
        (float) sendToEcho.valueToProportionOfLength (sendToEcho.getValue()));
    const float thumbX = (1262.0f + sendNorm * 76.0f) * sx;
    const float thumbY = 450.0f * sy;
    g.setColour (juce::Colour (0xff261d16));
    g.fillRoundedRectangle (thumbX - 7.0f * sx, thumbY - 17.0f * sy,
                            14.0f * sx, 34.0f * sy, 2.0f * sx);
    g.setColour (juce::Colour (0xffb9a183));
    g.fillRoundedRectangle (thumbX - 5.8f * sx, thumbY - 15.5f * sy,
                            11.6f * sx, 31.0f * sy, 1.6f * sx);
    g.setColour (juce::Colour (0xffb42b20));
    g.fillRoundedRectangle (thumbX - 1.2f * sx, thumbY - 11.0f * sy,
                            2.4f * sx, 22.0f * sy, 0.7f * sx);

    // FDBK BOOST: identical solid disk geometry in both states.
    // The static button artwork was removed from the transparent overlay.
    // No rim, halo, outline or size change on press.
    const bool pressed = processor.getAPVTS().getRawParameterValue ("feedbackThrow")->load() > 0.5f;
    constexpr float buttonRadius = 12.0f; // native overlay pixels
    g.setColour (pressed ? juce::Colour (0xffef3027) : juce::Colour (0xff861c1b));
    g.fillEllipse ((1408.0f - buttonRadius) * sx,
                   (452.0f - buttonRadius) * sy,
                   2.0f * buttonRadius * sx,
                   2.0f * buttonRadius * sy);

    // All LEDs use the same worn red lens. Exactly one selected in each bank.
    auto drawLed = [&g] (float x, float y, bool active)
    {
        g.setColour (juce::Colour (0xff1e100e));
        g.fillEllipse (x - 3.0f, y - 3.0f, 6.0f, 6.0f);
        g.setColour (active ? juce::Colour (0xffe73522) : juce::Colour (0xff6b221a));
        g.fillEllipse (x - 2.2f, y - 2.2f, 4.4f, 4.4f);
        if (active)
        {
            g.setColour (juce::Colour (0x88ff8b6a));
            g.fillEllipse (x - 1.3f, y - 1.7f, 1.7f, 1.7f);
        }
    };
    // Numeric markings are JUCE text, not part of the overlay PNG.
    // Draw in ORIGINAL 1774 x 887 coordinates to preserve LED alignment.
    // Colours and compact bold lettering are chosen to match the aged ivory print.
    auto drawMarking = [&g, sx, sy] (const juce::String& marking, float centerX, float centerY,
                             float width, float fontPixels)
    {
        juce::Graphics::ScopedSaveState textState (g);
        g.setColour (juce::Colour (0xffeddfbf));
        g.setFont (juce::Font (juce::FontOptions (fontPixels).withStyle ("Bold")));
        g.drawFittedText (marking, juce::Rectangle<int> (
            juce::roundToInt ((centerX - width * 0.5f) * sx),
            juce::roundToInt ((centerY - 7.5f) * sy),
            juce::roundToInt (width * sx),
            juce::roundToInt (15.0f * sy)), juce::Justification::centred, 1, 0.75f);
    };
    // LEVEL labels match the unchanged LED index order (top to bottom).
    static const char* levelMarkings[] = { "98%", "105%", "115%" };
    for (int i = 0; i < 3; ++i)
        drawMarking (levelMarkings[i], 1524.0f, 439.0f + 14.0f * i, 40.0f, 11.0f);
    // Time unit, drawn by JUCE beside the unchanged RELEASE artwork.
    drawMarking ("(s)", 1640.0f, 425.0f, 26.0f, 11.0f);
    // RELEASE labels appear BELOW each corresponding LED in two rows.
    static const char* releaseMarkings[] = { "0.05", "0.5", "1", "3", "5", "10" };
    for (int i = 0; i < 6; ++i)
        drawMarking (releaseMarkings[i],
                     i % 3 == 0 ? 1590.0f : i % 3 == 1 ? 1619.0f : 1647.0f,
                     451.0f + 24.0f * (i / 3), 38.0f, 11.0f);

    const int level = juce::jlimit (0, 2,
        (int) processor.getAPVTS().getRawParameterValue ("boostLevel")->load());
    for (int i = 0; i < 3; ++i)
        drawLed (1499.0f * sx, (439.0f + 14.0f * i) * sy, i == level);
    const int release = juce::jlimit (0, 5,
        (int) processor.getAPVTS().getRawParameterValue ("boostRelease")->load());
    for (int i = 0; i < 6; ++i)
        drawLed ((i % 3 == 0 ? 1590.0f : i % 3 == 1 ? 1619.0f : 1647.0f) * sx, (439.0f + 24.0f * (i / 3)) * sy, i == release);

}

void AdLSpaceDubEchoAudioProcessorEditor::timerCallback()
{
    repaint (juce::roundToInt (1008.0f * (float) getWidth() / designWidth),
             juce::roundToInt (328.0f * (float) getWidth() / designWidth),
             juce::roundToInt (365.0f * (float) getWidth() / designWidth),
             juce::roundToInt (68.0f * (float) getWidth() / designWidth));
    const float level = processor.getOutputVuLevel();
    const float smoothing = level > vuDisplay ? 0.13f : 0.055f;
    vuDisplay += smoothing * (level - vuDisplay);
    const float scale = (float) getWidth() / (float) designWidth;
    repaint (juce::roundToInt (1000.0f * scale), juce::roundToInt (105.0f * scale),
             juce::roundToInt (350.0f * scale) + 2, juce::roundToInt (225.0f * scale) + 2);
}

void AdLSpaceDubEchoAudioProcessorEditor::resized()
{
    // Centres détectés sur les cercles de graduation de l'image 1440x720,
    // et non estimés depuis une maquette redimensionnée.
    // V8 : diametres 94 px (standard) et 114 px (OSC), centres inchanges.
    // Les centres sont conserves pour ne pas introduire de decalages arbitraires.
    // Une capture de REAPER avec reperes permettra un recalage pixel par pixel.
    const float scale = (float) getWidth() / (float) designWidth;
    auto place = [scale] (juce::Slider& s, int cx, float cy, int diameter)
    {
        const float half = (float) diameter * 0.5f;
        s.setBounds (juce::roundToInt (((float) cx - half) * scale),
                     juce::roundToInt (((float) cy - half) * scale),
                     juce::roundToInt ((float) diameter * scale),
                     juce::roundToInt ((float) diameter * scale));
    };
    auto rect = [scale] (float x, float y, float w, float h)
    {
        return juce::Rectangle<int> (juce::roundToInt (x * scale),
                                     juce::roundToInt (y * scale),
                                     juce::roundToInt (w * scale),
                                     juce::roundToInt (h * scale));
    };
    sendToEcho.setBounds (rect (1250.0f * 1440.0f / 1774.0f, 433.0f * 720.0f / 887.0f, 100.0f * 1440.0f / 1774.0f, 35.0f * 720.0f / 887.0f));
    feedbackThrowButton.setBounds (rect ((1408.0f - 18.0f) * 1440.0f / 1774.0f, (452.0f - 18.0f) * 720.0f / 887.0f, 36.0f * 1440.0f / 1774.0f, 36.0f * 720.0f / 887.0f));
    boostLevelSwitch.setBounds (rect ((1476.0f - 14.0f) * 1440.0f / 1774.0f, (452.0f - 14.0f) * 720.0f / 887.0f, 28.0f * 1440.0f / 1774.0f, 28.0f * 720.0f / 887.0f));
    boostReleaseButton.setBounds (rect ((1564.0f - 14.0f) * 1440.0f / 1774.0f, (452.0f - 14.0f) * 720.0f / 887.0f, 28.0f * 1440.0f / 1774.0f, 28.0f * 720.0f / 887.0f));
    place (inputHighPass, 146, 209, 94);
    place (mix, 146, 331, 94);
    place (time, 377, 207, 94);
    place (feedback, 325, 329, 94);
    place (tone, 457, 331, 94);
    place (drive, 682, 207, 94);
    place (tapeAging, 843, 209, 94);
    place (wow, 642, 331, 94);
    place (flutter, 762, 331, 94);
    place (tapeHiss, 883, 331, 94);
    place (lowCut, 146, 501.2011f, 94);
    place (lowResonance, 146, 607.7989f, 94);
    place (highCut, 325, 501.2011f, 94);
    place (highResonance, 325, 607.7989f, 94);
    place (oscCharacter, 506, 547, 114);
    place (modulation, 712, 501.2011f, 94);
    place (modulationRate, 823, 607.7989f, 94);
    place (stereoWidth, 1007, 501.2011f, 94);
    place (stereoRate, 1290, 501.2011f, 94);
    place (dubChaos, 1177, 607.7989f, 94);
}
