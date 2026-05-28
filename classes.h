#ifndef CLASSES_H
#define CLASSES_H

#include <vector>

enum pieceColor {
    WHITE,
    BLACK
};

enum pinDirection {
    NONE,
    diagonalLeft,
    diagonalRight,
    diagonalBottomLeft,
    diagonalBottomRight,
    top,
    bottom,
    left,
    right
};

class Position;
Position* addToArr(int x, int y, Position* arr);

class Renderer {
public: 
    Renderer();
    ~Renderer();
};

class Inputhandler {
public:
    Inputhandler();
    ~Inputhandler();
};

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

class chessPiece {
private:
    pieceColor color;
    bool isPinnedToKing;
    pinDirection d;
public:
    chessPiece(pieceColor c, bool pinned, pinDirection direction);
    virtual ~chessPiece();
    virtual Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const = 0;
    virtual void set_hasMoved(bool hm);
    pieceColor get_PieceColor() const;
    bool get_isPinned() const;
    void set_isPinned(bool value);
    pinDirection get_pinDirection() const;
};

class pawn : public chessPiece {
private:
    bool hasMoved;
public:
    pawn(pieceColor c, bool pinned, pinDirection direction);
    ~pawn();
    Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class knight : public chessPiece {
public:
    knight(pieceColor c, bool pinned, pinDirection direction);
    ~knight();
    Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const override;
};

class bishop : public chessPiece {
private:
    void getDiagonalMoves(const chessPiece* board[8][8], const Position p, const int x_offset, const int y_offset, Position*& moves) const;
public:
    bishop(pieceColor c, bool pinned, pinDirection direction);
    ~bishop();
    Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const override;
};

class rook : public chessPiece {
private:
    bool hasMoved;
    void getLaneMoves(const chessPiece* board[8][8], const Position p, const int x_offset, const int y_offset, Position*& moves) const;
public:
    rook(pieceColor c, bool pinned, pinDirection direction);
    ~rook();
    Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class queen : public chessPiece {
public:
    queen(pieceColor c, bool pinned, pinDirection direction);
    ~queen();
    Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const override;
};

class king : public chessPiece {
private:
    bool hasMoved;
public:
    king(pieceColor c, bool pinned, pinDirection direction);
    ~king();
    Position* get_ValidMoves(const chessPiece* board[8][8], const Position p) const override;
    bool get_hasMoved() const;
    void set_hasMoved(bool hm) override;
};

class ChessBoard {
private:
    chessPiece* board[8][8];
    int blackMaterial;
    int whiteMaterial;
public:
    ChessBoard();
    ~ChessBoard();
    bool isInCheck(pieceColor p) const;
    void validateMovement(Movement m);
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
    Renderer renderer;
    Inputhandler inputhandler;
    bool isOver;
    pieceColor currentTurn;
    int lastCaptureMove;
    int currentMoveIndex;
public:
    game();
    ~game();
    void game_save();
    void game_replay();
    void game_run();
    void action_nextPosition();
    void action_pastPosition();
    void action_select(Position p);
    std::vector<Movement> get_Moves();
};

#endif // CLASSES_H