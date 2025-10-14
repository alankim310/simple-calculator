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
 * 		"Q"
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
 * 		Sqrt
 * 		Pow(x,i) -> Multiply x with itself i times
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
const char name_identifier = 'a';

//constant (immutable)
const char constant = 'C';

//square root identifier. 
const char square_root = 'S';

//pow function identifier
const char pow_function = 'P';

Token Token_stream::get()
{   //if full, get the value in the buffer and turn it false. 

	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	//case for quit
	case 'q':
		return Token(quit);

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
	case ',':
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
			if (s == "quit") return Token(name_identifier);

			//square root. input of sqrt will make square root token. 
			if (s == "sqrt") return Token(square_root);

			//pow function
			if (s == "pow") return Token(pow_function);

			//declaration of constant function
			if (s == "constant") return Token(constant);

			return Token(name_identifier, s);
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
	//true if the variable is constant
	bool constant;

	Variable(string n, double v) :name(n), value(v), constant(false) { }
	Variable(string n, double v, bool c) :name(n), value(v), constant(c) {}
	Variable() : name(""), value(0), constant(false) {}
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
	for (int i = 0; i < names.size(); ++i)
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

/*
 * Primary:
 * 		Number
 * 		"(" Expression ")"
 * 		"-" Primary
 * 		"+" Primary
 */

double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	
	//case for pow function
	case pow_function: {
		t = ts.get();
		if (t.kind != '(') error("Opening bracket is required");

		//first input
		double d = primary();

		//result will be stored in this variable
		double result = 1;

		//expect a comma after a number
		t = ts.get();
		if (t.kind != ',') error("Comma is missing");

		//truncate down the second input as an integer
		int i = primary();

		for (size_t n = 0; n < i; n++) {
			result *= d;
		}

		t = ts.get();

		//if no closing bracket, error
		if(t.kind != ')') error("Closing bracket is missing!");
		return result;
	}

	//case for square root
	case square_root: {
		double d = primary();
		
		//negative case
		if (d < 0)
			error("Radicand can't be negative!");

		d = sqrt(d);
		return d;
	}
	//return expression inside of bracket
	case '(':{
		double d = expression();
		t = ts.get();
		if (t.kind != ')') error("'(' expected");
		return d;
		}

	case '-':
		return -primary(); //negate number

	case '+':
		return abs(primary()); //always positive number;

	//actual number parsing happens in the Token_stream::get()
	case number:
		return t.value;
	
	case name_identifier: {
		string var_name = t.name; // save the variable name
		//look up the next token if it is =
		t = ts.get();
		//assigning new value
		if (t.kind == '=') {
			//get value after =
			double d = expression();
			set_value(var_name, d);
			return d;
		}
		//printing out variable value
		else {
			ts.unget(t);
			return get_value(var_name);
		}
	}
	
	//error case. 
	default:
		error("primary expected");
	}
}

/*
 * Term:
 * 		Primary
 * 		Term "*" Primary
 * 		Term "/" Primary
 * 		Term "%" Primary
 */

double term()
{
	//gets the leftmost number through primary
	double left = primary();
	while (true) {

		//get the next token
		Token t = ts.get();
		switch (t.kind) {


		//in case of * and /, it gets another primary and performs the operation
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}

		//The function only returns when it hits a token that's not * or / in the default case
		default:
			ts.unget(t);
			return left;
		}
	}
}

/* Expression:
 * 		Term
 * 		Expression "+" Term
 * 		Expression "-" Term
 */

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

/**
 * Checks if a declaration of variable is a constant. --> immutable. 
 * 
 * True if a variable is constant
 * False if a variable is not constant. 
 */
bool constant_declaration(Token token) {
	if (token.kind == constant) return true;
	return false;
}

/**
 * shows errors if 
 * 1) if a variable is declared already
 * 2)if t.kind != 'a' -> 'a' is represented by const name
 * 3)see if it is in format of let {variable} = {expression}
 */
void declaration_error(Token& t) {
	//if t.kind != 'a' -> 'a' is represented by const name
	if (t.kind != name_identifier) error("name expected in declaration");

	//if declared already, error
	string var_name = t.name;
	if (is_declared(var_name)) error(var_name, " declared twice");

	//see if it is in format of let {variable} = {expression}
	t = ts.get();
	if (t.kind != '=') error("= missing in declaration of ", var_name);
}

/**
 * Function that delcares a variable if it doesn't exist in the dictionary "names"
 * push back to the dictionary
 */
double declaration()
{
	Token t = ts.get();

	bool constant_variable = constant_declaration(t);

	//variable that is getting declared
	Variable declaring_variable {};

	//take the next token if a variable is constant
	//format: let const {variable} {value}
	if (constant_variable) {
		t = ts.get();
	}
	string var_name = t.name;

	declaration_error(t);

	double d = expression();

	Variable declaraing_variable(var_name, d, constant_variable);

	names.push_back(declaring_variable);
	return d;
}

//triggers declaration if the input starts with let. If not, compute an expression
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


/**
 * This function is designed to discard input until it finds a print token(;) after an error occurs. 
 */

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true) try {
		cout << prompt; //prints ">"
		Token t = ts.get();	//gets next token from input

		while (t.kind == print) t = ts.get(); //skip any semicolons

		if (t.kind == quit) return; //if token is quit, exit the function

		//put token back and evaluate expression / declaration
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

	//predefined name k meaning 1000
	Variable k("k", 1000);
	names.push_back(k);

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