/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2021.
*/
#include <iostream>
using namespace std;
#include "scan.h"

const char *names[] = {"read", "write", "id", "literal", "gets", "add",
                       "sub", "mul", "div", "lparen", "rparen", "eof", "if", "fi", "do", "od", "check", "==", "<>", ">", "<", ">=", "<="};

static token upcoming_token;

void error()
{
    cerr << "Got your self a syntax Error! \n";
    exit(1);
}

string answer = " ";

// Given a current token. We try and consume it. If it's Not a valid token in names, then we have to throw an error.
void match(token expected)
{
    if (upcoming_token == expected)
    {
        cout << "matched " << names[upcoming_token];
        if (upcoming_token == t_id || upcoming_token == t_literal)
            cout << ": " << token_image;
        cout << "\n";
        upcoming_token = scan();
    }
    else
    {
        error();
    }
}

void program();
void stmt_list();
void stmt();
void expr();
void term();
void term_tail();
void factor();
void factor_tail();
void add_op();
void mul_op();
void relation_op();
void expr_tail();
void relation();

void relation()
{
    switch (upcoming_token)
    {
    case t_lparen:
    case t_id:
    case t_literal:
        cout << "Relation -> E Et\n";
        expr();
        expr_tail();
        break;
    default:
        error();
    }
}

void expr_tail()
{
    switch (upcoming_token)
    {

    case t_equals:
    case t_carrot:
    case t_greater:
    case t_less:
    case t_lessE:
    case t_greaterE:
        cout << "predict  expr tail --> ro E\n";
        relation_op();
        expr();
        break;
    case t_id:
    case t_rparen:
    case t_read:
    case t_write:
    case t_if:
    case t_fi:
    case t_do:
    case t_od:
    case t_check:
    case t_eof:
    default:
        error();
    }
}

void program()
{
    //  cout << upcoming_token;
    switch (upcoming_token)
    {
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
    case t_if:
    case t_do:
    case t_check:
        cout << "predict program --> stmt_list eof\n";
        stmt_list();
        match(t_eof);
        break;
    default:
        error();
    }
}

void stmt_list()
{
    switch (upcoming_token)
    {
    case t_id:
    case t_read:
    case t_write:
    case t_check:
    case t_if:
    case t_do:
        cout << "predict stmt_list --> stmt stmt_list\n";
        stmt();
        stmt_list();
        break;
    case t_eof:
    case t_fi:
    case t_od:
        cout << "predict stmt_list --> epsilon\n";
        break; /* epsilon production */
    default:
        error();
    }
}

void stmt()
{
    // cout << upcoming_token;
    switch (upcoming_token)
    {
    case t_id:
        cout << "predict stmt --> id gets Relation\n";
        match(t_id);
        match(t_gets);
        expr();
        break;
    case t_read:
        cout << "predict stmt --> read id\n";
        match(t_read);
        match(t_id);
        break;
    case t_write:
        cout << "predict stmt --> write relation\n";
        match(t_write);
        expr();
        break;
    case t_if:
        cout << "predict stmt --> if relation SL if\n";
        match(t_if);
        relation();
        stmt_list();
        match(t_fi);
        break;
    case t_do:
        cout << "predict stmt --> do SL od\n";
        match(t_do);
        stmt_list();
        match(t_od);
        break;
    case t_check:
        cout << "predict stmt --> check relation \n";
        match(t_check);
        relation();
        break;

    default:
        error();
    }
}

void expr()
{
    switch (upcoming_token)
    {
    case t_id:
    case t_literal:
    case t_lparen:
        cout << "predict expr --> term term_tail\n";
        term();
        term_tail();
        break;
    default:
        error();
    }
}

void term()
{
    switch (upcoming_token)
    {
    case t_id:
    case t_literal:
    case t_lparen:
        cout << "predict term --> factor factor_tail\n";
        factor();
        factor_tail();
        break;
    default:
        error();
    }
}

void term_tail()
{
    switch (upcoming_token)
    {
    case t_add:
    case t_sub:
        cout << "predict term_tail --> add_op term term_tail\n";
        add_op();
        term();
        term_tail();
        break;
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_eof:
    case t_if:
    case t_do:
    case t_check:
    case t_fi:
    case t_od:
    case t_equals:
    case t_greaterE:
    case t_carrot:
    case t_lessE:
    case t_less:
    case t_greater:
        cout << "predict term_tail --> epsilon\n";
        break; /* epsilon production */
    default:
        error();
    }
}

void factor()
{
    switch (upcoming_token)
    {
    case t_literal:
        cout << "predict factor --> literal\n";
        match(t_literal);
        break;
    case t_id:
        cout << "predict factor --> id\n";
        match(t_id);
        break;
    case t_lparen:
        cout << "predict factor --> lparen expr rparen\n";
        match(t_lparen);
        expr();
        match(t_rparen);
        break;
    default:
        error();
    }
}

void factor_tail()
{
    switch (upcoming_token)
    {
    case t_mul:
    case t_div:
        cout << "predict factor_tail --> mul_op factor factor_tail\n";
        mul_op();
        factor();
        factor_tail();
        break;
    case t_add:
    case t_sub:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_greaterE:
    case t_greater:
    case t_lessE:
    case t_less:
    case t_carrot:
    case t_equals:
    case t_if:
    case t_fi:
    case t_do:
    case t_od:
    case t_check:
    case t_eof:
        cout << "predict factor_tail --> epsilon\n";
        break; /* epsilon production */
    default:
        error();
    }
}

void add_op()
{
    switch (upcoming_token)
    {
    case t_add:
        cout << "predict add_op --> add\n";
        match(t_add);
        break;
    case t_sub:
        cout << "predict add_op --> sub\n";
        match(t_sub);
        break;
    default:
        error();
    }
}

void mul_op()
{
    switch (upcoming_token)
    {
    case t_mul:
        cout << "predict mul_op --> mul\n";
        match(t_mul);
        break;
    case t_div:
        cout << "predict mul_op --> div\n";
        match(t_div);
        break;
    default:
        error();
    }
}
void relation_op()
{
    switch (upcoming_token)
    {
    case t_equals:
        cout << "predict rop -- > eqauls\n";
        match(t_equals);
        break;
    case t_carrot:
        cout << "predict rop --> <>\n";
        match(t_carrot);
        break;
    case t_greater:
        cout << "predict rop --> >\n";
        match(t_greater);
        break;
    case t_less:
        cout << "predict rop --> <\n";
        match(t_less);
        break;
    case t_lessE:
        cout << "predict rop --> <=\n";
        match(t_lessE);
        break;
    case t_greaterE:
        cout << "predict rop --> >=\n";
        match(t_greaterE);
        break;
    default:
        error();
    }
}

int main()
{
    upcoming_token = scan();
    program();
    cout << "hello"; 
    
       return 0;
}
