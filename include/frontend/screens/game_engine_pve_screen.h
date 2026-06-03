#ifndef GAME_ENGINE_PVE_SCREEN_H
#define GAME_ENGINE_PVE_SCREEN_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "game_session/game_session.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum gameEnginePveScreenAction
{
    gameEnginePveScreenActionNone

} gameEnginePveScreenAction;


// Lifecycle
gameEnginePveScreen* gameEnginePveScreen_create(sfVector2i, sfVector2i, const Resources*);
void gameEnginePveScreen_destroy(gameEnginePveScreen*);


// Update
void gameEnginePveScreen_update(gameEnginePveScreen*, int);


// Input
void gameEnginePveScreen_updateMouse(gameEnginePveScreen*, const Mouse*);


// State
void gameEnginePveScreen_setSession(gameEnginePveScreen*, GameSession*);

void gameEnginePveScreen_setActive(gameEnginePveScreen*, sfBool);
sfBool gameEnginePveScreen_isActive(const gameEnginePveScreen*);

gameEnginePveScreenAction gameEnginePveScreen_getAction(const gameEnginePveScreen*);
gameEnginePveScreenAction gameEnginePveScreen_consumeAction(gameEnginePveScreen*);


// Utility
void gameEnginePveScreen_draw(sfRenderWindow*, const gameEnginePveScreen*);

#endif // GAME_ENGINE_PVE_SCREEN_H