#include "frontend/data/screen_data.h"

SettingsData settingsData_getDefault(void)
{
    SettingsData data;

    data.soundEnabled = sfTrue;
    data.fullscreenEnabled = sfFalse;

    return data;
}

LocalPvPSetup localPvPSetup_getDefault(void)
{
    LocalPvPSetup setup;

    setup.startingTimeSeconds = 15 * 60;
    setup.incrementSeconds = 0;

    return setup;
}

VsEngineSetup vsEngineSetup_getDefault(void)
{
    VsEngineSetup setup;

    setup.playerColor = playerColorWhite;

    setup.engineLevel = 5;

    return setup;
}

GameSetupData gameSetupData_getDefault(void)
{
    GameSetupData data;

    data.mode = gameModeLocalPvP;
    data.data.localPvP = localPvPSetup_getDefault();

    return data;
}

GameSetupData gameSetupData_makeLocalPvP(LocalPvPSetup setup)
{
    GameSetupData data;

    data.mode = gameModeLocalPvP;
    data.data.localPvP = setup;

    return data;
}

GameSetupData gameSetupData_makeVsEngine(VsEngineSetup setup)
{
    GameSetupData data;

    data.mode = gameModeVsEngine;
    data.data.vsEngine = setup;

    return data;
}