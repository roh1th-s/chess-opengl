#include <stddef.h>
#include <stdlib.h>

#include "../types.h"
#include "board.h"
#include "movegen.h"

void chess_board_init(ChessBoard *self)
{
    // pawns
    for (int i = 0; i < 8; i++)
    {
        self->squares[i][1] = chess_piece_new(PIECE_PAWN, WHITE);
        self->squares[i][6] = chess_piece_new(PIECE_PAWN, BLACK);
    }
    // other pieces
    self->squares[0][0] = chess_piece_new(PIECE_ROOK, WHITE);
    self->squares[1][0] = chess_piece_new(PIECE_KNIGHT, WHITE);
    self->squares[2][0] = chess_piece_new(PIECE_BISHOP, WHITE);
    self->squares[3][0] = chess_piece_new(PIECE_QUEEN, WHITE);
    self->squares[4][0] = chess_piece_new(PIECE_KING, WHITE);
    self->squares[5][0] = chess_piece_new(PIECE_BISHOP, WHITE);
    self->squares[6][0] = chess_piece_new(PIECE_KNIGHT, WHITE);
    self->squares[7][0] = chess_piece_new(PIECE_ROOK, WHITE);

    self->squares[0][7] = chess_piece_new(PIECE_ROOK, BLACK);
    self->squares[1][7] = chess_piece_new(PIECE_KNIGHT, BLACK);
    self->squares[2][7] = chess_piece_new(PIECE_BISHOP, BLACK);
    self->squares[3][7] = chess_piece_new(PIECE_QUEEN, BLACK);
    self->squares[4][7] = chess_piece_new(PIECE_KING, BLACK);
    self->squares[5][7] = chess_piece_new(PIECE_BISHOP, BLACK);
    self->squares[6][7] = chess_piece_new(PIECE_KNIGHT, BLACK);
    self->squares[7][7] = chess_piece_new(PIECE_ROOK, BLACK);

    // empty squares
    for (int i = 0; i < 8; i++)
    {
        for (int j = 2; j < 6; j++)
        {
            self->squares[i][j] = NULL;
        }
    }

    self->last_move = NULL;
}

void chess_board_make_move(ChessBoard *self, ChessPiece *piece, ChessMove *move)
{
    Vec2i from = move->from;
    Vec2i to = move->to;

    ChessPiece *piece_on_target_square = self->squares[to.x][to.y];

    // capture
    if (piece_on_target_square != NULL)
    {
        chess_piece_delete(piece_on_target_square);
    }

    if (move->type == EN_PASSANT)
    {
        chess_piece_delete(self->squares[to.x][from.y]);
        self->squares[to.x][from.y] = NULL;
    }

    self->squares[to.x][to.y] = piece;
    self->squares[from.x][from.y] = NULL;

    if (self->last_move != NULL)
    {
        free(self->last_move);
    }

    self->last_move = malloc(sizeof(ChessMove));
    *self->last_move = *move;
}

void chess_board_promote_pawn(ChessBoard *self, ChessPiece *pawn, struct ChessMove *move,
                              PieceType promoted_type)
{
    chess_board_make_move(self, pawn, move);

    Vec2i to = move->to;
    self->squares[to.x][to.y]->type = promoted_type;
}