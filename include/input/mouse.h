#ifndef MOUSE_H
#define MOUSE_H

#include <SFML/Graphics.h>

typedef struct Mouse
{
    sfMouseButton button;

    sfBool pressed;
    sfBool justPressed;
    sfBool justReleased;

    sfVector2i position;

} Mouse;

Mouse* mouse_create(sfMouseButton);
Mouse* mouse_copy(const Mouse*);
void mouse_destroy(Mouse*);

void mouse_update(Mouse*, const sfRenderWindow*);

sfMouseButton mouse_getButton(const Mouse*);
sfBool mouse_isPressed(const Mouse*);
sfBool mouse_wasJustPressed(const Mouse*);
sfBool mouse_wasJustReleased(const Mouse*);
sfVector2i mouse_getPosition(const Mouse*);

#endif // MOUSE_H