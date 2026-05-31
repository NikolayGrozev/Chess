#include "Renderer.h"
#include <iostream>

Renderer::Renderer() {
    fallbackPlaceholder.setRadius(PIECE_SIZE / 2.0f);
    fallbackPlaceholder.setOrigin(PIECE_SIZE / 2.0f, PIECE_SIZE / 2.0f);

    if (!boardTexture.loadFromFile("assets/board2.png")) {
        std::cerr << "Warning: assets/board2.png missing!\n";
    }
    boardSprite.setTexture(boardTexture);
    boardSprite.setPosition(ASSET_PADDING, ASSET_PADDING);

    std::string colors[] = {"w", "b"};
    for (int c = 0; c < 2; c++) {
        pawnTex[c].loadFromFile("assets/" + colors[c] + "pawn.png");
        pawnSprite[c].setTexture(pawnTex[c]); configureSprite(pawnSprite[c], PIECE_SIZE);

        knightTex[c].loadFromFile("assets/" + colors[c] + "knight.png");
        knightSprite[c].setTexture(knightTex[c]); configureSprite(knightSprite[c], PIECE_SIZE);

        bishopTex[c].loadFromFile("assets/" + colors[c] + "bishop.png");
        bishopSprite[c].setTexture(bishopTex[c]); configureSprite(bishopSprite[c], PIECE_SIZE);

        rookTex[c].loadFromFile("assets/" + colors[c] + "rook.png");
        rookSprite[c].setTexture(rookTex[c]); configureSprite(rookSprite[c], PIECE_SIZE);

        queenTex[c].loadFromFile("assets/" + colors[c] + "queen.png");
        queenSprite[c].setTexture(queenTex[c]); configureSprite(queenSprite[c], PIECE_SIZE);

        kingTex[c].loadFromFile("assets/" + colors[c] + "king.png");
        kingSprite[c].setTexture(kingTex[c]); configureSprite(kingSprite[c], PIECE_SIZE);
    }

    if (!nextTurnTex.loadFromFile("assets/nextTurn.png")) {
        std::cerr << "Warning: assets/nextTurn.png missing!\n";
    }
    nextTurnSprite.setTexture(nextTurnTex);
    configureSprite(nextTurnSprite, 75.0f);

    if (!prevTurnTex.loadFromFile("assets/previousTurn.png")) {
        std::cerr << "Warning: assets/previousTurn.png missing!\n";
    }
    prevTurnSprite.setTexture(prevTurnTex);
    configureSprite(prevTurnSprite, 75.0f);

    if (!surrenderTex.loadFromFile("assets/surrender.png")) {
        std::cerr << "Warning: assets/surrender.png missing!\n";
    }
    surrenderSprite.setTexture(surrenderTex);
    configureSprite(surrenderSprite, 75.0f);

    if (!exitTex.loadFromFile("assets/exit.png")) {
        std::cerr << "Warning: assets/exit.png missing!\n";
    }
    exitSprite.setTexture(exitTex);
    // Manually scale to width 80 without distorting (80/300 = 0.266f)
    exitSprite.setOrigin(150.0f, 68.5f);
    exitSprite.setScale(80.0f / 300.0f, 80.0f / 300.0f);

    if (!slotTex.loadFromFile("assets/slot.png")) {
        std::cerr << "Warning: assets/slot.png missing!\n";
    }
    slotSprite.setTexture(slotTex);

    if (!sidebarFont.loadFromFile("assets/ArialCE.ttf")) {
        std::cerr << "Warning: assets/ArialCE.ttf missing!\n";
    }
}

Renderer::~Renderer() {}

void Renderer::configureSprite(sf::Sprite& sprite, float targetSize) {
    sf::FloatRect bounds = sprite.getLocalBounds();
    if (bounds.width > 0 && bounds.height > 0) {
        sprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
        sprite.setScale(targetSize / bounds.width, targetSize / bounds.height);
    }
}

