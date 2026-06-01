#ifndef LOCAL_PVP_SETUP_MENU_H
#define LOCAL_PVP_SETUP_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "frontend/data/screen_data.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum LocalPvPSetupMenuAction
{
    localPvPSetupMenuActionNone,

    localPvPSetupMenuActionStart,
    localPvPSetupMenuActionBack

} LocalPvPSetupMenuAction;

/* Lifecycle */
localPvPSetupMenu* localPvPSetupMenu_create(sfVector2i, sfVector2i, const Resources*);
void localPvPSetupMenu_destroy(localPvPSetupMenu*);

/* Input */
void localPvPSetupMenu_updateMouse(localPvPSetupMenu*, const Mouse*);
void localPvPSetupMenu_updateKeyboard(localPvPSetupMenu*, const sfEvent*);

/* State */
void localPvPSetupMenu_setActive(localPvPSetupMenu*, sfBool);
sfBool localPvPSetupMenu_isActive(const localPvPSetupMenu*);

LocalPvPSetupMenuAction localPvPSetupMenu_getAction(const localPvPSetupMenu*);
LocalPvPSetupMenuAction localPvPSetupMenu_consumeAction(localPvPSetupMenu*);

LocalPvPSetup localPvPSetupMenu_getData(const localPvPSetupMenu*);

/* Utility */
void localPvPSetupMenu_draw(sfRenderWindow*, const localPvPSetupMenu*);

#endif // LOCAL_PVP_SETUP_MENU_H