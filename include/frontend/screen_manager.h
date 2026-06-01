#ifndef SCREEN_MANAGER_H
#define SCREEN_MANAGER_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "input/mouse.h"
#include "resources/resources.h"
#include "frontend/data/screen_manager_request.h"

typedef struct screenManager screenManager;

typedef enum ScreenId
{
    screenIdMainMenu,
    screenIdSettingsMenu,
    screenIdCreditsMenu,
    screenIdGameSetupMenu

} ScreenId;

/* Lifecycle */
screenManager* screenManager_create(sfVector2i, sfVector2i, const Resources*);
void screenManager_destroy(screenManager*);

/* Input */
void screenManager_updateMouse(screenManager*, const Mouse*);

/* State */
void screenManager_setActive(screenManager*, sfBool);
sfBool screenManager_isActive(const screenManager*);

void screenManager_setScreen(screenManager*, ScreenId);
ScreenId screenManager_getScreen(const screenManager*);

ScreenManagerRequest screenManager_getRequest(const screenManager*);
ScreenManagerRequest screenManager_consumeRequest(screenManager*);

/* Utility */
void screenManager_draw(sfRenderWindow*, const screenManager*);

#endif // SCREEN_MANAGER_H