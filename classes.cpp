#include "classes.h"
#include <stdexcept>
#include <iostream>
#include <limits.h>
#include <vector>
#include <typeinfo>
#include <algorithm>
#include <SFML/Graphics.hpp>
#include <sstream>

using namespace std;

Position::Position() : x(INT_MAX), y(INT_MAX) {}

Position::Position(int x, int y) {
    if(x < 0 || x > 7 || y < 0 || y > 7){
        throw invalid_argument("Position coordinates out of bounds");
    }
    this->x = x;
    this->y = y;
}

Position::~Position() {}

int Position::get_x() const {
    return x;
}

int Position::get_y() const {
    return y;
}

Position * addToArr(int x, int y, Position * arr) {
    int newSize = 2;
    Position * newArr = nullptr;

    for(int i = 0; (arr+i)->get_x() != INT_MAX && (arr+i)->get_y() != INT_MAX; i++){
        newSize++;
    }

    newArr = new Position[newSize];
    newArr[0].x = x;
    newArr[0].y = y;
    newArr[newSize-1].x = INT_MAX;
    newArr[newSize-1].y = INT_MAX;

    for(int i = 1; i < newSize-1; i++){
        newArr[i].x = arr[i-1].x;
        newArr[i].y = arr[i-1].y;
    }

    delete[] arr;

    return newArr;
}

Movement::Movement() : from(0, 0), to(0, 0) {}

Movement::Movement(int x1, int y1, int x2, int y2) : from(x1, y1), to(x2, y2) {}

Movement::~Movement() {}

Position Movement::get_from() const {
    return from;
}

Position Movement::get_to() const {
    return to;
}

chessPiece::chessPiece(pieceColor c, int material) : color(c), material(material) {}

chessPiece::~chessPiece() {}

void chessPiece::set_hasMoved(bool hm) {
    return;
}

pieceColor chessPiece::get_PieceColor() const {
    return this->color;
}

int chessPiece::getMaterial() const{
    return material;
};

pawn::pawn(pieceColor c, int material) : chessPiece(c, material), hasMoved(0) {}

pawn::~pawn() {}

Position * pawn::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    const int y = p.get_y();
    const int x = p.get_x();

    int forward_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    int startRow = (this->get_PieceColor() == WHITE) ? 6 : 1;

    int right_offset = (this->get_PieceColor() == WHITE) ? 1 : -1;
    int left_offset = -1 * right_offset;
    pieceColor enemyColor = (this->get_PieceColor() == WHITE) ? BLACK : WHITE;

    if((y+forward_offset <= 7 && y+forward_offset >= 0)){
        if(board[y+forward_offset][x] == nullptr){
            validMoves = addToArr(x, y+forward_offset, validMoves);

            if(y == startRow && !hasMoved && board[y+(2*forward_offset)][x] == nullptr){
                validMoves = addToArr(x, y+(2*forward_offset), validMoves);
            }
        }

        if(
            (x+left_offset <= 7 && x+left_offset >= 0) &&
            board[y+forward_offset][x+left_offset] != nullptr &&
            board[y+forward_offset][x+left_offset]->get_PieceColor() == enemyColor
        ){
            validMoves = addToArr(x+left_offset, y+forward_offset, validMoves);
        }
        if(
            (x+right_offset <= 7 && x+right_offset >= 0) &&
            board[y+forward_offset][x+right_offset] != nullptr &&
            board[y+forward_offset][x+right_offset]->get_PieceColor() == enemyColor
        ){
            validMoves = addToArr(x+right_offset, y+forward_offset, validMoves);
        }
    }

    return validMoves;
}

bool pawn::get_hasMoved() const {
    return hasMoved;
}

void pawn::set_hasMoved(bool hm) {
    this->hasMoved = hm;
}

knight::knight(pieceColor c, int material) : chessPiece(c, material) {}

knight::~knight() {}

Position * knight::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    const int y = p.get_y();
    const int x = p.get_x();

    int xArr[8] = {2, 2, -2, -2, 1, -1, 1, -1};
    int yArr[8] = {1, -1, 1, -1, 2, 2, -2, -2};
    
    for(int i = 0; i < 8; i++){
        if(
            ((x + xArr[i] < 0 || x + xArr[i] > 7) || (y + yArr[i] < 0 || y + yArr[i] > 7)) ||
            (board[y + yArr[i]][x + xArr[i]] != nullptr && board[y + yArr[i]][x + xArr[i]]->get_PieceColor() == this->get_PieceColor())
        ){
            continue;
        }
        validMoves = addToArr(x + xArr[i], y + yArr[i], validMoves);
    }

    return validMoves;
}

bishop::bishop(pieceColor c, int material) : chessPiece(c, material) {}

bishop::~bishop() {}

void bishop::getDiagonalMoves(const MutableBoardMatrix& board, const Position p, const int x_offset, const int y_offset, Position *& moves) const {
    const int x = p.get_x();
    const int y = p.get_y();

    for(int i = 1; ; i++) {
        int targetX = x + (i * x_offset);
        int targetY = y + (i * y_offset);

        if(targetX < 0 || targetX > 7 || targetY < 0 || targetY > 7) {
            break;
        }

        if(board[targetY][targetX] != nullptr) {
            if(board[targetY][targetX]->get_PieceColor() == this->get_PieceColor()) {
                break; 
            } else {
                moves = addToArr(targetX, targetY, moves); 
                break; 
            }
        }

        moves = addToArr(targetX, targetY, moves);
    }
}

