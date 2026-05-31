#include <SFML/Graphics.hpp>
#include "Game.h"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <fstream>
#include <stdexcept>
#include <filesystem>
#include <vector>
#include <algorithm>

std::vector<std::string> getLatestSaves(size_t count) {
    std::vector<std::string> saves;
    std::string path = "saves";
    if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
        for (const auto& entry : std::filesystem::directory_iterator(path)) {
            if (entry.path().extension() == ".txt") {
                saves.push_back(entry.path().filename().string());
            }
        }
    }
    std::sort(saves.rbegin(), saves.rend());
    if (saves.size() > count) {
        saves.resize(count);
    }
    return saves;
}

int main() {
    std::cout << "============== Launching Chess Engine Menu ==============" << std::endl;

    sf::RenderWindow window(sf::VideoMode(1000, 1000), "Chess Engine");

    sf::Texture playTex, replayTex, exitTex, slotTex;
    if (!playTex.loadFromFile("assets/play.png")) std::cerr << "Missing assets/play.png\n";
    if (!replayTex.loadFromFile("assets/replay.png")) std::cerr << "Missing assets/replay.png\n";
    if (!exitTex.loadFromFile("assets/exit.png")) std::cerr << "Missing assets/exit.png\n";
    if (!slotTex.loadFromFile("assets/slot.png")) std::cerr << "Missing assets/slot.png\n";

    sf::Font arialFont;
    if (!arialFont.loadFromFile("assets/ArialCE.ttf")) std::cerr << "Missing assets/ArialCE.ttf\n";

    sf::Sprite playSprite(playTex), replaySprite(replayTex), exitSprite(exitTex);

    // The pngs are 300x137. Center them horizontally (X = 500 - 150 = 350).
    playSprite.setPosition(350.0f, 250.0f);
    replaySprite.setPosition(350.0f, 450.0f);

    enum MenuState { MAIN_MENU, REPLAY_MENU };
    MenuState currentState = MAIN_MENU;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
                sf::Vector2i rawMouse(event.mouseButton.x, event.mouseButton.y);
                sf::Vector2f mappedMouse = window.mapPixelToCoords(rawMouse);

                if (currentState == MAIN_MENU) {
                    // Play Button Bounds (350 to 650 X, 250 to 387 Y)
                    if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                        mappedMouse.y >= 250.0f && mappedMouse.y <= 387.0f) {
                        game chessMatch;
                        chessMatch.game_run(window);
                        
                        // Auto-save the match after returning to the menu
                        auto now = std::chrono::system_clock::now();
                        std::time_t now_time = std::chrono::system_clock::to_time_t(now);
                        std::stringstream ss;
                        ss << "saves/game_" << std::put_time(std::localtime(&now_time), "%Y-%m-%d_%H-%M-%S") << ".txt";
                        std::string uniqueFilename = ss.str();
                        
                        std::ofstream file(uniqueFilename, std::ios::out | std::ios::trunc);
                        if (file.is_open()) {
                            file << chessMatch;
                            file.close();
                            std::cout << "Match safely auto-logged to: " << uniqueFilename << std::endl;
                        } else {
                            std::cerr << "Warning: Could not open save file for writing!\n";
                        }
                    }
                    // Replay Button Bounds (350 to 650 X, 450 to 587 Y)
                    else if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                             mappedMouse.y >= 450.0f && mappedMouse.y <= 587.0f) {
                        currentState = REPLAY_MENU;
                    }
                    // Exit Button Bounds (350 to 650 X, 650 to 787 Y)
                    else if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                             mappedMouse.y >= 650.0f && mappedMouse.y <= 787.0f) {
                        window.close();
                    }
                } else if (currentState == REPLAY_MENU) {
                    // Exit Button Bounds in Replay Menu (350 to 650 X, 800 to 937 Y)
                    if (mappedMouse.x >= 350.0f && mappedMouse.x <= 650.0f &&
                        mappedMouse.y >= 800.0f && mappedMouse.y <= 937.0f) {
                        currentState = MAIN_MENU;
                    } else {
                        // Check slots. X = 275 to 725, Y = 50 + (i * 120) to + 100
                        std::vector<std::string> saves = getLatestSaves(6);
                        for (size_t i = 0; i < saves.size(); i++) {
                            float slotX = 275.0f;
                            float slotY = 50.0f + (i * 120.0f);
                            if (mappedMouse.x >= slotX && mappedMouse.x <= slotX + 450.0f &&
                                mappedMouse.y >= slotY && mappedMouse.y <= slotY + 100.0f) {
                                game chessMatch;
                                chessMatch.game_replay("saves/" + saves[i], window);
                                break;
                            }
                        }
                    }
                }
            }
        }

        window.clear(sf::Color(30, 30, 30));

        if (currentState == MAIN_MENU) {
            window.draw(playSprite);
            window.draw(replaySprite);
            exitSprite.setPosition(350.0f, 650.0f);
            window.draw(exitSprite);
        } else if (currentState == REPLAY_MENU) {
            std::vector<std::string> saves = getLatestSaves(6);
            for (size_t i = 0; i < saves.size(); i++) {
                float slotX = 275.0f;
                float slotY = 50.0f + (i * 120.0f);
                
                sf::Sprite slotSprite(slotTex);
                slotSprite.setPosition(slotX, slotY);
                window.draw(slotSprite);

                sf::Text text;
                text.setFont(arialFont);
                
                std::string displayName = saves[i];
                if (displayName.substr(0, 5) == "game_") displayName = displayName.substr(5);
                if (displayName.size() > 4 && displayName.substr(displayName.size() - 4) == ".txt") 
                    displayName = displayName.substr(0, displayName.size() - 4);
                
                text.setString(displayName);
                text.setCharacterSize(24);
                text.setFillColor(sf::Color::Black);
                
                sf::FloatRect textBounds = text.getLocalBounds();
                text.setOrigin(textBounds.left + textBounds.width / 2.0f, textBounds.top + textBounds.height / 2.0f);
                text.setPosition(slotX + 225.0f, slotY + 50.0f);
                
                window.draw(text);
            }
            
            exitSprite.setPosition(350.0f, 800.0f);
            window.draw(exitSprite);
        }

        window.display();
    }

    std::cout << "============== Engine Terminated ==============" << std::endl;
    return 0;
}