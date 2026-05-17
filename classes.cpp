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
        Position(int x, int y)
        {
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
        virtual Position * get_ValidMoves(chessPiece* board[8][8], Position p)const = 0;
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
        newArr[i].x = arr[i].x;
        newArr[i].y = arr[i].y;
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
        Position * get_ValidMoves(chessPiece* board[8][8], Position p)const override {
            //context: when calculating, we assume white is at the bottom of the board, and black is at the top,
            //reversing the numbers depending on piece color (right for white is left for black if we have 2 pieces facing each other)
            //all pinDirections, stored in a piece are from the piece's perspective, assuming you start with a king's pawn, the king is 
            //alligned to the bottom tile of you
            //The returned array of Possitions ends with a Position that has INT_MAX on both X and Y

            Position * validMoves = new Position[1];
            //the last element is the end of arr flag, every new addition is added at the front

            if(this->get_PieceColor() == WHITE){
                //starting pawns are on 6, they path to 0
                if(this->get_isPinned()){
                    if(
                        this->get_pinDirection() == pinDirection::left ||
                        this->get_pinDirection() == pinDirection::right ||
                        this->get_pinDirection() == pinDirection::diagonalBottomLeft ||
                        this->get_pinDirection() == pinDirection::diagonalBottomRight
                    ){
                        return validMoves;
                    }

                    if(
                        this->get_pinDirection() == pinDirection::diagonalLeft &&
                        board[p.get_y()-1][p.get_x()-1] != nullptr &&
                        board[p.get_y()-1][p.get_x()-1]->get_PieceColor() == BLACK
                    ){
                        validMoves = addToArr(p.get_x()-1, p.get_y()-1, validMoves);
                        return validMoves;
                    }

                    if(
                        this->get_pinDirection() == pinDirection::diagonalRight &&
                        board[p.get_y()-1][p.get_x()+1] != nullptr &&
                        board[p.get_y()-1][p.get_x()+1]->get_PieceColor() == BLACK
                    ){
                        validMoves = addToArr(p.get_x()+1, p.get_y()-1, validMoves);
                        return validMoves;
                    }
                }

                if(board[p.get_y()-1][p.get_x()] == nullptr){
                    validMoves = addToArr(p.get_x(), p.get_y()-1, validMoves);
                }

                if(
                    p.get_y() == 6 &&
                    !this->get_hasMoved() &&
                    board[5][p.get_x()] == nullptr &&
                    board[4][p.get_x()] == nullptr
                ){
                    validMoves = addToArr(p.get_x(), 4, validMoves);
                }

                if(
                    p.get_x() > 0 &&
                    board[p.get_y()-1][p.get_x()-1] != nullptr &&
                    board[p.get_y()-1][p.get_x()-1]->get_PieceColor() == BLACK &&
                    this->get_pinDirection() == NONE
                ){
                    //this->get_pinDirection() == NONE is needed here, because top and bottom are the only directions not checked
                    validMoves = addToArr(p.get_x()-1, p.get_y()-1, validMoves);
                }

                if(
                    p.get_x() < 7 &&
                    board[p.get_y()-1][p.get_x()+1] != nullptr &&
                    board[p.get_y()-1][p.get_x()+1]->get_PieceColor() == BLACK &&
                    this->get_pinDirection() == NONE
                ){
                    //this->get_pinDirection() == NONE is needed here, because top and bottom are the only directions not checked
                    //moving sideways is not allowed when pinned on a line of X
                    validMoves = addToArr(p.get_x()+1, p.get_y()-1, validMoves);
                }
                

            }else if(this->get_PieceColor() == BLACK){
                //starting pawns are on 1, they path to 7

                if(this->get_isPinned()){
                    if(
                        this->get_pinDirection() == pinDirection::left ||
                        this->get_pinDirection() == pinDirection::right ||
                        this->get_pinDirection() == pinDirection::diagonalBottomLeft ||
                        this->get_pinDirection() == pinDirection::diagonalBottomRight
                    ){
                        return validMoves;
                    }

                    if(
                        this->get_pinDirection() == pinDirection::diagonalLeft &&
                        board[p.get_y()+1][p.get_x()+1] != nullptr &&
                        board[p.get_y()+1][p.get_x()+1]->get_PieceColor() == WHITE
                    ){
                        validMoves = addToArr(p.get_x()+1, p.get_y()+1, validMoves);
                        return validMoves;
                    }

                    if(
                        this->get_pinDirection() == pinDirection::diagonalRight &&
                        board[p.get_y()+1][p.get_x()-1] != nullptr &&
                        board[p.get_y()+1][p.get_x()-1]->get_PieceColor() == WHITE
                    ){
                        validMoves = addToArr(p.get_x()-1, p.get_y()+1, validMoves);
                        return validMoves;
                    }
                }

                if(board[p.get_y()+1][p.get_x()] == nullptr){
                    validMoves = addToArr(p.get_x(), p.get_y()+1, validMoves);
                }

                if(
                    p.get_y() == 1 &&
                    !this->get_hasMoved() &&
                    board[2][p.get_x()] == nullptr &&
                    board[3][p.get_x()] == nullptr
                ){
                    validMoves = addToArr(p.get_x(), 3, validMoves);
                }

                if(
                    p.get_x() > 0 &&
                    board[p.get_y()+1][p.get_x()-1] != nullptr &&
                    board[p.get_y()+1][p.get_x()-1]->get_PieceColor() == WHITE &&
                    this->get_pinDirection() == NONE
                ){
                    //this->get_pinDirection() == NONE is needed here, because top and bottom are the only directions not checked
                    validMoves = addToArr(p.get_x()-1, p.get_y()+1, validMoves);
                }

                if(
                    p.get_x() < 7 &&
                    board[p.get_y()+1][p.get_x()+1] != nullptr &&
                    board[p.get_y()+1][p.get_x()+1]->get_PieceColor() == WHITE &&
                    this->get_pinDirection() == NONE
                ){
                    //this->get_pinDirection() == NONE is needed here, because top and bottom are the only directions not checked
                    //moving sideways is not allowed when pinned on a line of X
                    validMoves = addToArr(p.get_x()+1, p.get_y()+1, validMoves);
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
        Position * get_ValidMoves(chessPiece* board[8][8], Position p)const override {
            Position * validMoves = new Position[1];

            if(this->get_isPinned()){
                return validMoves;
            }

            if(this->get_PieceColor() == WHITE){
                if(
                    p.get_x()+2 < 8 && p.get_y()+1 < 8 && 
                    (board[p.get_y()+1][p.get_x()+2] == NULL || board[p.get_y()+1][p.get_x()+2]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()+2, p.get_y()+1, validMoves);
                }
                if(
                    p.get_x()+2 < 8 && p.get_y()-1 > -1 &&
                    (board[p.get_y()-1][p.get_x()+2] == NULL || board[p.get_y()-1][p.get_x()+2]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()+2, p.get_y()-1, validMoves);
                }

                if(
                    p.get_x()-2 > -1 && p.get_y()+1 < 8 &&
                    (board[p.get_y()+1][p.get_x()-2] == NULL || board[p.get_y()+1][p.get_x()-2]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()-2, p.get_y()+1, validMoves);
                }
                if(
                    p.get_x()-2 > -1 && p.get_y()-1 > -1 &&
                    (board[p.get_y()-1][p.get_x()-2] == NULL || board[p.get_y()-1][p.get_x()-2]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()-2, p.get_y()-1, validMoves);
                }

                if(
                    p.get_y()+2 < 8 && p.get_x()+1 < 8 &&
                    (board[p.get_y()+2][p.get_x()+1] == NULL || board[p.get_y()+2][p.get_x()+1]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()+1, p.get_y()+2, validMoves);
                }
                if(
                    p.get_y()+2 < 8 && p.get_x()-1 > -1 &&
                    (board[p.get_y()+2][p.get_x()-1] == NULL || board[p.get_y()+2][p.get_x()-1]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()-1, p.get_y()+2, validMoves);
                }

                if(
                    p.get_y()-2 > -1 && p.get_x()+1 < 8 &&
                    (board[p.get_y()-2][p.get_x()+1] == NULL || board[p.get_y()-2][p.get_x()+1]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()+1, p.get_y()-2, validMoves);
                }
                if(
                    p.get_y()-2 > -1 && p.get_x()-1 > -1 &&
                    (board[p.get_y()-2][p.get_x()-1] == NULL || board[p.get_y()-2][p.get_x()-1]->get_PieceColor() == BLACK)
                ){
                    validMoves = addToArr(p.get_x()-1, p.get_y()-2, validMoves);
                }

            }else if(this->get_PieceColor() == BLACK){
                if(
                    p.get_x()+2 < 8 && p.get_y()+1 < 8 && 
                    (board[p.get_y()+1][p.get_x()+2] == NULL || board[p.get_y()+1][p.get_x()+2]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()+2, p.get_y()+1, validMoves);
                }
                if(
                    p.get_x()+2 < 8 && p.get_y()-1 > -1 &&
                    (board[p.get_y()-1][p.get_x()+2] == NULL || board[p.get_y()-1][p.get_x()+2]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()+2, p.get_y()-1, validMoves);
                }

                if(
                    p.get_x()-2 > -1 && p.get_y()+1 < 8 &&
                    (board[p.get_y()+1][p.get_x()-2] == NULL || board[p.get_y()+1][p.get_x()-2]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()-2, p.get_y()+1, validMoves);
                }
                if(
                    p.get_x()-2 > -1 && p.get_y()-1 > -1 &&
                    (board[p.get_y()-1][p.get_x()-2] == NULL || board[p.get_y()-1][p.get_x()-2]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()-2, p.get_y()-1, validMoves);
                }

                if(
                    p.get_y()+2 < 8 && p.get_x()+1 < 8 &&
                    (board[p.get_y()+2][p.get_x()+1] == NULL || board[p.get_y()+2][p.get_x()+1]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()+1, p.get_y()+2, validMoves);
                }
                if(
                    p.get_y()+2 < 8 && p.get_x()-1 > -1 &&
                    (board[p.get_y()+2][p.get_x()-1] == NULL || board[p.get_y()+2][p.get_x()-1]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()-1, p.get_y()+2, validMoves);
                }

                if(
                    p.get_y()-2 > -1 && p.get_x()+1 < 8 &&
                    (board[p.get_y()-2][p.get_x()+1] == NULL || board[p.get_y()-2][p.get_x()+1]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()+1, p.get_y()-2, validMoves);
                }
                if(
                    p.get_y()-2 > -1 && p.get_x()-1 > -1 &&
                    (board[p.get_y()-2][p.get_x()-1] == NULL || board[p.get_y()-2][p.get_x()-1]->get_PieceColor() == WHITE)
                ){
                    validMoves = addToArr(p.get_x()-1, p.get_y()-2, validMoves);
                }
            }

            return validMoves;
        }
};

class bishop: public chessPiece{
    private:
    public:
        bishop(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction){}
        ~bishop(){}
        Position * get_ValidMoves(chessPiece* board[8][8], Position p)const override {

        }

};

class rook: public chessPiece{
    private:
        bool hasMoved;
    public:
        rook(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction), hasMoved(0){}
        ~rook(){}
        Position * get_ValidMoves(chessPiece* board[8][8], Position p)const override {

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
        Position * get_ValidMoves(chessPiece* board[8][8], Position p)const override {

        }

};

class king: public chessPiece{
    private:
        bool hasMoved;
    public:
        king(pieceColor c, bool pinned, pinDirection direction):chessPiece(c, pinned, direction), hasMoved(0){}
        ~king(){}
        Position * get_ValidMoves(chessPiece* board[8][8], Position p)const override {

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