#include <iomanip>
#include <cctype>
#include "State.h"

GameState::GameState(unsigned short moveNum, bool whiteTurn, bool isCheck, bool pawnJump, std::string prevMove)
    : move(moveNum), whiteToMove(whiteTurn), isCheck(isCheck), pawnJump(pawnJump), lastMove(prevMove) {}

std::map <std::string, std::unique_ptr<PieceBoard>> GameState::boards; // Static variables must be initialized outside of the class



/* Getters */
bool GameState::isInCheck() const {
    return isCheck;
}
bool GameState::getWhiteToMove() const {
    return whiteToMove;
}
unsigned short GameState::getMoveNum() const {
    return move;
}


void GameState::initFullBoard() {
    boards["bp"] = std::make_unique<PieceBoard>('b', 'p', 0x00FF000000000000); // 00000000 11111111 00000000 00000000 00000000 00000000 00000000 00000000
    boards["bN"] = std::make_unique<PieceBoard>('b', 'N', 0x4200000000000000); // 01000010 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    boards["bB"] = std::make_unique<PieceBoard>('b', 'B', 0x2400000000000000); // 00100100 00000000 00000000 00000000 00000000 00000000 00000000 00000000
    boards["bR"] = std::make_unique<PieceBoard>('b', 'R', 0x8100000000000000);
    boards["bQ"] = std::make_unique<PieceBoard>('b', 'Q', 0x1000000000000000);
    boards["bK"] = std::make_unique<PieceBoard>('b', 'K', 0x0800000000000000);

    boards["wp"] = std::make_unique<PieceBoard>('w', 'p', 0x000000000000FF00);
    boards["wN"] = std::make_unique<PieceBoard>('w', 'N', 0x0000000000000042);
    boards["wB"] = std::make_unique<PieceBoard>('w', 'B', 0x0000000000000024);
    boards["wR"] = std::make_unique<PieceBoard>('w', 'R', 0x0000000000000081);
    boards["wQ"] = std::make_unique<PieceBoard>('w', 'Q', 0x0000000000000010);
    boards["wK"] = std::make_unique<PieceBoard>('w', 'K', 0x0000000000000008);

    if (!boards["wp"] || !boards["bp"]) {
        std::cerr << "Pawn boards not initialized." << std::endl;
        throw std::runtime_error("Initialization failed for one or both pawn sets.");
    }
}

