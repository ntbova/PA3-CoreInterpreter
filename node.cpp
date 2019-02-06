// node.cpp - Includes parse, print, exec, and other functions for all nodes

#include <list>
#include <string>
#include <iostream>
#include <map>
#include <climits>
#include "tokenizer.h"
#include "node.h"

//#define NUM_TERMS 30

using namespace std;

string Node::padding = "";

string pad = "  ";

map<string,int> Node::symtab;

// Notes relavant to each class:
// * print public function will only be caled after parse function in ProgramNode completes.
// If ProgramNode completes parse successfully with no errors, it is assumed that file used contains
// valid core syntax and the results can be printed successfully.

bool Node::startParse(Tokenizer *t)
{
	Node::pn = new ProgramNode();
	
	Node::pn->parse(t);
	
	return true;
}

void Node::startPrint()
{
	Node::pn->print();
}

void Node::startExecute()
{
	Node::pn->execute();
	cout << "\n";
}

bool Node::checkTerminal(string token, Tokenizer *t)
{
	return t->currentToken().compare(token) == 0;
}

void Node::terminateReport(Tokenizer *t, int errorNumber)
{
	cout << "Error on Line " << t->currentLine();
	cout << ": " << Node::TerminalErrors[errorNumber];
	exit(1);
}

void Node::terminateReport(int errorNumber)
{
	cout << "\n " << Node::TerminalErrors[errorNumber];
	exit(1);
}

void ProgramNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("program", t)) { Node::terminateReport(t, 1); } // check for "program" termial 
	t->nextToken();

	// create ds object for pointer
	ds = new DeclSeqNode();

	ds->parse(t);

	if (!Node::checkTerminal("begin", t)) { Node::terminateReport(t, 2); } // check for "begin" terminal
	t->nextToken();

	ss = new StmtSeqNode();

	ss->parse(t);

	if (!Node::checkTerminal("end", t)) { Node::terminateReport(t, 3); } // check for "end" terminal
	t->nextToken(); // should be 33EOF at this point
	
	if (!Node::checkTerminal("33EOF", t)) { Node::terminateReport(t, 37); } // terminate if more stuff after end of program
	
}

void ProgramNode::print()
{
	
	cout << "program\n";
	padding.append(pad);
	
	ds->print();
	
	cout << padding << "begin\n";
	padding.append(pad);
	
	ss->print();
	
	padding.resize(padding.size() - 2);
	cout << padding << "end\n";
	
	
}

void ProgramNode::execute()
{
	ds->execute();

	ss->execute();
}

void DeclSeqNode::parse(Tokenizer *t)
{
	// Ensure that parser actually starts on a declaration
	dec = new DeclNode();
	
	dec->parse(t);

	if (!Node::checkTerminal("begin",t)) { alt = 2; ds = new DeclSeqNode(); ds->parse(t); }
	else { alt = 1; }

}

void DeclSeqNode::print()
{
	dec->print();
	
	if (alt == 2) { ds->print(); }
	
}

void DeclSeqNode::execute()
{
	dec->execute();

	if (alt == 2) { ds->execute(); }
}

void StmtSeqNode::parse(Tokenizer *t)
{
	stmt = new StmtNode();
	
	stmt->parse(t);

	if (Node::checkTerminal("if", t)
		|| Node::checkTerminal("while", t)
		|| Node::checkTerminal("write", t)
		|| Node::checkTerminal("read", t)
		|| t->processIdentifier(t->currentToken())) { alt = 2; ss = new StmtSeqNode(); ss->parse(t); }
	else { alt = 1; }
}

void StmtSeqNode::print()
{
	stmt->print();
	
	if (alt == 2) { ss->print(); }
}

void StmtSeqNode::execute()
{
	stmt->execute();

	if (alt == 2) { ss->execute(); }
}

void DeclNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("int",t)) { Node::terminateReport(t, 4); } // check for "int" terminal
	t->nextToken();

	idl = new IDListNode();
	
	idl->parse(t, true); // true flag so that identifiers get added to table

	if (!Node::checkTerminal(";",t)) { Node::terminateReport(t, 14); } // check for ";" terminal
	t->nextToken();	
}

