#include "game_session/game_clock.h"

#include <stdio.h>
#include <stdlib.h>


struct GameClock
{
    int enabled;

    int initialMilliseconds;
    int incrementMilliseconds;

    int whiteMilliseconds;
    int blackMilliseconds;

    PieceColor activeColor;
    PieceColor expiredColor;

    GameClockStatus status;
};


// Utility
static int clampTime(int milliseconds)
{
    if (milliseconds < 0)
    {
        return 0;
    }

    return milliseconds;
}


static int* gameClock_getMutableTime(GameClock* clock, PieceColor color)
{
    if (color == pieceColorWhite)
    {
        return &clock->whiteMilliseconds;
    }

    if (color == pieceColorBlack)
    {
        return &clock->blackMilliseconds;
    }

    return NULL;
}


// Lifecycle
GameClock* gameClock_create(int initialMilliseconds, int incrementMilliseconds)
{
    GameClock* clock;

    clock = malloc(sizeof(GameClock));

    if (clock == NULL)
    {
        return NULL;
    }

    gameClock_configure(clock, initialMilliseconds, incrementMilliseconds);

    return clock;
}


void gameClock_destroy(GameClock* clock)
{
    free(clock);
}


void gameClock_configure(GameClock* clock, int initialMilliseconds, int incrementMilliseconds)
{
    if (clock == NULL)
    {
        return;
    }

    clock->enabled = initialMilliseconds > 0;

    clock->initialMilliseconds = clampTime(initialMilliseconds);
    clock->incrementMilliseconds = clampTime(incrementMilliseconds);

    gameClock_reset(clock);
}


void gameClock_reset(GameClock* clock)
{
    if (clock == NULL)
    {
        return;
    }

    clock->whiteMilliseconds = clock->initialMilliseconds;
    clock->blackMilliseconds = clock->initialMilliseconds;

    clock->activeColor = pieceColorWhite;
    clock->expiredColor = pieceColorNone;

    if (clock->enabled)
    {
        clock->status = gameClockStatusPaused;
    }
    else
    {
        clock->status = gameClockStatusStopped;
    }
}


// State
int gameClock_hasTimeControl(const GameClock* clock)
{
    if (clock == NULL)
    {
        return 0;
    }

    return clock->enabled;
}


GameClockStatus gameClock_getStatus(const GameClock* clock)
{
    if (clock == NULL)
    {
        return gameClockStatusStopped;
    }

    return clock->status;
}


PieceColor gameClock_getActiveColor(const GameClock* clock)
{
    if (clock == NULL)
    {
        return pieceColorNone;
    }

    return clock->activeColor;
}


PieceColor gameClock_getExpiredColor(const GameClock* clock)
{
    if (clock == NULL)
    {
        return pieceColorNone;
    }

    return clock->expiredColor;
}


int gameClock_getInitialMilliseconds(const GameClock* clock)
{
    if (clock == NULL)
    {
        return 0;
    }

    return clock->initialMilliseconds;
}


int gameClock_getIncrementMilliseconds(const GameClock* clock)
{
    if (clock == NULL)
    {
        return 0;
    }

    return clock->incrementMilliseconds;
}


int gameClock_getRemainingMilliseconds(const GameClock* clock, PieceColor color)
{
    if (clock == NULL)
    {
        return 0;
    }

    if (color == pieceColorWhite)
    {
        return clock->whiteMilliseconds;
    }

    if (color == pieceColorBlack)
    {
        return clock->blackMilliseconds;
    }

    return 0;
}


// Control
void gameClock_start(GameClock* clock, PieceColor color)
{
    if (clock == NULL)
    {
        return;
    }

    if (!clock->enabled)
    {
        return;
    }

    if (color != pieceColorWhite && color != pieceColorBlack)
    {
        return;
    }

    if (clock->status == gameClockStatusExpired)
    {
        return;
    }

    clock->activeColor = color;
    clock->status = gameClockStatusRunning;
}


void gameClock_pause(GameClock* clock)
{
    if (clock == NULL)
    {
        return;
    }

    if (clock->status == gameClockStatusRunning)
    {
        clock->status = gameClockStatusPaused;
    }
}


void gameClock_resume(GameClock* clock)
{
    if (clock == NULL)
    {
        return;
    }

    if (!clock->enabled)
    {
        return;
    }

    if (clock->status == gameClockStatusPaused)
    {
        clock->status = gameClockStatusRunning;
    }
}


void gameClock_stop(GameClock* clock)
{
    if (clock == NULL)
    {
        return;
    }

    if (clock->status != gameClockStatusExpired)
    {
        clock->status = gameClockStatusStopped;
    }
}


void gameClock_setActiveColor(GameClock* clock, PieceColor color)
{
    if (clock == NULL)
    {
        return;
    }

    if (color != pieceColorWhite && color != pieceColorBlack)
    {
        return;
    }

    clock->activeColor = color;
}


// Update
void gameClock_update(GameClock* clock, int elapsedMilliseconds)
{
    int* time;

    if (clock == NULL)
    {
        return;
    }

    if (!clock->enabled)
    {
        return;
    }

    if (clock->status != gameClockStatusRunning)
    {
        return;
    }

    if (elapsedMilliseconds <= 0)
    {
        return;
    }

    time = gameClock_getMutableTime(clock, clock->activeColor);

    if (time == NULL)
    {
        return;
    }

    *time -= elapsedMilliseconds;

    if (*time <= 0)
    {
        *time = 0;
        clock->expiredColor = clock->activeColor;
        clock->status = gameClockStatusExpired;
    }
}


void gameClock_finishTurn(GameClock* clock, PieceColor color)
{
    int* time;

    if (clock == NULL)
    {
        return;
    }

    if (!clock->enabled)
    {
        return;
    }

    if (clock->status == gameClockStatusExpired)
    {
        return;
    }

    time = gameClock_getMutableTime(clock, color);

    if (time == NULL)
    {
        return;
    }

    *time += clock->incrementMilliseconds;
}


// Utility
void gameClock_writeTime(const GameClock* clock, PieceColor color, char* buffer, int bufferSize)
{
    int milliseconds;
    int totalSeconds;
    int hours;
    int minutes;
    int seconds;

    if (buffer == NULL || bufferSize <= 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (clock == NULL)
    {
        return;
    }

    milliseconds = gameClock_getRemainingMilliseconds(clock, color);
    totalSeconds = (milliseconds + 999) / 1000;

    hours = totalSeconds / 3600;
    minutes = (totalSeconds % 3600) / 60;
    seconds = totalSeconds % 60;

    if (hours > 0)
    {
        snprintf(buffer, bufferSize, "%d:%02d:%02d", hours, minutes, seconds);
        return;
    }

    snprintf(buffer, bufferSize, "%02d:%02d", minutes, seconds);
}


const char* gameClockStatus_toString(GameClockStatus status)
{
    switch (status)
    {
        case gameClockStatusStopped:
            return "Stopped";

        case gameClockStatusRunning:
            return "Running";

        case gameClockStatusPaused:
            return "Paused";

        case gameClockStatusExpired:
            return "Expired";

        default:
            return "Invalid";
    }
}