#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "input/mouse.h"
#include "resources/resources.h"
#include "frontend/data/screen_data.h"

typedef enum SettingsMenuAction
{
    settingsMenuActionNone,

    settingsMenuActionApply,
    settingsMenuActionControls,
    settingsMenuActionBack

} SettingsMenuAction;

/* Lifecycle */
settingsMenu* settingsMenu_create(sfVector2i, sfVector2i, const Resources*);
void settingsMenu_destroy(settingsMenu*);

/* Input */
void settingsMenu_updateMouse(settingsMenu*, const Mouse*);

/* State */
void settingsMenu_setActive(settingsMenu*, sfBool);
sfBool settingsMenu_isActive(const settingsMenu*);

SettingsMenuAction settingsMenu_getAction(const settingsMenu*);
SettingsMenuAction settingsMenu_consumeAction(settingsMenu*);

SettingsData settingsMenu_getData(const settingsMenu*);

/* Utility */
void settingsMenu_draw(sfRenderWindow*, const settingsMenu*);

#endif // SETTINGS_MENU_H