void DeclNode::print()
{
	cout << padding << "int ";
	
	idl->print();
	
	cout << ";\n";
}

void DeclNode::execute()
{
	idl->execute(0); // Int 1 to indicate that the execution is a declaration
}

void IDListNode::parse(Tokenizer *t, bool addToTable)
{
	idn = new IdNode();
	
	string idName = idn->parse(t); // Returns string DeclNode, determine if duplicates exist

	if (addToTable)
	{
		// Check if identifier is being declared twice.
		if(symtab.count(idName) > 0) { Node::terminateReport(t, 38); }

		symtab.insert(pair<string,int>(idName, INT_MIN));
	}

	// if a comma is present, there are more IDs in the ID list to parse
	if (Node::checkTerminal(",", t)) { alt = 2; t->nextToken(); idl = new IDListNode(); idl->parse(t, addToTable); }	
	
	else { alt = 1; }

	
}

void IDListNode::print()
{
	idn->print();
	
	if (alt == 2) { cout << ", "; idl->print(); }
}

void IDListNode::execute(int exectype)
{
	string idName;

	switch (exectype)
	{
		case 1: // Read in value for ID
			idn->executeIn();
			break;

		case 2: // Write out value for ID
			idName = idn->executeOut();

			if ( symtab.at(idName) == INT_MIN ) { terminateReport(39); }

			cout << "\n" << idName << " = " << to_string(symtab.at(idName)); 

			break;
	}	

	if (alt == 2) { idl->execute(exectype); }
}

void StmtNode::parse(Tokenizer *t)
{
	// NOTE: Might want to make the isIdentifier function public for this
	// If the current token is an identifier, parse it as an assignment
	if (t->processIdentifier(t->currentToken())) { alt = 1; ass = new AssignNode(); ass->parse(t); } // If it's an identifier, use the AssignNode parser

	else if (Node::checkTerminal("if", t)) { alt = 2; ifn = new IfNode(); ifn->parse(t); } // check if terminal is an if

	else if (Node::checkTerminal("while", t)) { alt = 3; loop = new LoopNode(); loop->parse(t); } // check if terminal is a loop

	else if (Node::checkTerminal("read", t)) { alt = 4; in = new InNode(); in->parse(t); } // check if terminal is a read

	else if (Node::checkTerminal("write", t)) { alt = 5; out = new OutNode(); out->parse(t); } // check if terminal is a write

	// else if (!Node::checkTerminal("end", t)) { Node::terminateReport(t, 30); } // NOTE: need an index for this error message
	
	// t->nextToken();
	
}

void StmtNode::print()
{
	// New Statement, New Line with Padding
	cout << padding;
	
	switch (alt)
	{
		case 1: // Assignment
			ass->print();
			break;
		case 2: // If
			ifn->print();
			break;
		case 3:
			loop->print();
			break;
		case 4:
			in->print();
			break;
		case 5:
			out->print();
			break;
	}
}

void StmtNode::execute()
{
	switch (alt)
	{
		case 1: // Assignment
			ass->execute();
			break;
		case 2: // If
			ifn->execute();
			break;
		case 3:
			loop->execute();
			break;
		case 4:
			in->execute();
			break;
		case 5:
			out->execute();
			break;
	}
}

void AssignNode::parse(Tokenizer *t)
{
	id = new IdNode();
	
	id->parse(t);

	if (!Node::checkTerminal("=", t)) { Node::terminateReport(t, 16); } // terminate if token is not '='
	t->nextToken();

	exp = new ExpNode();
	
	exp->parse(t);	

	if (!Node::checkTerminal(";", t)) { Node::terminateReport(t, 14); } // check for ";" terminal
	t->nextToken();	
}

void AssignNode::print()
{
	id->print();
	
	cout << " = ";
	
	exp->print();
	
	cout << ";\n";
}

