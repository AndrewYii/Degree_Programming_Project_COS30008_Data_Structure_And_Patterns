#include <iostream>
#include "Game.h"


int main() {
	// Singleton Game instance
    Game* game = Game::getInstance();
    // Initialise game systems
    game->initialise();
    // Run main game loop
    if (game->isRunning()) {
        game->run();
    }
    // Cleanup
    game->shutdown();
    // Destroy singleton instance
    Game::destroy();
    return 0;
}


