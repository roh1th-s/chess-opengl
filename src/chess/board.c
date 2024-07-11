#include <stddef.h>

#include "board.h"

void chess_board_init(ChessBoard* self) {
    // pawns
    for (int i = 0; i < 8; i++) {
        self->squares[1][i] = chess_piece_new(PIECE_PAWN, WHITE);
        self->squares[6][i] = chess_piece_new(PIECE_PAWN, BLACK);
    }
    // other pieces
    self->squares[0][0] = chess_piece_new(PIECE_ROOK, WHITE);
    self->squares[0][1] = chess_piece_new(PIECE_KNIGHT, WHITE);
    self->squares[0][2] = chess_piece_new(PIECE_BISHOP, WHITE);
    self->squares[0][3] = chess_piece_new(PIECE_QUEEN, WHITE);
    self->squares[0][4] = chess_piece_new(PIECE_KING, WHITE);
    self->squares[0][5] = chess_piece_new(PIECE_BISHOP, WHITE);
    self->squares[0][6] = chess_piece_new(PIECE_KNIGHT, WHITE);
    self->squares[0][7] = chess_piece_new(PIECE_ROOK, WHITE);

    self->squares[7][0] = chess_piece_new(PIECE_ROOK, BLACK);
    self->squares[7][1] = chess_piece_new(PIECE_KNIGHT, BLACK);
    self->squares[7][2] = chess_piece_new(PIECE_BISHOP, BLACK);
    self->squares[7][3] = chess_piece_new(PIECE_QUEEN, BLACK);
    self->squares[7][4] = chess_piece_new(PIECE_KING, BLACK);
    self->squares[7][5] = chess_piece_new(PIECE_BISHOP, BLACK);
    self->squares[7][6] = chess_piece_new(PIECE_KNIGHT, BLACK);
    self->squares[7][7] = chess_piece_new(PIECE_ROOK, BLACK);

    // empty squares
    for (int i = 2; i < 6; i++) {
        for (int j = 0; j < 8; j++) {
            self->squares[i][j] = NULL;
        }
    }



}