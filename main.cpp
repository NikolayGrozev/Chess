#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "classes.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

int main() {
    std::cout << "============== Launching Chess Engine Menu ==============" << std::endl;

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess Engine");

    sf::Texture playTex, replayTex, exitTex;
    if (!playTex.loadFromFile("assets/play.png")) std::cerr << "Missing assets/play.png\n";
    if (!replayTex.loadFromFile("assets/replay.png")) std::cerr << "Missing assets/replay.png\n";
    if (!exitTex.loadFromFile("assets/exit.png")) std::cerr << "Missing assets/exit.png\n";

    sf::Sprite playSprite(playTex), replaySprite(replayTex), exitSprite(exitTex);

    // The pngs are 300x137. Center them horizontally (X = 500 - 150 = 350).
    playSprite.setPosition(350.0f, 250.0f);
    replaySprite.setPosition(350.0f, 450.0f);
    exitSprite.setPosition(350.0f, 650.0f);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i rawMouse(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f mappedMouse = window.mapPixelToCoords(rawMouse);

                // Play Button Bounds (350 to 650 X, 250 to 387 Y)
                if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                    mappedMouse.y >= 250.0f && mappedMouse.y <= 387.0f) {
                    game chessMatch;
                    chessMatch.game_run(window);
                    
                    // Note: Auto-saving is currently disabled during menu testing
                }
                
                // Replay Button Bounds (350 to 650 X, 450 to 587 Y)
                if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                    mappedMouse.y >= 450.0f && mappedMouse.y <= 587.0f) {
                    std::cout << "Replay Menu Placeholder Clicked!" << std::endl;
                }
                
                // Exit Button Bounds (350 to 650 X, 650 to 787 Y)
                if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                    mappedMouse.y >= 650.0f && mappedMouse.y <= 787.0f) {
                    window.close();
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));
        window.draw(playSprite);
        window.draw(replaySprite);
        window.draw(exitSprite);
        window.display();
    }

    std::cout << "============== Engine Terminated ==============" << std::endl;
    return 0;
}