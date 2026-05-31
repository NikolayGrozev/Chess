main.exe: main.cpp Core.o Piece.o ChessBoard.o Renderer.o Game.o
	g++ main.cpp Core.o Piece.o ChessBoard.o Renderer.o Game.o -o main.exe -lsfml-graphics -lsfml-window -lsfml-system

Core.o: Core.cpp Core.h
	g++ -c Core.cpp -o Core.o

Piece.o: Piece.cpp Piece.h Core.h
	g++ -c Piece.cpp -o Piece.o

ChessBoard.o: ChessBoard.cpp ChessBoard.h Piece.h Core.h
	g++ -c ChessBoard.cpp -o ChessBoard.o

Renderer.o: Renderer.cpp Renderer.h ChessBoard.h Core.h
	g++ -c Renderer.cpp -o Renderer.o

Game.o: Game.cpp Game.h ChessBoard.h Renderer.h Core.h
	g++ -c Game.cpp -o Game.o

clean:
	del -f *.o *.exe