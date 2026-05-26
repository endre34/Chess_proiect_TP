#include "main_menu.h"

#include "button.h"
#include "display_field.h"

#include <stdlib.h>


struct mainMenu
{
    DisplayField* titlebar;

    Button* play;
    Button* settings;
    Button* credits;
    Button* exit;

    sfBool active;
};

mainMenu* mainMenu_create(sfVector2i topLeft, sfVector2i bottomRight)
{

}

void mainMenu_destroy(mainMenu* menu)
{
    displayField_destroy(menu->titlebar);

    button_destroy(menu->play);
    button_destroy(menu->settings);
    button_destroy(menu->credits);
    button_destroy(menu->exit);

    free(menu);
}

void mainMenu_updateMouse(mainMenu* menu, const Mouse* mouse)
{
    button_updateMouse(menu->play, mouse);
    button_updateMouse(menu->settings, mouse);
    button_updateMouse(menu->credits, mouse);
    button_updateMouse(menu->exit, mouse);
}

void mainMenu_draw(sfRenderWindow* window, const mainMenu* menu)
{
    displayField_draw(window, menu->titlebar);

    button_draw(window, menu->play);
    button_draw(window, menu->settings);
    button_draw(window, menu->credits);
    button_draw(window, menu->exit);
}

