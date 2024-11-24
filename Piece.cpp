#include "Piece.h"

using std::set;
using std::vector;
using std::array;

vector<set<short>> Piece::movesList(8);

Piece::Piece(char type, char color) : pieceType(type), pieceColor(color) {
    if (movesList[0].empty())
        initializeMoves();
    possibleLegalMoves = {};

    std::string piece;
    piece += color;
    piece += type;

    if (piece == "bp") {possibleLegalMoves = {-7, -8, -9, -16};}
    else if (piece == "wp") {possibleLegalMoves = {7, 8, 9, 16};}
    else if (type == 'R') {
        for (short i = 1; i < BOARD_SIZE; i++) {
            possibleLegalMoves.insert(i); // On A-file
            possibleLegalMoves.insert(-i); // On H-file
            possibleLegalMoves.insert((BOARD_SIZE - 1) * i); // On first rank
            possibleLegalMoves.insert(-(BOARD_SIZE - 1) * i); // On eighth rank
        }
    } else if (type == 'N') {
        possibleLegalMoves = {-17, -15, -10, -6, 6, 10, 15, 17};
    }


}

void Piece::initializeMoves() {
    set<short> blackPawnMoves;
    set<short> whitePawnMoves;
    set<short> rookMoves;
    set<short> dsBishopMoves;
    set<short> lsBishopMoves;
    set<short> knightMoves;
    set<short> queenMoves;
    set<short> kingMoves;

    // BLACK PAWNS
    blackPawnMoves = {-7, -8, -9, -16};
    movesList[0] = blackPawnMoves;

    // WHITE PAWNS
    whitePawnMoves = {7, 8, 9, 16};
    movesList[1] = whitePawnMoves;

    // ROOKS
    for (short i = 1; i < BOARD_SIZE; i++) {
        rookMoves.insert(i); // On A-file
        rookMoves.insert(-i); // On H-file
        rookMoves.insert((BOARD_SIZE - 1) * i); // On first rank
        rookMoves.insert(-(BOARD_SIZE - 1) * i); // On eighth rank
    }
    movesList[2] = rookMoves;

    // BISHOPS, DARK SQUARE AND LIGHT SQUARE SEPARATELY
    for (short i = 1; i < BOARD_SIZE; i++) {
        // NOTE: It is impossible for one bishop to make all of these moves, as one bishop cannot have access to both long diagonals. 
        dsBishopMoves.insert(BOARD_SIZE * i + i); // On A1
        dsBishopMoves.insert(-BOARD_SIZE * i - i); // On H8
        lsBishopMoves.insert(-BOARD_SIZE * i + i); // On A8
        lsBishopMoves.insert(BOARD_SIZE * i - i); // On H1
    }
    // For second-longest diagonals:
    for (short i = 1; i < (BOARD_SIZE - 1); i++) {
        lsBishopMoves.insert(BOARD_SIZE * i + i); // On A1
        lsBishopMoves.insert(-BOARD_SIZE * i - i); // On H8
        dsBishopMoves.insert(-BOARD_SIZE * i + i); // On A8
        dsBishopMoves.insert(BOARD_SIZE * i - i); // On H1
    }
    movesList[3] = dsBishopMoves;
    movesList[4] = lsBishopMoves;
    
    // KNIGHTS
    knightMoves = {-17, -15, -10, -6, 6, 10, 15, 17};
    movesList[5] = knightMoves;

    // QUEENS
    queenMoves.insert(rookMoves.begin(), rookMoves.end());
    queenMoves.insert(dsBishopMoves.begin(), dsBishopMoves.end());
    queenMoves.insert(lsBishopMoves.begin(), lsBishopMoves.end());
    movesList[6] = queenMoves;

    // KINGS
    kingMoves = {-9, -8, -7, -2, -1, 1, 2, 7, 8, 9};
    movesList[7] = kingMoves;
}


/*
Pawn::Pawn(char color) : Piece('p', color) {
    possibleLegalMoves = (color == 'b')? Piece::movesList[0] : Piece::movesList[1];
    value = 1;
}


Rook::Rook(char color) : Piece('R', color) {
    possibleLegalMoves = Piece::movesList[2];
    value = 5;
}


Bishop::Bishop(char color) : Piece('B', color) {
    possibleLegalMoves = (color == 'b')? Piece::movesList[3] : Piece::movesList[4];
    value = 3;
}


Knight::Knight(char color) : Piece('N', color) {
    possibleLegalMoves = Piece::movesList[5];
    value = 3;
}


Queen::Queen(char color) : Piece('Q', color) {
    possibleLegalMoves = Piece::movesList[6];
    value = 9;
}


King::King(char color) : Piece('K', color) {
    possibleLegalMoves = Piece::movesList[7];
    value = 0;
}
*/