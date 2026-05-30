#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>     // Required for passing arrays of coordinates
#include <typeinfo>   // Required for typeid expression checks
#include "classes.h"  // Pulls your structural blueprints

class ChessVisualizer {
private:
    sf::Texture boardTexture;
    sf::Texture pawnTex[2], knightTex[2], bishopTex[2], rookTex[2], queenTex[2], kingTex[2];
    sf::Sprite boardSprite;
    sf::Sprite pawnSprite[2], knightSprite[2], bishopSprite[2], rookSprite[2], queenSprite[2], kingSprite[2];
    sf::CircleShape fallbackPlaceholder;

    // =========================================================================
    // BOARD GEOMETRY CONFIGURATION
    // =========================================================================
    const float SQUARE_SIZE   = 100.0f; 
    const float ASSET_PADDING = 50.0f;  // Outer black background thickness
    const float BOARD_OUTLINE = 50.0f;  // Board PNG frame thickness
    
    // Total combined displacement to find matrix index (0,0) -> 100px
    const float OFFSET_X      = ASSET_PADDING + BOARD_OUTLINE; 
    const float OFFSET_Y      = ASSET_PADDING + BOARD_OUTLINE; 
    
    const float PIECE_SIZE    = 75.0f;  

    void configureSprite(sf::Sprite& sprite, float targetSize) {
        sf::FloatRect bounds = sprite.getLocalBounds();
        if (bounds.width > 0 && bounds.height > 0) {
            sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            float scaleX = targetSize / bounds.width;
            float scaleY = targetSize / bounds.height;
            sprite.setScale(scaleX, scaleY);
        }
    }

public:
    ChessVisualizer() {
        fallbackPlaceholder.setRadius(PIECE_SIZE / 2.0f);
        fallbackPlaceholder.setOrigin(PIECE_SIZE / 2.0f, PIECE_SIZE / 2.0f);

        if (!boardTexture.loadFromFile("assets/board2.png")) {
            std::cerr << "Warning: assets/board2.png not found!\n";
        }
        boardSprite.setTexture(boardTexture);
        boardSprite.setPosition(ASSET_PADDING, ASSET_PADDING);

        std::string colors[] = {"w", "b"};
        for (int c = 0; c < 2; c++) {
            pawnTex[c].loadFromFile("assets/" + colors[c] + "pawn.png");
            pawnSprite[c].setTexture(pawnTex[c]);
            configureSprite(pawnSprite[c], PIECE_SIZE);

            knightTex[c].loadFromFile("assets/" + colors[c] + "knight.png");
            knightSprite[c].setTexture(knightTex[c]);
            configureSprite(knightSprite[c], PIECE_SIZE);

            bishopTex[c].loadFromFile("assets/" + colors[c] + "bishop.png");
            bishopSprite[c].setTexture(bishopTex[c]);
            configureSprite(bishopSprite[c], PIECE_SIZE);

            rookTex[c].loadFromFile("assets/" + colors[c] + "rook.png");
            rookSprite[c].setTexture(rookTex[c]);
            configureSprite(rookSprite[c], PIECE_SIZE);

            queenTex[c].loadFromFile("assets/" + colors[c] + "queen.png");
            queenSprite[c].setTexture(queenTex[c]);
            configureSprite(queenSprite[c], PIECE_SIZE);

            kingTex[c].loadFromFile("assets/" + colors[c] + "king.png");
            kingSprite[c].setTexture(kingTex[c]);
            configureSprite(kingSprite[c], PIECE_SIZE);
        }
    }

    sf::Vector2i mapPixelToGrid(float mouseX, float mouseY) const {
        if (mouseX < OFFSET_X || mouseY < OFFSET_Y) {
            return sf::Vector2i(-1, -1);
        }
        int gridX = static_cast<int>((mouseX - OFFSET_X) / SQUARE_SIZE);
        int gridY = static_cast<int>((mouseY - OFFSET_Y) / SQUARE_SIZE);

        if (gridX >= 0 && gridX < 8 && gridY >= 0 && gridY < 8) {
            return sf::Vector2i(gridX, gridY);
        }
        return sf::Vector2i(-1, -1);
    }

