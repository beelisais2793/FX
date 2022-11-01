#include <time.h>
#include "kxmx_bluemchen.h"

#include "Dynamics/balance.h"

#include "../commons.h"
#include "../generatorbank.h"
#include "../effectbank.h"


using namespace kxmx;
using namespace daisy;
using namespace daisysp;
using namespace orchard;

Bluemchen bluemchen;

Parameter knob1;
Parameter knob2;

Parameter knob1_dac;
Parameter knob2_dac;

Parameter cv1;
Parameter cv2;

float knob1Value;
float knob2Value;


Balance balancer;

float sampleRate;

GeneratorBank generatorBank;
EffectBank effectBank;

daisy::UI ui;

FullScreenItemMenu mainMenu;
FullScreenItemMenu randomizerMenu;
FullScreenItemMenu polyEditMenu;
FullScreenItemMenu boolEditMenu;
FullScreenItemMenu normEditMenu;
UiEventQueue eventQueue;

const int kNumMainMenuItems = 4;
AbstractMenu::ItemConfig mainMenuItems[kNumMainMenuItems];
const int kNumRandomizerMenuItems = 4;
AbstractMenu::ItemConfig randomizerMenuItems[kNumRandomizerMenuItems];
const int kNumPolyEditMenuItems = 4;
AbstractMenu::ItemConfig polyEditMenuItems[kNumPolyEditMenuItems];
const int kNumNormEditMenuItems = 4;
AbstractMenu::ItemConfig normEditMenuItems[kNumNormEditMenuItems];

/*
// control menu items
const char* controlListValues[] = {"Frequency", "Structure", "Brightness", "Damping", "Position"};
MappedStringListValue controlListValueTwo(controlListValues, 5, 1);
MappedStringListValue controlListValueThree(controlListValues, 5, 2);
MappedStringListValue controlListValueFour(controlListValues, 5, 3);
*/

enum class RandomType
{
    NONE,
    ALL,
    GENERATORS,
    EFFECTS,
};
RandomType randomize{RandomType::ALL};

void RandomizeAll(void* context)
{
    randomize = RandomType::ALL;
}

void RandomizeGenerators(void *context)
{
    randomize = RandomType::GENERATORS;
}

void RandomizeEffects(void *context)
{
    randomize = RandomType::EFFECTS;
}

int basePitch;

bool useEnvelope{true};


void Randomize()
{
    if (RandomType::ALL == randomize) 
    {
        generatorBank.Randomize();
        effectBank.Randomize();
    }
    else if (RandomType::GENERATORS == randomize)
    {
        generatorBank.Randomize();
    }
    else if (RandomType::EFFECTS == randomize)
    {
        effectBank.Randomize();
    }
    randomize = RandomType::NONE;
}

void UpdateControls()
{
    knob1.Process();
    knob2.Process();

    bluemchen.seed.dac.WriteValue(daisy::DacHandle::Channel::ONE, static_cast<uint16_t>(knob1_dac.Process()));
    bluemchen.seed.dac.WriteValue(daisy::DacHandle::Channel::TWO, static_cast<uint16_t>(knob2_dac.Process()));

    cv1.Process();
    cv2.Process();

    generatorBank.SetEnvelopeGate(useEnvelope ? cv1.Value() > 0.5f : true);

    if (std::abs(knob2Value - knob2.Value()) > 0.01f)
    {
        basePitch = 24 + knob2.Value() * 60;
        knob2Value = knob2.Value();
    }
    float cvPitch{fmap(cv2.Value(), -30.f, 30.f)}; 
    generatorBank.SetPitch(basePitch + cvPitch);
}

using OledDisplayType = decltype(Bluemchen::display);
int canvasOledDisplay = 0;
int bttnEncoder = 0;
int encoderMain = 0;

// These will be called from the UI system. @see InitUi() in UiSystemDemo.cpp
void FlushCanvas(const daisy::UiCanvasDescriptor &canvasDescriptor)
{
    if (canvasDescriptor.id_ == canvasOledDisplay)
    {
        OledDisplayType &display = *((OledDisplayType *)(canvasDescriptor.handle_));
        display.Update();
    }
}
void ClearCanvas(const daisy::UiCanvasDescriptor &canvasDescriptor)
{
    if (canvasDescriptor.id_ == canvasOledDisplay)
    {
        OledDisplayType &display = *((OledDisplayType *)(canvasDescriptor.handle_));
        display.Fill(false);
    }
}

