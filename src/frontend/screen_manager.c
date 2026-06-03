#include "frontend/screen_manager.h"

#include "frontend/screens/main_menu.h"
#include "frontend/screens/settings_menu.h"
#include "frontend/screens/controls_menu.h"
#include "frontend/screens/credits_menu.h"
#include "frontend/screens/game_setup_menu.h"
#include "frontend/screens/local_pvp_setup_menu.h"
#include "frontend/screens/engine_pve_setup_menu.h"
#include "frontend/screens/game_local_pvp_screen.h"
#include "frontend/screens/game_engine_pve_screen.h"
#include "game_session/game_session.h"

#include <stdlib.h>


struct screenManager
{
    mainMenu* main;
    settingsMenu* settings;
    controlsMenu* controls;
    creditsMenu* credits;

    gameSetupMenu* gameSetup;
    localPvpSetupMenu* localPvPSetup;
    enginePveSetupMenu* enginePvESetup;

    gameLocalPvpScreen* gameLocalPvP;
    gameEnginePveScreen* gameEnginePvE;

    GameSession* currentSession;

    ScreenId currentScreen;
    ScreenManagerRequest request;

    sfBool active;
};


static void screenManager_setOnlyActiveScreen(screenManager*, ScreenId);
static void screenManager_destroyCurrentSession(screenManager*);
static PieceColor screenManager_playerColorToPieceColor(PlayerColor);
static void screenManager_startLocalPvPGame(screenManager*, LocalPvPSetup);
static void screenManager_startEnginePvEGame(screenManager*, VsEngineSetup);

static void screenManager_updateMainMenu(screenManager*, const Mouse*);
static void screenManager_updateSettingsMenu(screenManager*, const Mouse*);
static void screenManager_updateControlsMenu(screenManager*, const Mouse*);
static void screenManager_updateCreditsMenu(screenManager*, const Mouse*);

static void screenManager_updateGameSetupMenu(screenManager*, const Mouse*);
static void screenManager_updatelocalPvpSetupMenu(screenManager*, const Mouse*);
static void screenManager_updateenginePveSetupMenu(screenManager*, const Mouse*);
static void screenManager_updategameLocalPvpScreen(screenManager*, const Mouse*);
static void screenManager_updategameEnginePveScreen(screenManager*, const Mouse*);


static void screenManager_setOnlyActiveScreen(screenManager* manager, ScreenId screen)
{
    mainMenu_setActive(manager->main, sfFalse);
    settingsMenu_setActive(manager->settings, sfFalse);
    controlsMenu_setActive(manager->controls, sfFalse);
    creditsMenu_setActive(manager->credits, sfFalse);

    gameSetupMenu_setActive(manager->gameSetup, sfFalse);
    localPvpSetupMenu_setActive(manager->localPvPSetup, sfFalse);
    enginePveSetupMenu_setActive(manager->enginePvESetup, sfFalse);

    gameLocalPvpScreen_setActive(manager->gameLocalPvP, sfFalse);
    gameEnginePveScreen_setActive(manager->gameEnginePvE, sfFalse);

    manager->currentScreen = screen;

    switch (screen)
    {
        case screenIdMainMenu:
            mainMenu_setActive(manager->main, sfTrue);
            break;

        case screenIdSettingsMenu:
            settingsMenu_setActive(manager->settings, sfTrue);
            break;

        case screenIdControlsMenu:
            controlsMenu_setActive(manager->controls, sfTrue);
            break;

        case screenIdCreditsMenu:
            creditsMenu_setActive(manager->credits, sfTrue);
            break;

        case screenIdGameSetupMenu:
            gameSetupMenu_setActive(manager->gameSetup, sfTrue);
            break;

        case screenIdLocalPvpSetupMenu:
            localPvpSetupMenu_setActive(manager->localPvPSetup, sfTrue);
            break;

        case screenIdEnginePveSetupMenu:
            enginePveSetupMenu_setActive(manager->enginePvESetup, sfTrue);
            break;

        case screenIdGameLocalPvp:
            gameLocalPvpScreen_setActive(manager->gameLocalPvP, sfTrue);
            break;

        case screenIdGameEnginePve:
            gameEnginePveScreen_setActive(manager->gameEnginePvE, sfTrue);
            break;
    }
}


