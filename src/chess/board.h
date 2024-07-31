#if !defined(BOARD_H)
#define BOARD_H

#include "../types.h"
#include "piece.h"

typedef struct
{
    ChessPiece *squares[8][8];
} ChessBoard;

void chess_board_init(ChessBoard *self);
void chess_board_move_piece(ChessBoard *self, ChessPiece *piece, Vec2i from, Vec2i to);

#endif