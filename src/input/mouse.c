#include "input/mouse.h"

#include <stdlib.h>

Mouse* mouse_create(sfMouseButton button)
{
    Mouse* mouse;

    mouse = malloc(sizeof(Mouse));

    mouse->button = button;

    mouse->pressed = sfFalse;
    mouse->justPressed = sfFalse;
    mouse->justReleased = sfFalse;

    mouse->position = (sfVector2i){0, 0};

    return mouse;
}

Mouse* mouse_copy(const Mouse* mouse)
{
    Mouse* copy;

    copy = malloc(sizeof(Mouse));

    copy->button = mouse->button;

    copy->pressed = mouse->pressed;
    copy->justPressed = mouse->justPressed;
    copy->justReleased = mouse->justReleased;

    copy->position = mouse->position;

    return copy;
}

void mouse_destroy(Mouse* mouse)
{
    free(mouse);
}

void mouse_update(Mouse* mouse, const sfRenderWindow* window)
{
    sfBool previousPressed;
    sfBool currentPressed;

    previousPressed = mouse->pressed;
    currentPressed = sfMouse_isButtonPressed(mouse->button);

    mouse->pressed = currentPressed;

    if (!previousPressed && currentPressed)
        mouse->justPressed = sfTrue;
    else
        mouse->justPressed = sfFalse;

    if (previousPressed && !currentPressed)
        mouse->justReleased = sfTrue;
    else
        mouse->justReleased = sfFalse;

    mouse->position = sfMouse_getPositionRenderWindow(window);
}

sfMouseButton mouse_getButton(const Mouse* mouse)
{
    return mouse->button;
}

sfBool mouse_isPressed(const Mouse* mouse)
{
    return mouse->pressed;
}

sfBool mouse_wasJustPressed(const Mouse* mouse)
{
    return mouse->justPressed;
}

sfBool mouse_wasJustReleased(const Mouse* mouse)
{
    return mouse->justReleased;
}

sfVector2i mouse_getPosition(const Mouse* mouse)
{
    return mouse->position;
}