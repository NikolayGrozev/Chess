#include "classes.h"
#include <stdexcept>
#include <iostream>
#include <limits.h>
#include <vector>

using namespace std;

Renderer::Renderer() {}
Renderer::~Renderer() {}

Inputhandler::Inputhandler() {}
Inputhandler::~Inputhandler() {}

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
    int newSize = 1;
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

chessPiece::chessPiece(pieceColor c, bool pinned, pinDirection direction) 
    : color(c), isPinnedToKing(pinned), d(direction) {}

chessPiece::~chessPiece() {}

void chessPiece::set_hasMoved(bool hm) {}

pieceColor chessPiece::get_PieceColor() const {
    return this->color;
}

bool chessPiece::get_isPinned() const {
    return isPinnedToKing;
}

void chessPiece::set_isPinned(bool value) {
    this->isPinnedToKing = value;
}

pinDirection chessPiece::get_pinDirection() const {
    return this->d;
}

pawn::pawn(pieceColor c, bool pinned, pinDirection direction) 
    : chessPiece(c, pinned, direction), hasMoved(0) {}

pawn::~pawn() {}

Position * pawn::get_ValidMoves(const chessPiece* board[8][8], const Position p) const {
    Position * validMoves = new Position[1];

    const int y = p.get_y();
    const int x = p.get_x();

    int forward_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    int startRow = (this->get_PieceColor() == WHITE) ? 6 : 1;

    int right_offset = (this->get_PieceColor() == WHITE) ? 1 : -1;
    int left_offset = -1 * right_offset;
    pieceColor enemyColor = (this->get_PieceColor() == WHITE) ? BLACK : WHITE;

    bool canMoveForward = (
        this->get_pinDirection() == pinDirection::top ||
        this->get_pinDirection() == pinDirection::bottom ||
        this->get_pinDirection() == pinDirection::NONE
    ) ? true : false;

    bool canAttackLeft = (
        this->get_pinDirection() == pinDirection::diagonalLeft ||
        this->get_pinDirection() == pinDirection::diagonalBottomRight ||
        this->get_pinDirection() == pinDirection::NONE
    ) ? true : false; 

    bool canAttackRight = (
        this->get_pinDirection() == pinDirection::diagonalRight ||
        this->get_pinDirection() == pinDirection::diagonalBottomLeft ||
        this->get_pinDirection() == pinDirection::NONE
    ) ? true : false; 

    if(canMoveForward && (y+forward_offset <= 7 && y+forward_offset >= 0)){
        if(board[y+forward_offset][x] == nullptr){
            validMoves = addToArr(x, y+forward_offset, validMoves);

            if(y == startRow && board[y+(2*forward_offset)][x] == nullptr){
                validMoves = addToArr(x, y+(2*forward_offset), validMoves);
            }
        }
    }

    if(canAttackLeft && (y+forward_offset <= 7 && y+forward_offset >= 0)){
        if(
            (x+left_offset <= 7 && x+left_offset >= 0) &&
            board[y+forward_offset][x+left_offset] != nullptr &&
            board[y+forward_offset][x+left_offset]->get_PieceColor() == enemyColor
        ){
            validMoves = addToArr(x+left_offset, y+forward_offset, validMoves);
        }
    }

    if(canAttackRight && (y+forward_offset <= 7 && y+forward_offset >= 0)){
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

knight::knight(pieceColor c, bool pinned, pinDirection direction) 
    : chessPiece(c, pinned, direction) {}

knight::~knight() {}

Position * knight::get_ValidMoves(const chessPiece* board[8][8], const Position p) const {
    Position * validMoves = new Position[1];

    if(this->get_isPinned()){
        return validMoves;
    }

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

bishop::bishop(pieceColor c, bool pinned, pinDirection direction) 
    : chessPiece(c, pinned, direction) {}

bishop::~bishop() {}

void bishop::getDiagonalMoves(const chessPiece* board[8][8], const Position p, const int x_offset, const int y_offset, Position *& moves) const {
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

Position * bishop::get_ValidMoves(const chessPiece* board[8][8], const Position p) const {
    Position * validMoves = new Position[1];

    const int x = p.get_x();
    const int y = p.get_y();

    const int leftDiagonal_x_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int rightDiagonal_x_offset = (this->get_PieceColor() == WHITE) ? 1 : -1 ;
    const int diagonal_y_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int bottomDiagonal_y_offset = (this->get_PieceColor() == WHITE) ? 1 : -1 ;

    if(
        this->get_pinDirection() == pinDirection::top ||
        this->get_pinDirection() == pinDirection::bottom ||
        this->get_pinDirection() == pinDirection::left ||
        this->get_pinDirection() == pinDirection::right
    ){
        return validMoves;
    }

    bool canMoveDiagLeft = (
        this->get_pinDirection() == pinDirection::NONE ||
        this->get_pinDirection() == pinDirection::diagonalLeft ||
        this->get_pinDirection() == pinDirection::diagonalBottomRight
    ) ? true : false;
    bool canMoveDiagRight = (
        this->get_pinDirection() == pinDirection::NONE ||
        this->get_pinDirection() == pinDirection::diagonalRight ||
        this->get_pinDirection() == pinDirection::diagonalBottomLeft
    ) ? true : false;

    if(canMoveDiagLeft) {
        getDiagonalMoves(board, p, leftDiagonal_x_offset, diagonal_y_offset, validMoves); 
        getDiagonalMoves(board, p,  rightDiagonal_x_offset,  bottomDiagonal_y_offset, validMoves); 
    }

    if(canMoveDiagRight) {
        getDiagonalMoves(board, p,  rightDiagonal_x_offset, diagonal_y_offset, validMoves); 
        getDiagonalMoves(board, p, leftDiagonal_x_offset,  bottomDiagonal_y_offset, validMoves); 
    }

    return validMoves;
}

rook::rook(pieceColor c, bool pinned, pinDirection direction) 
    : chessPiece(c, pinned, direction), hasMoved(0) {}

rook::~rook() {}

void rook::getLaneMoves(const chessPiece* board[8][8], const Position p, const int x_offset, const int y_offset, Position *& moves) const {
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

Position * rook::get_ValidMoves(const chessPiece* board[8][8], const Position p) const {
    Position * validMoves = new Position[1];

    const int x = p.get_x();
    const int y = p.get_y();

    const int right_x_offset = (this->get_PieceColor() == WHITE) ? 1 : -1;
    const int left_x_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int top_y_offset = (this->get_PieceColor() == WHITE) ? -1 : 1;
    const int bottom_y_offset = (this->get_PieceColor() == WHITE) ? 1 : -1;

    if(this->get_isPinned() && (
        this->get_pinDirection() == pinDirection::diagonalLeft ||
        this->get_pinDirection() == pinDirection::diagonalRight||
        this->get_pinDirection() == pinDirection::diagonalBottomLeft ||
        this->get_pinDirection() == pinDirection::diagonalBottomRight
    )){
        return validMoves;
    }

    bool canMoveHorizontal = (
        this->get_pinDirection() == pinDirection::NONE ||
        this->get_pinDirection() == pinDirection::left ||
        this->get_pinDirection() == pinDirection::right
    )? true: false;
    bool canMoveVertical = (
        this->get_pinDirection() == pinDirection::NONE ||
        this->get_pinDirection() == pinDirection::top ||
        this->get_pinDirection() == pinDirection::bottom
    )? true: false ;

    if(canMoveHorizontal) {
        getLaneMoves(board, p, left_x_offset,  0, validMoves); 
        getLaneMoves(board, p,  right_x_offset,  0, validMoves); 
    }

    if(canMoveVertical) {
        getLaneMoves(board, p,  0, top_y_offset, validMoves); 
        getLaneMoves(board, p,  0,  bottom_y_offset, validMoves); 
    }

    return validMoves;
}

bool rook::get_hasMoved() const {
    return hasMoved;
}

void rook::set_hasMoved(bool hm) {
    this->hasMoved = hm;
}

queen::queen(pieceColor c, bool pinned, pinDirection direction) 
    : chessPiece(c, pinned, direction) {}

queen::~queen() {}

Position * queen::get_ValidMoves(const chessPiece* board[8][8], const Position p) const {
    Position * validMoves = new Position[1];

    rook r(this->get_PieceColor(), this->get_isPinned(), this->get_pinDirection());
    bishop b(this->get_PieceColor(), this->get_isPinned(), this->get_pinDirection());

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

king::king(pieceColor c, bool pinned, pinDirection direction) 
    : chessPiece(c, pinned, direction), hasMoved(0) {}

king::~king() {}

Position * king::get_ValidMoves(const chessPiece* board[8][8], const Position p) const {
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

bool ChessBoard::isInCheck(pieceColor p) const {
    return false; 
}

void ChessBoard::validateMovement(Movement m) {}

void ChessBoard::applyMovement(Movement m) {}

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
    renderer(),
    inputhandler(),
    board()
{}

game::~game() {}

void game::game_save() {}

void game::game_replay() {}

void game::game_run() {}

void game::action_nextPosition() {}

void game::action_pastPosition() {}

void game::action_select(Position p) {}

vector<Movement> game::get_Moves() {
    return this->moves;
}