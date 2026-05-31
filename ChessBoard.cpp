#include "ChessBoard.h"
#include <stdexcept>
#include <iostream>
#include <vector>

ChessBoard::ChessBoard() : enPassantTarget() { // Initializes to INT_MAX by default
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            this->board[i][j] = nullptr;
        }
    }
    blackMaterial = 0;
    whiteMaterial = 0;
}

Position ChessBoard::getEnPassantTarget() const { return enPassantTarget; }
void ChessBoard::setEnPassantTarget(Position p) { enPassantTarget = p; }
void ChessBoard::clearEnPassantTarget() { enPassantTarget = Position(); }

void ChessBoard::clearBoard() {
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(this->board[i][j] != nullptr){
                delete this->board[i][j];
                this->board[i][j] = nullptr;
            }
        }
    }
    this->whiteMaterial = 0;
    this->blackMaterial = 0;
    this->clearEnPassantTarget();
}

ChessBoard::~ChessBoard() {
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            if(this->board[i][j] != nullptr){
                delete this->board[i][j];
                this->board[i][j] = nullptr;
            }
        }
    }
}

MutableBoardMatrix ChessBoard:: getBoard() const{
    return board;
}

bool ChessBoard::isInCheck(pieceColor p, const MutableBoardMatrix& matrix) const {
    int kingX = -1, kingY = -1;
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const chessPiece* piece = matrix[y][x];
            if (piece != nullptr && piece->get_PieceColor() == p) {
                if (typeid(*piece) == typeid(king)) {
                    kingX = x;
                    kingY = y;
                    break;
                }
            }
        }
        if (kingX != -1) break;
    }

    if (kingX == -1 || kingY == -1) {
        return false;
    }

    Position kingPos(kingX, kingY);
    pieceColor enemyColor = (p == WHITE) ? BLACK : WHITE;

    rook dummyRook(p, 5);
    Position* rookMoves = dummyRook.get_ValidMoves(matrix, kingPos);
    for (int i = 0; rookMoves[i].get_x() != INT_MAX && rookMoves[i].get_y() != INT_MAX; i++) {
        int tx = rookMoves[i].get_x();
        int ty = rookMoves[i].get_y();
        const chessPiece* enemy = matrix[ty][tx];
        if (enemy != nullptr && enemy->get_PieceColor() == enemyColor) {
            if (typeid(*enemy) == typeid(rook) || typeid(*enemy) == typeid(queen)) {
                delete[] rookMoves;
                return true;
            }
        }
    }
    delete[] rookMoves;

    bishop dummyBishop(p, 3);
    Position* bishopMoves = dummyBishop.get_ValidMoves(matrix, kingPos);
    for (int i = 0; bishopMoves[i].get_x() != INT_MAX && bishopMoves[i].get_y() != INT_MAX; i++) {
        int tx = bishopMoves[i].get_x();
        int ty = bishopMoves[i].get_y();
        const chessPiece* enemy = matrix[ty][tx];
        if (enemy != nullptr && enemy->get_PieceColor() == enemyColor) {
            if (typeid(*enemy) == typeid(bishop) || typeid(*enemy) == typeid(queen)) {
                delete[] bishopMoves;
                return true;
            }
        }
    }
    delete[] bishopMoves;

    knight dummyKnight(p, 3);
    Position* knightMoves = dummyKnight.get_ValidMoves(matrix, kingPos);
    for (int i = 0; knightMoves[i].get_x() != INT_MAX && knightMoves[i].get_y() != INT_MAX; i++) {
        int tx = knightMoves[i].get_x();
        int ty = knightMoves[i].get_y();
        const chessPiece* enemy = matrix[ty][tx];
        if (enemy != nullptr && enemy->get_PieceColor() == enemyColor) {
            if (typeid(*enemy) == typeid(knight)) {
                delete[] knightMoves;
                return true;
            }
        }
    }
    delete[] knightMoves;

    pawn dummyPawn(p, 1);
    Position* pawnMoves = dummyPawn.get_ValidMoves(matrix, kingPos);
    for (int i = 0; pawnMoves[i].get_x() != INT_MAX && pawnMoves[i].get_y() != INT_MAX; i++) {
        int tx = pawnMoves[i].get_x();
        int ty = pawnMoves[i].get_y();
        const chessPiece* enemy = matrix[ty][tx];
        if (enemy != nullptr && enemy->get_PieceColor() == enemyColor) {
            if (typeid(*enemy) == typeid(pawn)) {
                delete[] pawnMoves;
                return true;
            }
        }
    }
    delete[] pawnMoves;

    king dummyKing(p, 0);
    Position* kingMoves = dummyKing.get_ValidMoves(matrix, kingPos);
    for (int i = 0; kingMoves[i].get_x() != INT_MAX && kingMoves[i].get_y() != INT_MAX; i++) {
        int tx = kingMoves[i].get_x();
        int ty = kingMoves[i].get_y();
        const chessPiece* enemy = matrix[ty][tx];
        if (enemy != nullptr && enemy->get_PieceColor() == enemyColor) {
            if (typeid(*enemy) == typeid(king)) {
                delete[] kingMoves;
                return true;
            }
        }
    }
    delete[] kingMoves;

    return false;
}

