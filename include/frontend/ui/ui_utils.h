#ifndef UI_UTILS_H
#define UI_UTILS_H

#include <SFML/Graphics.h>

static const sfIntRect ZeroRect = (sfIntRect){0};
static const sfVector2i ZeroIntVect = (sfVector2i){0};

int sfVector2i_cmp(sfVector2i, sfVector2i);

#endif // UI_UTILS_H