#ifndef ENGINE_PVE_SETUP_MENU_H
#define ENGINE_PVE_SETUP_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "frontend/data/screen_data.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum enginePveSetupMenuAction
{
    enginePveSetupMenuActionNone,

    enginePveSetupMenuActionStart,
    enginePveSetupMenuActionBack

} enginePveSetupMenuAction;

/* Lifecycle */
enginePveSetupMenu* enginePveSetupMenu_create(sfVector2i, sfVector2i, const Resources*);
void enginePveSetupMenu_destroy(enginePveSetupMenu*);

/* Input */
void enginePveSetupMenu_updateMouse(enginePveSetupMenu*, const Mouse*);
void enginePveSetupMenu_updateKeyboard(enginePveSetupMenu*, const sfEvent*);

/* State */
void enginePveSetupMenu_setActive(enginePveSetupMenu*, sfBool);
sfBool enginePveSetupMenu_isActive(const enginePveSetupMenu*);

enginePveSetupMenuAction enginePveSetupMenu_getAction(const enginePveSetupMenu*);
enginePveSetupMenuAction enginePveSetupMenu_consumeAction(enginePveSetupMenu*);

VsEngineSetup enginePveSetupMenu_getData(const enginePveSetupMenu*);

/* Utility */
void enginePveSetupMenu_draw(sfRenderWindow*, const enginePveSetupMenu*);

#endif // ENGINE_PVE_SETUP_MENU_H