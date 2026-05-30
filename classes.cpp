#include "classes.h"
#include <stdexcept>
#include <iostream>
#include <limits.h>
#include <vector>
#include <typeinfo>

using namespace std;
Renderer::Renderer() {}
Renderer::~Renderer() {}
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

chessPiece::chessPiece(pieceColor c) : color(c) {}

chessPiece::~chessPiece() {}

void chessPiece::set_hasMoved(bool hm) {}

pieceColor chessPiece::get_PieceColor() const {
    return this->color;
}

pawn::pawn(pieceColor c) : chessPiece(c), hasMoved(0) {}

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

knight::knight(pieceColor c) : chessPiece(c) {}

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

bishop::bishop(pieceColor c) : chessPiece(c) {}

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

rook::rook(pieceColor c) : chessPiece(c), hasMoved(0) {}

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

queen::queen(pieceColor c) : chessPiece(c) {}

queen::~queen() {}

Position * queen::get_ValidMoves(const MutableBoardMatrix& board, const Position p) const {
    Position * validMoves = new Position[1];

    rook r(this->get_PieceColor());
    bishop b(this->get_PieceColor());

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

king::king(pieceColor c) : chessPiece(c), hasMoved(0) {}

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

ChessBoard::ChessBoard() {
    for(int i = 0; i < 8; i++){
        for(int j = 0; j < 8; j++){
            this->board[i][j] = nullptr;
        }
    }
    blackMaterial = 0;
    whiteMaterial = 0;
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
    // 1. Locate the King of the specified color 'p' on the PROVIDED matrix
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

    // 2. Check Rook / Queen threats
    rook dummyRook(p);
    Position* rookMoves = dummyRook.get_ValidMoves(matrix, kingPos);
    for (int i = 0; rookMoves[i].get_x() != INT_MAX && rookMoves[i].get_y() != INT_MAX; i++) {
        int tx = rookMoves[i].get_x();
        int ty = rookMoves[i].get_y();
        const chessPiece* enemy = matrix[ty][tx]; // Safely reading from the provided matrix
        if (enemy != nullptr && enemy->get_PieceColor() == enemyColor) {
            if (typeid(*enemy) == typeid(rook) || typeid(*enemy) == typeid(queen)) {
                delete[] rookMoves;
                return true;
            }
        }
    }
    delete[] rookMoves;

    // 3. Check Bishop / Queen threats
    bishop dummyBishop(p);
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

    // 4. Check Knight threats
    knight dummyKnight(p);
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

    // 5. Check Pawn threats
    pawn dummyPawn(p);
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

    // 6. Check King threats
    king dummyKing(p);
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

    // 1. Grab the RAW pseudo-legal moves using the baseline matrix
    MutableBoardMatrix baseMatrix = this->getBoard();
    Position* pseudoMoves = movingPiece->get_ValidMoves(baseMatrix, fromPos);

    int pseudoCount = 0;
    while (pseudoMoves[pseudoCount].get_x() != INT_MAX) {
        pseudoCount++;
    }

    std::vector<Position> safeMoves;
    safeMoves.reserve(32); // Pre-allocate to prevent heap reallocations
    pieceColor myColor = movingPiece->get_PieceColor();

    // 2. SIMULATION FILTER
    for (int i = 0; i < pseudoCount; i++) {
        int toX = pseudoMoves[i].get_x();
        int toY = pseudoMoves[i].get_y();

        // Copy the board matrix purely for this single simulation (instant copy)
        MutableBoardMatrix simMatrix = baseMatrix;
        
        // Execute the move ON THE COPY ONLY
        simMatrix[toY][toX] = simMatrix[fromPos.get_y()][fromPos.get_x()];
        simMatrix[fromPos.get_y()][fromPos.get_x()] = nullptr;

        // Verify King Safety against the simulated board state
        if (!this->isInCheck(myColor, simMatrix)) {
            safeMoves.push_back(Position(toX, toY));
        }
    }

    delete[] pseudoMoves;

    // 3. Package results into your standard array format
    Position* strictlyLegalArr = new Position[safeMoves.size() + 1];
    for (size_t i = 0; i < safeMoves.size(); i++) {
        strictlyLegalArr[i] = safeMoves[i];
    }
    strictlyLegalArr[safeMoves.size()] = Position(); 

    return strictlyLegalArr;
}

void ChessBoard::applyMovement(Movement m) {
    int fromX = m.get_from().get_x();
    int fromY = m.get_from().get_y();
    int toX   = m.get_to().get_x();
    int toY   = m.get_to().get_y();

    chessPiece* movingPiece = this->board[fromY][fromX];

    if (movingPiece == nullptr) {
        throw std::invalid_argument("Cannot apply movement: No piece found at the starting coordinates.");
    }

    chessPiece* targetPiece = this->board[toY][toX];

    if (targetPiece != nullptr) {
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

game::game() :
    isOver(false),
    lastCaptureMove(0),
    currentMoveIndex(0),
    currentTurn(WHITE),
    board()
{}

game::~game() {}

void game::game_save() {}

void game::game_replay() {}

void game::game_run() {
    
}

vector<Movement> game::get_Moves() {
    return this->moves;
}