sf::Vector2i Renderer::mapPixelToGrid(float mouseX, float mouseY, pieceColor viewColor) const {
    if (mouseX < OFFSET_X || mouseY < OFFSET_Y) return sf::Vector2i(-1, -1);
    int gridX = static_cast<int>((mouseX - OFFSET_X) / SQUARE_SIZE);
    int gridY = static_cast<int>((mouseY - OFFSET_Y) / SQUARE_SIZE);
    
    if (gridX >= 0 && gridX < 8 && gridY >= 0 && gridY < 8) {
        // FLIP MOUSE TRACKING FOR BLACK
        if (viewColor == BLACK) {
            gridX = 7 - gridX;
            gridY = 7 - gridY;
        }
        return sf::Vector2i(gridX, gridY);
    }
    return sf::Vector2i(-1, -1);
}

void Renderer::drawGridHighlights(sf::RenderWindow& window, const std::vector<sf::Vector2i>& coordinates, pieceColor viewColor) const {
    sf::CircleShape indicator(20.0f);
    indicator.setOrigin(20.0f, 20.0f);   
    indicator.setFillColor(sf::Color(255, 0, 0, 160)); 

    for (const auto& coord : coordinates) {
        if (coord.x >= 0 && coord.x < 8 && coord.y >= 0 && coord.y < 8) {
            // INVERT HIGHLIGHT COORDINATES IF BLACK
            int drawX = (viewColor == WHITE) ? coord.x : 7 - coord.x;
            int drawY = (viewColor == WHITE) ? coord.y : 7 - coord.y;

            float cX = OFFSET_X + (drawX * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);
            float cY = OFFSET_Y + (drawY * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);
            indicator.setPosition(cX, cY);
            window.draw(indicator);
        }
    }
}

void Renderer::drawCoreChessboard(sf::RenderWindow& window, const ChessBoard& board, pieceColor viewColor) {
    if (boardTexture.getNativeHandle() != 0) {
        // ROTATE THE ENTIRE BOARD TEXTURE 180 DEGREES FOR BLACK
        if (viewColor == BLACK) {
            sf::FloatRect bounds = boardSprite.getLocalBounds();
            boardSprite.setOrigin(bounds.width / 2.0f, bounds.height / 2.0f);
            boardSprite.setPosition(ASSET_PADDING + bounds.width / 2.0f, ASSET_PADDING + bounds.height / 2.0f);
            boardSprite.setRotation(180.0f);
        } else {
            boardSprite.setOrigin(0.0f, 0.0f);
            boardSprite.setPosition(ASSET_PADDING, ASSET_PADDING);
            boardSprite.setRotation(0.0f);
        }
        window.draw(boardSprite);
    } else {
        sf::RectangleShape defaultBoard(sf::Vector2f(900.0f, 900.0f));
        defaultBoard.setPosition(ASSET_PADDING, ASSET_PADDING);
        defaultBoard.setFillColor(sf::Color(34, 139, 34));
        window.draw(defaultBoard);
    }

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            chessPiece* piece = board.at(x, y);
            if (piece != nullptr) {
                sf::Sprite* spr = nullptr;
                int cIdx = (piece->get_PieceColor() == WHITE) ? 0 : 1;

                if (typeid(*piece) == typeid(pawn))           spr = &pawnSprite[cIdx];
                else if (typeid(*piece) == typeid(knight))    spr = &knightSprite[cIdx];
                else if (typeid(*piece) == typeid(bishop))    spr = &bishopSprite[cIdx];
                else if (typeid(*piece) == typeid(rook))      spr = &rookSprite[cIdx];
                else if (typeid(*piece) == typeid(queen))     spr = &queenSprite[cIdx];
                else if (typeid(*piece) == typeid(king))      spr = &kingSprite[cIdx];

                // INVERT PIECE PLACEMENT COORDINATES IF BLACK
                int drawX = (viewColor == WHITE) ? x : 7 - x;
                int drawY = (viewColor == WHITE) ? y : 7 - y;

                float cX = OFFSET_X + (drawX * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);
                float cY = OFFSET_Y + (drawY * SQUARE_SIZE) + (SQUARE_SIZE / 2.0f);

                if (spr != nullptr && spr->getTexture() != nullptr && spr->getTexture()->getNativeHandle() != 0) {
                    spr->setPosition(cX, cY);
                    window.draw(*spr);
                } else {
                    const_cast<sf::CircleShape&>(fallbackPlaceholder).setPosition(cX, cY);
                    const_cast<sf::CircleShape&>(fallbackPlaceholder).setFillColor(cIdx == 0 ? sf::Color(240,240,240) : sf::Color(40,40,40));
                    window.draw(fallbackPlaceholder);
                }
            }
        }
    }
}

