#ifndef CHESS_BOARD_VIEW_H
#define CHESS_BOARD_VIEW_H

#include <SFML/Graphics.h>

#include "chess/board.h"
#include "chess/game.h"
#include "chess/move.h"
#include "game_session/game_session.h"
#include "input/mouse.h"
#include "resources/resources.h"

typedef struct ChessBoardView ChessBoardView;

/* Lifecycle */
ChessBoardView* chessBoardView_create(const Resources*);
void chessBoardView_destroy(ChessBoardView*);

/* State */
void chessBoardView_setSession(ChessBoardView*, GameSession*);
GameSession* chessBoardView_getSession(const ChessBoardView*);

void chessBoardView_setBounds(ChessBoardView*, sfFloatRect);
sfFloatRect chessBoardView_getBounds(const ChessBoardView*);

void chessBoardView_clearSelection(ChessBoardView*);

/* Input */
void chessBoardView_updateMouse(ChessBoardView*, const Mouse*);

/* Utility */
sfBool chessBoardView_getSquareAtPoint(const ChessBoardView*, sfVector2i, BoardSquare*);
void chessBoardView_drawPieceIcon(sfRenderWindow*, const ChessBoardView*, Piece, sfFloatRect);
void chessBoardView_draw(sfRenderWindow*, const ChessBoardView*);

#endif // CHESS_BOARD_VIEW_H