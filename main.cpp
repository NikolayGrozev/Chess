#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include "classes.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <stdexcept>

int main() {
    std::cout << "============== Launching Chess Engine Match ==============" << std::endl;
    
    // 1. Allocate your structural game orchestration framework context
    game chessMatch;

    // 2. Hand control fully over to your active gameplay loop thread 
    // This will open a 1000x1000 SFML window, configure default board assets,
    // and track player alternating clicks safely until a draw or window termination.
    chessMatch.game_run();

    std::cout << "============== Match Controller Terminated ==============" << std::endl;

    // 1. Get current system time point
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // 2. Format it into a structured year-month-day_hour-minute-second string
    std::stringstream ss;
    ss << "saves/game_" << std::put_time(std::localtime(&now_time), "%Y-%m-%d_%H-%M-%S") << ".txt";
    std::string uniqueFilename = ss.str();

    // 3. Save it via your streamlined operator
    std::ofstream file(uniqueFilename, std::ios::out | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr<<"Could not open file";
        exit(1);
    }
    file << chessMatch;
    file.close();
    std::cout << "Match safely auto-logged to: " << uniqueFilename << std::endl;
    return 0;
}