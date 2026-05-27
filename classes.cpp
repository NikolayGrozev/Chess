#include <stdexcept>
#include <iostream>
#include <limits.h>

using namespace std;

enum pieceColor{
    WHITE,
    BLACK
};

enum pinDirection{
    NONE,
    diagonalLeft,
    diagonalRight,
    diagonalBottomLeft,
    diagonalBottomRight,
    top,
    bottom,
    left,
    right
};

class Renderer{
    private:
    public: 
        Renderer(){}
        ~Renderer(){}
};

class Inputhandler{
    private:
    public:
        Inputhandler(){}
        ~Inputhandler(){}
};

class Position{
    private:
        int x;
        int y;
    public:
        Position():x(INT_MAX), y(INT_MAX) {}
        Position(int x, int y){
            if(x < 0 || x > 7 || y < 0 || y > 7){
                throw invalid_argument("Position coordinates out of bounds");
            }
            this->x = x;
            this->y = y;
        }
        ~Position(){}
        int get_x()const{
            return x;
        }
        int get_y()const{
            return y;
        }
        
        friend Position * addToArr(int x, int y, Position * arr);
};

class Movement{
    private:
        Position from;
        Position to;
    public:
        Movement():from(0, 0), to(0, 0){}
        Movement(int x1, int y1, int x2, int y2):from(x1, y1), to(x2, y2){}
        ~Movement(){};
        Position get_from()const{
            return from;
        }
        Position get_to()const{
            return to;
        }
};

class chessPiece{
    private:
        pieceColor color;
        bool isPinnedToKing;
        pinDirection d;
    public:
        chessPiece(pieceColor c, bool pinned, pinDirection direction): color(c), isPinnedToKing(pinned), d(direction){};
        virtual ~chessPiece(){};
        virtual Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const = 0;
        virtual void set_hasMoved(bool hm){};
        pieceColor get_PieceColor()const{
            return this->color;
        }
        bool get_isPinned()const{
            return isPinnedToKing;
        }
        void set_isPinned(bool value){
            this->isPinnedToKing = value;
        }
        pinDirection get_pinDirection()const{
            return this->d;
        }

};

