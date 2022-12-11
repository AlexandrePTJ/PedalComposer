#include <Arduino.h>

#include "pedalcomposer_pins.h"
#include "pedalcomposer_consts.h"

/********************************************************/
/* Static conf                                          */
/********************************************************/

/********************************************************/
/* Buttons instances                                    */
/********************************************************/

/********************************************************/
/* 7-Segment control                                    */
/********************************************************/

/********************************************************/
/* Current state                                        */
/********************************************************/
int g_dotValue = HIGH;

/********************************************************/
/* Helpers methods                                      */
/********************************************************/
void displayNumber(int value)
{
    if (value > 9 || value < 0)
    {
        return;
    }

    const auto &code = g_BCDTable[value];
    digitalWrite(PIN_DA, code[0]);
    digitalWrite(PIN_DB, code[1]);
    digitalWrite(PIN_DC, code[2]);
    digitalWrite(PIN_DD, code[3]);
}

/********************************************************/
/* Chip setup                                           */
/********************************************************/
void setup()
{
    // Configure 7-segment display driver
    pinMode(PIN_DA, OUTPUT);
    pinMode(PIN_DB, OUTPUT);
    pinMode(PIN_DC, OUTPUT);
    pinMode(PIN_DD, OUTPUT);
    pinMode(PIN_DDP, OUTPUT);
}

/********************************************************/
/* Main loop                                            */
/********************************************************/
void loop()
{
    g_dotValue = g_dotValue == HIGH ? LOW : HIGH;

    digitalWrite(PIN_DDP, g_dotValue);

    delay(500);
}