void AssignNode::execute()
{
	string idName = id->executeOut();
	int idValue = exp->execute();

	symtab.at(idName) = idValue;
}

void IfNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("if", t)) { Node::terminateReport(t, 5); } // terminate if "if" not found
	t->nextToken();

	cond = new CondNode();
	
	cond->parse(t);

	if (!Node::checkTerminal("then", t)) { Node::terminateReport(t, 6); } // terminate if "end" not found
	t->nextToken();

	ssIf = new StmtSeqNode();
	
	ssIf->parse(t);  // parse sequence statment following the if

	if (Node::checkTerminal("else", t)) { t->nextToken(); ssElse = new StmtSeqNode(); ssElse->parse(t); alt = 2; } // check if "else" is present. If it is, parse a SeqStmt for it too
	else { alt = 1; }
	
	if (!Node::checkTerminal("end", t)) { Node::terminateReport(t, 3); } // NOTE: need proper report index here
	t->nextToken();	
	
	if (!Node::checkTerminal(";", t)) { Node::terminateReport(t, 14); } // check for ";" terminal
	t->nextToken();	
}

void IfNode::print()
{
	cout << "if ";
	
	cond->print();
	
	cout << "then\n";
	
	padding.append(pad); // Pad for statement sequence block
	
	ssIf->print();
	
	padding.resize(padding.size() - 2); // remove padding from statement sequence
	
	if (alt == 2)
	{
		cout << padding << "else\n";
		
		padding.append(pad);
	
		ssElse->print();
	
		padding.resize(padding.size() - 2);
	}
	
	cout << padding << "end;\n";
}

void IfNode::execute()
{
	bool condTrue = cond->execute();

	if (condTrue) { ssIf->execute(); }

	if (alt == 2 && !condTrue) { ssElse->execute(); }

}

void LoopNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("while", t)) { Node::terminateReport(t, 8); } // check for "while" terminal
	t->nextToken();

	cond = new CondNode();
	
	cond->parse(t);

	if (!Node::checkTerminal("loop", t)) { Node::terminateReport(t, 9); } // check for "loop" terminal
	t->nextToken();
	
	ss = new StmtSeqNode();

	ss->parse(t);

	if (!Node::checkTerminal("end", t)) { Node::terminateReport(t, 3); } // check for "end" terminal
	t->nextToken();

	if (!Node::checkTerminal(";", t)) { Node::terminateReport(t, 14); } // check for ";" terminal
	t->nextToken();
}

void LoopNode::print()
{
	cout << "while ";
	
	cond->print();
	
	cout << "loop\n";
	
	padding.append(pad);
	
	ss->print();

	padding.resize(padding.size() - 2);	
	
	cout << padding << "end;\n";
}

void LoopNode::execute()
{
	bool condTrue = cond->execute();

	while (condTrue)
	{
		ss->execute();

		condTrue = cond->execute(); // Check conditional again after previous statment section completes
	}
}

void InNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("read", t)) { Node::terminateReport(t, 10); } // check for "read" terminal
	t->nextToken();
	
	idl = new IDListNode();

	idl->parse(t, false);

	if (!Node::checkTerminal(";", t)) { Node::terminateReport(t, 14); } // check for ';' terminal
	t->nextToken();		
}

void InNode::print()
{
	cout << "read ";

	idl->print();
	
	cout << ";\n";
}

void InNode::execute()
{
	idl->execute(1);
}


void OutNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("write", t)) { Node::terminateReport(t, 11); } // check for "write" terminal
	t->nextToken();
	
	idl = new IDListNode();

	idl->parse(t, false);

	if (!Node::checkTerminal(";", t)) { Node::terminateReport(t, 14); } // check for ';' terminal
	t->nextToken();		
}

void OutNode::print()
{
	cout << "write ";
	
	idl->print();
	
	cout << ";\n";
}

void OutNode::execute()
{
	idl->execute(2);
}

