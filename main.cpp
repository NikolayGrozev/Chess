#include <SFML/Graphics.hpp>
#include <iostream>
#include "classes.h"

int main() {
    std::cout << "============== Launching Chess Engine Match ==============" << std::endl;
    
    // 1. Allocate your structural game orchestration framework context
    game chessMatch;

    // 2. Hand control fully over to your active gameplay loop thread 
    // This will open a 1000x1000 SFML window, configure default board assets,
    // and track player alternating clicks safely until a draw or window termination.
    chessMatch.game_run();

    std::cout << "============== Match Controller Terminated ==============" << std::endl;
    return 0;
}