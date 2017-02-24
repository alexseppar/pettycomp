CXXOPT =-std=c++11 -Wall -g3 ${ADD} 

all: compile

compile: main.o tree_node.o parser.o interpretator.o hash_table.o lexer.o
	$(CXX) $(CXXOPT) main.o tree_node.o parser.o interpretator.o hash_table.o lexer.o -o calc
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
lexer.o: lexer.cpp
	$(CXX) -c $(CXXOPT) lexer.cpp
all_test:
	$(CXX) check.cpp -o check
	cp answers.txt answ.txt
	for i in 0 1 2 3 4 5 6 7 8 9; do ./calc ./test$$i.txt >> out$$i.txt; ./check out$$i.txt $$i; rm out$$i.txt; done
	rm answ.txt 
clean:
	rm -rf *.o

