#ifndef PIECEBOARD_H
#define PIECEBOARD_H

#include <string>
#include <array>
#include <iostream>
#include <memory>
#include <cstdint>
#include <map>
#include "Constants.h"

class PieceBoard {
public:
    char pieceColor;
    char pieceType;
    uint64_t bitboard;
    
    PieceBoard(char color, char type, uint64_t positions);
};

void printBoard(const PieceBoard& board);
uint64_t getBitBoard(const std::string& piece, std::map<std::string, std::unique_ptr<PieceBoard>>& boards);

// Apparently, including a comment after #endif is good practice for identifying corresponding include guard for long header files
#endif // PIECEBOARD_H