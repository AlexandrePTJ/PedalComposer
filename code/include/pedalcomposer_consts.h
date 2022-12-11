#ifndef BDM_PEDAL_COMPOSER_CONSTS_H
#define BDM_PEDAL_COMPOSER_CONSTS_H

// pins state to number for CD4543 BCD driver
static const int g_BCDTable[10][4] = {
    {0, 0, 0, 0}, // 0
    {1, 0, 0, 0}, // 1
    {0, 1, 0, 0}, // 2
    {1, 1, 0, 0}, // 3
    {0, 0, 1, 0}, // 4
    {1, 0, 1, 0}, // 5
    {0, 1, 1, 0}, // 6
    {1, 1, 1, 0}, // 7
    {0, 0, 0, 1}, // 8
    {1, 0, 0, 1}  // 9
};

#endif // BDM_PEDAL_COMPOSER_CONSTS_H