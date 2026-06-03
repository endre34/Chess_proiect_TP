#include "game_session/game_session.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct PositionCounter
{
    char key[GAME_POSITION_KEY_MAX];
    int count;

} PositionCounter;


struct GameSession
{
    Game* game;
    GameClock* clock;

    GameSessionMode mode;
    PieceColor humanColor;
    int engineLevel;

    GameSessionStatus status;
    GameSessionEndReason endReason;
    PieceColor winner;

    GameSessionMoveRecord moves[GAME_SESSION_MOVE_MAX];
    int moveCount;

    PositionCounter positions[GAME_SESSION_POSITION_MAX];
    int positionCount;
};


// Utility
static void appendText(char* buffer, int bufferSize, const char* text)
{
    int currentLength;
    int remaining;

    if (buffer == NULL || text == NULL || bufferSize <= 0)
    {
        return;
    }

    currentLength = (int)strlen(buffer);

    if (currentLength >= bufferSize - 1)
    {
        return;
    }

    remaining = bufferSize - currentLength;
    snprintf(buffer + currentLength, remaining, "%s", text);
}


static void gameSession_clearHistory(GameSession* session)
{
    if (session == NULL)
    {
        return;
    }

    session->moveCount = 0;
    session->positionCount = 0;
}


static int gameSession_addPosition(GameSession* session)
{
    char key[GAME_POSITION_KEY_MAX];
    int i;

    if (session == NULL)
    {
        return 0;
    }

    game_writePositionKey(session->game, key, GAME_POSITION_KEY_MAX);

    for (i = 0; i < session->positionCount; i++)
    {
        if (strcmp(session->positions[i].key, key) == 0)
        {
            session->positions[i].count++;
            return session->positions[i].count;
        }
    }

    if (session->positionCount >= GAME_SESSION_POSITION_MAX)
    {
        return 1;
    }

    snprintf(session->positions[session->positionCount].key, GAME_POSITION_KEY_MAX, "%s", key);
    session->positions[session->positionCount].count = 1;
    session->positionCount++;

    return 1;
}


static void gameSession_recordMove(GameSession* session, Move move)
{
    GameSessionMoveRecord* record;

    if (session == NULL)
    {
        return;
    }

    if (session->moveCount >= GAME_SESSION_MOVE_MAX)
    {
        return;
    }

    record = &session->moves[session->moveCount];

    record->move = move;
    move_writeUci(move, record->uci, MOVE_UCI_MAX);
    game_writePositionKey(session->game, record->positionKey, GAME_POSITION_KEY_MAX);

    session->moveCount++;
}


static void gameSession_setDraw(GameSession* session, GameSessionEndReason reason)
{
    session->status = gameSessionStatusDraw;
    session->endReason = reason;
    session->winner = pieceColorNone;

    gameClock_pause(session->clock);
}


static void gameSession_setTimeout(GameSession* session, PieceColor expiredColor)
{
    session->endReason = gameSessionEndReasonTimeout;

    if (expiredColor == pieceColorWhite)
    {
        session->status = gameSessionStatusBlackWon;
        session->winner = pieceColorBlack;
        return;
    }

    if (expiredColor == pieceColorBlack)
    {
        session->status = gameSessionStatusWhiteWon;
        session->winner = pieceColorWhite;
        return;
    }

    session->status = gameSessionStatusDraw;
    session->winner = pieceColorNone;
}


static void gameSession_updateFromGame(GameSession* session)
{
    GameStatus status;

    if (session == NULL)
    {
        return;
    }

    status = game_getStatus(session->game);

    if (status == gameStatusActive)
    {
        session->status = gameSessionStatusActive;
        session->endReason = gameSessionEndReasonNone;
        session->winner = pieceColorNone;
        return;
    }

    if (status == gameStatusWhiteCheckmate)
    {
        session->status = gameSessionStatusBlackWon;
        session->endReason = gameSessionEndReasonCheckmate;
        session->winner = pieceColorBlack;
        gameClock_pause(session->clock);
        return;
    }

    if (status == gameStatusBlackCheckmate)
    {
        session->status = gameSessionStatusWhiteWon;
        session->endReason = gameSessionEndReasonCheckmate;
        session->winner = pieceColorWhite;
        gameClock_pause(session->clock);
        return;
    }

    if (status == gameStatusStalemate)
    {
        gameSession_setDraw(session, gameSessionEndReasonStalemate);
        return;
    }

    if (status == gameStatusInsufficientMaterial)
    {
        gameSession_setDraw(session, gameSessionEndReasonInsufficientMaterial);
    }
}