static void screenManager_destroyCurrentSession(screenManager* manager)
{
    if (manager == NULL)
    {
        return;
    }

    if (manager->currentSession != NULL)
    {
        gameSession_destroy(manager->currentSession);
        manager->currentSession = NULL;
    }

    gameLocalPvpScreen_setSession(manager->gameLocalPvP, NULL);
    gameEnginePveScreen_setSession(manager->gameEnginePvE, NULL);
}


static PieceColor screenManager_playerColorToPieceColor(PlayerColor color)
{
    if (color == playerColorBlack)
    {
        return pieceColorBlack;
    }

    if (color == playerColorRandom)
    {
        if (rand() % 2 == 0)
        {
            return pieceColorWhite;
        }

        return pieceColorBlack;
    }

    return pieceColorWhite;
}


static void screenManager_startLocalPvPGame(screenManager* manager, LocalPvPSetup setup)
{
    int initialMilliseconds;
    int incrementMilliseconds;

    screenManager_destroyCurrentSession(manager);

    initialMilliseconds = (int)setup.startingTimeSeconds * 1000;
    incrementMilliseconds = (int)setup.incrementSeconds * 1000;

    manager->currentSession = gameSession_create(
        gameSessionModeLocalPvp,
        pieceColorWhite,
        initialMilliseconds,
        incrementMilliseconds
    );

    if (manager->currentSession == NULL)
    {
        return;
    }

    gameLocalPvpScreen_setSetup(manager->gameLocalPvP, setup);
    gameLocalPvpScreen_setSession(manager->gameLocalPvP, manager->currentSession);

    screenManager_setOnlyActiveScreen(manager, screenIdGameLocalPvp);
}


static void screenManager_startEnginePvEGame(screenManager* manager, VsEngineSetup setup)
{
    PieceColor humanColor;

    humanColor = screenManager_playerColorToPieceColor(setup.playerColor);

    screenManager_destroyCurrentSession(manager);

    manager->currentSession = gameSession_create(
        gameSessionModeEnginePve,
        humanColor,
        0,
        0
    );

    if (manager->currentSession == NULL)
    {
        return;
    }

    gameSession_setEngineLevel(manager->currentSession, setup.engineLevel);
    gameEnginePveScreen_setSession(manager->gameEnginePvE, manager->currentSession);

    screenManager_setOnlyActiveScreen(manager, screenIdGameEnginePve);
}


static void screenManager_updateMainMenu(screenManager* manager, const Mouse* mouse)
{
    MainMenuAction action;

    mainMenu_updateMouse(manager->main, mouse);

    action = mainMenu_consumeAction(manager->main);

    switch (action)
    {
        case mainMenuActionNone:
            break;

        case mainMenuActionPlay:
            screenManager_setOnlyActiveScreen(manager, screenIdGameSetupMenu);
            break;

        case mainMenuActionSettings:
            screenManager_setOnlyActiveScreen(manager, screenIdSettingsMenu);
            break;

        case mainMenuActionCredits:
            screenManager_setOnlyActiveScreen(manager, screenIdCreditsMenu);
            break;

        case mainMenuActionExit:
            manager->request.type = screenManagerRequestExit;
            break;
    }
}


static void screenManager_updateSettingsMenu(screenManager* manager, const Mouse* mouse)
{
    SettingsMenuAction action;

    settingsMenu_updateMouse(manager->settings, mouse);

    action = settingsMenu_consumeAction(manager->settings);

    switch (action)
    {
        case settingsMenuActionNone:
            break;

        case settingsMenuActionApply:
            manager->request.type = screenManagerRequestApplySettings;
            manager->request.data.settings = settingsMenu_getData(manager->settings);
            break;

        case settingsMenuActionControls:
            screenManager_setOnlyActiveScreen(manager, screenIdControlsMenu);
            break;

        case settingsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
            break;
    }
}


