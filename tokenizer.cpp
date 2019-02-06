#include <iostream> // used for cout/cin
#include <fstream> // uses ifstream to get text from listed file
#include <list> // used for fileStrings list in Tokenizer
#include <set> // used for ReservedSpecial set in Tokenizer
#include <string> // Used to store lines/words as strings
#include "tokenizer.h" // Tokenizer interface

using namespace std;

string Tokenizer::currentToken()
{
	return stringTokens.front();	
}

int Tokenizer::currentLine()
{
	return lineNumbers.front();
}

void Tokenizer::nextToken()
{
	// Will work up until the EOF token is reached
	if ((stringTokens.front()).compare("33EOF") != 0)
	{
		fileTokens.pop_front();
		stringTokens.pop_front();
		lineNumbers.pop_front();
	}
}

int Tokenizer::processLine(string line)
{
	string tToken = ""; // word token
	int valid = 1; 
	
	for (int i = 0; i < line.length() && valid; i++)
	{
		if (!iswspace(line.at(i))) // Check to see if current character is whitespace
		{
			tToken += line.at(i); // String builds the current word
		}
		else if (tToken.length() > 0) // Check to see if the temporary token has anything to process
		{
			valid = processWord(tToken);
			tToken = "";
		}

	}

	// Check tToken one last time (for words at the very end of line)
	if (tToken.length() > 0)
	{
		// Add whitespace padding between special characters here

		string word = addWhiteSpace(tToken);

		if (word.compare(tToken) != 0)
		{
			processLine(word);
		}

		valid = processWord(tToken);
		tToken = "";
	}



	return valid;
}

string Tokenizer::addWhiteSpace(string word)
{
	string finalWord = "";

	// Check for Double character Special symbols first
	for (int i = 0; i < word.length(); i++)
	{
		int isDouble = 0; // flag for whether substring is a double special
		int isSingle = 0;
		string sample = word.substr(i, 2);

		// Check only the Special double characters first
		for (int j = 11; j < 15 && i < (word.length() - 1); j++)
		{
			if (sample.compare(Special[j]) == 0)
			{
				finalWord += " ";
				finalWord += sample;
				finalWord += " ";
				
				isDouble = 1;
				i++;

				break;
			}

		}

		if (!isDouble)
		{
			sample = word.at(i);
		}

		// Next, check for Single character Special symbols
		for (int j = 0; j < 17 && !isDouble; j++)
		{
			if (sample.compare(Special[j]) == 0)
			{
				finalWord += " ";
				finalWord += sample;
				finalWord += " ";	

				isSingle = 1;

				break;				
			}

		}


		if (!isDouble && !isSingle)
		{
			finalWord += sample;
		}
		
	}


	return finalWord;
}

int Tokenizer::processInteger(string word)
{
	int valid = 1;

	if (word.length() <= 8)
	{
		for (int i = 0; i < word.length() && valid; i++)
		{
			if (!isdigit(word.at(i)))
			{
				valid = 0;
			}
		}

	}
	else
	{
		valid = 0;
	}

	return valid;
}

int Tokenizer::processIdentifier(string word)
{
	int valid = 1;
	int firstIsUpper = 0;
	int atNum = 0;

	if (word.length() <= 8)
	{
		
		for (int i = 0; i < word.length() && valid; i++)
		{
			if (isdigit(word.at(i)) && !firstIsUpper)
			{
				valid = 0;
			}
			else if (isdigit(word.at(i)))
			{
				atNum = 1;
			}
			else if (!isupper(word.at(i)))
			{
				valid = 0;
			}
			else if (isupper(word.at(i)) && atNum)
			{
				valid = 0;
			}
			else
			{
				if (i == 0)
				{
					firstIsUpper = 1;
				}
			}
		}

	}
	else
	{
		valid = 0;
	}

	return valid;
}

