#include "piece.h"
#include <stdlib.h>

ChessPiece *chess_piece_new(PieceType type, ChessColor color)
{
    ChessPiece *self = (ChessPiece *)malloc(sizeof(ChessPiece));
    self->type = type;
    self->color = color;
    return self;
}

void chess_piece_delete(ChessPiece *self) { free(self); }