static void screenManager_updateControlsMenu(screenManager* manager, const Mouse* mouse)
{
    ControlsMenuAction action;

    controlsMenu_updateMouse(manager->controls, mouse);

    action = controlsMenu_consumeAction(manager->controls);

    switch (action)
    {
        case controlsMenuActionNone:
            break;

        case controlsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdSettingsMenu);
            break;
    }
}


static void screenManager_updateCreditsMenu(screenManager* manager, const Mouse* mouse)
{
    CreditsMenuAction action;

    creditsMenu_updateMouse(manager->credits, mouse);

    action = creditsMenu_consumeAction(manager->credits);

    switch (action)
    {
        case creditsMenuActionNone:
            break;

        case creditsMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
            break;
    }
}


static void screenManager_updateGameSetupMenu(screenManager* manager, const Mouse* mouse)
{
    GameSetupMenuAction action;

    gameSetupMenu_updateMouse(manager->gameSetup, mouse);

    action = gameSetupMenu_consumeAction(manager->gameSetup);

    switch (action)
    {
        case gameSetupMenuActionNone:
            break;

        case gameSetupMenuActionLocalPvP:
            screenManager_setOnlyActiveScreen(manager, screenIdLocalPvpSetupMenu);
            break;

        case gameSetupMenuActionVsEngine:
            screenManager_setOnlyActiveScreen(manager, screenIdEnginePveSetupMenu);
            break;

        case gameSetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);
            break;
    }
}


static void screenManager_updatelocalPvpSetupMenu(screenManager* manager, const Mouse* mouse)
{
    localPvpSetupMenuAction action;
    LocalPvPSetup setup;

    localPvpSetupMenu_updateMouse(manager->localPvPSetup, mouse);

    action = localPvpSetupMenu_consumeAction(manager->localPvPSetup);

    switch (action)
    {
        case localPvpSetupMenuActionNone:
            break;

        case localPvpSetupMenuActionStart:
            setup = localPvpSetupMenu_getData(manager->localPvPSetup);
            screenManager_startLocalPvPGame(manager, setup);
            break;

        case localPvpSetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdGameSetupMenu);
            break;
    }
}


static void screenManager_updateenginePveSetupMenu(screenManager* manager, const Mouse* mouse)
{
    enginePveSetupMenuAction action;
    VsEngineSetup setup;

    enginePveSetupMenu_updateMouse(manager->enginePvESetup, mouse);

    action = enginePveSetupMenu_consumeAction(manager->enginePvESetup);

    switch (action)
    {
        case enginePveSetupMenuActionNone:
            break;

        case enginePveSetupMenuActionStart:
            setup = enginePveSetupMenu_getData(manager->enginePvESetup);
            screenManager_startEnginePvEGame(manager, setup);
            break;

        case enginePveSetupMenuActionBack:
            screenManager_setOnlyActiveScreen(manager, screenIdGameSetupMenu);
            break;
    }
}


static void screenManager_updategameLocalPvpScreen(screenManager* manager, const Mouse* mouse)
{
    gameLocalPvpScreen_updateMouse(manager->gameLocalPvP, mouse);
}


static void screenManager_updategameEnginePveScreen(screenManager* manager, const Mouse* mouse)
{
    gameEnginePveScreen_updateMouse(manager->gameEnginePvE, mouse);
}