void InitUi()
{
    UI::SpecialControlIds specialControlIds;
    specialControlIds.okBttnId = bttnEncoder;      // Encoder button is our okay button
    specialControlIds.menuEncoderId = encoderMain; // Encoder is used as the main menu navigation encoder

    // This is the canvas for the OLED display.
    UiCanvasDescriptor oledDisplayDescriptor;
    oledDisplayDescriptor.id_ = canvasOledDisplay;      // the unique ID
    oledDisplayDescriptor.handle_ = &bluemchen.display; // a pointer to the display
    oledDisplayDescriptor.updateRateMs_ = 50;           // 50ms == 20Hz
    //oledDisplayDescriptor.screenSaverTimeOut = 0;  // display always on
    oledDisplayDescriptor.clearFunction_ = &ClearCanvas;
    oledDisplayDescriptor.flushFunction_ = &FlushCanvas;

    ui.Init(eventQueue,
            specialControlIds,
            {oledDisplayDescriptor},
            canvasOledDisplay);
}

void InitUiPages()
{
    // ====================================================================
    // The main menu
    // ====================================================================

    mainMenuItems[0].type = daisy::AbstractMenu::ItemType::openUiPageItem;
    mainMenuItems[0].text = "Random";
    mainMenuItems[0].asOpenUiPageItem.pageToOpen = &randomizerMenu;

    mainMenu.Init(mainMenuItems, kNumMainMenuItems);

    // ====================================================================
    // The "control edit" menu
    // ====================================================================

    randomizerMenuItems[0].type = daisy::AbstractMenu::ItemType::callbackFunctionItem;
    randomizerMenuItems[0].text = "All";
    randomizerMenuItems[0].asCallbackFunctionItem.callbackFunction = &RandomizeAll;

    randomizerMenuItems[1].type = daisy::AbstractMenu::ItemType::callbackFunctionItem;
    randomizerMenuItems[1].text = "Gen";
    randomizerMenuItems[1].asCallbackFunctionItem.callbackFunction = &RandomizeGenerators;

    randomizerMenuItems[2].type = daisy::AbstractMenu::ItemType::callbackFunctionItem;
    randomizerMenuItems[2].text = "Fx";
    randomizerMenuItems[2].asCallbackFunctionItem.callbackFunction = &RandomizeEffects;

    randomizerMenuItems[3].type = daisy::AbstractMenu::ItemType::closeMenuItem;
    randomizerMenuItems[3].text = "Back";

    randomizerMenu.Init(randomizerMenuItems, kNumRandomizerMenuItems);
}

void GenerateUiEvents()
{
    if (bluemchen.encoder.RisingEdge())
        eventQueue.AddButtonPressed(bttnEncoder, 1);

    if (bluemchen.encoder.FallingEdge())
        eventQueue.AddButtonReleased(bttnEncoder);

    const auto increments = bluemchen.encoder.Increment();
    if (increments != 0)
        eventQueue.AddEncoderTurned(encoderMain, increments, 12);
}

void AudioCallback(AudioHandle::InputBuffer in, AudioHandle::OutputBuffer out, size_t size)
{
    bluemchen.ProcessAllControls();
    GenerateUiEvents();

    for (size_t i = 0; i < size; i++)
    {
        float left{0.f};
        float right{0.f};

        generatorBank.Process(left, right);
        effectBank.Process(left, right);

        OUT_L[i] = balancer.Process(left, 0.25f);
        OUT_R[i] = balancer.Process(right, 0.25f);
    }

    if (RandomType::NONE != randomize) 
    {
        Randomize();
    }
}

int main(void)
{
    bluemchen.Init();
    bluemchen.StartAdc();

    knob1.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 1.0f, Parameter::LINEAR);
    knob2.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 1.0f, Parameter::LINEAR);

    knob1_dac.Init(bluemchen.controls[bluemchen.CTRL_1], 0.0f, 1.0f, Parameter::LINEAR);
    knob2_dac.Init(bluemchen.controls[bluemchen.CTRL_2], 0.0f, 1.0f, Parameter::LINEAR);

    cv1.Init(bluemchen.controls[bluemchen.CTRL_3], 0.0f, 1.0f, Parameter::LINEAR);
    cv2.Init(bluemchen.controls[bluemchen.CTRL_4], 0.0f, 1.0f, Parameter::LINEAR);

    sampleRate = bluemchen.AudioSampleRate();

    InitUi();
    InitUiPages();
    ui.OpenPage(mainMenu);

    UI::SpecialControlIds ids;

    generatorBank.Init(sampleRate);
    effectBank.Init(sampleRate);
    balancer.Init(sampleRate);

    // New seed.
    srand(time(NULL));
    Randomize();

    bluemchen.StartAudio(AudioCallback);

    while (1)
    {
        ui.Process();
        UpdateControls();
        //UpdateMenu();
        //UpdateOled();
    }
}