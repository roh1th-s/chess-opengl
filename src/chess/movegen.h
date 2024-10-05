#if !defined(MOVEGEN_H)
#define MOVEGEN_H

#include "../types.h"
#include "board.h"
#include "piece.h"

#define MAX_PAWN_MOVES 4
#define MAX_KNIGHT_MOVES 8
#define MAX_BISHOP_MOVES 13
#define MAX_ROOK_MOVES 14
#define MAX_QUEEN_MOVES 27
#define MAX_KING_MOVES 8

typedef enum CastlingRightsRemoved
{
    CASTLING_RIGHT_NONE,
    CASTLING_RIGHT_KINGSIDE,
    CASTLING_RIGHT_QUEENSIDE,
    CASTLING_RIGHT_BOTH
} CastlingRightsRemoved;

typedef struct ChessMove
{
    Vec2i from;
    Vec2i to;

    enum MoveType
    {
        NORMAL,
        CASTLE_KINGSIDE,
        CASTLE_QUEENSIDE,
        EN_PASSANT,
        PROMOTION
    } type;

    bool is_capture;
    PieceType captured_type;
    CastlingRightsRemoved castling_rights_removed; 
    CastlingRightsRemoved opponent_castling_rights_removed;
} ChessMove;

typedef struct
{
    ChessMove *moves;
    int n_moves;
} MoveList;

MoveList generate_pseudo_legal_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board, bool only_attacking);
MoveList generate_legal_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board);
MoveList generate_pawn_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board, bool only_attacking);
MoveList generate_knight_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board);
MoveList generate_bishop_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board);
MoveList generate_rook_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board);
MoveList generate_queen_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board);
MoveList generate_king_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board, bool only_attacking);

#endif