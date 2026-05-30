main.exe: main.cpp classes.o
	g++ main.cpp classes.o -o main.exe

testVisualization.exe: testVisualization.cpp classes.o
	g++ testVisualization.cpp classes.o -o testVisualization.exe -lsfml-graphics -lsfml-window -lsfml-system

classes.o: classes.cpp classes.h
	g++ -c  classes.cpp -o classes.o


clean:
	del -f *.o *.exe