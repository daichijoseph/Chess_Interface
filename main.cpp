#include <iostream>
#include "State.h"

int main() {
    GameState game = GameState();
    bool found = false;
    std::map<std::string, uint64_t> legalMoves;

    std::cout << "\n\nInitializing board...";
    game.initFullBoard();
    std::cout << "\n\n\nCurrent board:\n\n\n";
    game.printFullBoard();
    while (true) {
        try {
            legalMoves = game.getLegalMoves();
        } catch (const std::exception& e) {
            std::cerr << "Failed to get legal moves: " << e.what() << std::endl;
            return 1;
        }
        
        std::cout << "\n\n";
        
        if (game.getWhiteToMove())
            std::cout << "White to move.";
        else
            std::cout << "Black to move.";
        std::cout << std::endl;

        std::string move;
        std::cin >> move;
        
        try {
            found = game.makeMove(move);
        } catch (std::exception& e) {
            std::cerr << "Move failed: " << e.what() << std::endl;
        }
        if (!found) {
            std::cout << "\n\nCurrent legal pawn moves: ";
            for (const auto& pair : legalMoves)
                std::cout << pair.first << " ";
            continue;
        }
        std::cout << "\n\nCurrent board:\n\n\n";
        game.printFullBoard();
    }
    

    game.cleanupFullBoard();

    return 0;
}