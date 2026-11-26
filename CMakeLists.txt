cmake_minimum_required(VERSION 3.22)
project(adLSpaceDubEcho VERSION 0.1.18 LANGUAGES C CXX)
if(WIN32)
    set(_juce_default "C:/AudioDev/JUCE")
else()
    set(_juce_default "$ENV{HOME}/JUCE")
endif()
set(JUCE_PATH "${_juce_default}" CACHE PATH "Path to JUCE source directory")
option(ADL_BUILD_CLAP "Build CLAP plugin" ON)
set(CLAP_JUCE_EXTENSIONS_PATH "" CACHE PATH "Local clap-juce-extensions checkout")
if(NOT EXISTS "${JUCE_PATH}/CMakeLists.txt")
    message(FATAL_ERROR "JUCE not found at ${JUCE_PATH}. Specify -DJUCE_PATH=<path>.")
endif()
add_subdirectory("${JUCE_PATH}" JUCE)
juce_add_plugin(adLSpaceDubEcho
    COMPANY_NAME "adL"
    BUNDLE_ID "audio.adl.space.dubecho"
    PLUGIN_MANUFACTURER_CODE AdLe
    PLUGIN_CODE Dube
    PRODUCT_NAME "adLSpaceDubEcho"
    DESCRIPTION "Dub echo effect"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT FALSE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD FALSE
    FORMATS VST3 Standalone
    VST3_CATEGORIES Fx Delay
)
juce_generate_juce_header(adLSpaceDubEcho)
juce_add_binary_data(adLSpaceDubEchoAssets SOURCES
    Source/Assets/Mainframe.png
    Source/Assets/Stripe.png
    Source/Assets/Frames.png
    Source/Assets/Meter.png
    Source/Assets/Knob.png
    Source/Assets/Labels.png
    Source/Assets/Graduations.png
    Source/Assets/Boost.png
)
target_sources(adLSpaceDubEcho PRIVATE Source/PluginProcessor.cpp Source/PluginEditor.cpp)
target_compile_features(adLSpaceDubEcho PRIVATE cxx_std_17)
target_compile_definitions(adLSpaceDubEcho PUBLIC JUCE_WEB_BROWSER=0 JUCE_USE_CURL=0 JUCE_VST3_CAN_REPLACE_VST2=0)
target_link_libraries(adLSpaceDubEcho PRIVATE adLSpaceDubEchoAssets juce::juce_audio_utils juce::juce_dsp
    PUBLIC juce::juce_recommended_config_flags juce::juce_recommended_lto_flags juce::juce_recommended_warning_flags)

if(ADL_BUILD_CLAP)
    if(CLAP_JUCE_EXTENSIONS_PATH)
        add_subdirectory("${CLAP_JUCE_EXTENSIONS_PATH}" clap-juce-extensions)
    else()
        include(FetchContent)
        FetchContent_Declare(clap_juce_extensions
            GIT_REPOSITORY https://github.com/free-audio/clap-juce-extensions.git
            GIT_TAG main
            GIT_SUBMODULES_RECURSE TRUE)
        FetchContent_MakeAvailable(clap_juce_extensions)
    endif()
    clap_juce_extensions_plugin(TARGET adLSpaceDubEcho
        CLAP_ID "audio.adl.space.dubecho"
        CLAP_FEATURES audio-effect delay stereo)
endif()