void CondNode::parse(Tokenizer *t)
{
	// NOTE: Like earlier nodes, initial token terminals determine what alt to use
	if (Node::checkTerminal("(", t)) 
	{ 
		alt = 1; 
		comp = new CompNode(); 
		comp->parse(t); 
	} // If it starts with '(', parse it as a comparison
	
	else if (Node::checkTerminal("!", t)) { alt = 2; cond1 = new CondNode(); cond1->parse(t); } // cond1 parse with not
	
	else if (Node::checkTerminal("[", t)) // also check whether it's 'and' or 'or' for alt number
	{
		t->nextToken();
		
		// Go ahead and initialize here, should parse for either and or or
		cond1 = new CondNode();
		
		cond1->parse(t);
		
		// cond1->parse(t);
		
		if (Node::checkTerminal("and", t)) { t->nextToken(); cond2 = new CondNode(); alt = 3; cond2->parse(t); } // parse for and

		else if (Node::checkTerminal("or", t)) { t->nextToken(); cond2 = new CondNode(); alt = 4; cond2->parse(t); } // parse for or

		else { Node::terminateReport(t, 33); }

		// If it reaches this point without being terminated, then also parse for the second conditional
		
		if (!Node::checkTerminal("]", t)) { Node::terminateReport(t, 19); }
		
		t->nextToken();

	}

	else { Node::terminateReport(t, 34); }

	// Not checking tokens here, this is not needed
	// t->nextToken();	
}

void CondNode::print()
{	
	switch (alt)
	{
		case 1:
			comp->print();
			break;
		case 2:
			cout << "!";
			cond1->print();
			break;
		case 3:
			cout << "[ ";
			cond1->print();
			cout << "and ";
			cond2->print();
			cout << "] ";
			break;
		case 4:
			cout << "[ ";
			cond1->print();
			cout << "or ";
			cond2->print();
			cout << "] ";
			break;
	}
}

bool CondNode::execute()
{
	bool condTrue = false;
	
	switch (alt)
	{
		case 1:
			condTrue = comp->execute();
			break;
		case 2:
			condTrue = !(cond1->execute());
			break;
		case 3:
			condTrue = (cond1->execute() && cond2->execute());
			break;
		case 4:
			condTrue = (cond1->execute() || cond2->execute());
			break;
	}

	return condTrue;
}

void CompNode::parse(Tokenizer *t)
{
	if (!Node::checkTerminal("(", t)) { Node::terminateReport(t, 20); }
	t->nextToken();
	fac1 = new FacNode();
	fac1->parse(t);
	compop = new CompOpNode();
	compop->parse(t);
	fac2 = new FacNode();
	fac2->parse(t);
	if (!Node::checkTerminal(")", t)) { Node::terminateReport(t, 21); }
	t->nextToken();
}

void CompNode::print()
{
	cout << "( ";
	fac1->print();
	compop->print();
	fac2->print();
	cout << " ) ";
}

bool CompNode::execute()
{
	bool compTrue = false;
	int valueOne = fac1->execute(), valueTwo = fac2->execute();

	int comptype = compop->execute();

	switch (comptype)
	{
		case 1:
			compTrue = valueOne != valueTwo;
			break;
		case 2:
			compTrue = valueOne == valueTwo;
			break;
		case 3:
			compTrue = valueOne < valueTwo;
			break;
		case 4:
			compTrue = valueOne > valueTwo;
			break;
		case 5:
			compTrue = valueOne <= valueTwo;
			break;
		case 6:
			compTrue = valueOne >= valueTwo;
			break;
	}

	return compTrue;
}

void ExpNode::parse(Tokenizer *t)
{
	term = new TermNode();
	
	term->parse(t);
	alt = 1; // assume first alt until we find + or -

	if (Node::checkTerminal("+", t)) { alt = 2; t->nextToken(); exp = new ExpNode(); exp->parse(t); }

	else if (Node::checkTerminal("-", t)) { alt = 3; t->nextToken(); exp = new ExpNode(); exp->parse(t); }

	// Don't need to terminate even if + or - aren't found
	// else { Node::terminateReport(t, 37); }	
}