void GameState::printFullBoard () {
    bool whiteSquare;
    for (int rank = BOARD_SIZE - 1; rank >= 0; rank--) {
        std::cout << rank + 1 << "   ";
        for (int file = BOARD_SIZE - 1; file >= 0; file--) {
            bool found = false;
            whiteSquare = (rank + file) % 2 == 0;

            std::string backgroundColor = whiteSquare ? "\033[48;5;180m" : "\033[48;5;130m"; // Candidate to replace blue: \033[48;5;130m

            for (const auto& pair : boards) {
                std::cout << backgroundColor;
                if (pair.second->bitboard & (1ULL << (rank * 8 + file))) {
                    if (pair.first[0] == 'b')
                        std::cout << BLACK_PIECE_COLOR << pair.first << RESET_COLOR;
                    else
                        std::cout << WHITE_PIECE_COLOR << pair.first << RESET_COLOR;
                    found = true;
                    break;
                }
            }
            if (!found)
                std::cout << backgroundColor << "  " << RESET_COLOR;
            // if (file != 0)
            // std::cout << "  ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl << "    A B C D E F G H";
}

void GameState::cleanupFullBoard() {
    boards.clear();
}

/* Method returning the string associated with a bitboard with one bit set, shifted by some offset (offset < 0: shift right) */

std::string GameState::getPosString(uint64_t pos, short offset) const {
    if (pos == 0) {
        std::cerr << "There are no pieces of this type." << std::endl;
        return "";
    }
    short index = static_cast<short>(__builtin_ctzll(pos)) + offset; // __builtin_ctzll() returns number of trailing zeros before the set bit
    char rank = static_cast<char>('0' + (1 + index / BOARD_SIZE));
    char file = FILES[index % BOARD_SIZE];
    std::string square;
    square += file;
    square += rank;
    return square;
}

std::map<std::string, uint64_t> GameState::getLegalMoves() {
    std::map<std::string, uint64_t> legalMoves = getPawnMoves();
    return legalMoves;
}



/* Method that takes a bitboard with one bit set and offset as input, and returns digit/string pair corresponding to one square */
std::map<uint64_t, std::string> GameState::squareLoc(short offset, uint64_t lsb) const {
    std::map<uint64_t, std::string> square;
    uint64_t destSquare = whiteToMove ? (lsb << offset) : (lsb >> offset);
    std::string posString = whiteToMove ? getPosString(lsb, offset) : getPosString(lsb, -offset);
    square[destSquare] = posString;
    return square;
}

/* Takes a string representing a square and outputs a bitboard with only the bit corresponding to that square set */
uint64_t GameState::boardLoc(std::string square) const {
    try {
        if (square.length() != 2 || !('a' <= square[0] && square[0] <= 'h') || !('1' <= square[1] && square[1] <= '8')) {
            throw std::runtime_error("Invalid square string as input to the boardLoc method");
        }
    } catch (const std::runtime_error& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        // cleanupFullBoard();
        boards.clear(); // For some reason, unable to call cleanupFullBoard()
        exit(EXIT_FAILURE);
    }
    short offset = 8 * static_cast<short>(square[1] - '1') + static_cast<short>('h' - square[0]);
    return 1ULL << offset;
}

/* Returns all possible pawn moves for a certain color, NOT CONSIDERING CHECKS AND REVEALING THE KING.
   NOTE: this method was unintendedly long and convolluted. It was definitely very experimental in nature,
   and I think I'll be finding some other way of figuring out legal moves for other pieces, which is more
   streamlined and universal. */
std::map<std::string, uint64_t> GameState::getPawnMoves() {
    std::map<std::string, uint64_t> pawnMoves;
    std::string piece;
    uint64_t promotionBound;
    uint64_t startRankLowerBound;
    uint64_t startRankUpperBound;
    uint64_t blackBoard = GameState::blackBoard();
    uint64_t whiteBoard = GameState::whiteBoard();
    std::map<uint64_t, std::string> destSquare;
    std::map<uint64_t, std::string>::iterator iter;

    if (whiteToMove) {
        piece = "wp";
        promotionBound = static_cast<uint64_t>(1) << 56;
        startRankLowerBound = 1 << 8;
        startRankUpperBound = 1 << 15;
    } else {
        piece = "bp";
        promotionBound = 1 << 7;
        startRankLowerBound = static_cast<uint64_t>(1) << 48;
        startRankUpperBound = static_cast<uint64_t>(1) << 55;
    }
    // 'auto&' is useful for limiting unnecessary verbosity; here, it replaces 'std::unique_pts<PieceBoard>&', which is a reference to a std::unique_pts<PieceBoard>
    auto& pawnsPtr = GameState::boards[piece];
    if (pawnsPtr) {
        uint64_t bitboard = pawnsPtr->bitboard;
        while (bitboard) {
            uint64_t lsb = bitboard & -bitboard;
            bitboard &= (bitboard - 1); // Clears the least significant bit!

            destSquare = squareLoc(8, lsb);
            iter = destSquare.begin();
            if (!(iter->first & (blackBoard | whiteBoard))) {
                if (whiteToMove && iter->second[1] == '8') {
                    // Promotion
                    for (char p : PIECES) {
                        pawnMoves[(iter->second + '=' + p)] = ((boards[piece]->bitboard - lsb) | iter->first);
                    }
                } else
                    pawnMoves[iter->second] = ((boards[piece]->bitboard - lsb) | iter->first);
                
                // If moving one forward is legal, maybe two forward is legal as well
                destSquare = squareLoc(16, lsb);
                iter = destSquare.begin();
                if (startRankLowerBound <= lsb && lsb <= startRankUpperBound && !(iter->first & (blackBoard | whiteBoard)))  // If lsb is on the starting rank and there isn't a piece two squares ahead:
                    pawnMoves[iter->second] = ((boards[piece]->bitboard - lsb) | iter->first);
            }
            
            // For captures
            destSquare = squareLoc(7, lsb);
            iter = destSquare.begin();
            uint64_t otherBoard = whiteToMove ? blackBoard : whiteBoard;
            std::string source = getPosString(lsb, 0);
            source[1] = 'x';
            // Edge cases for if the pawn is on the a-file or the h-file; logic is tricky, switches depending on the piece color
            if (((__builtin_ctzll(iter->first) % 8) != 7 || !whiteToMove) && ((__builtin_ctzll(iter->first) % 8) != 0 || whiteToMove) && (iter->first & otherBoard)) {
                if (((iter->first >= promotionBound) && whiteToMove) || ((iter->first <= promotionBound) && !whiteToMove))
                    for (char p : PIECES) {
                        pawnMoves[source + iter->second + '=' + p] = ((boards[piece]->bitboard - lsb) | iter->first);
                    }
                else
                    pawnMoves[source + iter->second] = ((boards[piece]->bitboard - lsb) | iter->first);
            }
            destSquare = squareLoc(9, lsb);
            iter = destSquare.begin();
            if (((__builtin_ctzll(iter->first) % 8) != 0 || !whiteToMove) && ((__builtin_ctzll(iter->first) % 8) != 7 || whiteToMove) && (iter->first & otherBoard)) {
                if (((iter->first >= promotionBound) && whiteToMove) || ((iter->first <= promotionBound) && !whiteToMove))
                    for (char p : PIECES) {
                        pawnMoves[source + iter->second + '=' + p] = ((boards[piece]->bitboard - lsb) | iter->first);
                    }
                else 
                    pawnMoves[source + iter->second] = ((boards[piece]->bitboard - lsb) | iter->first);
            }

            // En passant
            std::string enPassant = GameState::enPassant(lastMove, lsb);
            if (enPassant != "") {
                uint64_t dest = boardLoc(enPassant.substr(2));
                pawnMoves[enPassant] = ((boards[piece]->bitboard - lsb) | dest);

            }
            
        }
    }
    return pawnMoves;
}


/* Returns legal rook moves, not accounting for check 
std::map<std::string, uint64_t> GameState::getRookMoves() {
    std::map<std::string, uint64_t> rookMoves;
    std::string piece = whiteToMove? "wR" : "bR";
    uint64_t fullBoard = GameState::blackBoard() & GameState::whiteBoard();
    uint64_t otherBoard = whiteToMove? GameState::blackBoard() : GameState::whiteBoard();
    uint64_t originalBitboard = boards[piece]->bitboard;
    uint64_t currentRook;
    short currentRookPos;
    uint64_t legalMoves;
    uint64_t upperBound = 0;
    uint64_t lowerBound = 0;

    while (originalBitboard) {
        currentRook = originalBitboard & -originalBitboard;
        originalBitboard &= (originalBitboard - 1);
        try {
            legalMoves = getRookBoard(currentRook);
        }
        catch (const std::runtime_error& e) {
            std::cerr << "Caught an exception: " << e.what() << std::endl;
            cleanupFullBoard();
            exit(EXIT_FAILURE);
        }

        // Up the current file:
        for (short rank = currentRook % 8; rank < currentRook; rank += 8) {
            uint64_t curr = 1ULL << rank;
            if ((curr) & fullBoard) {
                lowerBound = curr;
            }
        }
        for (short rank = currentRook + 8; rank <= 63; rank += 8) {
            uint64_t curr = 1ULL << rank;
            if ((curr) & fullBoard) {
                upperBound = curr;
                break;
            }
        }
        for (short rank = lowerBound; rank <= upperBound; rank += 8) {
            legalMoves |= 1ULL << rank;
        }

        // Across the rank:
        for (short file = currentRook / 8; file < currentRook; file++) {
            uint64_t curr = 1ULL << file;
            if ((curr) & fullBoard) {
                lowerBound = curr;
            }
        }
        for (short file = currentRook + 1; file < currentRook / 8 + 8; file++) {
            uint64_t curr = 1ULL << file;
            if ((curr) & fullBoard) {
                upperBound = curr;
                break;
            }
        }
        for (short file = lowerBound; file <= upperBound; file++) {
            legalMoves |= 1ULL << file;
        }

        legalMoves = ~(legalMoves & currentRook);

    }
}
*/


/* Methods returning bitboards with all bits where corresponding squares are occupied by any piece of a single color */

uint64_t GameState::blackBoard() const {
    uint64_t blackboard = 0;
    for (const auto& pair : GameState::boards) {
        if (pair.first[0] == 'b')
            blackboard |= pair.second->bitboard;
    }
    return blackboard;
}
uint64_t GameState::whiteBoard() const {
    uint64_t whiteboard = 0;
    for (const auto& pair : GameState::boards) {
        if (pair.first[0] == 'w')
            whiteboard |= pair.second->bitboard;
    }
    return whiteboard;
}


/* Method returning a string corresponding to the move made when capturing En Passant */

std::string GameState::enPassant(std::string lastMove, uint64_t currentSquare) const {
    std::string currPos = getPosString(currentSquare, 0);
    std::string move;
    if (lastMove == "")
        return lastMove;
    if (pawnJump && whiteToMove && lastMove[1] == '5' && currPos[1] == '5' && std::abs(static_cast<int>(currPos[0]) - static_cast<int>(lastMove[0])) == 1)
        move += std::string(1, currPos[0]) + 'x' + lastMove[0] + '6';
    if (pawnJump && !whiteToMove && lastMove[1] == '4' && currPos[1] == '4' && std::abs(static_cast<int>(currPos[0]) - static_cast<int>(lastMove[0])) == 1)
        move += std::string(1, currPos[0]) + 'x' + lastMove[0] + '3';
    return move;
}



/* Reutrns bitboard with all possible bits corresponding to rook moves set 

uint64_t getRookBoard(const uint64_t rookBoard) {

    uint64_t movesMask = 0;
    if (rookBoard != (rookBoard & -rookBoard))
        throw std::runtime_error("Bitboard passed as input needs to have exactly one set bit.");
    short rookIndex = __builtin_ctzll(rookBoard);
    short rankIndex = rookIndex / BOARD_SIZE;
    short fileIndex = rookIndex % BOARD_SIZE;

    for (short i = rankIndex * BOARD_SIZE; i < (rankIndex + 1) * BOARD_SIZE; i++)
        movesMask |= (1ULL << i);
    for (short i = fileIndex; i < 63; i += 8)
        movesMask ^= (1ULL << i);
    
    return movesMask;
}
*/


/* Make move, update state */

bool GameState::makeMove(std::string candidate) {
    std::map<std::string, uint64_t> legalMoves = getLegalMoves();
    bool found = false;
    uint64_t bitBoard;
    uint64_t destSquare;
    std::string bitboardKey;
    std::string croissantKey = whiteToMove ? "bp" : "wp";
    for (const auto& pair : legalMoves) {
        if (candidate == pair.first) {
            found = true;
            bitBoard = pair.second;
            break;
        }
    }
    if (!found) {
        std::cout << "\nPlease enter a valid move.";
        return false;
    }

    pawnJump = false;

    // This if statement takes care of messy promotion bitboard update logic
    if (islower(candidate[0]) && (bitBoard >= static_cast<uint64_t>(1) << 56 || __builtin_ctzll(bitBoard) <= 7)) {
        for (char c : candidate) {
            if (isupper(c)) {
                destSquare = whiteToMove? findMSB(bitBoard) : (bitBoard & -bitBoard);
                try {
                    bitboardKey = whiteToMove? (std::string("w") + c) : (std::string("b") + c);
                    if (boards.find(bitboardKey) == boards.end()) {
                        throw std::runtime_error("The key generated in the makeMove() method was not in the boards map (whiteToMove = true).");
                    }
                } catch (const std::runtime_error& e) {
                        std::cerr << "Caught an exception: " << e.what() << std::endl;
                        cleanupFullBoard();
                        exit(EXIT_FAILURE);
                }
                if (whiteToMove)
                    // We haven't updated the pawn boards yet
                    boards["wp"]->bitboard = bitBoard - destSquare;
                else
                    boards["bp"]->bitboard = bitBoard - destSquare;
                for (const auto& pair : boards) {
                    if (pair.second->bitboard & destSquare) {
                        pair.second->bitboard -= destSquare; // Remove piece being taken
                        break;
                    }
                }
                boards[bitboardKey]->bitboard |= destSquare;
                break;
            }
        }
    // If no promotion, updating the bitboards is simpler
    } else if (candidate[0] != 'O') {
        if (isupper(candidate[0]))
            bitboardKey = whiteToMove ? std::string("w") + candidate[0] : std::string("b") + candidate[0];
        else {
            bitboardKey = whiteToMove ? "wp" : "bp";
        }
        uint64_t moveDiff = bitBoard ^ boards[bitboardKey]->bitboard;
        if (bitboardKey[1] == 'p' && findMSB(moveDiff) == ((moveDiff & -moveDiff) << 16))
            pawnJump = true;
        if (candidate[1] == 'x') {
            destSquare = bitBoard & moveDiff; // Isolates the bit where a piece is currently being taken / moving to!
            found = false;
            for (const auto& pair : boards) {
                if (pair.second->bitboard & destSquare) {
                    found = true;
                    pair.second->bitboard -= destSquare; // Remove the piece being taken from its bitboard
                    break;
                }
            }
            if (!found) {
                if (whiteToMove)
                    boards[croissantKey]->bitboard &= ~boardLoc(std::string(1, candidate[2]) + "5");
                else
                    boards[croissantKey]->bitboard &= ~boardLoc(std::string(1, candidate[2]) + "4");
            }
            
        }

        boards[bitboardKey]->bitboard = bitBoard;
    } // else {
        // insert castling bitboard logic
    // }
    
    whiteToMove = !whiteToMove;
    if (whiteToMove) {
        move++;
    }
    lastMove = candidate;
    return true;
}



/* Finds msb of a uint64_t */

uint64_t GameState::findMSB(uint64_t input) const {
    uint64_t copy = input;
    uint64_t msb = 0;
    while (copy) {
        msb = copy & -copy;
        copy &= (copy - 1);
    }
    return msb;
}
