#ifndef GAME_SETUP_MENU_H
#define GAME_SETUP_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum GameSetupMenuAction
{
    gameSetupMenuActionNone,

    gameSetupMenuActionLocalPvP,
    gameSetupMenuActionVsEngine,
    gameSetupMenuActionBack

} GameSetupMenuAction;

/* Lifecycle */
gameSetupMenu* gameSetupMenu_create(sfVector2i, sfVector2i, const Resources*);
void gameSetupMenu_destroy(gameSetupMenu*);

/* Input */
void gameSetupMenu_updateMouse(gameSetupMenu*, const Mouse*);

/* State */
void gameSetupMenu_setActive(gameSetupMenu*, sfBool);
sfBool gameSetupMenu_isActive(const gameSetupMenu*);

GameSetupMenuAction gameSetupMenu_getAction(const gameSetupMenu*);
GameSetupMenuAction gameSetupMenu_consumeAction(gameSetupMenu*);

/* Utility */
void gameSetupMenu_draw(sfRenderWindow*, const gameSetupMenu*);

#endif // GAME_SETUP_MENU_H