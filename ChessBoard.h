#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <string>
#include "Core.h"
#include "Piece.h"

class ChessBoard {
private:
    MutableBoardMatrix board;
    int blackMaterial;
    int whiteMaterial;
    Position enPassantTarget;
public:
    ChessBoard();
    ~ChessBoard();
    
    MutableBoardMatrix getBoard() const;

    Position getEnPassantTarget() const;
    void setEnPassantTarget(Position p);
    void clearEnPassantTarget();
    void clearBoard();

    bool isInCheck(pieceColor p, const MutableBoardMatrix& matrix) const;
    Position* getStrictlyLegalMoves(Position fromPos) const;
    bool hasNoValidMoves(pieceColor color) const;

    void applyMovement(Movement m);
    chessPiece* at(int x, int y) const;
    void place(chessPiece*& piece, int x, int y);
    int get_whiteMaterial() const;
    int get_blackMaterial() const;
    void set_whiteMaterial(int m);
    void set_blackMaterial(int m);
};

#endif // CHESSBOARD_H