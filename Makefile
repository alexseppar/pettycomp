CXX_OPT += -std=c++11 -Wall

all: compile

compile: main.o tree_node.o parser.o interpretator.o hash_table.o 
	$(CXX) $(CXX_OPT) main.o tree_node.o parser.o interpretator.o hash_table.o  -o calc
main.o: main.cpp
	$(CXX) -c $(CXX_OPT) main.cpp
tree_node.o: tree_node.cpp
	$(CXX) -c $(CXX_OPT) tree_node.cpp
parser.o: parser.cpp
	$(CXX) -c $(CXX_OPT) parser.cpp
interpretator.o: interpretator.cpp
	$(CXX) -c $(CXX_OPT) interpretator.cpp
hash_table.o: hash_table.cpp
	$(CXX) -c $(CXX_OPT) hash_table.cpp
check: 
	./calc ${TEST} 
clean:
	rm -rf *.o 
