#ifndef LOCAL_PVP_SETUP_MENU_H
#define LOCAL_PVP_SETUP_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "frontend/data/screen_data.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum localPvpSetupMenuAction
{
    localPvpSetupMenuActionNone,

    localPvpSetupMenuActionStart,
    localPvpSetupMenuActionBack

} localPvpSetupMenuAction;

/* Lifecycle */
localPvpSetupMenu* localPvpSetupMenu_create(sfVector2i, sfVector2i, const Resources*);
void localPvpSetupMenu_destroy(localPvpSetupMenu*);

/* Input */
void localPvpSetupMenu_updateMouse(localPvpSetupMenu*, const Mouse*);
void localPvpSetupMenu_updateKeyboard(localPvpSetupMenu*, const sfEvent*);

/* State */
void localPvpSetupMenu_setActive(localPvpSetupMenu*, sfBool);
sfBool localPvpSetupMenu_isActive(const localPvpSetupMenu*);

localPvpSetupMenuAction localPvpSetupMenu_getAction(const localPvpSetupMenu*);
localPvpSetupMenuAction localPvpSetupMenu_consumeAction(localPvpSetupMenu*);

LocalPvPSetup localPvpSetupMenu_getData(const localPvpSetupMenu*);

/* Utility */
void localPvpSetupMenu_draw(sfRenderWindow*, const localPvpSetupMenu*);

#endif // LOCAL_PVP_SETUP_MENU_H