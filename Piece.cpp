#include "Piece.h"

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