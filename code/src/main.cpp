#include <Arduino.h>
#include <EEPROM.h>
#include <ezButton.h>

/********************************************************/
/* Notes                                                */
/********************************************************/
/*
  - With ezButton, pinMode is INPUT_PULLUP. So HIGH is no-press, LOW is press
  - Putting controller Off, will enable all Fx
*/

/********************************************************/
/* Static conf                                          */
/********************************************************/

// Global configurations
const int ButtonDebounceTimeMs = 50;
const int FxCount = 6;
const int PresetCount = FxCount + 1; // 0=all on, 1-6=users

// Overall buttons
const int EditButtonPin = 29;
const int OnOffButtonPin = 22;

// FX buttons
const int Fx1ButtonPin = 23;
const int Fx2ButtonPin = 24;
const int Fx3ButtonPin = 25;
const int Fx4ButtonPin = 26;
const int Fx5ButtonPin = 27;
const int Fx6ButtonPin = 28;

// FX In/Out control pins
const int Fx1InCtrlPin = 40;
const int Fx1OutCtrlPin = 39;
const int Fx2InCtrlPin = 38;
const int Fx2OutCtrlPin = 37;
const int Fx3InCtrlPin = 36;
const int Fx3OutCtrlPin = 35;
const int Fx4InCtrlPin = 1;
const int Fx4OutCtrlPin = 2;
const int Fx5InCtrlPin = 3;
const int Fx5OutCtrlPin = 4;
const int Fx6InCtrlPin = 5;
const int Fx6OutCtrlPin = 6;

// 7-segments display
const int DisplaySegAPin = 14;
const int DisplaySegBPin = 15;
const int DisplaySegCPin = 16;
const int DisplaySegDPin = 17;
const int DisplaySegEPin = 18;
const int DisplaySegFPin = 19;
const int DisplaySeggPin = 20;
const int DisplaySegDotPin = 21;

// EEPROM addresses
const int PresetsAddress = 0;

/********************************************************/
/* Buttons instances                                    */
/********************************************************/
ezButton editButton(EditButtonPin);
ezButton onOffButton(OnOffButtonPin);
ezButton fxButtons[] = {
    ezButton(Fx1ButtonPin),
    ezButton(Fx2ButtonPin),
    ezButton(Fx3ButtonPin),
    ezButton(Fx4ButtonPin),
    ezButton(Fx5ButtonPin),
    ezButton(Fx6ButtonPin)};

/********************************************************/
/* FX Controls                                          */
/********************************************************/
typedef struct _SFxControl
{
  byte in;
  byte out;
} SFxControl;

const SFxControl fxControls[] = {
    {.in = Fx1InCtrlPin, .out = Fx1OutCtrlPin},
    {.in = Fx2InCtrlPin, .out = Fx2OutCtrlPin},
    {.in = Fx3InCtrlPin, .out = Fx3OutCtrlPin},
    {.in = Fx4InCtrlPin, .out = Fx4OutCtrlPin},
    {.in = Fx5InCtrlPin, .out = Fx5OutCtrlPin},
    {.in = Fx6InCtrlPin, .out = Fx6OutCtrlPin}};

/********************************************************/
/* Presets                                              */
/*   - i : preset id                                    */
/*   - j : fx id                                        */
/********************************************************/
bool presets[PresetCount][FxCount] = {false};

/********************************************************/
/* Display                                              */
/********************************************************/

/********************************************************/
/* Current state                                        */
/********************************************************/
bool isInEditMode = false;
bool isOff = false;
int currentPreset = 1;

/********************************************************/
/* Helpers methods                                      */
/********************************************************/

void displayDot(bool enable)
{
  digitalWrite(DisplaySegDotPin, enable ? HIGH : LOW);
}

void displayNumber(int number)
{
  // TODO : Segment lib is too heavy
}

void setFxStatus(int fxId, bool enable)
{
  if (enable)
  {
    digitalWrite(fxControls[fxId].in, HIGH);
    digitalWrite(fxControls[fxId].out, HIGH);
  }
  else
  {
    // Change output first to avoid glitch
    digitalWrite(fxControls[fxId].out, LOW);
    digitalWrite(fxControls[fxId].in, LOW);
  }
}

void setCurrentPreset(int presetId)
{
  if (presetId != currentPreset)
  {
    for (byte i = 0; i < FxCount; ++i)
    {
      setFxStatus(i, presets[presetId][i]);
    }

    currentPreset = presetId;

    displayNumber(currentPreset);
  }
}

/********************************************************/
/* Chip setup                                           */
/********************************************************/
void setup()
{
  // All ezButton's pins are setup as INPUT_PULLUP in class ctor
  // Just configure debounce time
  editButton.setDebounceTime(ButtonDebounceTimeMs);
  onOffButton.setDebounceTime(ButtonDebounceTimeMs);
  for (byte i = 0; i < FxCount; ++i)
  {
    fxButtons[i].setDebounceTime(ButtonDebounceTimeMs);
  }

  // Setup mux control pins
  for (byte i = 0; i < FxCount; ++i)
  {
    pinMode(fxControls[i].in, OUTPUT);
    pinMode(fxControls[i].out, OUTPUT);
  }

  // Setup display pins to output
  pinMode(DisplaySegAPin, OUTPUT);
  pinMode(DisplaySegBPin, OUTPUT);
  pinMode(DisplaySegCPin, OUTPUT);
  pinMode(DisplaySegDPin, OUTPUT);
  pinMode(DisplaySegEPin, OUTPUT);
  pinMode(DisplaySegFPin, OUTPUT);
  pinMode(DisplaySeggPin, OUTPUT);
  pinMode(DisplaySegDotPin, OUTPUT);

  // Load data from eeprom
  EEPROM.get(PresetsAddress, presets);

  // Init preset-0 to all on
  for (byte i = 0; i < FxCount; ++i)
  {
    presets[0][i] = true;
  }

  // Call at the end. Once everything is ready
  setCurrentPreset(currentPreset);
}

/********************************************************/
/* Main loop                                            */
/********************************************************/
void loop()
{
  // Read buttons states
  editButton.loop();
  onOffButton.loop();
  for (byte i = 0; i < FxCount; ++i)
  {
    fxButtons[i].loop();
  }

  if (editButton.isReleased() && currentPreset != 0) // switch edit mode on/off
  {
    // leaving
    if (isInEditMode)
    {
      EEPROM.put(PresetsAddress, presets);
    }

    // toggle
    isInEditMode = !isInEditMode;

    displayDot(isInEditMode);
  }
  else if (onOffButton.getState() == LOW) // switch controller on/off
  {
    if (isOff)
    {
      isOff = true;
      setCurrentPreset(0);
    }
  }
  else // handle fx buttons
  {
    // Toggle back off mode
    if (!isOff)
    {
      isOff = false;
    }

    // Not in edit mode, look for preset changes
    if (!isInEditMode)
    {
      for (byte i = 0; i < FxCount; ++i)
      {
        if (fxButtons[i].isPressed())
        {
          setCurrentPreset(i + 1);
          break;
        }
      }
    }
    // In Edit mode, toggle Fx from button
    else
    {
      for (byte i = 0; i < FxCount; ++i)
      {
        if (fxButtons[i].isPressed())
        {
          presets[currentPreset][i] = !presets[currentPreset][i];
          setFxStatus(i, presets[currentPreset][i]);
          break;
        }
      }
    }
  }
}