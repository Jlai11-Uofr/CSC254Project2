# CSC254Project2

Completed by: Sizhe Li (sli96) & Jonathan Lai

-------------------------------------
Dear TAs:

Our project consists of five basic parts as denoted by the project page.
https://www.cs.rochester.edu/courses/254/fall2021/assignments/syntax_error_recovery.shtml

### 1. Translate into C++
- This was done by replacing stdio and stlib libaries in both the scan.cpp and parse.cpp with their equialvent expressions in C++. 
- The g++ compiler on the csug network was also now used to build the project using the makefile.


### 2. Extending the calculator language

- New routines such as relation, relation_op, expr_tail were added to the recurrent descent parser for the extended calculator langauge.
- To accomdate if / do in the lanauge and check. The routine statement had to be also extended to support this.
- As a result of new langauges being needed to be supported, these enums were added and then had to be succesfully scanned by the scanner. We do so by exntending the symbols the scanner is able to accept.
- Specifically, for the parser, EPS, FIRST, and FOLLOW sets were derived by hand. We then use them to change recursive descent parser to adapt to the extended language. Following the example illustrated by Figure 2.17 in the book.


### 3. Lexical error recovery
- A lexical error can be informally defined as a character that is not reconized by the scanner (common occurence is because of user error). For example, our extended calculator lanauge does not accept !, and as such as our scanner need not scan this. As a result, given our starter code, it would thrown an error and exit the program. However lexical errors should fixed so we could idenfify all other errors whether they be lexical or syntax. This can be done in a simple way. In any step we run into a lexical error, we simply choose to ignore it in the scanner. and restart the scanning algorthim using recursion.
- If this error is simple such as check 2 <? 2. simply removing the ? allows us to have a valid statement.


### 4. Wirths syntax error recovery 
Using the EPS, FIRST, and FOLLOW sets derived earlier. We can very easily implement the Wirths syntax error recovery program following the pseudocode provided in the example 2.45 of the companion site. Specifically, we have constructed the check_for_error function, as illustrated in the book. Specifically, if the parsing routine for nonterminal discovers an error at the beginning of its code, it deletes incoming tokens until it finds a member of FIRST(nonterminal), in which case it proceeds, or a member of FOLLOW(nonterminal), in which case it returns.


### 5. Syntax tree output

- We chose to not store an explicit syntax tree structure for simplicity. We utilize the recurrent descent parser program to grow the tree on the fly.
- We do so by recursively growing the syntax tree within each subroutine, stored in a string under the linear, parenthesized form.
- Upon seeing an error, we would output syntax error messages instead of the syntax tree.

### 6. How to run 
- Please run "make" first to compile the program.
- Next, please run ./parse to start the program. Enter any valid input and then terminate with eof with ctrld. 
- For quick testing, we have provided two test cases (one for syntax error and one for correct syntax tree).
- For testing syntax error, please run "./parse < test_syntax_error.txt". 
- For testing correct syntax tree output, please run "./parse < test_correct_syntax_tree.txt"