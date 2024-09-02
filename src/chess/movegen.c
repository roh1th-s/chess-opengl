#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#include "movegen.h"

MoveList generate_pseudo_legal_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board,
                                     bool only_attacking)
{
    switch (piece->type)
    {
    case PIECE_PAWN:
        return generate_pawn_moves(piece, square, board, only_attacking);
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
        return generate_king_moves(piece, square, board, only_attacking);
        break;
    }

    return (MoveList){NULL, 0};
}

MoveList generate_legal_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board)
{
    MoveList pseudo_legal_moves = generate_pseudo_legal_moves(piece, square, board, false);
    ChessMove *legal_moves = malloc(pseudo_legal_moves.n_moves * sizeof(ChessMove));
    int n_legal_moves = 0;

    for (int i = 0; i < pseudo_legal_moves.n_moves; i++)
    {
        ChessMove *move = &pseudo_legal_moves.moves[i];

        ChessMove *prev_last_move = NULL;

        if (move->type == PROMOTION)
        {
            PieceType types_to_check[] = {PIECE_QUEEN, PIECE_KNIGHT};
            bool is_legal = true;

            for (int j = 0; j < 2; j++)
            {
                if (board->last_move != NULL)
                {
                    // make a copy of the last move to restore it after testing the move
                    prev_last_move = malloc(sizeof(ChessMove));
                    *prev_last_move = *board->last_move;
                }
                chess_board_promote_pawn(board, piece, move, types_to_check[j]);

                if (chess_board_is_in_check(board, piece->color))
                {
                    is_legal = false;
                }

                chess_board_undo_last_move(board, prev_last_move);

                if (!is_legal)
                {
                    break;
                }
            }

            if (is_legal)
            {
                legal_moves[n_legal_moves++] = *move;
            }
        }
        else
        {
            if (board->last_move != NULL)
            {
                // make a copy of the last move to restore it after testing the move
                prev_last_move = malloc(sizeof(ChessMove));
                *prev_last_move = *board->last_move;
            }

            chess_board_make_move(board, piece, move);

            if (!chess_board_is_in_check(board, piece->color))
            {
                legal_moves[n_legal_moves++] = *move;
            }

            chess_board_undo_last_move(board, prev_last_move);
        }
    }

    free(pseudo_legal_moves.moves);

    return (MoveList){legal_moves, n_legal_moves};
}