Position * bishop::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    const int x = p.get_x();
    const int y = p.get_y();

    const int leftDiagonal_x_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int rightDiagonal_x_offset = (this->get_PieceColor() == WHITE) ? 1 : -1 ;
    const int diagonal_y_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int bottomDiagonal_y_offset = (this->get_PieceColor() == WHITE) ? 1 : -1 ;

    getDiagonalMoves(board, p, leftDiagonal_x_offset, diagonal_y_offset, validMoves); 
    getDiagonalMoves(board, p,  rightDiagonal_x_offset,  bottomDiagonal_y_offset, validMoves);
    getDiagonalMoves(board, p,  rightDiagonal_x_offset, diagonal_y_offset, validMoves); 
    getDiagonalMoves(board, p, leftDiagonal_x_offset,  bottomDiagonal_y_offset, validMoves); 
    
    return validMoves;
}

rook::rook(pieceColor c, int material) : chessPiece(c, material), hasMoved(0) {}

rook::~rook() {}

void rook::getLaneMoves(const MutableBoardMatrix& board, const Position p, const int x_offset, const int y_offset, Position *& moves) const {
    const int x = p.get_x();
    const int y = p.get_y();
    pieceColor myColor = this->get_PieceColor();

    for(int i = 1; ; i++) {
        int targetX = x + (i * x_offset);
        int targetY = y + (i * y_offset);

        if(targetX < 0 || targetX > 7 || targetY < 0 || targetY > 7) {
            break;
        }

        if(board[targetY][targetX] != nullptr) {
            if(board[targetY][targetX]->get_PieceColor() == myColor) {
                break; 
            } else {
                moves = addToArr(targetX, targetY, moves); 
                break; 
            }
        }

        moves = addToArr(targetX, targetY, moves);
    }
}

Position * rook::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    const int x = p.get_x();
    const int y = p.get_y();

    const int right_x_offset = (this->get_PieceColor() == WHITE) ? 1 : -1;
    const int left_x_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int top_y_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int bottom_y_offset = (this->get_PieceColor() == WHITE) ? 1 : -1;

    getLaneMoves(board, p, left_x_offset,  0, validMoves); 
    getLaneMoves(board, p,  right_x_offset,  0, validMoves); 
    getLaneMoves(board, p,  0, top_y_offset, validMoves); 
    getLaneMoves(board, p,  0,  bottom_y_offset, validMoves); 

    return validMoves;
}

bool rook::get_hasMoved() const {
    return hasMoved;
}

void rook::set_hasMoved(bool hm) {
    this->hasMoved = hm;
}

queen::queen(pieceColor c, int material) : chessPiece(c, material) {}

queen::~queen() {}

Position * queen::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    rook r(this->get_PieceColor(), 5);
    bishop b(this->get_PieceColor(), 3);

    Position * rookMoves = r.get_ValidMoves(board, p);
    Position * bishopMoves = b.get_ValidMoves(board, p);

    for(int i = 0; rookMoves[i].get_x() != INT_MAX && rookMoves[i].get_y() != INT_MAX; i++){
        validMoves = addToArr(rookMoves[i].get_x(), rookMoves[i].get_y(), validMoves);
    }
    for(int i = 0; bishopMoves[i].get_x() != INT_MAX && bishopMoves[i].get_y() != INT_MAX; i++){
        validMoves = addToArr(bishopMoves[i].get_x(), bishopMoves[i].get_y(), validMoves);
    }

    delete[] rookMoves;
    delete[] bishopMoves;

    return validMoves;
}

king::king(pieceColor c, int material) : chessPiece(c, 0), hasMoved(0) {}

king::~king() {}

Position * king::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    int xArr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int yArr[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

    const int y = p.get_y();
    const int x = p.get_x();

    for(int i = 0; i < 8; i++){
        if(
            ((y+yArr[i] > 7 || y+yArr[i] < 0) || (x+xArr[i]) > 7 || x+xArr[i] < 0) ||
            (board[y+yArr[i]][x+xArr[i]] != nullptr && board[y+yArr[i]][x+xArr[i]]->get_PieceColor() == this->get_PieceColor())
        ){
            continue;
        }

        validMoves = addToArr(x + xArr[i], y + yArr[i], validMoves);
    }

    return validMoves;
}

bool king::get_hasMoved() const {
    return hasMoved;
}

void king::set_hasMoved(bool hm) {
    this->hasMoved = hm;
}

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
        throw runtime_error("tried to place a piece on an occupied tile");
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

// =========================================================================
// RENDERER IMPLEMENTATION
// =========================================================================
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
}

// =========================================================================
// GAME CONTROL LOGIC
// =========================================================================
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

vector<Movement> game::get_Moves() {
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

void game::game_run() {
    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess Match - Active Gameplay");
    
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

    while (window.isOpen() && !this->isOver) {

        // =====================================================================
        // ACTIVE TURN STATE VALIDATION SCANNER (CHECKMATE / STALEMATE)
        // =====================================================================
        if (board.hasNoValidMoves(this->currentTurn)) {
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
            break; // Drop out of rendering frames immediately
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
                        break; // End the game
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
void game::game_replay(std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open replay file: " << filename << std::endl;
        return;
    }
    file >> *this;
    file.close();

    setupInitialBoard();

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess Match - Replay");
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
                }
            }
        }

        window.clear(sf::Color::Black);
        // Display board from WHITE's perspective as requested
        renderer.replay_renderBoard(window, board, WHITE);
        window.display();
    }
}

