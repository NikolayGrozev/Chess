#include "Core.h"
#include <stdexcept>

Position::Position() : x(INT_MAX), y(INT_MAX) {}

Position::Position(int x, int y) {
    if(x < 0 || x > 7 || y < 0 || y > 7){
        throw std::invalid_argument("Position coordinates out of bounds");
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