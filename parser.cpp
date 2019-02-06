#include <iostream> // used for cout/cin
#include <fstream> // uses ifstream to get text from listed file
#include <list> // used for fileStrings list in Tokenizer
#include <set> // used for ReservedSpecial set in Tokenizer
#include <map>
#include <string> // Used to store lines/words as strings
#include "tokenizer.h"
#include "node.h"

using namespace std;

int buildTokens(Tokenizer *fileTokenizer, char *filename)
{
	
	ifstream infile(filename);

	string line;
	int valid = 1;

	while (getline(infile,line) && valid)
	{
		valid = fileTokenizer->processLine(fileTokenizer->addWhiteSpace(line));
		fileTokenizer->currentLineNumber += 1;
	}

	// If file processed correctly, go ahead and add EOF (33) token
	if (valid)
	{
		valid = fileTokenizer->processLine("eof");
	}

	// Display tokens after each line has been processed
	// Up until the EOF token has been reached
	// NOTE: No longer needed for parser. Debug for Tokenizer
	// while (valid && ((fileTokenizer->currentToken()).compare("33EOF") != 0))
	// {
		// cout << "\n" << fileTokenizer->currentToken();
		// fileTokenizer->nextToken();
	// }

	infile.close();	

	return valid;
}


int main(int argc, char *argv[])
{

	// cout << "\nUsing file " << argv[1] << ":\n";
	// open the file listed in the program argument
	ifstream infile(argv[1]);

	Tokenizer *fileTokenizer = new Tokenizer();

	if (buildTokens(fileTokenizer, argv[1]))
	{
		Node *parent = new Node();
		
		bool parse_valid = parent->startParse(fileTokenizer);
		
		if (parse_valid)
		{
			// cout << "\nParse Successful!\n";
			
			parent->startExecute();
		}
	}



}