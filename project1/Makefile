
Matrix: Gauss.cpp Gauss.h Matrix.cpp
	clang++ -o Matrix Matrix.cpp -Wall -g

debug: Matrix
	gdb Matrix

Gauss: Gauss.class Main.class
	scala Main
Gauss.class: Gauss.scala
	fsc Gauss.scala 

clean:
	rm -f *~ *.class *.o Matrix


