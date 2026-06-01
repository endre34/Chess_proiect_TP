#include "frontend/ui/ui_utils.h"

#include <math.h>

int sfVector2i_cmp(sfVector2i v1, sfVector2i v2)
{
    return sqrt(pow(v1.x, 2) + pow(v1.y, 2)) - sqrt(pow(v2.x, 2) + pow(v2.y, 2));
}