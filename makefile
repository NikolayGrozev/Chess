main.exe: main.cpp classes.o
	g++ main.cpp classes.o -o main.exe

classes.o: classes.cpp classes.h
	g++ -c  classes.cpp -o classes.o

testVisualization.exe: testVisualization.cpp
	g++ testVisualization.cpp -o testVisualization.exe -lsfml-graphics -lsfml-window -lsfml-system

clean:
	del -f *.o *.exe