void ExpNode::print()
{

	term->print();

	if (alt == 2) { cout << " + "; exp->print(); }

	else if (alt == 3) { cout << " - "; exp->print(); }

}

int ExpNode::execute()
{
	int expValue = term->execute();

	if (alt == 2) { expValue += exp->execute(); }

	else if (alt == 3) { expValue -= exp->execute(); }

	return expValue;
}

void TermNode::parse(Tokenizer *t)
{
	fac = new FacNode();
	
	fac->parse(t);

	if (Node::checkTerminal("*", t)) { alt = 2; t->nextToken(); term = new TermNode(); term->parse(t); }

	else { alt = 1; } //t->nextToken(); - may still need this
}

void TermNode::print()
{
	fac->print();

	if (alt == 2) { cout << " * "; term->print(); }
}

int TermNode::execute()
{
	int termValue = fac->execute();

	if (alt == 2) { termValue = termValue * term->execute(); }

	return termValue;
}

void FacNode::parse(Tokenizer *t)
{
	if (Node::checkTerminal("(", t)) 
	{ 
		alt = 3;

		t->nextToken(); 

		exp = new ExpNode();
		
		exp->parse(t);

		if (!Node::checkTerminal(")", t)) { Node::terminateReport(t, 21); }
		
		t->nextToken();

	}

	// use processIdentifier function in the Tokenizer to see if current token is an Identifier
	else if (t->processIdentifier(t->currentToken())) { alt = 2; id = new IdNode(); id->parse(t); }

	else if (t->processInteger(t->currentToken())) { alt = 1; intn = new IntNode(); intn->parse(t); }

	else { Node::terminateReport(t, 35); }	
}

void FacNode::print()
{
	
	switch (alt)
	{
		case 1:
			intn->print();
			break;
		case 2:
			id->print();
			break;
		case 3:
			cout << "( ";
			exp->print();
			cout << " )";
			break;
	}

}

int FacNode::execute()
{
	int value;
	
	switch (alt)
	{
		case 1:
			value = intn->execute();
			break;
		case 2:
			// NOTE: May need to check if value is still null here
			value = symtab.at(id->executeOut());
			break;
		case 3:
			value = exp->execute();
			break;
	}
	
	return value;
}


void CompOpNode::parse(Tokenizer *t)
{
	if (Node::checkTerminal("!=", t)) { alt = 1; }

	else if (Node::checkTerminal("==", t)) { alt = 2; }

	else if (Node::checkTerminal("<", t)) { alt = 3; }

	else if (Node::checkTerminal(">", t)) { alt = 4; }

	else if (Node::checkTerminal("<=", t)) { alt = 5; }

	else if (Node::checkTerminal(">=", t)) { alt = 6; }

	else { Node::terminateReport(t, 36); }
	
	t->nextToken();
}

void CompOpNode::print()
{
	switch (alt)
	{
		case 1:
			cout << " != ";
			break;
		case 2:
			cout << " == ";
			break;
		case 3:
			cout << " < ";
			break;
		case 4:
			cout << " > ";
			break;
		case 5:
			cout << " <= ";
			break;
		case 6:
			cout << " >= ";
			break;
	}


}

int CompOpNode::execute()
{
	return alt;
}

string IdNode::parse(Tokenizer *t)
{
	if (t->processIdentifier(t->currentToken())) { idName = t->currentToken(); t->nextToken(); }

	else { Node::terminateReport(t, 32); }

	return idName;
}

void IdNode::print()
{
	cout << idName;

}

string IdNode::executeIn()
{
	int idValue = 0;

	cout << "\n" << idName << " =? ";
	cin >> idValue;

	symtab.at(idName) = idValue;

	return idName;
}

string IdNode::executeOut()
{
	return idName;
}

void IntNode::parse(Tokenizer *t)
{
	if (t->processInteger(t->currentToken())) { intValue = stoi(t->currentToken(), NULL,  10); t->nextToken(); }

	else { Node::terminateReport(t, 31); }	
}

void IntNode::print()
{
	cout << to_string(intValue);

}

int IntNode::execute()
{
	return intValue;
}
