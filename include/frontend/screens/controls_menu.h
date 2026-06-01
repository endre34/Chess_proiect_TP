#ifndef CONTROLS_MENU_H
#define CONTROLS_MENU_H

#include <SFML/Graphics.h>

#include "frontend/screens/menus.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef enum ControlsMenuAction
{
    controlsMenuActionNone,

    controlsMenuActionBack

} ControlsMenuAction;

/* Lifecycle */
controlsMenu* controlsMenu_create(sfVector2i, sfVector2i, const Resources*);
void controlsMenu_destroy(controlsMenu*);

/* Input */
void controlsMenu_updateMouse(controlsMenu*, const Mouse*);

/* State */
void controlsMenu_setActive(controlsMenu*, sfBool);
sfBool controlsMenu_isActive(const controlsMenu*);

ControlsMenuAction controlsMenu_getAction(const controlsMenu*);
ControlsMenuAction controlsMenu_consumeAction(controlsMenu*);

/* Utility */
void controlsMenu_draw(sfRenderWindow*, const controlsMenu*);

#endif // CONTROLS_MENU_H