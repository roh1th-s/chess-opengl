#include <stdbool.h>
#include <stddef.h>

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
}

MoveList generate_pawn_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    ChessMove *moves = malloc(MAX_PAWN_MOVES * sizeof(ChessMove));

    bool isWhite = piece->color == WHITE;
    int direction = isWhite ? 1 : -1;
    bool isOnStartingRank = isWhite ? square.y == 1 : square.y == 6;

    int n_moves = 0;

    if (board->squares[square.x][square.y + direction] == NULL)
    {
        moves[n_moves++] = (ChessMove){square, {square.x, square.y + direction}};

        if (isOnStartingRank && board->squares[square.x][square.y + 2 * direction] == NULL)
        {
            moves[n_moves++] = (ChessMove){square, {square.x, square.y + 2 * direction}};
        }
    }

    // TODO: en passant & promotion

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
            if (board->squares[x][y] == NULL)
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
            if (board->squares[x][y] == NULL)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }
            else
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
            if (board->squares[x][y] == NULL)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }
            else
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
            if (board->squares[x][y] == NULL)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }
            else
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
            if (board->squares[x][y] == NULL)
            {
                moves[n_moves++] = (ChessMove){square, {x, y}};
            }
        }
    }

    // TODO: castling

    return (MoveList){moves, n_moves};
}