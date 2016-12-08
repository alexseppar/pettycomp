OPT=-std=c++11

all: compile run

compile: main.o tree_node.o parser.o interpretator.o hash_table.o 
	g++ $(OPT) main.o tree_node.o parser.o interpretator.o hash_table.o  -o calc
main.o: main.cpp
	g++ -c $(OPT) main.cpp
tree_node.o: tree_node.cpp
	g++ -c $(OPT) tree_node.cpp
parser.o: parser.cpp
	g++ -c $(OPT) parser.cpp
interpretator.o: interpretator.cpp
	g++ -c $(OPT) interpretator.cpp
hash_table.o: hash_table.cpp
	g++ -c $(OPT) hash_table.cpp
run: 
	./calc ${TEST} 
clean:
	rm -rf *.o ./calc
// код фигня, давай переписывай