int Tokenizer::processWord(string word)
{
	// valid gets returned 0 if an invalid token is processed
	// if token is valid, then valid will equal 1
	int valid = 0; int isSpecialDouble = 0;

	// Check if it's an integer or identifier on its own
	if (processInteger(word)) 
	{
		fileTokens.push_back(to_string(31));
		stringTokens.push_back(word);
		valid = 1;
	}
	// Check with regex, and ensure length is correct for identifier
	// ...and yes I did in fact give up trying to address string length
	// in the regex and just did in code so there...
	else if (processIdentifier(word))
	{
		fileTokens.push_back(to_string(32));
		stringTokens.push_back(word);
		valid = 1;
	}
	else if (word.compare("eof") == 0)
	{
		fileTokens.push_back(to_string(33));
		stringTokens.push_back("33EOF"); // Invalid Identifier for EOF
		valid = 1;
	}
	else
	{
		// Check for Reserved tokens
		for (int i = 0; i < 13; i++)
		{
			// Reserved tokens should always be by themselves
			// so a direct comparsion is needed
			if (word.compare(Reserved[i]) == 0)
			{
				fileTokens.push_back(to_string(i+1));
				stringTokens.push_back(word);
				valid = 1;
			}

		}
		// Check for Special tokens
		// Specifically, check for double character special tokens first
		// so they don't get broken up into their single character counterparts
		// by mistake
		for (int i = 11; i < 15; i++)
		{
			// Check if a Special token is by itself
			if (word.compare(Special[i]) == 0)
			{
				// i + 14 relates to the specific token number in the array
				fileTokens.push_back(to_string(i+14));
				stringTokens.push_back(word);
				valid = 1;
				isSpecialDouble = 1;
			}


		}
		// Now, go ahead and check for the rest of the Special tokens
		// Also check to make sure a double character special hasn't been
		// processed already
		for (int i = 0; i < 17 ; i++)
		{
			if (i < 11 || i > 14) { // Don't recheck double speicals

				if (word.compare(Special[i]) == 0)
				{
					fileTokens.push_back(to_string(i+14));
					stringTokens.push_back(word);
					valid = 1;
				}

			}
		}

	}


	// If not valid at this point, report on the word that is not valid
	if (!valid)
	{
		cout << "\nError: [Line " << to_string(currentLineNumber) 
		<< "] Invalid ID token '" << word << "'\n\n"; 
		
	}
	else
	{
		lineNumbers.push_back(currentLineNumber);
	}

	return valid;
}

// class Tokenizer
// {

// 	public:

// 		Tokenizer()
// 		{

// 		}

// 		list<string> fileTokens; // initial string set gets tokenized
// 		list<string> stringTokens;

// 		// current file line number being processed
// 		// used for error reporting
// 		int currentLineNumber = 1;

// 		// Returns the current token as a string. 
// 		// repeated calls return the same value
// 		string currentToken()
// 		{
// 			return stringTokens.front();
// 		}

// 		// Advances to the next token
// 		void nextToken()
// 		{
// 			// Will work up until the EOF token is reached
// 			if ((stringTokens.front()).compare("33EOF") != 0)
// 			{
// 				fileTokens.pop_front();
// 				stringTokens.pop_front();
// 			}
// 		}

// 		// Takes a line from the ifstream and finds all valid tokens
// 		// within the line. Also reports errors on invalid tokens it finds
// 		int processLine(string line)
// 		{
// 			string tToken = ""; // word token
// 			int valid = 1; 
			
// 			for (int i = 0; i < line.length() && valid; i++)
// 			{
// 				if (!iswspace(line.at(i))) // Check to see if current character is whitespace
// 				{
// 					tToken += line.at(i); // String builds the current word
// 				}
// 				else if (tToken.length() > 0) // Check to see if the temporary token has anything to process
// 				{
// 					valid = processWord(tToken);
// 					tToken = "";
// 				}

// 			}

// 			// Check tToken one last time (for words at the very end of line)
// 			if (tToken.length() > 0)
// 			{
// 				// Add whitespace padding between special characters here

// 				string word = addWhiteSpace(tToken);

