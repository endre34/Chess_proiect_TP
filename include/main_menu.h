#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SFML/Graphics.h>

#include "interfaces.h"
#include "mouse.h"

/* Lifecycle */
mainMenu* mainMenu_create(sfVector2i, sfVector2i); // Top-Left & Bottom-Right
void mainMenu_destroy(mainMenu*);

/* Input */
void mainMenu_updateMouse(mainMenu*, const Mouse*);

/* Utility */
void mainMenu_draw(sfRenderWindow*, const mainMenu*);

#endif // MAIN_MENU_H