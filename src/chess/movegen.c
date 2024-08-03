#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "movegen.h"

MoveList generate_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    switch (piece->type)
    {
    case PIECE_PAWN:
        return generate_pawn_moves(piece, square, board);
        break;
    case PIECE_KNIGHT:
        return generate_knight_moves(piece, square, board);
        break;
    case PIECE_BISHOP:
        return generate_bishop_moves(piece, square, board);
        break;
    case PIECE_ROOK:
        return generate_rook_moves(piece, square, board);
        break;
    case PIECE_QUEEN:
        return generate_queen_moves(piece, square, board);
        break;
    case PIECE_KING:
        return generate_king_moves(piece, square, board);
        break;
    }

    return (MoveList){NULL, 0};
}

MoveList generate_pawn_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_PAWN_MOVES * sizeof(ChessMove));

    bool is_white = piece->color == WHITE;
    int direction = is_white ? 1 : -1;
    bool is_on_starting_rank = is_white ? square.y == 1 : square.y == 6;
    bool is_on_promotion_rank = is_white ? square.y == 6 : square.y == 1;

    int n_moves = 0;

    ChessPiece *piece_on_target_square = board->squares[square.x][square.y + direction];
    if (piece_on_target_square == NULL)
    {
        moves[n_moves++] =
            (ChessMove){square, {square.x, square.y + direction}, is_on_promotion_rank ? PROMOTION : NORMAL};

        // double push
        if (is_on_starting_rank && board->squares[square.x][square.y + 2 * direction] == NULL)
        {
            moves[n_moves++] = (ChessMove){square, {square.x, square.y + 2 * direction}};
        }
    }

    // diagonal captures
    for (int i = -1; i <= 1; i += 2)
    {
        int x = square.x + i;
        int y = square.y + direction;

        if (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_diagonal_square = board->squares[x][y];
            if (piece_on_diagonal_square != NULL && piece_on_diagonal_square->color != piece->color)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}, is_on_promotion_rank ? PROMOTION : NORMAL};
            }
        }
    }

    // en passant
    if (board->last_move != NULL)
    {
        Vec2i last_move_to = board->last_move->to;
        bool is_on_en_passant_rank = is_white ? square.y == 4 : square.y == 3;

        if (is_on_en_passant_rank)
        {
            bool last_move_was_pawn = board->squares[last_move_to.x][last_move_to.y]->type == PIECE_PAWN;
            bool last_move_was_double_push = abs(last_move_to.y - board->last_move->from.y) == 2;
            bool last_move_was_adjacent = abs(last_move_to.x - square.x) == 1;

            if (last_move_was_pawn && last_move_was_double_push && last_move_was_adjacent)
            {
                int x = last_move_to.x;
                int y = last_move_to.y + (is_white ? 1 : -1);
                moves[n_moves++] = (ChessMove){square, {x, y}, EN_PASSANT};
            }
        }
    }

    return (MoveList){moves, n_moves};
}

MoveList generate_knight_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_KNIGHT_MOVES * sizeof(ChessMove));

    int n_moves = 0;
    int directions[8][2] = {{-1, 2}, {1, 2}, {2, 1}, {2, -1}, {1, -2}, {-1, -2}, {-2, -1}, {-2, 1}};

    for (int i = 0; i < 8; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        if (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }
        }
    }

    return (MoveList){moves, n_moves};
}

MoveList generate_bishop_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_BISHOP_MOVES * sizeof(ChessMove));

    int n_moves = 0;
    int directions[4][2] = {{1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    for (int i = 0; i < 4; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        while (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }

            if (piece_on_target_square)
            {
                break;
            }

            x += directions[i][0];
            y += directions[i][1];
        }
    }

    return (MoveList){moves, n_moves};
}

MoveList generate_rook_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_ROOK_MOVES * sizeof(ChessMove));

    int n_moves = 0;
    int directions[4][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}};

    for (int i = 0; i < 4; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        while (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }

            if (piece_on_target_square)
            {
                break;
            }

            x += directions[i][0];
            y += directions[i][1];
        }
    }

    return (MoveList){moves, n_moves};
}

MoveList generate_queen_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_QUEEN_MOVES * sizeof(ChessMove));

    int n_moves = 0;
    int directions[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    for (int i = 0; i < 8; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        while (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }

            if (piece_on_target_square)
            {
                break;
            }

            x += directions[i][0];
            y += directions[i][1];
        }
    }

    return (MoveList){moves, n_moves};
}

MoveList generate_king_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_KING_MOVES * sizeof(ChessMove));

    int n_moves = 0;
    int directions[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    for (int i = 0; i < 8; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        if (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }
        }
    }

    // TODO: castling

    return (MoveList){moves, n_moves};
}