Position* ChessBoard::getStrictlyLegalMoves(Position fromPos) const {
    const chessPiece* movingPiece = this->board[fromPos.get_y()][fromPos.get_x()];
    
    if (movingPiece == nullptr) {
        Position* emptyArr = new Position[1]; 
        return emptyArr; 
    }

    MutableBoardMatrix baseMatrix = this->getBoard();
    Position* pseudoMoves = movingPiece->get_ValidMoves(baseMatrix, fromPos);

    

    // =========================================================================
    // EN PASSANT INJECTION (Add the coordinate if geometrically valid)
    // =========================================================================
    if (dynamic_cast<const pawn*>(movingPiece) != nullptr && enPassantTarget.get_x() != INT_MAX) {
        int currX = fromPos.get_x();
        int currY = fromPos.get_y();
        int dir = (movingPiece->get_PieceColor() == WHITE) ? -1 : 1;
        
        // If the target is diagonally forward-left or forward-right
        if (enPassantTarget.get_y() == currY + dir && std::abs(enPassantTarget.get_x() - currX) == 1) {
            pseudoMoves = addToArr(enPassantTarget.get_x(), enPassantTarget.get_y(), pseudoMoves);
        }
    }

    // =========================================================================
    // CASTLING INJECTION (Rule 1, 2, and 3)
    // =========================================================================
    const king* kPiece = dynamic_cast<const king*>(movingPiece);
    if (kPiece != nullptr && !kPiece->get_hasMoved()) {
        pieceColor myColor = kPiece->get_PieceColor();
        int y = fromPos.get_y(); // Y=7 for White, Y=0 for Black
        
        // Rule: You cannot castle out of check!
        if (!this->isInCheck(myColor, baseMatrix)) {
            
            // KINGSIDE CASTLE (Right side)
            const rook* rightRook = dynamic_cast<const rook*>(this->board[y][7]);
            if (rightRook != nullptr && !rightRook->get_hasMoved()) {
                // Are the two tiles between them empty?
                if (this->board[y][5] == nullptr && this->board[y][6] == nullptr) {
                    
                    // Simulate King stepping to x=5 and x=6 to check for threats
                    MutableBoardMatrix sim1 = baseMatrix; sim1[y][5] = sim1[y][4]; sim1[y][4] = nullptr;
                    MutableBoardMatrix sim2 = baseMatrix; sim2[y][6] = sim2[y][4]; sim2[y][4] = nullptr;
                    
                    if (!this->isInCheck(myColor, sim1) && !this->isInCheck(myColor, sim2)) {
                        pseudoMoves = addToArr(6, y, pseudoMoves); // Add Kingside Castle destination!
                    }
                }
            }

            // QUEENSIDE CASTLE (Left side)
            const rook* leftRook = dynamic_cast<const rook*>(this->board[y][0]);
            if (leftRook != nullptr && !leftRook->get_hasMoved()) {
                // Are the three tiles between them empty?
                if (this->board[y][1] == nullptr && this->board[y][2] == nullptr && this->board[y][3] == nullptr) {
                    
                    // Simulate King stepping to x=3 and x=2 (Tile x=1 does not need check-validation, only emptiness)
                    MutableBoardMatrix sim1 = baseMatrix; sim1[y][3] = sim1[y][4]; sim1[y][4] = nullptr;
                    MutableBoardMatrix sim2 = baseMatrix; sim2[y][2] = sim2[y][4]; sim2[y][4] = nullptr;
                    
                    if (!this->isInCheck(myColor, sim1) && !this->isInCheck(myColor, sim2)) {
                        pseudoMoves = addToArr(2, y, pseudoMoves); // Add Queenside Castle destination!
                    }
                }
            }
        }
    }

    int pseudoCount = 0;
    while (pseudoMoves[pseudoCount].get_x() != INT_MAX) {
        pseudoCount++;
    }

    std::vector<Position> safeMoves;
    safeMoves.reserve(32); 
    pieceColor myColor = movingPiece->get_PieceColor();

    for (int i = 0; i < pseudoCount; i++) {
        int toX = pseudoMoves[i].get_x();
        int toY = pseudoMoves[i].get_y();

        MutableBoardMatrix simMatrix = baseMatrix;
        
        simMatrix[toY][toX] = simMatrix[fromPos.get_y()][fromPos.get_x()];
        simMatrix[fromPos.get_y()][fromPos.get_x()] = nullptr;

        // =========================================================================
        // EN PASSANT SIMULATION FIX (Remove the phantom enemy piece)
        // =========================================================================
        if (dynamic_cast<const pawn*>(movingPiece) != nullptr && toX == enPassantTarget.get_x() && toY == enPassantTarget.get_y()) {
            simMatrix[fromPos.get_y()][toX] = nullptr; 
        }

        if (!this->isInCheck(myColor, simMatrix)) {
            safeMoves.push_back(Position(toX, toY));
        }
    }

    delete[] pseudoMoves;

    Position* strictlyLegalArr = new Position[safeMoves.size() + 1];
    for (size_t i = 0; i < safeMoves.size(); i++) {
        strictlyLegalArr[i] = safeMoves[i];
    }
    strictlyLegalArr[safeMoves.size()] = Position(); 

    return strictlyLegalArr;
}