    // =========================================================================
    // MOVE INDICATOR DRAWER FUNCTION (RADIUS CHANGED TO 20PX)
    // =========================================================================
    void drawGridHighlights(sf::RenderWindow& window, const std::vector<sf::Vector2i>& coordinates) {
        sf::CircleShape indicator;
        indicator.setRadius(20.0f);          // Updated to requested 20px radius size
        indicator.setOrigin(20.0f, 20.0f);   // Flawless tile geometric centering origin
        indicator.setFillColor(sf::Color(255, 0, 0, 160)); // Transparent red indicator overlay

        for (const auto& coord : coordinates) {
            if (coord.x >= 0 && coord.x < 8 && coord.y >= 0 && coord.y < 8) {
                float centerX = OFFSET_X + (coord.x * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);
                float centerY = OFFSET_Y + (coord.y * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);
                
                indicator.setPosition(centerX, centerY);
                window.draw(indicator);
            }
        }
    }

    void drawBoardState(sf::RenderWindow& window, const ChessBoard& chessBoard) {
        if (boardTexture.getNativeHandle() != 0) {
            window.draw(boardSprite);
        } else {
            sf::RectangleShape defaultBoard(sf::Vector2f(900.0f, 900.0f));
            defaultBoard.setPosition(ASSET_PADDING, ASSET_PADDING);
            defaultBoard.setFillColor(sf::Color(34, 139, 34));
            window.draw(defaultBoard);
        }

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                chessPiece* piece = chessBoard.at(x, y);
                
                if (piece != nullptr) {
                    sf::Sprite* spriteToDraw = nullptr;
                    int colorIdx = (piece->get_PieceColor() == WHITE) ? 0 : 1;

                    if (typeid(*piece) == typeid(pawn))           spriteToDraw = &pawnSprite[colorIdx];
                    else if (typeid(*piece) == typeid(knight))    spriteToDraw = &knightSprite[colorIdx];
                    else if (typeid(*piece) == typeid(bishop))    spriteToDraw = &bishopSprite[colorIdx];
                    else if (typeid(*piece) == typeid(rook))      spriteToDraw = &rookSprite[colorIdx];
                    else if (typeid(*piece) == typeid(queen))     spriteToDraw = &queenSprite[colorIdx];
                    else if (typeid(*piece) == typeid(king))      spriteToDraw = &kingSprite[colorIdx];

                    float centerX = OFFSET_X + (x * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);
                    float centerY = OFFSET_Y + (y * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);

                    if (spriteToDraw != nullptr && spriteToDraw->getTexture() != nullptr && spriteToDraw->getTexture()->getNativeHandle() != 0) {
                        spriteToDraw->setPosition(centerX, centerY);
                        window.draw(*spriteToDraw);
                    } else {
                        fallbackPlaceholder.setPosition(centerX, centerY);
                        if (colorIdx == 0) {
                            fallbackPlaceholder.setFillColor(sf::Color(240, 240, 240));
                            fallbackPlaceholder.setOutlineColor(sf::Color::Blue);
                        } else {
                            fallbackPlaceholder.setFillColor(sf::Color(40, 40, 40));
                            fallbackPlaceholder.setOutlineColor(sf::Color::Red);
                        }
                        fallbackPlaceholder.setOutlineThickness(2.0f);
                        window.draw(fallbackPlaceholder);
                    }
                }
            }
        }
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess Logic RTTI Visualizer");
    
    ChessBoard mockBoard;

    chessPiece* whitePawn   = new pawn(WHITE);
    chessPiece* blackKing   = new king(BLACK);
    chessPiece* whiteQueen  = new queen(WHITE);
    chessPiece* blackKnight = new knight(BLACK);
    
    mockBoard.place(blackKing, 0, 0);   // (x=0, y=0)
    mockBoard.place(blackKnight, 1, 1); // (x=1, y=1)
    mockBoard.place(whiteQueen, 4, 4);  // (x=4, y=4)
    mockBoard.place(whitePawn, 7, 6);   // (x=7, y=7)

    // Highlight tracking collection vectors
    std::vector<sf::Vector2i> activeHighlights;
    sf::Vector2i selectedPieceCoord(-1, -1); // Tracks which piece coordinate was clicked

    ChessVisualizer visualEngine;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    float mouseX = static_cast<float>(event.mouseButton.x);
                    float mouseY = static_cast<float>(event.mouseButton.y);

                    sf::Vector2i clickedGrid = visualEngine.mapPixelToGrid(mouseX, mouseY);

                    // =================================================================
                    // SELECTION & DISMISSAL LOGIC
                    // =================================================================
                    
                    // Check if user clicked an active red highlight circle move option
                    bool clickedLegalMove = false;
                    for (const auto& move : activeHighlights) {
                        if (clickedGrid.x == move.x && clickedGrid.y == move.y) {
                            clickedLegalMove = true;
                            break;
                        }
                    }

                    if (clickedLegalMove) {
                        std::cout << "Action: Executing move from (" 
                                  << selectedPieceCoord.x << ", " << selectedPieceCoord.y 
                                  << ") to (" << clickedGrid.x << ", " << clickedGrid.y << ")" << std::endl;
                        
                        // 1. Construct the movement payload using your custom class
                        Movement moveObj(selectedPieceCoord.x, selectedPieceCoord.y, clickedGrid.x, clickedGrid.y);

                        // 2. Apply it to the board (wrapped in try/catch to respect your safety exceptions)
                        try {
                            mockBoard.applyMovement(moveObj);
                            std::cout << "-> Move applied successfully! Board matrix updated." << std::endl;
                        } catch (const std::exception& e) {
                            std::cerr << "-> Move failed: " << e.what() << std::endl;
                        }

                        // 3. Clear indicators and selection state after executing the movement
                        activeHighlights.clear();
                        selectedPieceCoord = sf::Vector2i(-1, -1);
                    }
                    else if (clickedGrid.x != -1) {
// User clicked a normal board tile (not a legal move target)
                        chessPiece* targetPiece = mockBoard.at(clickedGrid.x, clickedGrid.y);

                        if (targetPiece != nullptr) {
                            // A piece was clicked -> Clear old highlights and switch selections
                            activeHighlights.clear();
                            selectedPieceCoord = clickedGrid;
                            std::cout << "\nSelected piece at: (" << clickedGrid.x << ", " << clickedGrid.y << ")" << std::endl;

                            // =========================================================
                            // DYNAMIC VALID MOVES CALCULATION (Works for ALL pieces)
                            // =========================================================
                            Position currentPos(clickedGrid.x, clickedGrid.y);
                            
                            // 1. Fetch the safe copy of the board matrix
                            MutableBoardMatrix calculationMatrix = mockBoard.getBoard();

                            // 2. Call the virtual get_ValidMoves on the clicked piece dynamically
                            Position* nativeMoves = mockBoard.getStrictlyLegalMoves(currentPos);

                            // 3. Extract the array data up to the INT_MAX terminator
                            for (int i = 0; nativeMoves[i].get_x() != INT_MAX && nativeMoves[i].get_y() != INT_MAX; i++) {
                                activeHighlights.push_back(sf::Vector2i(nativeMoves[i].get_x(), nativeMoves[i].get_y()));
                            }

                            std::cout << "-> Pulled " << activeHighlights.size() << " legal destination targets." << std::endl;
                            
                            // 4. Clean up heap memory
                            delete[] nativeMoves; 
                        } 
                        else {
                            // User clicked an empty square -> Deselect and clear circles
                            std::cout << "Clicked an empty square. Dismissing move highlights." << std::endl;
                            activeHighlights.clear();
                            selectedPieceCoord = sf::Vector2i(-1, -1);
                        }
                    } 
                    else {
                        // User clicked out-of-bounds (the border frame or black background canvas padding)
                        std::cout << "Clicked outside board. Dismissing move highlights." << std::endl;
                        activeHighlights.clear();
                        selectedPieceCoord = sf::Vector2i(-1, -1);
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        
        // 1. Draw the background chessboard sheet and the piece sprites
        visualEngine.drawBoardState(window, mockBoard);
        
        // 2. Draw the 20px move circles on top if a valid selection is active
        visualEngine.drawGridHighlights(window, activeHighlights);
        
        window.display();
    }

    return 0;
}