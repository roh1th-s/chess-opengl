#if !defined (BOARD_H)
#define BOARD_H

#include "piece.h"

typedef struct {
    ChessPiece* squares[8][8];
} ChessBoard;

void chess_board_init(ChessBoard* self);

#endif