screenManager* screenManager_create(sfVector2i topLeft, sfVector2i bottomRight, const Resources* resources)
{
    screenManager* manager;

    if (resources == NULL)
    {
        return NULL;
    }

    manager = malloc(sizeof(screenManager));

    if (manager == NULL)
    {
        return NULL;
    }

    manager->main = NULL;
    manager->settings = NULL;
    manager->controls = NULL;
    manager->credits = NULL;

    manager->gameSetup = NULL;
    manager->localPvPSetup = NULL;
    manager->enginePvESetup = NULL;

    manager->gameLocalPvP = NULL;
    manager->gameEnginePvE = NULL;

    manager->currentSession = NULL;

    manager->currentScreen = screenIdMainMenu;
    manager->request = screenManagerRequest_getNone();
    manager->active = sfTrue;

    manager->main = mainMenu_create(topLeft, bottomRight, resources);
    manager->settings = settingsMenu_create(topLeft, bottomRight, resources);
    manager->controls = controlsMenu_create(topLeft, bottomRight, resources);
    manager->credits = creditsMenu_create(topLeft, bottomRight, resources);

    manager->gameSetup = gameSetupMenu_create(topLeft, bottomRight, resources);
    manager->localPvPSetup = localPvpSetupMenu_create(topLeft, bottomRight, resources);
    manager->enginePvESetup = enginePveSetupMenu_create(topLeft, bottomRight, resources);

    manager->gameLocalPvP = gameLocalPvpScreen_create(topLeft, bottomRight, resources);
    manager->gameEnginePvE = gameEnginePveScreen_create(topLeft, bottomRight, resources);

    if (
        manager->main == NULL ||
        manager->settings == NULL ||
        manager->controls == NULL ||
        manager->credits == NULL ||
        manager->gameSetup == NULL ||
        manager->localPvPSetup == NULL ||
        manager->enginePvESetup == NULL ||
        manager->gameLocalPvP == NULL ||
        manager->gameEnginePvE == NULL
    )
    {
        screenManager_destroy(manager);
        return NULL;
    }

    screenManager_setOnlyActiveScreen(manager, screenIdMainMenu);

    return manager;
}


void screenManager_destroy(screenManager* manager)
{
    if (manager == NULL)
    {
        return;
    }

    screenManager_destroyCurrentSession(manager);

    if (manager->main != NULL)
    {
        mainMenu_destroy(manager->main);
    }

    if (manager->settings != NULL)
    {
        settingsMenu_destroy(manager->settings);
    }

    if (manager->controls != NULL)
    {
        controlsMenu_destroy(manager->controls);
    }

    if (manager->credits != NULL)
    {
        creditsMenu_destroy(manager->credits);
    }

    if (manager->gameSetup != NULL)
    {
        gameSetupMenu_destroy(manager->gameSetup);
    }

    if (manager->localPvPSetup != NULL)
    {
        localPvpSetupMenu_destroy(manager->localPvPSetup);
    }

    if (manager->enginePvESetup != NULL)
    {
        enginePveSetupMenu_destroy(manager->enginePvESetup);
    }

    if (manager->gameLocalPvP != NULL)
    {
        gameLocalPvpScreen_destroy(manager->gameLocalPvP);
    }

    if (manager->gameEnginePvE != NULL)
    {
        gameEnginePveScreen_destroy(manager->gameEnginePvE);
    }

    free(manager);
}


void screenManager_update(screenManager* manager, int elapsedMilliseconds)
{
    if (manager == NULL)
    {
        return;
    }

    if (!manager->active)
    {
        return;
    }

    switch (manager->currentScreen)
    {
        case screenIdGameLocalPvp:
            gameLocalPvpScreen_update(manager->gameLocalPvP, elapsedMilliseconds);
            break;

        case screenIdGameEnginePve:
            gameEnginePveScreen_update(manager->gameEnginePvE, elapsedMilliseconds);
            break;

        default:
            break;
    }
}