void Renderer::game_renderBoard(sf::RenderWindow& window, const ChessBoard& board, pieceColor viewColor) {
    drawCoreChessboard(window, board, viewColor);

    // ==========================================
    // DRAW SIDE UI (Fits perfectly in the X > 900 space)
    // ==========================================

    // 1. Turn Indicator (Circle)
    sf::CircleShape turnIndicator(30.0f);
    turnIndicator.setOrigin(30.0f, 30.0f);
    turnIndicator.setPosition(950.0f, 100.0f); 
    turnIndicator.setFillColor(viewColor == WHITE ? sf::Color::White : sf::Color::Black);
    turnIndicator.setOutlineThickness(2.0f);
    turnIndicator.setOutlineColor(sf::Color(150, 150, 150)); // Gray border so black circle doesn't vanish
    window.draw(turnIndicator);

    // 2. Surrender Button
    if (surrenderTex.getNativeHandle() != 0) {
        surrenderSprite.setPosition(950.0f, 200.0f);
        window.draw(surrenderSprite);
    }

    // 3. Material Display
    drawMaterialDisplay(window, board);
}

void Renderer::drawExitButton(sf::RenderWindow& window) {
    if (exitTex.getNativeHandle() != 0) {
        exitSprite.setPosition(950.0f, 300.0f);
        window.draw(exitSprite);
    }
}

void Renderer::drawMaterialDisplay(sf::RenderWindow& window, const ChessBoard& board) {
    if (slotTex.getNativeHandle() == 0) return;

    // Scale the 450x100 slot to fit the sidebar (90px wide -> scale = 90/450 = 0.2)
    float scale = 90.0f / 450.0f;
    float slotW = 450.0f * scale;  // 90px
    float slotH = 100.0f * scale;  // 20px

    // White material slot - positioned at the bottom-left of the sidebar
    float whiteX = 905.0f;
    float whiteY = 500.0f;
    slotSprite.setScale(scale, scale);
    slotSprite.setPosition(whiteX, whiteY);
    window.draw(slotSprite);

    sf::Text whiteText;
    whiteText.setFont(sidebarFont);
    whiteText.setString("W: " + std::to_string(board.get_whiteMaterial()));
    whiteText.setCharacterSize(12);
    whiteText.setFillColor(sf::Color::Black);
    sf::FloatRect wBounds = whiteText.getLocalBounds();
    whiteText.setOrigin(wBounds.left + wBounds.width / 2.0f, wBounds.top + wBounds.height / 2.0f);
    whiteText.setPosition(whiteX + slotW / 2.0f, whiteY + slotH / 2.0f);
    window.draw(whiteText);

    // Black material slot - directly below white
    float blackY = whiteY + slotH + 5.0f;
    slotSprite.setPosition(whiteX, blackY);
    window.draw(slotSprite);

    sf::Text blackText;
    blackText.setFont(sidebarFont);
    blackText.setString("B: " + std::to_string(board.get_blackMaterial()));
    blackText.setCharacterSize(12);
    blackText.setFillColor(sf::Color::Black);
    sf::FloatRect bBounds = blackText.getLocalBounds();
    blackText.setOrigin(bBounds.left + bBounds.width / 2.0f, bBounds.top + bBounds.height / 2.0f);
    blackText.setPosition(whiteX + slotW / 2.0f, blackY + slotH / 2.0f);
    window.draw(blackText);
}

void Renderer::replay_renderBoard(sf::RenderWindow& window, const ChessBoard& board, pieceColor viewColor) {
    drawCoreChessboard(window, board, viewColor);
    
    if (prevTurnTex.getNativeHandle() != 0) {
        prevTurnSprite.setPosition(950.0f, 300.0f);
        window.draw(prevTurnSprite);
    }
    
    if (nextTurnTex.getNativeHandle() != 0) {
        nextTurnSprite.setPosition(950.0f, 400.0f);
        window.draw(nextTurnSprite);
    }

    // Exit Button
    if (exitTex.getNativeHandle() != 0) {
        exitSprite.setPosition(950.0f, 200.0f);
        window.draw(exitSprite);
    }

    // Material Display
    drawMaterialDisplay(window, board);
}