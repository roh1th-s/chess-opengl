typedef enum {
    PAWN,
    KNIGHT,
    BISHOP,
    ROOK,
    KING,
    QUEEN,
} PieceType;

typedef enum {
    BLACK,
    WHITE
} Color;

typedef struct {
    PieceType type;
    Color color;
} ChessPiece;