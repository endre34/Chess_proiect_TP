#ifndef CREDITS_MENU_H
#define CREDITS_MENU_H

#include <SFML/Graphics.h>

#include "menus.h"
#include "mouse.h"
#include "resources.h"

typedef enum CreditsMenuAction
{
    creditsMenuActionNone,

    creditsMenuActionBack

} CreditsMenuAction;

/* Lifecycle */
creditsMenu* creditsMenu_create(sfVector2i, sfVector2i, const Resources*);
void creditsMenu_destroy(creditsMenu*);

/* Input */
void creditsMenu_updateMouse(creditsMenu*, const Mouse*);

/* State */
void creditsMenu_setActive(creditsMenu*, sfBool);
sfBool creditsMenu_isActive(const creditsMenu*);

CreditsMenuAction creditsMenu_getAction(const creditsMenu*);
CreditsMenuAction creditsMenu_consumeAction(creditsMenu*);

/* Utility */
void creditsMenu_draw(sfRenderWindow*, const creditsMenu*);

#endif // CREDITS_MENU_H