#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SFML/Graphics.h>

#include "menus.h"
#include "mouse.h"
#include "resources.h"

typedef enum MainMenuAction
{
    mainMenuActionNone,

    mainMenuActionPlay,
    mainMenuActionSettings,
    mainMenuActionCredits,
    mainMenuActionExit

} MainMenuAction;

/* Lifecycle */
mainMenu* mainMenu_create(sfVector2i, sfVector2i, const Resources*); // Top-Left & Bottom-Right
void mainMenu_destroy(mainMenu*);

/* Input */
void mainMenu_updateMouse(mainMenu*, const Mouse*);

/* State */
void mainMenu_setActive(mainMenu*, sfBool);
sfBool mainMenu_isActive(const mainMenu*);

MainMenuAction mainMenu_getAction(const mainMenu*);
MainMenuAction mainMenu_consumeAction(mainMenu*);

/* Utility */
void mainMenu_draw(sfRenderWindow*, const mainMenu*);

#endif // MAIN_MENU_H