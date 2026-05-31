#ifndef CLASSES_H
#define CLASSES_H

#include <vector>
#include <array>  
#include <SFML/Graphics.hpp>

enum pieceColor {
    WHITE,
    BLACK
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

class Renderer {
private:
    sf::Texture boardTexture;
    sf::Texture pawnTex[2], knightTex[2], bishopTex[2], rookTex[2], queenTex[2], kingTex[2];
    sf::Sprite boardSprite;
    sf::Sprite pawnSprite[2], knightSprite[2], bishopSprite[2], rookSprite[2], queenSprite[2], kingSprite[2];
    sf::CircleShape fallbackPlaceholder;

    // Board Layout Metrics
    const float SQUARE_SIZE   = 100.0f; 
    const float ASSET_PADDING = 50.0f;  
    const float BOARD_OUTLINE = 50.0f;  
    const float OFFSET_X      = ASSET_PADDING + BOARD_OUTLINE; 
    const float OFFSET_Y      = ASSET_PADDING + BOARD_OUTLINE; 
    const float PIECE_SIZE    = 75.0f;  

    void configureSprite(sf::Sprite& sprite, float targetSize);
    void drawCoreChessboard(sf::RenderWindow& window, const ChessBoard& board, pieceColor viewColor);

public: 
    Renderer();
    ~Renderer();
    
    sf::Vector2i mapPixelToGrid(float mouseX, float mouseY, pieceColor viewColor) const;
    void drawGridHighlights(sf::RenderWindow& window, const std::vector<sf::Vector2i>& coordinates, pieceColor viewColor) const;
    
    // Core state-driven rendering methods
    void game_renderBoard(sf::RenderWindow& window, const ChessBoard& board, pieceColor viewColor);
    void replay_renderBoard(sf::RenderWindow& window, const ChessBoard& board, pieceColor viewColor);
};

class game {
private:
    ChessBoard board;
    std::vector<Movement> moves;
    bool isOver;
    pieceColor currentTurn;
    int moves_Since_Last_Capture;
    Renderer renderer;
public:
    game();
    ~game();
    bool insufficientMaterial(pieceColor color) const;
    void game_replay();
    void game_run();
    std::vector<Movement> get_Moves();

    friend std::ostream& operator<<(std::ostream& os, const game& g);
    friend std::istream& operator>>(std::istream& in, game& g);
};

#endif // CLASSES_H