#if !defined(CHESS_PIECE_H)
#define CHESS_PIECE_H

typedef enum {
    PIECE_PAWN,
    PIECE_KNIGHT,
    PIECE_BISHOP,
    PIECE_ROOK,
    PIECE_KING,
    PIECE_QUEEN,
} PieceType;

typedef enum {
    BLACK = 0,
    WHITE
} ChessColor;

typedef struct {
    PieceType type;
    ChessColor color;
} ChessPiece;

ChessPiece *chess_piece_new(PieceType type, ChessColor color);

#endif