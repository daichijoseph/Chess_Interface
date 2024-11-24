# Compiler
CXX = g++

# Compile flags
## Using C++14 standard with all warnings enabled
CXXFLAGS = -std=c++14 -Wall -g

# LDFLAGS (now empty since AddressSanitizer is disabled)
LDFLAGS =

chess: main.o PieceBoard.o State.o Piece.o
	$(CXX) $(CXXFLAGS) main.o PieceBoard.o State.o Piece.o -o chess

main.o: main.cpp State.h
	$(CXX) $(CXXFLAGS) -c main.cpp

PieceBoard.o: PieceBoard.cpp PieceBoard.h
	$(CXX) $(CXXFLAGS) -c PieceBoard.cpp

State.o: State.cpp State.h
	$(CXX) $(CXXFLAGS) -c State.cpp

Piece.o: Piece.cpp Piece.h
	$(CXX) $(CXXFLAGS) -c Piece.cpp

clean:
	rm -f *.o chess
