// // node.h - contains interfaces for all node classes
// #include <string>
// #include <list>
// #include "tokenizer.h"
#define NUM_TERMS 40

using namespace std;

class ProgramNode;
class DeclSeqNode;
class StmtSeqNode;
class DeclNode;
class IDListNode;
class StmtNode;
class AssignNode;
class IfNode;
class LoopNode;
class InNode;
class OutNode;
class CondNode;
class CompNode;
class ExpNode;
class TermNode;
class FacNode;
class CompOpNode;
class IdNode;
class LetSeqNode;
class LetNode;
class IntNode;
class DigitNode;

class Node
{
public:

	bool startParse(Tokenizer *t);
	
	void startPrint();

	void startExecute();

protected:

	string TerminalErrors [NUM_TERMS] = 
	{
	"Ignore Me lol.\n", // 0
	"Missing terminal 'program' in Program Node\n", // 1
	"Missing terminal 'begin' in Program Node\n", // 2
	"Missing terminal 'end' in Program Node\n", // 3
	"Missing terminal 'int' in Declaration Node\n", // 4
	"Missing terminal 'if'\n", // 5
	"Missing terminal 'then'\n", // 6
	"Missing terminal 'else'\n", // 7
	"Missing terminal 'while'\n", // 8
	"Missing terminal 'loop'\n", // 9
	"Missing terminal 'read'\n", // 10
	"Missing terminal 'write'\n", // 11
	"Missing terminal 'and'\n", // 12
	"Missing terminal 'or'\n", // 13
	"Missing terminal ';'\n", // 14
	"Missing terminal ','\n", // 15
	"Missing terminal '='\n", // 16
	"Missing terminal '!'\n", // 17
	"Missing terminal '['\n", // 18
	"Missing terminal ']'\n", // 19
	"Missing terminal '('\n", // 20
	"Missing terminal ')'\n", // 21
	"Missing terminal '+'\n", // 22
	"Missing terminal '-'\n", // 23
	"Missing terminal '*'\n", // 24
	"Missing terminal '!=\'\n", // 25
	"Missing terminal '=='\n", // 26
	"Missing terminal '>='\n", // 27
	"Missing terminal '<='\n", // 28
	"Missing terminal '>'\n", // 29
	"Missing terminal '<'\n", // 30
	"Invalid use of integer\n", // 31
	"Invalid use of identifier\n", // 32
	"Missing 'and' or 'or' in [] brackets of conditional\n", // 33
	"Invalid conditional statement\n", // 34
	"Invalid factorial for assign statement\n", // 35
	"Invalid comparison operation\n", // 36
	"Junk tokens after final program 'end'.\n", // 37
	"Identifier declared more than once.", // 38
	"Identifier is uninitialized." // 39
	
	};

	static string padding;

	static map<string,int> symtab; // symbol table used during execution

	bool checkTerminal(string token, Tokenizer *t);
	
	void terminateReport(Tokenizer *t, int errorNumber);

	void terminateReport(int errorNumber);
	
	ProgramNode *pn = NULL;

};
class ProgramNode: public Node
{
public:
	ProgramNode() 
	{
		
	}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:

	// NOTE: 
	DeclSeqNode *ds = NULL;
	StmtSeqNode *ss = NULL;
};
class DeclSeqNode: public Node
{
public:

	DeclSeqNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:
	int alt;

	// NOTE: Any instance of a class containing an instance of itself will use pointers to the reference
	DeclSeqNode *ds = NULL; 
	DeclNode *dec = NULL;	
};
class StmtSeqNode: public Node
{
public:
	StmtSeqNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:
	int alt;

	StmtNode *stmt = NULL;
	StmtSeqNode *ss = NULL;
};
class DeclNode: public Node
{
public:
	DeclNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:

	IDListNode *idl = NULL;
	
};
class IDListNode: public Node
{
public:
	IDListNode() {}

	void parse(Tokenizer *t, bool addToTable); // Returns string DeclNode, determine if duplicates exist

	void print();

	void execute(int exectype);

private:
	int alt;

	IdNode *idn = NULL;
	IDListNode *idl = NULL;	
};
class StmtNode: public Node
{
public:
	StmtNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:

	int alt;

	AssignNode *ass = NULL;
	IfNode *ifn = NULL;
	LoopNode *loop = NULL;
	InNode *in = NULL;
	OutNode *out = NULL;
	
};
class AssignNode: public Node
{
public:
	AssignNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:
	IdNode *id = NULL;
	ExpNode *exp = NULL;
};
class IfNode: public Node
{
public:
	IfNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:
	int alt;
	
	CondNode *cond = NULL;
	StmtSeqNode *ssIf = NULL, *ssElse = NULL;
};
class LoopNode: public Node
{
public:
	LoopNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:

	CondNode *cond = NULL;
	StmtSeqNode *ss = NULL;
};
class InNode: public Node
{
public:
	InNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:

	IDListNode *idl = NULL;
	
};
class OutNode: public Node
{
public:

	OutNode() {}

	void parse(Tokenizer *t);

	void print();

	void execute();

private:
	
	IDListNode *idl = NULL;
};
class CondNode: public Node
{
public:
	CondNode() {}

	void parse(Tokenizer *t);

	void print();

	bool execute();

private:
	int alt;

	CompNode *comp = NULL;
	CondNode *cond1 = NULL, *cond2 = NULL;
};
class CompNode: public Node
{
public:
	CompNode() {}

	void parse(Tokenizer *t);

	void print();

	bool execute();

private:

	FacNode *fac1 = NULL, *fac2 = NULL;
	CompOpNode *compop = NULL;
	
};
class ExpNode: public Node
{
public:
	ExpNode() {}

	void parse(Tokenizer *t);

	void print();

	int execute();

private:
	int alt;

	TermNode *term = NULL;
	ExpNode *exp = NULL;
};
class TermNode: public Node
{
public:
	TermNode() {}

	void parse(Tokenizer *t);

	void print();

	int execute();

private:
	int alt;

	FacNode *fac = NULL;
	TermNode *term = NULL; 
};
class FacNode: public Node
{
public:
	FacNode() {}

	void parse(Tokenizer *t);

	void print();

	int execute();

private:
	int alt;

	IntNode *intn = NULL;
	IdNode *id = NULL;
	ExpNode *exp = NULL;
	
};
class CompOpNode: public Node
{
public:
	CompOpNode() {}

	void parse(Tokenizer *t);

	void print();

	int execute();

private:

	int alt;
	
};
class IdNode: public Node
{
public:
	IdNode() {}

	string parse(Tokenizer *t);

	void print();

	string executeIn();

	string executeOut();

private:

	string idName;
	
};

class IntNode: public Node
{
public:
	IntNode() {}

	void parse(Tokenizer *t);

	void print();

	int execute();

private:

	int intValue;
	
};
