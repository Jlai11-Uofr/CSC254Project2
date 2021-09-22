/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2021.
*/
#include <iostream>
using namespace std;
#include "scan.h"

const char* names[] = {"read", "write", "id", "literal", "gets", "add",
                       "sub", "mul", "div", "lparen", "rparen", "eof"};

static token upcoming_token;

void error () {
      cerr << "Got your self a syntax Error! \n";
    exit (1);
}

void match (token expected) {
    if (upcoming_token == expected) {
        cout << "matched " << names[upcoming_token];
        if (upcoming_token == t_id || upcoming_token == t_literal)
            cout << ": " << token_image;
        cout << "\n";
        upcoming_token = scan ();
    }
    else error ();
}

void program ();
void stmt_list ();
void stmt ();
void expr ();
void term ();
void term_tail ();
void factor ();
void factor_tail ();
void add_op ();
void mul_op ();

void program () {
    switch (upcoming_token) {
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict program --> stmt_list eof\n";
            stmt_list ();
            match (t_eof);
            break;
        default: error ();
    }
}

void stmt_list () {
    switch (upcoming_token) {
        case t_id:
        case t_read:
        case t_write:
            cout << "predict stmt_list --> stmt stmt_list\n";
            stmt ();
            stmt_list ();
            break;
        case t_eof:
            cout << "predict stmt_list --> epsilon\n";
            break;          /* epsilon production */
        default: error ();
    }
}

void stmt () {
    switch (upcoming_token) {
        case t_id:
            cout << "predict stmt --> id gets expr\n" ;
            match (t_id);
            match (t_gets);
            expr ();
            break;
        case t_read:
            cout << "predict stmt --> read id\n";
            match (t_read);
            match (t_id);
            break;
        case t_write:
            cout << "predict stmt --> write expr\n";
            match (t_write);
            expr ();
            break;
        default: error ();
    }
}

void expr () {
    switch (upcoming_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict expr --> term term_tail\n";
            term ();
            term_tail ();
            break;
        default: error ();
    }
}

void term () {
    switch (upcoming_token) {
        case t_id:
        case t_literal:
        case t_lparen:
            cout << "predict term --> factor factor_tail\n";
            factor ();
            factor_tail ();
            break;
        default: error ();
    }
}

void term_tail () {
    switch (upcoming_token) {
        case t_add:
        case t_sub:
            cout << "predict term_tail --> add_op term term_tail\n";
            add_op ();
            term ();
            term_tail ();
            break;
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict term_tail --> epsilon\n";
            break;          /* epsilon production */
        default: error ();
    }
}

void factor () {
    switch (upcoming_token) {
        case t_literal:
            cout << "predict factor --> literal\n";
            match (t_literal);
            break;
        case t_id :
            cout << "predict factor --> id\n";
            match (t_id);
            break;
        case t_lparen:
            cout << "predict factor --> lparen expr rparen\n";
            match (t_lparen);
            expr ();
            match (t_rparen);
            break;
        default: error ();
    }
}

void factor_tail () {
    switch (upcoming_token) {
        case t_mul:
        case t_div:
            cout << "predict factor_tail --> mul_op factor factor_tail\n";
            mul_op ();
            factor ();
            factor_tail ();
            break;
        case t_add:
        case t_sub:
        case t_rparen:
        case t_id:
        case t_read:
        case t_write:
        case t_eof:
            cout << "predict factor_tail --> epsilon\n";
            break;          /* epsilon production */
        default: error ();
    }
}

void add_op () {
    switch (upcoming_token) {
        case t_add:
            cout <<  "predict add_op --> add\n";
            match (t_add);
            break;
        case t_sub:
            cout << "predict add_op --> sub\n";
            match (t_sub);
            break;
        default: error ();
    }
}

void mul_op () {
    switch (upcoming_token) {
        case t_mul:
            cout << "predict mul_op --> mul\n";
            match (t_mul);
            break;
        case t_div:
           cout <<"predict mul_op --> div\n";
            match (t_div);
            break;
        default: error ();
    }
}

int main () {
    upcoming_token = scan ();
    program ();
    return 0;
}