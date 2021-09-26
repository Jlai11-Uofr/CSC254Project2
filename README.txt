# CSC254Project2

---------------------------------------------------------
Completed by: Sizhe Li (sli96) & Jonathan Lai (jlai11)
---------------------------------------------------------

Dear TAs:

Our project consists of five basic parts as denoted by the project page.
https://www.cs.rochester.edu/courses/254/fall2021/assignments/syntax_error_recovery.shtml

---------------------------------------------------------
### 1. Translate into C++
- This was done by replacing stdio and stlib libaries in both the scan.cpp and parse.cpp with their equialvent expressions in C++. 
- The g++ compiler on the csug network was also now used to build the project using the makefile.
---------------------------------------------------------

---------------------------------------------------------
### 2. Extending the calculator language
- New enums were added in scan.h to accomodate to the extended language. Correspondingly, new routines and symbols were added to the parser and scanner.
- For the parser, specifically, EPS, FIRST, and FOLLOW sets were derived by hand.
  We then use them to add new parser routines, such as relation, relation_op, expr_tail, as well as modifying the old routines to adapt to the extended language. 
  We do so by following the example illustrated by Figure 2.17 in the book.
---------------------------------------------------------


---------------------------------------------------------
### 3. Lexical error recovery
- In any step we run into a lexical error, we use the strategy as mentioned in the section 2.4 of the book. 
  Specifically, we (1) throw away the current, invalid token; 
                   (2) skip forward until a character is found that can legitimately begin a new token; 
                   (3) restart the scanning algorithm
---------------------------------------------------------

---------------------------------------------------------
### 4. Wirths syntax error recovery 
- Using the EPS, FIRST, and FOLLOW sets derived earlier.
  We can very easily implement the Wirths syntax error recovery program following the pseudocode provided in the example 2.45 of the companion site.
  Specifically, we have constructed the check_for_error function, as illustrated in the book. If the parsing routine for nonterminal discovers an error at the beginning of its code,
  it deletes incoming tokens until it finds a member of FIRST(nonterminal), in which case it proceeds, or a member of FOLLOW(nonterminal), in which case it returns.
---------------------------------------------------------

---------------------------------------------------------
### 5. Syntax tree output
- We utilize the recurrent descent parser program to grow the tree on the fly.
- We do so by recursively growing the syntax tree within each subroutine, stored in a string under the linear, parenthesized form.
- Upon seeing an error, we would output syntax error messages instead of the syntax tree.
---------------------------------------------------------

---------------------------------------------------------
### 6. How to run 
- Please run "make" first to compile the program.
- Next, for customized testing, please run "./parse" to start the program.
  Enter any valid input and then terminate with eof with ctrld. 
- For quick testing, we have provided two test cases (one for syntax error and one for correct syntax tree) for you.
  a. For testing syntax error, please run "./parse < test_syntax_error.txt". 
  b. For testing correct syntax tree output, please run "./parse < test_correct_syntax_tree.txt"
---------------------------------------------------------

Thank you for your time,

Sizhe and Jonathan