void screenManager_updateMouse(screenManager* manager, const Mouse* mouse)
{
    if (manager == NULL || mouse == NULL)
    {
        return;
    }

    if (!manager->active)
    {
        return;
    }

    switch (manager->currentScreen)
    {
        case screenIdMainMenu:
            screenManager_updateMainMenu(manager, mouse);
            break;

        case screenIdSettingsMenu:
            screenManager_updateSettingsMenu(manager, mouse);
            break;

        case screenIdControlsMenu:
            screenManager_updateControlsMenu(manager, mouse);
            break;

        case screenIdCreditsMenu:
            screenManager_updateCreditsMenu(manager, mouse);
            break;

        case screenIdGameSetupMenu:
            screenManager_updateGameSetupMenu(manager, mouse);
            break;

        case screenIdLocalPvpSetupMenu:
            screenManager_updatelocalPvpSetupMenu(manager, mouse);
            break;

        case screenIdEnginePveSetupMenu:
            screenManager_updateenginePveSetupMenu(manager, mouse);
            break;

        case screenIdGameLocalPvp:
            screenManager_updategameLocalPvpScreen(manager, mouse);
            break;

        case screenIdGameEnginePve:
            screenManager_updategameEnginePveScreen(manager, mouse);
            break;
    }
}


void screenManager_updateKeyboard(screenManager* manager, const sfEvent* event)
{
    if (manager == NULL || event == NULL)
    {
        return;
    }

    if (!manager->active)
    {
        return;
    }

    switch (manager->currentScreen)
    {
        case screenIdLocalPvpSetupMenu:
            localPvpSetupMenu_updateKeyboard(manager->localPvPSetup, event);
            break;

        case screenIdEnginePveSetupMenu:
            enginePveSetupMenu_updateKeyboard(manager->enginePvESetup, event);
            break;

        default:
            break;
    }
}


void screenManager_setActive(screenManager* manager, sfBool active)
{
    if (manager == NULL)
    {
        return;
    }

    manager->active = active;
}


sfBool screenManager_isActive(const screenManager* manager)
{
    if (manager == NULL)
    {
        return sfFalse;
    }

    return manager->active;
}


void screenManager_setScreen(screenManager* manager, ScreenId screen)
{
    if (manager == NULL)
    {
        return;
    }

    screenManager_setOnlyActiveScreen(manager, screen);
}


ScreenId screenManager_getScreen(const screenManager* manager)
{
    if (manager == NULL)
    {
        return screenIdMainMenu;
    }

    return manager->currentScreen;
}

const GameSession* screenManager_getCurrentSession(const screenManager* manager)
{
    if (manager == NULL)
    {
        return NULL;
    }

    return manager->currentSession;
}


ScreenManagerRequest screenManager_getRequest(const screenManager* manager)
{
    if (manager == NULL)
    {
        return screenManagerRequest_getNone();
    }

    return manager->request;
}


ScreenManagerRequest screenManager_consumeRequest(screenManager* manager)
{
    ScreenManagerRequest request;

    if (manager == NULL)
    {
        return screenManagerRequest_getNone();
    }

    request = manager->request;
    manager->request = screenManagerRequest_getNone();

    return request;
}


void screenManager_draw(sfRenderWindow* window, const screenManager* manager)
{
    if (window == NULL || manager == NULL)
    {
        return;
    }

    if (!manager->active)
    {
        return;
    }

    switch (manager->currentScreen)
    {
        case screenIdMainMenu:
            mainMenu_draw(window, manager->main);
            break;

        case screenIdSettingsMenu:
            settingsMenu_draw(window, manager->settings);
            break;

        case screenIdControlsMenu:
            controlsMenu_draw(window, manager->controls);
            break;

        case screenIdCreditsMenu:
            creditsMenu_draw(window, manager->credits);
            break;

        case screenIdGameSetupMenu:
            gameSetupMenu_draw(window, manager->gameSetup);
            break;

        case screenIdLocalPvpSetupMenu:
            localPvpSetupMenu_draw(window, manager->localPvPSetup);
            break;

        case screenIdEnginePveSetupMenu:
            enginePveSetupMenu_draw(window, manager->enginePvESetup);
            break;

        case screenIdGameLocalPvp:
            gameLocalPvpScreen_draw(window, manager->gameLocalPvP);
            break;

        case screenIdGameEnginePve:
            gameEnginePveScreen_draw(window, manager->gameEnginePvE);
            break;
    }
}