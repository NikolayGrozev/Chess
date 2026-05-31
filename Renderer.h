#ifndef RENDERER_H
#define RENDERER_H

#include <vector>
#include <string>
#include <SFML/Graphics.hpp>
#include "Core.h"
#include "ChessBoard.h"

class Renderer {
private:
    sf::Texture boardTexture;
    sf::Texture pawnTex[2], knightTex[2], bishopTex[2], rookTex[2], queenTex[2], kingTex[2];
    sf::Sprite boardSprite;
    sf::Sprite pawnSprite[2], knightSprite[2], bishopSprite[2], rookSprite[2], queenSprite[2], kingSprite[2];
    sf::Texture nextTurnTex, prevTurnTex, surrenderTex, exitTex, slotTex;
    sf::Sprite nextTurnSprite, prevTurnSprite, surrenderSprite, exitSprite, slotSprite;
    sf::Font sidebarFont;
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
    void drawExitButton(sf::RenderWindow& window);
    void drawMaterialDisplay(sf::RenderWindow& window, const ChessBoard& board);
};

#endif // RENDERER_H