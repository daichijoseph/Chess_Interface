#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <array>

// Initialization of board size needs to be here (and not in some other file) so that all translation units (cpp files) can access it
constexpr short BOARD_SIZE = 8; 
constexpr std::array<char, 8> FILES = {'h', 'g', 'f', 'e', 'd', 'c', 'b', 'a'};
constexpr std::array<char, 4> PIECES = {'N', 'B', 'R', 'Q'};

const std::string RESET_COLOR = "\033[0m"; // Default color text
const std::string WHITE_PIECE_COLOR = "\033[1m\033[97m"; // Bright white text
const std::string BLACK_PIECE_COLOR = "\033[1m\033[38;5;0m"; // Black text


#endif