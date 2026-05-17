main.exe: main.cpp classes.o
	g++ main.cpp classes.o -o main.exe

classes.o: classes.cpp
	g++ -c  classes.cpp -o classes.o

clean:
	del -f *.o *.exe