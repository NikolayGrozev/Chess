#ifndef PIECE_H
#define PIECE_H

#include <array>
#include "Core.h"

class chessPiece;

using BoardMatrix = std::array<std::array<const chessPiece*, 8>, 8>;
using MutableBoardMatrix = std::array<std::array<chessPiece*, 8>, 8>;

class chessPiece {
private:
    pieceColor color;
    int material;
public:
    chessPiece(pieceColor c, int material);
    virtual ~chessPiece();
    virtual Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const = 0;
    virtual void set_hasMoved(bool hm);
    pieceColor get_PieceColor() const;
    int getMaterial() const;
};

class pawn : public chessPiece {
private:
    bool hasMoved;
public:
    pawn(pieceColor c, int material);
    ~pawn();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class knight : public chessPiece {
public:
    knight(pieceColor c, int material);
    ~knight();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
};

class bishop : public chessPiece {
private:
    void getDiagonalMoves(const MutableBoardMatrix& board, const Position p, const int x_offset, const int y_offset, Position*& moves) const;
public:
    bishop(pieceColor c, int material);
    ~bishop();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
};

class rook : public chessPiece {
private:
    bool hasMoved;
    void getLaneMoves(const MutableBoardMatrix& board, const Position p, const int x_offset, const int y_offset, Position*& moves) const;
public:
    rook(pieceColor c, int material);
    ~rook();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class queen : public chessPiece {
public:
    queen(pieceColor c, int material);
    ~queen();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
};

class king : public chessPiece {
private:
    bool hasMoved;
public:
    king(pieceColor c, int material);
    ~king();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

#endif // PIECE_H