bool ChessBoard:: hasNoValidMoves(pieceColor color) const{
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            const chessPiece* piece = this->board[y][x];
            
            // Look for pieces matching the requested color
            if (piece != nullptr && piece->get_PieceColor() == color) {
                Position currentPos(x, y);
                Position* legalMoves = this->getStrictlyLegalMoves(currentPos);
                
                if (legalMoves[0].get_x() != INT_MAX) {
                    delete[] legalMoves; 
                    return false; 
                }
                
                delete[] legalMoves;
            }
        }
    }

    return true;
};

void ChessBoard::applyMovement(Movement m) {
    int fromX = m.get_from().get_x();
    int fromY = m.get_from().get_y();
    int toX   = m.get_to().get_x();
    int toY   = m.get_to().get_y();

    chessPiece* movingPiece = this->board[fromY][fromX];
    if (movingPiece == nullptr) throw std::invalid_argument("No piece found at coordinates.");

    // =========================================================================
    // CASTLING EXECUTION (Move the Rook!)
    // =========================================================================
    if (dynamic_cast<king*>(movingPiece) != nullptr && std::abs(toX - fromX) == 2) {
        if (toX > fromX) { 
            // Kingside: Move Rook from x=7 to x=5
            chessPiece* rook = this->board[fromY][7];
            this->board[fromY][5] = rook;
            this->board[fromY][7] = nullptr;
            rook->set_hasMoved(true);
        } else { 
            // Queenside: Move Rook from x=0 to x=3
            chessPiece* rook = this->board[fromY][0];
            this->board[fromY][3] = rook;
            this->board[fromY][0] = nullptr;
            rook->set_hasMoved(true);
        }
    }

    // EN PASSANT CAPTURE DELETION
    if (dynamic_cast<pawn*>(movingPiece) != nullptr && toX == enPassantTarget.get_x() && toY == enPassantTarget.get_y()) {
        chessPiece* epVictim = this->board[fromY][toX]; 
        if (epVictim != nullptr) {
            // Deduct Material
            if (epVictim->get_PieceColor() == WHITE) this->whiteMaterial -= epVictim->getMaterial();
            else this->blackMaterial -= epVictim->getMaterial();

            delete epVictim;
            this->board[fromY][toX] = nullptr;
        }
    }

    // STANDARD CAPTURE DELETION
    chessPiece* targetPiece = this->board[toY][toX];
    if (targetPiece != nullptr) {
        // Deduct Material
        if (targetPiece->get_PieceColor() == WHITE) this->whiteMaterial -= targetPiece->getMaterial();
        else this->blackMaterial -= targetPiece->getMaterial();

        delete targetPiece; 
    }

    this->board[toY][toX] = movingPiece;
    this->board[fromY][fromX] = nullptr;
    movingPiece->set_hasMoved(true);
}

chessPiece * ChessBoard::at(int x, int y) const {
    return board[y][x];
}

void ChessBoard::place(chessPiece *& piece, int x, int y) {
    if(board[y][x] != nullptr){
        throw std::runtime_error("tried to place a piece on an occupied tile");
    }
    board[y][x] = piece;
}

int ChessBoard::get_whiteMaterial() const {
    return whiteMaterial;
}

int ChessBoard::get_blackMaterial() const {
    return blackMaterial;
}

void ChessBoard:: set_whiteMaterial(int m){
    this->whiteMaterial = m;
};
void ChessBoard:: set_blackMaterial(int m){
    this->blackMaterial = m;
};