// 				if (word.compare(tToken) != 0)
// 				{
// 					processLine(word);
// 				}

// 				valid = processWord(tToken);
// 				tToken = "";
// 			}



// 			return valid;
// 		}

// 		// Adds white space around special characters for each line.
// 		// Called before processLine
// 		// Handles edge cases with Special tokens that are two characters long
// 		// being next to Special tokens that are one character long, and so on
// 		string addWhiteSpace(string word)
// 		{
// 			string finalWord = "";

// 			// Check for Double character Special symbols first
// 			for (int i = 0; i < word.length(); i++)
// 			{
// 				int isDouble = 0; // flag for whether substring is a double special
// 				int isSingle = 0;
// 				string sample = word.substr(i, 2);

// 				// Check only the Special double characters first
// 				for (int j = 11; j < 15 && i < (word.length() - 1); j++)
// 				{
// 					if (sample.compare(Special[j]) == 0)
// 					{
// 						finalWord += " ";
// 						finalWord += sample;
// 						finalWord += " ";
						
// 						isDouble = 1;
// 						i++;

// 						break;
// 					}

// 				}

// 				if (!isDouble)
// 				{
// 					sample = word.at(i);
// 				}

// 				// Next, check for Single character Special symbols
// 				for (int j = 0; j < 17 && !isDouble; j++)
// 				{
// 					if (sample.compare(Special[j]) == 0)
// 					{
// 						finalWord += " ";
// 						finalWord += sample;
// 						finalWord += " ";	

// 						isSingle = 1;

// 						break;				
// 					}

// 				}


// 				if (!isDouble && !isSingle)
// 				{
// 					finalWord += sample;
// 				}
				
// 			}


// 			return finalWord;
// 		}
		
// 		int processIdentifier(string word)
// 		{
// 			int valid = 1;
// 			int firstIsUpper = 0;
// 			int atNum = 0;

// 			if (word.length() <= 8)
// 			{
// 				for (int i = 0; i < word.length() && valid; i++)
// 				{
// 					if (isdigit(word.at(i)) && !firstIsUpper)
// 					{
// 						valid = 0;
// 					}
// 					else if (isdigit(word.at(i)))
// 					{
// 						atNum = 1;
// 					}
// 					else if (!isupper(word.at(i)))
// 					{
// 						valid = 0;
// 					}
// 					else if (isupper(word.at(i)) && atNum)
// 					{
// 						valid = 0;
// 					}
// 					else
// 					{
// 						if (i == 0)
// 						{
// 							firstIsUpper = 1;
// 						}
// 					}
// 				}

// 			}
// 			else
// 			{
// 				valid = 0;
// 			}

// 			return valid;
// 		}

// 		int processInteger(string word)
// 		{
// 			int valid = 1;

// 			if (word.length() <= 8)
// 			{
// 				for (int i = 0; i < word.length() && valid; i++)
// 				{
// 					if (!isdigit(word.at(i)))
// 					{
// 						valid = 0;
// 					}
// 				}

// 			}
// 			else
// 			{
// 				valid = 0;
// 			}

// 			return valid;
// 		}

// 	private:


// 		int processWord(string word)
// 		{
// 			// valid gets returned 0 if an invalid token is processed
// 			// if token is valid, then valid will equal 1
// 			int valid = 0; int isSpecialDouble = 0;

// 			// Check if it's an integer or identifier on its own
// 			if (processInteger(word)) 
// 			{
// 				fileTokens.push_back(to_string(31));
// 				stringTokens.push_back(word);
// 				valid = 1;
// 			}
// 			// Check with regex, and ensure length is correct for identifier
// 			// ...and yes I did in fact give up trying to address string length
// 			// in the regex and just did in code so there...
// 			else if (processIdentifier(word))
// 			{
// 				fileTokens.push_back(to_string(32));
// 				stringTokens.push_back(word);
// 				valid = 1;
// 			}
// 			else if (word.compare("eof") == 0)
// 			{
// 				fileTokens.push_back(to_string(33));
// 				stringTokens.push_back("33EOF"); // Invalid Identifier for EOF
// 				valid = 1;
// 			}
// 			else
// 			{
// 				// Check for Reserved tokens
// 				for (int i = 0; i < 13; i++)
// 				{
// 					// Reserved tokens should always be by themselves
// 					// so a direct comparsion is needed
// 					if (word.compare(Reserved[i]) == 0)
// 					{
// 						fileTokens.push_back(to_string(i+1));
// 						stringTokens.push_back(word);
// 						valid = 1;
// 					}

