#ifndef GAME_H
#define GAME_H

#include <vector>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "Core.h"
#include "ChessBoard.h"
#include "Renderer.h"

class game {
private:
    ChessBoard board;
    std::vector<Movement> moves;
    bool isOver;
    pieceColor currentTurn;
    int moves_Since_Last_Capture;
    Renderer renderer;
public:
    game();
    ~game();
    bool insufficientMaterial(pieceColor color) const;
    void setupInitialBoard();
    void game_replay(std::string filename, sf::RenderWindow& window);
    void game_run(sf::RenderWindow& window);
    std::vector<Movement> get_Moves();

    friend std::ostream& operator<<(std::ostream& os, const game& g);
    friend std::istream& operator>>(std::istream& in, game& g);
};

#endif // GAME_H