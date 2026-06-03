#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <SFML/Graphics.h>

#include "game_session/game_session.h"
#include "frontend/data/screen_manager_request.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef struct screenManager screenManager;

typedef enum ScreenId
{
    screenIdMainMenu,
    screenIdSettingsMenu,
    screenIdControlsMenu,
    screenIdCreditsMenu,

    screenIdGameSetupMenu,
    screenIdLocalPvpSetupMenu,
    screenIdEnginePveSetupMenu,

    screenIdGameLocalPvp,
    screenIdGameEnginePve

} ScreenId;


// Lifecycle
screenManager* screenManager_create(sfVector2i, sfVector2i, const Resources*);
void screenManager_destroy(screenManager*);


// Update
void screenManager_update(screenManager*, int);


// Input
void screenManager_updateMouse(screenManager*, const Mouse*);
void screenManager_updateKeyboard(screenManager*, const sfEvent*);


// State
void screenManager_setActive(screenManager*, sfBool);
sfBool screenManager_isActive(const screenManager*);

void screenManager_setScreen(screenManager*, ScreenId);
ScreenId screenManager_getScreen(const screenManager*);

ScreenManagerRequest screenManager_getRequest(const screenManager*);
ScreenManagerRequest screenManager_consumeRequest(screenManager*);

const GameSession* screenManager_getCurrentSession(const screenManager*);


// Utility
void screenManager_draw(sfRenderWindow*, const screenManager*);

#endif // SCREEN_MANAGER_H