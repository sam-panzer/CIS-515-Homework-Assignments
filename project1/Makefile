GTK=`pkg-config --cflags --libs gtk+-2.0`
EXEC=Matrix
OPTS=-Wall -O2

CC=clang++

$(EXEC): Gauss.h Matrix.cpp Comparisons.cpp
	$(CC) -o Matrix Matrix.cpp $(OPTS) $(GTK)
	$(CC) -o Comparisons Comparisons.cpp $(OPTS) $(GTK)

debug: $(EXEC)
	gdb Matrix

Gauss: Gauss.class Main.class
	scala Main

clean:
	rm -f *~ *.o Matrix


