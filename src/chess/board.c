#include <stddef.h>

#include "board.h"
#include "../types.h"

void chess_board_init(ChessBoard* self) {
    // pawns
    for (int i = 0; i < 8; i++) {
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
    for (int i = 0; i < 8; i++) {
        for (int j = 2; j < 6; j++) {
            self->squares[i][j] = NULL;
        }
    }
}

void chess_board_move_piece(ChessBoard* self, ChessPiece* piece, Vec2i from, Vec2i to) {
    self->squares[to.x][to.y] = piece;
    self->squares[from.x][from.y] = NULL;
}