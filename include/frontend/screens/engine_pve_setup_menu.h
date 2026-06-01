#ifndef ENGINE_PVE_SETUP_MENU_H
#define ENGINE_PVE_SETUP_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "frontend/data/screen_data.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum EnginePvESetupMenuAction
{
    enginePvESetupMenuActionNone,

    enginePvESetupMenuActionStart,
    enginePvESetupMenuActionBack

} EnginePvESetupMenuAction;

/* Lifecycle */
enginePvESetupMenu* enginePvESetupMenu_create(sfVector2i, sfVector2i, const Resources*);
void enginePvESetupMenu_destroy(enginePvESetupMenu*);

/* Input */
void enginePvESetupMenu_updateMouse(enginePvESetupMenu*, const Mouse*);
void enginePvESetupMenu_updateKeyboard(enginePvESetupMenu*, const sfEvent*);

/* State */
void enginePvESetupMenu_setActive(enginePvESetupMenu*, sfBool);
sfBool enginePvESetupMenu_isActive(const enginePvESetupMenu*);

EnginePvESetupMenuAction enginePvESetupMenu_getAction(const enginePvESetupMenu*);
EnginePvESetupMenuAction enginePvESetupMenu_consumeAction(enginePvESetupMenu*);

VsEngineSetup enginePvESetupMenu_getData(const enginePvESetupMenu*);

/* Utility */
void enginePvESetupMenu_draw(sfRenderWindow*, const enginePvESetupMenu*);

#endif // ENGINE_PVE_SETUP_MENU_H