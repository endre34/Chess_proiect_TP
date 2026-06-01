#ifndef SCREEN_MANAGER_REQUEST_H
#define SCREEN_MANAGER_REQUEST_H

#include "frontend/data/screen_data.h"

typedef enum ScreenManagerRequestType
{
    screenManagerRequestNone,

    screenManagerRequestExit,

    screenManagerRequestStartGame,
    screenManagerRequestApplySettings

} ScreenManagerRequestType;

typedef struct ScreenManagerRequest
{
    ScreenManagerRequestType type;

    union
    {
        GameSetupData gameSetup;
        SettingsData settings;

    } data;

} ScreenManagerRequest;

ScreenManagerRequest screenManagerRequest_getNone(void);

#endif // SCREEN_MANAGER_REQUEST_H