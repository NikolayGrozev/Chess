#ifndef CORE_H
#define CORE_H

#include <limits.h>

enum pieceColor {
    WHITE,
    BLACK
};

class Position;
Position* addToArr(int x, int y, Position* arr);

class Position {
private:
    int x;
    int y;
public:
    Position();
    Position(int x, int y);
    ~Position();
    int get_x() const;
    int get_y() const;
    
    friend Position* addToArr(int x, int y, Position* arr);
};

class Movement {
private:
    Position from;
    Position to;
public:
    Movement();
    Movement(int x1, int y1, int x2, int y2);
    ~Movement();
    Position get_from() const;
    Position get_to() const;
};

#endif // CORE_H