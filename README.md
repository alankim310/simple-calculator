# Simple Calculator

A small command-line expression calculator implemented in C++.

This project implements a simple calculator that reads expressions from standard input and writes results to standard output. It supports basic arithmetic, grouping, unary operators, factorial, two functions (sqrt and pow), variables and constants.

## Features

- Basic arithmetic: `+`, `-`, `*`, `/`
- Grouping: use `( )` for inner grouping and `{ }` for outer grouping
- Unary operators: `+x` (positive), `-x` (negation)
- Factorial: append `!` to an integer literal (e.g. `5!`)
- Functions:
  - `sqrt(x)` — square root (error on negative radicand)
  - `pow(x,i)` — x raised to integer i (i is taken as integer)
- Variables: declare with `let` (e.g. `let x = 3.5`)
- Constants: declare immutable with `let const` (e.g. `let const pi = 3.14`)
- Special predefined name: `k = 1000` (pre-populated in the program)
- Input/Output control:
  - `;` — print / evaluate the current expression
  - `q` or `quit` — exit the calculator


You will see a prompt `> ` where you can type expressions. Terminate an expression with `;` to see the result.

## Usage examples

- Arithmetic and grouping:
  > (2 + 3) * 4 ;
  = 20

- Factorial:
  > 5! ;
  = 120

- pow and sqrt:
  > pow(2,3) ;
  = 8
  > sqrt(25) ;
  = 5

- Variables and constants:
  > let a = 2 ;
  = 2
  > a * 3 ;
  = 6
  > let const pi = 3.14159 ;
  = 3.14159
  # attempting to set a constant later will produce an error

- Quit:
  > q
  # or
  > quit

## Notes & limitations

- `pow(x,i)` treats the second parameter as an integer (truncating/using integer value).
- Factorial is implemented for non-negative integer literals. Behavior on non-integer inputs depends on how the tokenization yields `number` tokens.
- The program reports runtime errors (e.g. division by zero, undefined names) and then discards input until the next `;`.

If you'd like, I can also:
- Add commonly used constants (pi, e) into the program automatically
- Add a `help` command that prints this README text in the program
- Add `%` (modulus) support using `fmod`