// Lifecycle
GameSession* gameSession_create(GameSessionMode mode, PieceColor humanColor, int initialMilliseconds, int incrementMilliseconds)
{
    GameSession* session;

    session = malloc(sizeof(GameSession));

    if (session == NULL)
    {
        return NULL;
    }

    session->game = game_create();

    if (session->game == NULL)
    {
        free(session);
        return NULL;
    }

    session->clock = gameClock_create(initialMilliseconds, incrementMilliseconds);

    if (session->clock == NULL)
    {
        game_destroy(session->game);
        free(session);
        return NULL;
    }

    session->mode = mode;
    session->humanColor = humanColor;
    session->engineLevel = 0;

    gameSession_reset(session);

    return session;
}


void gameSession_destroy(GameSession* session)
{
    if (session == NULL)
    {
        return;
    }

    gameClock_destroy(session->clock);
    game_destroy(session->game);
    free(session);
}


void gameSession_reset(GameSession* session)
{
    if (session == NULL)
    {
        return;
    }

    game_reset(session->game);
    gameClock_reset(session->clock);

    session->status = gameSessionStatusActive;
    session->endReason = gameSessionEndReasonNone;
    session->winner = pieceColorNone;

    gameSession_clearHistory(session);
    gameSession_addPosition(session);

    gameClock_start(session->clock, pieceColorWhite);
}


// State
const Game* gameSession_getGame(const GameSession* session)
{
    if (session == NULL)
    {
        return NULL;
    }

    return session->game;
}


const GameClock* gameSession_getClock(const GameSession* session)
{
    if (session == NULL)
    {
        return NULL;
    }

    return session->clock;
}


GameSessionMode gameSession_getMode(const GameSession* session)
{
    if (session == NULL)
    {
        return gameSessionModeLocalPvp;
    }

    return session->mode;
}


GameSessionStatus gameSession_getStatus(const GameSession* session)
{
    if (session == NULL)
    {
        return gameSessionStatusActive;
    }

    return session->status;
}


GameSessionEndReason gameSession_getEndReason(const GameSession* session)
{
    if (session == NULL)
    {
        return gameSessionEndReasonNone;
    }

    return session->endReason;
}


PieceColor gameSession_getWinner(const GameSession* session)
{
    if (session == NULL)
    {
        return pieceColorNone;
    }

    return session->winner;
}


PieceColor gameSession_getHumanColor(const GameSession* session)
{
    if (session == NULL)
    {
        return pieceColorNone;
    }

    return session->humanColor;
}

int gameSession_getEngineLevel(const GameSession* session)
{
    if (session == NULL)
    {
        return 0;
    }

    return session->engineLevel;
}


void gameSession_setEngineLevel(GameSession* session, int engineLevel)
{
    if (session == NULL)
    {
        return;
    }

    if (engineLevel < 0)
    {
        engineLevel = 0;
    }

    if (engineLevel > 20)
    {
        engineLevel = 20;
    }

    session->engineLevel = engineLevel;
}


// View
BoardPerspective gameSession_getPerspective(const GameSession* session)
{
    if (session == NULL)
    {
        return boardPerspectiveWhite;
    }

    if (session->mode == gameSessionModeLocalPvp)
    {
        return board_perspectiveFromPieceColor(game_getSideToMove(session->game));
    }

    return board_perspectiveFromPieceColor(session->humanColor);
}


// Time
void gameSession_update(GameSession* session, int elapsedMilliseconds)
{
    PieceColor expiredColor;

    if (session == NULL)
    {
        return;
    }

    if (session->status != gameSessionStatusActive)
    {
        return;
    }

    gameClock_update(session->clock, elapsedMilliseconds);

    if (gameClock_getStatus(session->clock) != gameClockStatusExpired)
    {
        return;
    }

    expiredColor = gameClock_getExpiredColor(session->clock);
    gameSession_setTimeout(session, expiredColor);
}


