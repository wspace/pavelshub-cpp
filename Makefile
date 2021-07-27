all: wspace assembler

assembler: assembler.o
	g++ -o assembler assembler.o
assembler.o: assembler.cpp
	g++ -ggdb -Wall -c assembler.cpp

wspace: wspace.o
	g++ -o wspace wspace.o
wspace.o: wspace.cpp
	g++ -ggdb -Wall -c wspace.cpp

clean:
	rm -f wspace.o assembler.o
