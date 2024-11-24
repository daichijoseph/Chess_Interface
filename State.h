#ifndef STATE_H
#define STATE_H

#include <iostream>
#include <map>
#include "Piece.h"
#include "PieceBoard.h"

class GameState {
protected:
    static std::map<std::string, std::unique_ptr<PieceBoard>> boards;
    unsigned short move;
    bool whiteToMove;
    bool isCheck;
    bool pawnJump;
    std::string lastMove;
    std::map<std::string, uint64_t> legalMoves;
public:
    GameState(unsigned short moveNum = 0, bool whiteTurn = true, bool isCheck = false, bool pawnJump = false, std::string prevMove = "");
    // Keyword 'const' ensures methods with this signature do not alter the GameState object in any way
    bool isInCheck() const;
    bool getWhiteToMove() const;
    unsigned short getMoveNum() const;

    void initFullBoard();
    void printFullBoard();
    void cleanupFullBoard();

    std::string getPosString(uint64_t pos, short offset) const;
    std::map<uint64_t, std::string> squareLoc(short offset, uint64_t lsb) const;
    uint64_t boardLoc(std::string square) const;
    std::map<std::string, uint64_t> getLegalMoves();
    std::map<std::string, uint64_t> getPawnMoves();
    // std::map<std::string, uint64_t> getRookMoves();

    uint64_t blackBoard() const;
    uint64_t whiteBoard() const;

    // uint64_t getRookBoard(const uint64_t rookBoard);

    std::string enPassant(std::string lastMove, uint64_t lsb) const; 

    bool makeMove(std::string candidate);

    uint64_t findMSB(uint64_t) const;
};

#endif
//test