Position * addToArr(int x, int y, Position * arr){

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

class pawn: public chessPiece{
    private:
        bool hasMoved;
    public:
        pawn(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction), hasMoved(0){}
        ~pawn(){}
        Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const override {
            //context: when calculating, we assume white is at the bottom of the board, and black is at the top,
            //reversing the numbers depending on piece color (right for white is left for black if we have 2 pieces facing each other)
            //all pinDirections, stored in a piece are from the piece's perspective, assuming you start with a king's pawn, the king is 
            //alligned to the bottom tile of you
            //The returned array of Possitions ends with a Position that has INT_MAX on both X and Y

            Position * validMoves = new Position[1];
            //the last element is the end of arr flag, every new addition is added at the front

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
        bool get_hasMoved()const{
            return hasMoved;
        }
        void set_hasMoved(bool hm)override{
            this->hasMoved = hm;
        }

};

class knight: public chessPiece{
    private:
    public:
        knight(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction){}
        ~knight(){}
        Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const override {
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
};

class bishop: public chessPiece{
    private:
        void getDiagonalMoves(const chessPiece* board[8][8], const Position p, const int x_offset, const int y_offset, Position *& moves) const {
            const int x = p.get_x();
            const int y = p.get_y();

            // Loop starting from 1, multiplying by the offset
            for(int i = 1; ; i++) {
                int targetX = x + (i * x_offset);
                int targetY = y + (i * y_offset);

                // 1. Boundary check FIRST
                if(targetX < 0 || targetX > 7 || targetY < 0 || targetY > 7) {
                    break;
                }

                // 2. Collision check
                if(board[targetY][targetX] != nullptr) {
                    if(board[targetY][targetX]->get_PieceColor() == this->get_PieceColor()) {
                        break; // Blocked by a friendly piece
                    } else {
                        moves = addToArr(targetX, targetY, moves); // Capture enemy
                        break; // Sliding pieces must stop after a capture
                    }
                }

                // 3. Empty square, add and continue sliding
                moves = addToArr(targetX, targetY, moves);
            }
        }
    public:
        bishop(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction){}
        ~bishop(){}
        Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const override {

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
                getDiagonalMoves(board, p, leftDiagonal_x_offset, diagonal_y_offset, validMoves); // diagonalLeft
                getDiagonalMoves(board, p,  rightDiagonal_x_offset,  bottomDiagonal_y_offset, validMoves); // diagonalBottomRight
            }

            if(canMoveDiagRight) {
                getDiagonalMoves(board, p,  rightDiagonal_x_offset, diagonal_y_offset, validMoves); // diagonalRight
                getDiagonalMoves(board, p, leftDiagonal_x_offset,  bottomDiagonal_y_offset, validMoves); // diagonalBottomLeft
            }

                return validMoves;
        }
};

class rook: public chessPiece{
    private:
        bool hasMoved;
    void getLaneMoves(const chessPiece* board[8][8], const Position p, const int x_offset, const int y_offset, Position *& moves) const {
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
    public:
        rook(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction), hasMoved(0){}
        ~rook(){}
        Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const override {

            Position * validMoves = new Position[1];

            const int x = p.get_x();
            const int y = p.get_y();

            const int right_x_offset = (this->get_PieceColor() == WHITE) ? 1:-1;
            const int left_x_offset = (this->get_PieceColor() == WHITE) ? -1:1;
            const int top_y_offset = (this->get_PieceColor() == WHITE) ? -1:1;
            const int bottom_y_offset = (this->get_PieceColor() == WHITE) ? 1:-1;


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
                getLaneMoves(board, p, left_x_offset,  0, validMoves); // Left
                getLaneMoves(board, p,  right_x_offset,  0, validMoves); // Right
            }

            if(canMoveVertical) {
                getLaneMoves(board, p,  0, top_y_offset, validMoves); // Up
                getLaneMoves(board, p,  0,  bottom_y_offset, validMoves); // Down
            }

            return validMoves;
        }
        bool get_hasMoved() const{
            return hasMoved;
        }
        void set_hasMoved(bool hm)override{
            this->hasMoved = hm;
        }
};

class queen: public chessPiece{
    private:
    public:
        queen(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction){}
        ~queen(){}
        Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const override {
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

            // 4. CRITICAL: Prevent Memory Leak
            delete[] rookMoves;
            delete[] bishopMoves;

            return validMoves;
        }

};

class king: public chessPiece{
    private:
        bool hasMoved;
    public:
        king(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction), hasMoved(0){}
        ~king(){}
        Position * get_ValidMoves(const chessPiece* board[8][8], const Position p)const override {

            Position * validMoves = new Position[1];

            int xArr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
            int yArr[8] = {-1, 0, 1, -1, 1, -1, 0, 1};

            const int y = p.get_y();
            const int x = p.get_x();

            for(int i = 0; i < 8; i++){
                if(
                    ((y+yArr[i] > 7 || y+yArr[i] < 0) || (x+xArr[i]) > 7 || x+xArr[i] < 0) ||
                    board[y+yArr[i]][x+xArr[i]] != nullptr && board[y+yArr[i]][x+xArr[i]]->get_PieceColor() == this->get_PieceColor()
                ){
                    continue;
                }

                validMoves = addToArr(x + xArr[i], y + yArr[i], validMoves);
            }

            return validMoves;
        }
        bool get_hasMoved() const{
            return hasMoved;
        }
        void set_hasMoved(bool hm)override{
            this->hasMoved = hm;
        }
};

class ChessBoard{
    private:
        chessPiece * board[8][8];
        int blackMaterial;
        int whiteMaterial;
    public:
        ChessBoard(){
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    this->board[i][j] = nullptr;
                }
            }
            blackMaterial = 0;
            whiteMaterial = 0;
        }
        ~ChessBoard(){
            for(int i = 0; i < 8; i++){
                for(int j = 0; j < 8; j++){
                    if(this->board[i][j] != nullptr){
                        delete this->board[i][j];
                        this->board[i][j] = nullptr;
                    }
                }
            }
        }
        bool isInCheck(pieceColor p)const{

        }
        void validateMovement(Movement m){

        }
        void applyMovement(Movement m){
            
        }

        chessPiece * at(int x, int y)const{

        }
        void place(chessPiece * piece, int x, int y){

        }

        int get_whiteMaterial()const {
            return whiteMaterial;
        }
        int get_blackMaterial()const {
            return blackMaterial;
        }
        //get_chessBoard 
};

class game{
    private:
        ChessBoard board;
        Movement * moves;
        int movesAmount;
        Renderer renderer;
        Inputhandler inputhandler;
        bool isOver;
        pieceColor currentTurn;
        int lastCaptureMove;
        int currentMoveIndex;
    public:
        game():
        movesAmount(0),
        isOver(false),
        lastCaptureMove(0),
        currentMoveIndex(0),
        currentTurn(WHITE),
        renderer(),
        inputhandler(),
        moves(nullptr),
        board()
        {};
        ~game(){
            delete[] this->moves;
        };

        void game_save(){} //saves game to a file, to be replayed
        void game_replay(){} // reads game from a file and replays it, not allowing to move pieces, just past and next positions
        void game_run(){}

        void action_nextPosition(){} // checks the next postion played, doesn't change the game while playing or replaying
        void action_pastPosition(){} // checks the past position played, doesn't change the game while playing or replaying
        void action_select(Position p) {}

        Movement * get_Moves(){
            return this->moves;
        }
        int get_movesAmount(){
            return movesAmount;
        }
};