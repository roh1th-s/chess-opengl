#include <stdlib.h>
#include "piece.h"

ChessPiece *chess_piece_new(PieceType type, ChessColor color)
{
    ChessPiece *self = (ChessPiece *)malloc(sizeof(ChessPiece));
    self->type = type;
    self->color = color;
    return self;
}