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

    self->white_king_pos = (Vec2i){4, 0};
    self->black_king_pos = (Vec2i){4, 7};

    // empty squares
    for (int i = 0; i < 8; i++)
    {
        for (int j = 2; j < 6; j++)
        {
            self->squares[i][j] = NULL;
        }
    }

    self->last_move = NULL;
    self->castling_rights = (CastlingRights){1, 1, 1, 1};
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

    switch (move->type)
    {
    case EN_PASSANT:
        chess_piece_delete(self->squares[to.x][from.y]);
        self->squares[to.x][from.y] = NULL;
        break;
    case CASTLE_KINGSIDE:
        if (piece->color == WHITE)
        {
            self->squares[5][0] = self->squares[7][0];
            self->squares[7][0] = NULL;
        }
        else
        {
            self->squares[5][7] = self->squares[7][7];
            self->squares[7][7] = NULL;
        }
        break;
    case CASTLE_QUEENSIDE:
        if (piece->color == WHITE)
        {
            self->squares[3][0] = self->squares[0][0];
            self->squares[0][0] = NULL;
        }
        else
        {
            self->squares[3][7] = self->squares[0][7];
            self->squares[0][7] = NULL;
        }
        break;
    }

    self->squares[to.x][to.y] = piece;
    self->squares[from.x][from.y] = NULL;

    if (piece->type == PIECE_KING)
    {
        if (piece->color == WHITE)
        {
            self->white_king_pos = to;
        }
        else
        {
            self->black_king_pos = to;
        }
    }

    CastlingRightsRemoved removed_rights = move->castling_rights_removed;
    if (removed_rights != CASTLING_RIGHT_NONE)
    {
        if (removed_rights == CASTLING_RIGHT_KINGSIDE)
        {
            piece->color == WHITE ? (self->castling_rights.white_king_side = 0)
                                  : (self->castling_rights.black_king_side = 0);
        }
        else if (removed_rights == CASTLING_RIGHT_QUEENSIDE)
        {
            piece->color == WHITE ? (self->castling_rights.white_queen_side = 0)
                                  : (self->castling_rights.black_queen_side = 0);
        }
        else if (removed_rights == CASTLING_RIGHT_BOTH)
        {
            piece->color == WHITE
                ? (self->castling_rights.white_king_side = 0, self->castling_rights.white_queen_side = 0)
                : (self->castling_rights.black_king_side = 0, self->castling_rights.black_queen_side = 0);
        }
    }

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

void chess_board_undo_last_move(ChessBoard *self, ChessMove *prev_last_move)
{
    ChessMove *last_move = self->last_move;
    Vec2i from = last_move->from;
    Vec2i to = last_move->to;

    ChessPiece *moved_piece = self->squares[to.x][to.y];

    switch (last_move->type)
    {
    case EN_PASSANT:
        self->squares[to.x][from.y] = chess_piece_new(PIECE_PAWN, !self->squares[to.x][to.y]->color);
        break;
    case CASTLE_KINGSIDE:
        self->squares[7][from.y] = self->squares[5][from.y];
        self->squares[5][from.y] = NULL;
        break;
    case CASTLE_QUEENSIDE:
        self->squares[0][from.y] = self->squares[3][from.y];
        self->squares[3][from.y] = NULL;
        break;
    case PROMOTION:
        moved_piece->type = PIECE_PAWN; // make it a pawn again
        break;
    }

    self->squares[from.x][from.y] = moved_piece;
    self->squares[to.x][to.y] = last_move->is_capture && last_move->type != EN_PASSANT
                                    ? chess_piece_new(last_move->captured_type, !moved_piece->color)
                                    : NULL;

    if (moved_piece->type == PIECE_KING)
    {
        if (moved_piece->color == WHITE)
        {
            self->white_king_pos = from;
        }
        else
        {
            self->black_king_pos = from;
        }
    }

    // restore castling rights
    CastlingRightsRemoved removed_rights = last_move->castling_rights_removed;
    if (removed_rights != CASTLING_RIGHT_NONE)
    {
        if (removed_rights == CASTLING_RIGHT_KINGSIDE)
        {
            moved_piece->color == WHITE ? (self->castling_rights.white_king_side = 1)
                                        : (self->castling_rights.black_king_side = 1);
        }
        else if (removed_rights == CASTLING_RIGHT_QUEENSIDE)
        {
            moved_piece->color == WHITE ? (self->castling_rights.white_queen_side = 1)
                                        : (self->castling_rights.black_queen_side = 1);
        }
        else if (removed_rights == CASTLING_RIGHT_BOTH)
        {
            moved_piece->color == WHITE
                ? (self->castling_rights.white_king_side = 1, self->castling_rights.white_queen_side = 1)
                : (self->castling_rights.black_king_side = 1, self->castling_rights.black_queen_side = 1);
        }
    }

    if (self->last_move != NULL)
    {
        free(self->last_move);
    }

    self->last_move = prev_last_move; // restore the previous last move
}

bool chess_board_is_square_attacked(ChessBoard *self, Vec2i square, ChessColor color)
{
    // TODO: optimize this

    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ChessPiece *piece = self->squares[i][j];
            if (piece != NULL && piece->color != color)
            {
                MoveList move_list = generate_pseudo_legal_moves(piece, (Vec2i){i, j}, self, true);
                for (int k = 0; k < move_list.n_moves; k++)
                {
                    if (move_list.moves[k].to.x == square.x && move_list.moves[k].to.y == square.y)
                    {
                        return true;
                    }
                }
                free(move_list.moves);
            }
        }
    }
    return false;
}

bool chess_board_is_in_check(ChessBoard *self, ChessColor color)
{
    Vec2i king_pos = color == WHITE ? self->white_king_pos : self->black_king_pos;
    return chess_board_is_square_attacked(self, king_pos, color);
}

bool chess_board_does_side_have_legal_moves(ChessBoard *self, ChessColor color)
{
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            ChessPiece *piece = self->squares[i][j];
            if (piece != NULL && piece->color == color)
            {
                MoveList move_list = generate_legal_moves(piece, (Vec2i){i, j}, self);
                if (move_list.n_moves > 0)
                {
                    return true;
                }
                free(move_list.moves);
            }
        }
    }
    return false;
}

bool chess_board_is_in_checkmate(ChessBoard *self, ChessColor color, bool do_check_detection)
{
    if (do_check_detection && !chess_board_is_in_check(self, color))
    {
        return false;
    }

    return !chess_board_does_side_have_legal_moves(self, color);
}

bool chess_board_is_in_stalemate(ChessBoard *self, ChessColor color, bool do_check_detection)
{
    if (do_check_detection && chess_board_is_in_check(self, color))
    {
        return false;
    }

    return !chess_board_does_side_have_legal_moves(self, color);
}