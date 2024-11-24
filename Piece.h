#ifndef PIECE_H
#define PIECE_H

#include <iostream>
#include <vector>
#include <set>
#include <cstdint>
#include "Constants.h"

class Piece {
public:
    char pieceType;
    char pieceColor;
    char value;
    std::set<short> possibleLegalMoves;
    static std::vector<std::set<short>> movesList;

    Piece(char type, char color);

    static void initializeMoves();
};

/*
class Pawn : public Piece {
public:
    Pawn(char color);
};


class Rook : public Piece {
public:
    Rook(char color);
};


class Bishop : public Piece {
public:
    Bishop(char color);
};


class Knight : public Piece {
public:
    Knight(char color);
};


class Queen : public Piece {
public:
    Queen(char color);
};


class King : public Piece {
public:
    King(char color);
};
*/

#endif