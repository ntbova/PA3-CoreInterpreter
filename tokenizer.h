using namespace std;

class Tokenizer
{
public:
	Tokenizer () 
	{

	}

	list<int> lineNumbers;
	list<string> fileTokens;
	list<string> stringTokens;

	int currentLineNumber = 1;

	int currentLine();
	string currentToken();
	void nextToken();
	int processLine(string line);
	string addWhiteSpace(string word);
	int processIdentifier(string word);
	int processInteger(string word);

private:

	int processWord(string word);

	//The set of all reserved and special words in CORE
	const string Reserved [13] =
	{ "program", "begin", "end", "int", "if", "then", "else", "while"
	,"loop", "read", "write", "and", "or"}; 
	const string Special [17] = 
	{ ";", ",", "=", "!", "[", "]" ,"(", ")", "+", "-"
	, "*", "!=", "==", ">=", "<=", ">", "<"};
};