
output: tokenizer.o node.o parser.o
	g++ tokenizer.o node.o parser.o -o Interpreter

tokenizer.o: tokenizer.cpp
	g++ -c -std=c++11 tokenizer.cpp

node.o: node.cpp
	g++ -c -std=c++11 node.cpp

parser.o: parser.cpp
	g++ -c -std=c++11 parser.cpp

debug:
	g++ -g -c -std=c++11 tokenizer.cpp
	g++ -g -c -std=c++11 node.cpp
	g++ -g -c -std=c++11 parser.cpp
	g++ -g tokenizer.o node.o parser.o -o Parser

clean:
	rm Interpreter tokenizer.o node.o parser.o

target: dependencies
	action