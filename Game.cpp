#include <fstream>
#include "Game.h"
#include <sstream>
#include <algorithm>
#include <vector>

game::game() :
    isOver(false),
    moves_Since_Last_Capture(0),
    currentTurn(WHITE),
    board()
{}

game::~game() {}

bool game::insufficientMaterial(pieceColor color) const {
    int minorPieceScore = 0;
    
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            chessPiece* piece = board.at(x, y);
            if (piece == nullptr) continue;

            if (typeid(*piece) == typeid(pawn) && piece->get_PieceColor() == color) {
                return false; 
            }
            // Group with parentheses to override native && operator precedence rules
            if ((typeid(*piece) == typeid(rook) || typeid(*piece) == typeid(queen)) && piece->get_PieceColor() == color) {
                return false; 
            }
            if (typeid(*piece) == typeid(knight) && piece->get_PieceColor() == color) {
                minorPieceScore += 3;
            }
            if (typeid(*piece) == typeid(bishop) && piece->get_PieceColor() == color) {
                minorPieceScore += 3;
            }
        }
    }

    return (minorPieceScore <= 3);
}

std::vector<Movement> game::get_Moves() {
    return this->moves;
}

std::ostream& operator<<(std::ostream& os, const game& g){
    for (const auto& move : g.moves) {
        os << move.get_from().get_x() << "," << move.get_from().get_y() 
           << "->" 
           << move.get_to().get_x() << "," << move.get_to().get_y() 
           << "\n";
    }
    return os;
}
std::istream& operator>>(std::istream& is, game& g) {
    g.moves.clear();

    std::string line;

    while (std::getline(is, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        int fromX, fromY, toX, toY;
        char comma, dash, greaterThan;

        // Extract expected structure: X , Y - > X , Y
        if (ss >> fromX >> comma >> fromY >> dash >> greaterThan >> toX >> comma >> toY) {
            // Reconstruct the structural movement abstraction object
            Movement loadedMove(fromX, fromY, toX, toY);
            g.moves.push_back(loadedMove);
        }
    }
    return is;
}

void game::game_run(sf::RenderWindow& window) {
    sf::View view(sf::FloatRect(0.f, 0.f, 1000.f, 1000.f));
    window.setView(view);

    std::vector<sf::Vector2i> activeHighlights;
    sf::Vector2i selectedPieceCoord(-1, -1);
    
    this->isOver = false;
    this->currentTurn = WHITE;
    this->moves_Since_Last_Capture = 0;

    // =========================================================================
    // STANDARD CHESS BOARD PIECE PLACEMENT INITIALIZATION
    // =========================================================================
    this->setupInitialBoard();

    std::cout << "Starting Match: White's Turn" << std::endl;

    while (window.isOpen()) {

        // =====================================================================
        // ACTIVE TURN STATE VALIDATION SCANNER (CHECKMATE / STALEMATE)
        // =====================================================================
        if (!this->isOver && board.hasNoValidMoves(this->currentTurn)) {
            MutableBoardMatrix currentMatrix = board.getBoard();
            
            if (board.isInCheck(this->currentTurn, currentMatrix)) {
                std::cout << "\n==============================================" << std::endl;
                std::cout << " MATCH OVER: CHECKMATE!" << std::endl;
                std::cout << " " << ((this->currentTurn == WHITE) ? "White" : "Black") << " has no legal escape options." << std::endl;
                std::cout << " " << ((this->currentTurn == WHITE) ? "BLACK" : "WHITE") << " WINS THE MATCH!" << std::endl;
                std::cout << "==============================================" << std::endl;
            } else {
                std::cout << "\n==============================================" << std::endl;
                std::cout << " MATCH OVER: STALEMATE!" << std::endl;
                std::cout << " " << ((this->currentTurn == WHITE) ? "White" : "Black") << " is not in check but has zero legal moves." << std::endl;
                std::cout << " The match ends in a DRAW." << std::endl;
                std::cout << "==============================================" << std::endl;
            }
            
            this->isOver = true;
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return; 
            }

            if (event.type == sf::Event::Resized) {
                float newWidth = std::max(1000.f, static_cast<float>(event.size.width));
                float newHeight = std::max(1000.f, static_cast<float>(event.size.height));
                window.setSize(sf::Vector2u(newWidth, newHeight));

                float windowRatio = newWidth / newHeight;
                float targetRatio = 1.0f;
                float vpW = 1.0f, vpH = 1.0f, vpX = 0.0f, vpY = 0.0f;

                if (windowRatio > targetRatio) {
                    vpW = targetRatio / windowRatio;
                    vpX = (1.0f - vpW) / 2.0f;
                } else {
                    vpH = windowRatio / targetRatio;
                    vpY = (1.0f - vpH) / 2.0f;
                }
                view.setViewport(sf::FloatRect(vpX, vpY, vpW, vpH));
                window.setView(view);
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
sf::Vector2i rawMouse(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f mappedMouse = window.mapPixelToCoords(rawMouse);

                    if (this->isOver) {
                        // Exit button bounds in sidebar (Center is 950,300. Scaled Size 80x36.5)
                        if (mappedMouse.x >= 910.0f && mappedMouse.x <= 990.0f && 
                            mappedMouse.y >= 281.7f && mappedMouse.y <= 318.3f) {
                            return; // Drop out to the main menu
                        }
                        continue; // Ignore piece clicks if game is over
                    }

                    // =========================================================
                    // SURRENDER BUTTON CLICK DETECTION
                    // (Center is 950,200. Size 75x75)
                    // =========================================================
                    if (mappedMouse.x >= 912.5f && mappedMouse.x <= 987.5f && 
                        mappedMouse.y >= 162.5f && mappedMouse.y <= 237.5f) {
                        
                        std::cout << "\n==============================================" << std::endl;
                        std::cout << " MATCH OVER: SURRENDER!" << std::endl;
                        std::cout << " " << ((this->currentTurn == WHITE) ? "White" : "Black") << " has thrown in the towel." << std::endl;
                        std::cout << " " << ((this->currentTurn == WHITE) ? "BLACK" : "WHITE") << " WINS THE MATCH!" << std::endl;
                        std::cout << "==============================================" << std::endl;
                        
                        this->isOver = true;
                        continue; // Stay in the loop to show the exit button
                    }

                    // PASS THE CURRENT TURN TO THE MOUSE MAPPER
                    sf::Vector2i clickedGrid = renderer.mapPixelToGrid(mappedMouse.x, mappedMouse.y, this->currentTurn);
                    bool clickedLegalMove = false;
                    for (const auto& move : activeHighlights) {
                        if (clickedGrid.x == move.x && clickedGrid.y == move.y) {
                            clickedLegalMove = true;
                            break;
                        }
                    }

                    if (clickedLegalMove) {
                        Movement finalizedMove(selectedPieceCoord.x, selectedPieceCoord.y, clickedGrid.x, clickedGrid.y);
                        
                        chessPiece* destinationTarget = board.at(clickedGrid.x, clickedGrid.y);
                        chessPiece* movingPiece = board.at(selectedPieceCoord.x, selectedPieceCoord.y);

                        // 1. Detect Double-Step (Check this BEFORE we move the piece!)
                        bool isDoubleStep = false;
                        if (dynamic_cast<pawn*>(movingPiece) != nullptr) {
                            if (std::abs(clickedGrid.y - selectedPieceCoord.y) == 2) {
                                isDoubleStep = true;
                            }
                        }

                        if (destinationTarget != nullptr || typeid(*movingPiece) == typeid(pawn)) {
                            this->moves_Since_Last_Capture = 0;
                        } else {
                            this->moves_Since_Last_Capture++;
                        }

                        // 2. Apply the move (Executes the matrix updates and handles EP deletions natively)
                        board.applyMovement(finalizedMove);
                        moves.push_back(finalizedMove);

                        // 3. Clear the previous turn's target (It expires exactly right now)
                        board.clearEnPassantTarget();

                        // 4. Set the new target if a double-step just occurred
                        if (isDoubleStep) {
                            int midY = selectedPieceCoord.y + ((clickedGrid.y - selectedPieceCoord.y) / 2);
                            board.setEnPassantTarget(Position(clickedGrid.x, midY));
                        }

                        activeHighlights.clear();
                        selectedPieceCoord = sf::Vector2i(-1, -1);

                        this->currentTurn = (this->currentTurn == WHITE) ? BLACK : WHITE;
                        std::cout << "Turn Swapped. Current Player: " 
                                  << ((currentTurn == WHITE) ? "White" : "Black") << std::endl;

                        if (this->moves_Since_Last_Capture >= 100) { 
                            std::cout << "Match Terminated: 50-Move Rule Triggered (Draw)!" << std::endl;
                            this->isOver = true;
                        }
                        if (insufficientMaterial(WHITE) && insufficientMaterial(BLACK)) {
                            std::cout << "Match Terminated: Insufficient Material Rule Triggered (Draw)!" << std::endl;
                            this->isOver = true;
                        }
                    }
                    else if (clickedGrid.x != -1) {
                        chessPiece* selectedPiece = board.at(clickedGrid.x, clickedGrid.y);

                        if (selectedPiece != nullptr && selectedPiece->get_PieceColor() == this->currentTurn) {
                            activeHighlights.clear();
                            selectedPieceCoord = clickedGrid;

                            Position nativePos(clickedGrid.x, clickedGrid.y);
                            Position* legalMovesArray = board.getStrictlyLegalMoves(nativePos);

                            for (int i = 0; legalMovesArray[i].get_x() != INT_MAX && legalMovesArray[i].get_y() != INT_MAX; i++) {
                                activeHighlights.push_back(sf::Vector2i(legalMovesArray[i].get_x(), legalMovesArray[i].get_y()));
                            }

                            delete[] legalMovesArray; 
                        } else {
                            activeHighlights.clear();
                            selectedPieceCoord = sf::Vector2i(-1, -1);
                        }
                    } else {
                        activeHighlights.clear();
                        selectedPieceCoord = sf::Vector2i(-1, -1);
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        
        // PASS CURRENT TURN TO RENDERERS
        renderer.game_renderBoard(window, board, this->currentTurn); 
        renderer.drawGridHighlights(window, activeHighlights, this->currentTurn);
        
        if (this->isOver) {
            renderer.drawExitButton(window);
        }

        window.display();
    }
}

void game::setupInitialBoard() {
    board.clearBoard();

    // 1. Place Pawns across rows 1 (Black) and 6 (White)
    for (int x = 0; x < 8; x++) {
        chessPiece* bPawn = new pawn(BLACK, 1);
        chessPiece* wPawn = new pawn(WHITE, 1);
        board.place(bPawn, x, 1);
        board.place(wPawn, x, 6);
        board.set_blackMaterial(board.get_blackMaterial()+bPawn->getMaterial());
        board.set_whiteMaterial(board.get_whiteMaterial()+wPawn->getMaterial());
    }

    // 2. Place Rooks
    chessPiece* bRook1 = new rook(BLACK, 5);   chessPiece* bRook2 = new rook(BLACK, 5);
    chessPiece* wRook1 = new rook(WHITE, 5);   chessPiece* wRook2 = new rook(WHITE, 5);
    board.place(bRook1, 0, 0); board.place(bRook2, 7, 0);
    board.place(wRook1, 0, 7); board.place(wRook2, 7, 7);
    board.set_blackMaterial(board.get_blackMaterial()+(2*bRook1->getMaterial()));
    board.set_whiteMaterial(board.get_whiteMaterial()+(2*wRook1->getMaterial()));

    // 3. Place Knights
    chessPiece* bKnight1 = new knight(BLACK, 3); chessPiece* bKnight2 = new knight(BLACK, 3);
    chessPiece* wKnight1 = new knight(WHITE, 3); chessPiece* wKnight2 = new knight(WHITE, 3);
    board.place(bKnight1, 1, 0); board.place(bKnight2, 6, 0);
    board.place(wKnight1, 1, 7); board.place(wKnight2, 6, 7);
    board.set_blackMaterial(board.get_blackMaterial()+(2*bKnight1->getMaterial()));
    board.set_whiteMaterial(board.get_whiteMaterial()+(2*wKnight1->getMaterial()));

    // 4. Place Bishops
    chessPiece* bBishop1 = new bishop(BLACK, 3); chessPiece* bBishop2 = new bishop(BLACK, 3);
    chessPiece* wBishop1 = new bishop(WHITE, 3); chessPiece* wBishop2 = new bishop(WHITE, 3);
    board.place(bBishop1, 2, 0); board.place(bBishop2, 5, 0);
    board.place(wBishop1, 2, 7); board.place(wBishop2, 5, 7);
    board.set_blackMaterial(board.get_blackMaterial()+(2*bBishop1->getMaterial()));
    board.set_whiteMaterial(board.get_whiteMaterial()+(2*wBishop1->getMaterial()));

    // 5. Place Queens (File x=3)
    chessPiece* bQueen = new queen(BLACK, 9);
    chessPiece* wQueen = new queen(WHITE, 9);
    board.place(bQueen, 3, 0);
    board.place(wQueen, 3, 7);
    board.set_blackMaterial(board.get_blackMaterial()+bQueen->getMaterial());
    board.set_whiteMaterial(board.get_whiteMaterial()+wQueen->getMaterial());

    // 6. Place Kings (File x=4)
    chessPiece* bKing = new king(BLACK, 0);
    chessPiece* wKing = new king(WHITE, 0);
    board.place(bKing, 4, 0);
    board.place(wKing, 4, 7);
}

#include <fstream>
void game::game_replay(std::string filename, sf::RenderWindow& window) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open replay file: " << filename << std::endl;
        return;
    }
    file >> *this;
    file.close();

    setupInitialBoard();

    sf::View view(sf::FloatRect(0.f, 0.f, 1000.f, 1000.f));
    window.setView(view);

    size_t currentIndex = 0;
    
    std::cout << "Starting Replay of " << filename << " with " << moves.size() << " moves." << std::endl;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
                return; 
            }
            if (event.type == sf::Event::Resized) {
                float newWidth = std::max(1000.f, static_cast<float>(event.size.width));
                float newHeight = std::max(1000.f, static_cast<float>(event.size.height));
                window.setSize(sf::Vector2u(newWidth, newHeight));
                float windowRatio = newWidth / newHeight;
                float targetRatio = 1.0f;
                float vpW = 1.0f, vpH = 1.0f, vpX = 0.0f, vpY = 0.0f;
                if (windowRatio > targetRatio) {
                    vpW = targetRatio / windowRatio;
                    vpX = (1.0f - vpW) / 2.0f;
                } else {
                    vpH = windowRatio / targetRatio;
                    vpY = (1.0f - vpH) / 2.0f;
                }
                view.setViewport(sf::FloatRect(vpX, vpY, vpW, vpH));
                window.setView(view);
            }
            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i rawMouse(event.mouseButton.x, event.mouseButton.y);
                    sf::Vector2f mappedMouse = window.mapPixelToCoords(rawMouse);

                    // Check bounds for "Previous Turn" (Center 950, 300, Size 75x75)
                    if (mappedMouse.x >= 912.5f && mappedMouse.x <= 987.5f &&
                        mappedMouse.y >= 262.5f && mappedMouse.y <= 337.5f) {
                        if (currentIndex > 0) {
                            currentIndex--;
                            setupInitialBoard();
                            for (size_t i = 0; i < currentIndex; i++) {
                                board.applyMovement(moves[i]);
                            }
                            std::cout << "Reverted to move " << currentIndex << std::endl;
                        }
                    }
                    
                    // Check bounds for "Next Turn" (Center 950, 400, Size 75x75)
                    if (mappedMouse.x >= 912.5f && mappedMouse.x <= 987.5f &&
                        mappedMouse.y >= 362.5f && mappedMouse.y <= 437.5f) {
                        if (currentIndex < moves.size()) {
                            board.applyMovement(moves[currentIndex]);
                            currentIndex++;
                            std::cout << "Applied move " << currentIndex << " of " << moves.size() << std::endl;
                        }
                    }

                    // Check bounds for "Exit" button (Center 950, 200. Scaled ~80x36.5)
                    if (mappedMouse.x >= 910.0f && mappedMouse.x <= 990.0f &&
                        mappedMouse.y >= 181.7f && mappedMouse.y <= 218.3f) {
                        return; // Drop back to the main menu
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        // Display board from WHITE's perspective as requested
        renderer.replay_renderBoard(window, board, WHITE);
        window.display();
    }
}