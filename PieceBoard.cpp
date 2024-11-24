#include "PieceBoard.h"

PieceBoard::PieceBoard(char color, char type, uint64_t positions): pieceColor(color), pieceType(type), bitboard(positions) {}

void printBoard (const PieceBoard& board) {
    for (int rank = BOARD_SIZE - 1; rank >= 0; rank--) {
        for (int file = 0; file < BOARD_SIZE; file++) {
            if (board.bitboard & (1ULL << (rank * BOARD_SIZE + file))) {
                if (board.pieceColor == 'b')
                    std::cout << BLACK_PIECE_COLOR << board.pieceColor << board.pieceType << RESET_COLOR;
                else
                    std::cout << WHITE_PIECE_COLOR << board.pieceColor << board.pieceType << RESET_COLOR;
            }
            else {
                if ((rank + file) % 2)
                    std::cout << "--";
                else
                    std::cout << "||";
            }
            if (file != BOARD_SIZE - 1)
                std::cout << "  ";
        }
        std::cout <<std::endl << std::endl;
    }
}

uint64_t getBitBoard(const std::string& piece, std::map<std::string, std::unique_ptr<PieceBoard>>& boards) {
    auto iter = boards.find(piece);
    if (iter != boards.end())
        return boards[piece]->bitboard;
    throw std::runtime_error("Piece not found: " + piece);
}