MoveList generate_pawn_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board,
                             bool only_attacking)
{
    ChessMove *moves = malloc(MAX_PAWN_MOVES * sizeof(ChessMove));

    bool is_white = piece->color == WHITE;
    int direction = is_white ? 1 : -1;
    bool is_on_starting_rank = is_white ? square.y == 1 : square.y == 6;
    bool is_on_promotion_rank = is_white ? square.y == 6 : square.y == 1;

    int n_moves = 0;

    ChessPiece *piece_on_target_square = board->squares[square.x][square.y + direction];
    if (piece_on_target_square == NULL && !only_attacking)
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

            // if attacking moves are requested, return diagonal moves irrespective of the piece on the square
            if (only_attacking ||
                (piece_on_diagonal_square != NULL && piece_on_diagonal_square->color != piece->color))
            {
                moves[n_moves++] = (ChessMove){square,
                                               {x, y},
                                               is_on_promotion_rank ? PROMOTION : NORMAL,
                                               true,
                                               only_attacking ? 0 : piece_on_diagonal_square->type};
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
                moves[n_moves++] = (ChessMove){square, {x, y}, EN_PASSANT, true, PIECE_PAWN};
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
                bool is_capture = piece_on_target_square != NULL;
                moves[n_moves++] = (ChessMove){
                    square, {x, y}, NORMAL, is_capture, is_capture ? piece_on_target_square->type : 0};
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
                bool is_capture = piece_on_target_square != NULL;
                moves[n_moves++] = (ChessMove){
                    square, {x, y}, NORMAL, is_capture, is_capture ? piece_on_target_square->type : 0};
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

    bool is_queen_side_rook = square.x == 0 && (square.y == 0 || square.y == 7);
    bool is_king_side_rook = square.x == 7 && (square.y == 0 || square.y == 7);

    // check what rights are removed if the rook moves
    CastlingRightsRemoved castling_rights_removed = CASTLING_RIGHT_NONE;
    bool king_side_allowed = piece->color == WHITE ? board->castling_rights.white_king_side
                                                   : board->castling_rights.black_king_side;
    bool queen_side_allowed = piece->color == WHITE ? board->castling_rights.white_queen_side
                                                    : board->castling_rights.black_queen_side;

    if (is_queen_side_rook && queen_side_allowed)
    {
        castling_rights_removed = CASTLING_RIGHT_QUEENSIDE;
    }
    else if (is_king_side_rook && king_side_allowed)
    {
        castling_rights_removed = CASTLING_RIGHT_KINGSIDE;
    }

    for (int i = 0; i < 4; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        while (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                bool is_capture = piece_on_target_square != NULL;
                moves[n_moves++] = (ChessMove){square,
                                               {x, y},
                                               NORMAL,
                                               is_capture,
                                               is_capture ? piece_on_target_square->type : 0,
                                               castling_rights_removed};
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
                bool is_capture = piece_on_target_square != NULL;
                moves[n_moves++] = (ChessMove){
                    square, {x, y}, NORMAL, is_capture, is_capture ? piece_on_target_square->type : 0};
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

MoveList generate_king_moves(const ChessPiece *piece, Vec2i square, const ChessBoard *board,
                             bool only_attacking)
{
    ChessMove *moves = malloc(MAX_KING_MOVES * sizeof(ChessMove));

    int n_moves = 0;
    int directions[8][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, -1}, {-1, 1}};

    CastlingRightsRemoved castling_rights_removed = CASTLING_RIGHT_NONE;
    bool king_side_allowed = piece->color == WHITE ? board->castling_rights.white_king_side
                                                   : board->castling_rights.black_king_side;
    bool queen_side_allowed = piece->color == WHITE ? board->castling_rights.white_queen_side
                                                    : board->castling_rights.black_queen_side;

    if (king_side_allowed && queen_side_allowed)
    {
        castling_rights_removed = CASTLING_RIGHT_BOTH;
    }
    else if (queen_side_allowed)
    {
        castling_rights_removed = CASTLING_RIGHT_QUEENSIDE;
    }
    else if (king_side_allowed)
    {
        castling_rights_removed = CASTLING_RIGHT_KINGSIDE;
    }

    for (int i = 0; i < 8; i++)
    {
        int x = square.x + directions[i][0];
        int y = square.y + directions[i][1];

        if (x >= 0 && x < 8 && y >= 0 && y < 8)
        {
            ChessPiece *piece_on_target_square = board->squares[x][y];
            if (piece_on_target_square == NULL || piece_on_target_square->color != piece->color)
            {
                bool is_capture = piece_on_target_square != NULL;
                moves[n_moves++] = (ChessMove){square,
                                               {x, y},
                                               NORMAL,
                                               is_capture,
                                               is_capture ? piece_on_target_square->type : 0,
                                               castling_rights_removed};
            }
        }
    }

    // castling
    if (!only_attacking)
    {

        bool is_check = chess_board_is_square_attacked(board, square, piece->color);
        bool king_side_allowed = piece->color == WHITE ? board->castling_rights.white_king_side
                                                       : board->castling_rights.black_king_side;
        bool queen_side_allowed = piece->color == WHITE ? board->castling_rights.white_queen_side
                                                        : board->castling_rights.black_queen_side;

        // king side
        if (!is_check && king_side_allowed)
        {
            bool is_castling_legal = true;

            Vec2i squares_to_check[] = {{5, square.y}, {6, square.y}};
            for (int i = 0; i < 2; i++)
            {
                if (board->squares[squares_to_check[i].x][squares_to_check[i].y] != NULL)
                {
                    // square is occupied
                    is_castling_legal = false;
                    break;
                }
                if (chess_board_is_square_attacked(board, squares_to_check[i], piece->color))
                {
                    is_castling_legal = false;
                    break;
                }
            }
            if (is_castling_legal)
            {
                moves[n_moves++] =
                    (ChessMove){square, {6, square.y}, CASTLE_KINGSIDE, false, 0, castling_rights_removed};
            }
        }

        // queen side
        if (!is_check && queen_side_allowed)
        {
            bool is_castling_legal = true;

            Vec2i squares_to_check[] = {{1, square.y}, {2, square.y}, {3, square.y}};
            for (int i = 0; i < 3; i++)
            {
                if (board->squares[squares_to_check[i].x][squares_to_check[i].y] != NULL)
                {
                    // square is occupied
                    is_castling_legal = false;
                    break;
                }
                if (i != 1 && chess_board_is_square_attacked(board, squares_to_check[i], piece->color))
                {
                    is_castling_legal = false;
                    break;
                }
            }
            if (is_castling_legal)
            {
                moves[n_moves++] =
                    (ChessMove){square, {2, square.y}, CASTLE_QUEENSIDE, false, 0, castling_rights_removed};
            }
        }
    }

    return (MoveList){moves, n_moves};
}