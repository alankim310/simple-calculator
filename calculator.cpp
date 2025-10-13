#include "std_lib_facilities.h"

/**
 * This program implements a basic expression calculator. 
 * Input from cin; output to cout. 
 * The grammar for input is:
 * 
 * Statement:
 * 		Expression
 * 		Print
 * 		Quit
 * 
 * Print: 
 * 		";"
 * 
 * Quit: 
 * 		"q"
 * 
 * Expression:
 * 		Term
 * 		Expression "+" Term
 * 		Expression "-" Term
 * 
 * Term:
 * 		Primary
 * 		Term "*" Primary
 * 		Term "/" Primary
 * 		Term "%" Primary
 * 
 * Primary:
 * 		Number
 * 		"(" Expression ")"
 * 		"-" Primary
 * 		"+" Primary
 * 
 * Number:
 * 		floating-point literal
 * 
 * Input comes through cin through the Token_stream called ts. 
 */

 //q does not work to quit now. 
struct Token {
    //attributes: kind, value, name
	char kind;
	double value;
	string name;

    //constructors using ch and val -- name not included
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }

	//Token case for variable declaration
	Token(char ch, string var) :kind(ch), name(var) { }
};

class Token_stream {

    //attributes

    //if full, there is a ch to refer to in the buffer. 
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();    //getting next token

	//retrieve a character back to buffer. 
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

Token Token_stream::get()
{   //if full, get the value in the buffer and turn it false. 

	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {

        //operation / syntax are read through get method
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);

        //case of number:
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	//case for numbers

	{	/*don't just get a single token but take the whole numbers 
        through switching to val as an input variable. 
        */
       cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
    break;
	}


	default:
	//case for variables!

        //if ch is an alphabet
		if (isalpha(ch)) {
			string s;
			s += ch;

            //check until there is a white space

			//fix: Don't assign ch to s. 
			//Instead, add onto preexisting characters.
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch))) s += ch; 
			
			//any nonvalid character (white space) gets retrieved. 
			cin.unget();

			if (s == "let") return Token(let);
			if (s == "quit") return Token(name);
			return Token(name, s);
		}
		error("Bad token");
	}
}

/**
 * 
 * If the token buffer is full and the buffered token kind equals the requested char c,
 * it just clears the buffer and returns. 
 * 
 * Otherwise, It clears full and then reads characters from standard input,
 * discarding them until it reads a character equal to c, then returns. 
 */
void Token_stream::ignore(char c)
{

	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch; //hasn't been initialized. Has to come back later
	while (cin >> ch)
		if (ch == c) return;
}


/**
 * Dictionary ADT
 * Variable struct maps values to its name. 
 * For example, pi = 3.1415....
 */
struct Variable {
	string name;
	double value;
	Variable(string n, double v) :name(n), value(v) { }
};

//vector of names. (needs to find where variables are getting pushed back to this vector.)
vector<Variable> names;

//get value using key. 
double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	error("get: undefined name ", s);
}

//setting a new value to a key s. 
void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}


//checks if a key exists in the vector names. 
bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("'(' expected");
	}
	case '-':
		return -primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	default:
		error("primary expected");
	}
}

double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;
	if (is_declared(name)) error(name, " declared twice");
	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);
	double d = expression();
	names.push_back(Variable(name, d));
	return d;
}

double statement()
{
	Token t = ts.get();
	switch (t.kind) {
	case let:
		return declaration();
	default:
		ts.unget(t);
		return expression();
	}
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt;
		Token t = ts.get();
		while (t.kind == print) t = ts.get();
		if (t.kind == quit) return;
		ts.unget(t);
		cout << result << statement() << endl;
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}