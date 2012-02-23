GTK=`pkg-config --cflags --libs gtk+-2.0`
EXEC=Matrix

$(EXEC): Gauss.cpp Gauss.h Matrix.cpp
	clang++ -o Matrix Matrix.cpp -Wall -g $(GTK)

debug: $(EXEC)
	gdb Matrix

Gauss: Gauss.class Main.class
	scala Main
Gauss.class: Gauss.scala
	fsc Gauss.scala 

clean:
	rm -f *~ *.class *.o Matrix


