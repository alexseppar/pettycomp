CXXOPT =-std=c++11 -Wall -g3 

all: compile

compile: main.o tree_node.o parser.o interpretator.o hash_table.o 
	$(CXX) $(CXXOPT) main.o tree_node.o parser.o interpretator.o hash_table.o  -o calc
main.o: main.cpp
	$(CXX) -c $(CXXOPT) main.cpp
tree_node.o: tree_node.cpp
	$(CXX) -c $(CXXOPT) tree_node.cpp
parser.o: parser.cpp
	$(CXX) -c $(CXXOPT) parser.cpp
interpretator.o: interpretator.cpp
	$(CXX) -c $(CXXOPT) interpretator.cpp
hash_table.o: hash_table.cpp
	$(CXX) -c $(CXXOPT) hash_table.cpp
check: 
	./calc ${TEST} 
clean:
	rm -rf *.o 
