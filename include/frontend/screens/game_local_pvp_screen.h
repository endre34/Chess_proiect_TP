#ifndef GAME_LOCAL_PVP_SCREEN_H
#define GAME_LOCAL_PVP_SCREEN_H

#include <SFML/Graphics.h>

#include "frontend/data/screen_data.h"
#include "frontend/screens/menus.h"
#include "game_session/game_session.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum gameLocalPvpScreenAction
{
    gameLocalPvpScreenActionNone

} gameLocalPvpScreenAction;


// Lifecycle
gameLocalPvpScreen* gameLocalPvpScreen_create(sfVector2i, sfVector2i, const Resources*);
void gameLocalPvpScreen_destroy(gameLocalPvpScreen*);


// Update
void gameLocalPvpScreen_update(gameLocalPvpScreen*, int);


// Input
void gameLocalPvpScreen_updateMouse(gameLocalPvpScreen*, const Mouse*);


// State
void gameLocalPvpScreen_setSession(gameLocalPvpScreen*, GameSession*);
void gameLocalPvpScreen_setSetup(gameLocalPvpScreen*, LocalPvPSetup);

void gameLocalPvpScreen_setActive(gameLocalPvpScreen*, sfBool);
sfBool gameLocalPvpScreen_isActive(const gameLocalPvpScreen*);

gameLocalPvpScreenAction gameLocalPvpScreen_getAction(const gameLocalPvpScreen*);
gameLocalPvpScreenAction gameLocalPvpScreen_consumeAction(gameLocalPvpScreen*);


// Utility
void gameLocalPvpScreen_draw(sfRenderWindow*, const gameLocalPvpScreen*);

#endif // GAME_LOCAL_PVP_SCREEN_H