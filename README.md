# CSC254Project2
CSC 254 Programming Language Design and Implementation, Fall 2021
Instructor: Michael Scott


Sizhe ( Lester ) Li & Jonathan Lai

Our project consists of five basic parts as denoted by the project page.
https://www.cs.rochester.edu/courses/254/fall2021/assignments/syntax_error_recovery.shtml

1.

The first step of our project was to convert our starter code that was written in C to a C++ Implementation in which no calls to C libraries are made for input / output.
This was done by replacing stdio and stlib libaries in both the scan.cpp and parse.cpp with their equialvent expressions in C++. 
The g++ compiler on the csug network was also now used to build the project using the makefile.


2. 

The second step of our project was to extend the calculator Language beyond the basic version that was given to us in the textbook.
New routines such as relation, relation_op, expr_tail were added to the recurrent descent parser to extend the lanauge for the extended calculator langauge.
To accomdate if / do in the lanauge and check. The routine statement had to be also extended to support this.
As a result of new langauges being needed to be supported, these enums were added and then had to be succesfully scanned by the scanner. 
This is a pretty simple operation by just simply extending the symbols the scanner is able to accept.


3.
A lexical error can be informally defined as a character that is not reconized by the scanner (common occurence is because of user error). For example, our extended calculator lanauge does not accept !, and as such as our scanner
need not scan this. As a result, given our starter code, it would thrown an error and exit the program. However lexical errors should fixed so we could idenfify all other errors 
whether they be lexical or syntax. This can be done in a simple way. In any step we run into a lexical error, we simply choose to ignore it in the scanner. and restart the scanning algorthim.
If this error is simple such as check 2 <? 2. simply removing the ? allows us to have a valid statement.


4.
Wirths error syntax_error_recovery




5.
Printing the syntax tree is much easier if we do not have to store an explicity syntax tree strucutre. We can utilize the strucutre of our recurrent descent parser to grow the tree as we
go into each production. simply grow a string that is passed. This can be done by just ensuring each production returns a string and storing the result of each subroutine in a string.
The only slight differnce that is needed now is whenever we see a match, we can hardcode the token that is being matched to our string. Ex, if we match "t_read", we can simply just appened read.
As our AST can only be built correctly if there are no errors, if we see an error we do not build our AST our output it. If there is an error, it will output all of the error messages as it occurs 
and then print a general error message.


Our project can built using our makefile. Simply do make and it should produce a parse file.

Run ./parse to start the program.

Enter any valid input and then terminate with eof with ctrld. 

Alternatily you can pipe your tests in the following way by putting your test in a file (ex. test.txt ) and doiing the following.

We have provided a basic test you can run with the following command.


./parse < test.txt

test.txt contains

 read n
   cp := 2
   do check n > 0
       found := 0
       cf1 := 2
       cf1s := cf1 * cf1
       do check cf1s <= cp
           cf2 := 2
           pr := cf1 * cf2
           do check pr <= cp
               if pr == cp
                   found := 1
               fi
               cf2 := cf2 + 1
               pr := cf1 * cf2
           od
           cf1 := cf1 + 1
           cf1s := cf1 * cf1
       od
       if found == 0
           write cp
           n := n - 1
       fi
       cp := cp + 1
   od

   will result in
    (program [ (read "n")
 (:=  "cp" (    (num  "2") ))
 (do
[  (check(    >   (id  "n")   (num  "0") ))
 (:=  "found" (    (num  "0") ))
 (:=  "cf1" (    (num  "2") ))
 (:=  "cf1s" (   (   * (id  "cf1") (id  "cf1")) ))
 (do
[  (check(   <=   (id  "cf1s")   (id  "cp") ))
 (:=  "cf2" (    (num  "2") ))
 (:=  "pr" (   (   * (id  "cf1") (id  "cf2")) ))
 (do
[  (check(   <=   (id  "pr")   (id  "cp") ))
 (if
((    ==   (id  "pr")   (id  "cp") ))
[  (:=  "found" (    (num  "1") ))
]
)
 (:=  "cf2" (  (   +   (id  "cf2")  (num  "1") ) ))
 (:=  "pr" (   (   * (id  "cf1") (id  "cf2")) ))
]
)
 (:=  "cf1" (  (   +   (id  "cf1")  (num  "1") ) ))
 (:=  "cf1s" (   (   * (id  "cf1") (id  "cf1")) ))
]
)
 (if
((    ==   (id  "found")   (num  "0") ))
[  (write(    (id  "cp") ))
 (:=  "n" (  (  -   (id  "n")  (num  "1") ) ))
]
)
 (:=  "cp" (  (   +   (id  "cp")  (num  "1") ) ))
]
)
]
)


as it is a valid input

On syntax error, it will display something like this
found syntax error at FT
found syntax error at TT
found syntax error at ET
found syntax error at SL
found syntax error at matching
no ast cuz error


if we encounter a lexical error, we will see something a lexical error message.