// 				}
// 				// Check for Special tokens
// 				// Specifically, check for double character special tokens first
// 				// so they don't get broken up into their single character counterparts
// 				// by mistake
// 				for (int i = 11; i < 15; i++)
// 				{
// 					// Check if a Special token is by itself
// 					if (word.compare(Special[i]) == 0)
// 					{
// 						// i + 14 relates to the specific token number in the array
// 						fileTokens.push_back(to_string(i+14));
// 						stringTokens.push_back(word);
// 						valid = 1;
// 						isSpecialDouble = 1;
// 					}


// 				}
// 				// Now, go ahead and check for the rest of the Special tokens
// 				// Also check to make sure a double character special hasn't been
// 				// processed already
// 				for (int i = 0; i < 17 ; i++)
// 				{
// 					if (i < 11 || i > 14) { // Don't recheck double speicals

// 						if (word.compare(Special[i]) == 0)
// 						{
// 							fileTokens.push_back(to_string(i+14));
// 							stringTokens.push_back(word);
// 							valid = 1;
// 						}

// 					}
// 				}

// 			}


// 			// If not valid at this point, report on the word that is not valid
// 			if (!valid)
// 			{
// 				cout << "\nError: [Line " << to_string(currentLineNumber) 
// 				<< "] Invalid ID token '" << word << "'\n\n"; 
				
// 			}

// 			return valid;
// 		}



	 
// 		//The set of all reserved and special words in CORE
// 		const string Reserved [13] =
// 		{ "program", "begin", "end", "int", "if", "then", "else", "while"
// 		,"loop", "read", "write", "and", "or"}; 
// 		const string Special [17] = 
// 		{ ";", ",", "=", "!", "[", "]" ,"(", ")", "+", "-"
// 		, "*", "!=", "==", ">=", "<=", ">", "<"};
// };

// int main(int argc, char *argv[])
// {
// 	string line;
// 	// cout << "\nUsing file " << argv[1] << ":\n";

// 	// open the file listed in the program argument
// 	ifstream infile(argv[1]);

// 	Tokenizer fileTokenizer;

// 	//valid value. Sets to 0 if an invalid token is discovered
// 	int valid = 1;

// 	while (getline(infile,line) && valid)
// 	{
// 		// cout << line << '\n';
// 		valid = fileTokenizer.processLine(fileTokenizer.addWhiteSpace(line));
// 		fileTokenizer.currentLineNumber += 1;
// 		// DEBUG CODE: check to see if line is valid
// 		// cout << to_string(valid) << "\n\n";
// 	}

// 	// If file processed correctly, go ahead and add EOF (33) token
// 	if (valid)
// 	{
// 		valid = fileTokenizer.processLine("eof");

// 		// cout << to_string(valid) << "\n\n";
// 	}

// 	// Display tokens after each line has been processed
// 	// Up until the EOF token has been reached
// 	while (valid && ((fileTokenizer.currentToken()).compare("33EOF") != 0))
// 	{
// 		cout << "\n" << fileTokenizer.currentToken();
// 		fileTokenizer.nextToken();

// 		// Acount for EOF token before loop ends
// 		// if (((fileTokenizer.currentToken()).compare("33EOF") == 0))
// 		// {
// 		// 	cout << "\n" << fileTokenizer.currentToken() << "\n\n";
// 		// }
// 	}

// 	infile.close();

// 	return 0;
// }

