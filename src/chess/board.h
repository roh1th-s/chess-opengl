#if !defined(BOARD_H)
#define BOARD_H

#include "../types.h"
#include "piece.h"

// forward declaration to avoid circular dependency
struct ChessMove;

typedef struct
{
    ChessPiece *squares[8][8];
    struct ChessMove *last_move;
} ChessBoard;

void chess_board_init(ChessBoard *self);
void chess_board_make_move(ChessBoard *self, ChessPiece *piece, struct ChessMove *move);
void chess_board_promote_pawn(ChessBoard *self, ChessPiece *pawn, struct ChessMove *move,
                              PieceType promoted_type);

#endif