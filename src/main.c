#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <SFML/Graphics.h>

#include "frontend/screen_manager.h"
#include "input/mouse.h"
#include "resources/resources.h"


#define WINDOW_WIDTH 1600
#define WINDOW_HEIGHT 900
#define WINDOW_TITLE "Chess"


static void handleRequest(sfRenderWindow* window, screenManager* manager)
{
    ScreenManagerRequest request;

    request = screenManager_consumeRequest(manager);

    switch (request.type)
    {
        case screenManagerRequestNone:
            break;

        case screenManagerRequestExit:
            sfRenderWindow_close(window);
            break;

        case screenManagerRequestApplySettings:
            break;

        case screenManagerRequestStartGame:
            break;
    }
}


int main(void)
{
    Resources* resources;
    sfRenderWindow* window;
    sfRectangleShape* background;
    screenManager* manager;
    Mouse* mouse;
    sfClock* frameClock;
    sfContextSettings settings;
    sfEvent event;
    int status;

    srand((unsigned int)time(NULL));

    resources = NULL;
    window = NULL;
    background = NULL;
    manager = NULL;
    mouse = NULL;
    frameClock = NULL;
    status = 0;

    resources = resources_create();

    if (resources == NULL)
    {
        printf("Failed to create resources.\n");
        return 1;
    }

    settings = (sfContextSettings){0};
    settings.antialiasingLevel = 8;

    window = sfRenderWindow_create(
        (sfVideoMode){WINDOW_WIDTH, WINDOW_HEIGHT, 32},
        WINDOW_TITLE,
        sfClose,
        &settings
    );

    if (window == NULL)
    {
        printf("Failed to create window.\n");
        resources_destroy(resources);
        return 1;
    }

    sfRenderWindow_setFramerateLimit(window, 60);

    background = sfRectangleShape_create();

    if (background == NULL)
    {
        printf("Failed to create background.\n");
        sfRenderWindow_destroy(window);
        resources_destroy(resources);
        return 1;
    }

    sfRectangleShape_setSize(background, (sfVector2f){WINDOW_WIDTH, WINDOW_HEIGHT});
    sfRectangleShape_setTexture(
        background,
        resources_getTexture(resources, resourceTextureBackground),
        sfTrue
    );

    manager = screenManager_create(
        (sfVector2i){0, 0},
        (sfVector2i){WINDOW_WIDTH, WINDOW_HEIGHT},
        resources
    );

    if (manager == NULL)
    {
        printf("Failed to create screen manager.\n");
        sfRectangleShape_destroy(background);
        sfRenderWindow_destroy(window);
        resources_destroy(resources);
        return 1;
    }

    mouse = mouse_create(sfMouseLeft);

    if (mouse == NULL)
    {
        printf("Failed to create mouse.\n");
        screenManager_destroy(manager);
        sfRectangleShape_destroy(background);
        sfRenderWindow_destroy(window);
        resources_destroy(resources);
        return 1;
    }

    frameClock = sfClock_create();

    if (frameClock == NULL)
    {
        printf("Failed to create frame clock.\n");
        mouse_destroy(mouse);
        screenManager_destroy(manager);
        sfRectangleShape_destroy(background);
        sfRenderWindow_destroy(window);
        resources_destroy(resources);
        return 1;
    }

    while (sfRenderWindow_isOpen(window))
    {
        sfTime elapsed;
        int elapsedMilliseconds;

        while (sfRenderWindow_pollEvent(window, &event))
        {
            if (event.type == sfEvtClosed)
            {
                sfRenderWindow_close(window);
            }

            if (event.type == sfEvtKeyReleased && event.key.code == sfKeyEscape)
            {
                sfRenderWindow_close(window);
            }

            screenManager_updateKeyboard(manager, &event);
        }

        elapsed = sfClock_restart(frameClock);
        elapsedMilliseconds = sfTime_asMilliseconds(elapsed);

        screenManager_update(manager, elapsedMilliseconds);

        mouse_update(mouse, window);
        screenManager_updateMouse(manager, mouse);

        handleRequest(window, manager);

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawRectangleShape(window, background, NULL);
        screenManager_draw(window, manager);
        sfRenderWindow_display(window);
    }

    sfClock_destroy(frameClock);
    mouse_destroy(mouse);
    screenManager_destroy(manager);
    sfRectangleShape_destroy(background);
    sfRenderWindow_destroy(window);
    resources_destroy(resources);

    return status;
}