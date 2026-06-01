#ifndef SCREEN_DATA_H
#define SCREEN_DATA_H

#include <SFML/Config.h>

/* ================= Settings data ================= */

typedef struct SettingsData
{
    sfBool soundEnabled;
    sfBool fullscreenEnabled;

} SettingsData;

/* ================= Game setup data ================= */

typedef enum GameMode
{
    gameModeLocalPvP,
    gameModeVsEngine

} GameMode;

typedef enum PlayerColor
{
    playerColorWhite,
    playerColorBlack,
    playerColorRandom

} PlayerColor;

typedef struct LocalPvPSetup
{
    unsigned int startingTimeSeconds;
    unsigned int incrementSeconds;

} LocalPvPSetup;

typedef struct VsEngineSetup
{
    PlayerColor playerColor;

    int engineLevel;

} VsEngineSetup;

typedef struct GameSetupData
{
    GameMode mode;

    union
    {
        LocalPvPSetup localPvP;
        VsEngineSetup vsEngine;

    } data;

} GameSetupData;

/* ================= Default data ================= */

SettingsData settingsData_getDefault(void);

LocalPvPSetup localPvPSetup_getDefault(void);
VsEngineSetup vsEngineSetup_getDefault(void);

GameSetupData gameSetupData_getDefault(void);

/* ================= Game setup constructors ================= */

GameSetupData gameSetupData_makeLocalPvP(LocalPvPSetup);
GameSetupData gameSetupData_makeVsEngine(VsEngineSetup);

#endif // SCREEN_DATA_H