#include <stdexcept>
#include <iostream>

using namespace std;

enum pieceColor{
    WHITE,
    BLACK
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
        Position(int x, int y):x(x), y(y)
        {}
        ~Position()
        {}
        int get_x()const{
            return x;
        }
        int get_y()const{
            return y;
        }
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
    public:
        chessPiece(pieceColor c): color(c), isPinnedToKing(0){};
        virtual ~chessPiece(){};
        virtual Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const = 0;
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
};

class pawn: public chessPiece{
    private:
        bool hasMoved;
    public:
        pawn(pieceColor c):chessPiece(c), hasMoved(0){}
        ~pawn(){}
        Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const override {

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
        knight(pieceColor c):chessPiece(c){}
        ~knight(){}
        Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const override {

        }
};

class bishop: public chessPiece{
    private:
    public:
        bishop(pieceColor c):chessPiece(c){}
        ~bishop(){}
        Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const override {

        }

};

class rook: public chessPiece{
    private:
        bool hasMoved;
    public:
        rook(pieceColor c):chessPiece(c), hasMoved(0){}
        ~rook(){}
        Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const override {

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
        queen(pieceColor c):chessPiece(c){}
        ~queen(){}
        Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const override {

        }

};

class king: public chessPiece{
    private:
        bool hasMoved;
    public:
        king(pieceColor c):chessPiece(c), hasMoved(0){}
        ~king(){}
        Position * get_ValidMoves(chessPiece* board[8][8], int x, int y)const override {

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