void gameSession_pauseClock(GameSession* session)
{
    if (session == NULL)
    {
        return;
    }

    gameClock_pause(session->clock);
}


void gameSession_resumeClock(GameSession* session)
{
    if (session == NULL)
    {
        return;
    }

    if (session->status != gameSessionStatusActive)
    {
        return;
    }

    gameClock_resume(session->clock);
}


int gameSession_getRemainingMilliseconds(const GameSession* session, PieceColor color)
{
    if (session == NULL)
    {
        return 0;
    }

    return gameClock_getRemainingMilliseconds(session->clock, color);
}


void gameSession_writeTime(const GameSession* session, PieceColor color, char* buffer, int bufferSize)
{
    if (buffer == NULL || bufferSize <= 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (session == NULL)
    {
        return;
    }

    gameClock_writeTime(session->clock, color, buffer, bufferSize);
}


// Moves
int gameSession_makeMove(GameSession* session, BoardSquare from, BoardSquare to)
{
    Move move;
    PieceColor moverColor;
    int repetitionCount;

    if (session == NULL)
    {
        return 0;
    }

    if (session->status != gameSessionStatusActive)
    {
        return 0;
    }

    moverColor = game_getSideToMove(session->game);

    if (!game_makeMoveRecord(session->game, from, to, &move))
    {
        return 0;
    }

    gameClock_finishTurn(session->clock, moverColor);
    gameClock_setActiveColor(session->clock, game_getSideToMove(session->game));

    gameSession_recordMove(session, move);
    gameSession_updateFromGame(session);

    if (session->status != gameSessionStatusActive)
    {
        return 1;
    }

    repetitionCount = gameSession_addPosition(session);

    if (repetitionCount >= 3)
    {
        gameSession_setDraw(session, gameSessionEndReasonThreefoldRepetition);
    }

    return 1;
}


int gameSession_getMoveCount(const GameSession* session)
{
    if (session == NULL)
    {
        return 0;
    }

    return session->moveCount;
}


const GameSessionMoveRecord* gameSession_getMoveRecord(const GameSession* session, int index)
{
    if (session == NULL)
    {
        return NULL;
    }

    if (index < 0 || index >= session->moveCount)
    {
        return NULL;
    }

    return &session->moves[index];
}


// Stockfish
void gameSession_writeStockfishPositionCommand(const GameSession* session, char* buffer, int bufferSize)
{
    int i;

    if (buffer == NULL || bufferSize <= 0)
    {
        return;
    }

    buffer[0] = '\0';

    if (session == NULL)
    {
        return;
    }

    appendText(buffer, bufferSize, "position startpos");

    if (session->moveCount == 0)
    {
        return;
    }

    appendText(buffer, bufferSize, " moves");

    for (i = 0; i < session->moveCount; i++)
    {
        appendText(buffer, bufferSize, " ");
        appendText(buffer, bufferSize, session->moves[i].uci);
    }
}


// Utility
const char* gameSessionStatus_toString(GameSessionStatus status)
{
    switch (status)
    {
        case gameSessionStatusActive:
            return "Active";

        case gameSessionStatusWhiteWon:
            return "White won";

        case gameSessionStatusBlackWon:
            return "Black won";

        case gameSessionStatusDraw:
            return "Draw";

        default:
            return "Invalid";
    }
}


const char* gameSessionEndReason_toString(GameSessionEndReason reason)
{
    switch (reason)
    {
        case gameSessionEndReasonNone:
            return "None";

        case gameSessionEndReasonCheckmate:
            return "Checkmate";

        case gameSessionEndReasonStalemate:
            return "Stalemate";

        case gameSessionEndReasonInsufficientMaterial:
            return "Insufficient material";

        case gameSessionEndReasonThreefoldRepetition:
            return "Threefold repetition";

        case gameSessionEndReasonTimeout:
            return "Timeout";

        default:
            return "Invalid";
    }
}