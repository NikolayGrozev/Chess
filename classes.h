#ifndef CLASSES_H
#define CLASSES_H

#include <vector>
#include <array>  

enum pieceColor {
    WHITE,
    BLACK
};

class Renderer {
public: 
    Renderer();
    ~Renderer();
};

class Position;
Position* addToArr(int x, int y, Position* arr);

class Position {
private:
    int x;
    int y;
public:
    Position();
    Position(int x, int y);
    ~Position();
    int get_x() const;
    int get_y() const;
    
    friend Position* addToArr(int x, int y, Position* arr);
};

class Movement {
private:
    Position from;
    Position to;
public:
    Movement();
    Movement(int x1, int y1, int x2, int y2);
    ~Movement();
    Position get_from() const;
    Position get_to() const;
};

class chessPiece; 

using BoardMatrix = std::array<std::array<const chessPiece*, 8>, 8>;
using MutableBoardMatrix = std::array<std::array<chessPiece*, 8>, 8>;

class chessPiece {
private:
    pieceColor color;
public:
    chessPiece(pieceColor c);
    virtual ~chessPiece();
    virtual Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const = 0;
    virtual void set_hasMoved(bool hm);
    pieceColor get_PieceColor() const;
};

class pawn : public chessPiece {
private:
    bool hasMoved;
public:
    pawn(pieceColor c);
    ~pawn();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class knight : public chessPiece {
public:
    knight(pieceColor c);
    ~knight();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
};

class bishop : public chessPiece {
private:
    void getDiagonalMoves(const MutableBoardMatrix& board, const Position p, const int x_offset, const int y_offset, Position*& moves) const;
public:
    bishop(pieceColor c);
    ~bishop();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
};

class rook : public chessPiece {
private:
    bool hasMoved;
    void getLaneMoves(const MutableBoardMatrix& board, const Position p, const int x_offset, const int y_offset, Position*& moves) const;
public:
    rook(pieceColor c);
    ~rook();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class queen : public chessPiece {
public:
    queen(pieceColor c);
    ~queen();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
};

class king : public chessPiece {
private:
    bool hasMoved;
public:
    king(pieceColor c);
    ~king();
    Position* get_ValidMoves(const MutableBoardMatrix& board, const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class ChessBoard {
private:
    MutableBoardMatrix board;
    int blackMaterial;
    int whiteMaterial;
public:
    ChessBoard();
    ~ChessBoard();
    
    MutableBoardMatrix getBoard() const;

    bool isInCheck(pieceColor p, const MutableBoardMatrix& matrix) const;
    Position* getStrictlyLegalMoves(Position fromPos) const;
    void applyMovement(Movement m);
    chessPiece* at(int x, int y) const;
    void place(chessPiece*& piece, int x, int y);
    int get_whiteMaterial() const;
    int get_blackMaterial() const;
};

class game {
private:
    ChessBoard board;
    std::vector<Movement> moves;
    bool isOver;
    pieceColor currentTurn;
    int lastCaptureMove;
    int currentMoveIndex;
    Renderer renderer;
public:
    game();
    ~game();
    void game_save();
    void game_replay();
    void game_run();
    std::vector<Movement> get_Moves();
};

#endif // CLASSES_H