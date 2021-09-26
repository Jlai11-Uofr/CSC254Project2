/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2021.
*/

#include <map>
#include <list>
#include <iostream>
#include <algorithm>
#include "scan.h"

using namespace std;

const char *names[] = {"read", "write", "id", "literal", "gets", "add",
                       "sub", "mul", "div", "lparen", "rparen", "eof", "if", "fi", "do", "od", "check", "==", "<>", ">",
                       "<", ">=", "<="};

static token upcoming_token;

// flag that controls whether an error has been found
// if so, then we do not print the AST
static bool no_error_found = true;
static map<string, bool> EPS;
static map<string, list<token>> FIRST;
static map<string, list<token>> FOLLOW;

// helper function for checking if a list contains a query element
// credit: https://stackoverflow.com/questions/24139428/check-if-element-is-in-the-list-contains
template <class _container, class _Ty>
inline bool contains(_container _C, const _Ty &_Val)
{
    return std::find(_C.begin(), _C.end(), _Val) != _C.end();
}

// initalizing the EPS
void initialize_EPS()
{
    EPS.insert({"P", false});
    EPS.insert({"SL", true});
    EPS.insert({"S", false});
    EPS.insert({"R", false});
    EPS.insert({"E", false});
    EPS.insert({"T", false});
    EPS.insert({"F", false});
    EPS.insert({"ET", true});
    EPS.insert({"TT", true});
    EPS.insert({"FT", true});
    EPS.insert({"ro", false});
    EPS.insert({"ao", false});
    EPS.insert({"mo", false});
}

//initializing the FIRST SET
void initialize_FIRST()
{
    list<token> P = {t_id, t_read, t_write, t_if, t_do, t_check, t_eof};
    list<token> SL = {t_id, t_read, t_write, t_if, t_do, t_check};
    list<token> S = SL;
    list<token> F = {t_lparen, t_id, t_literal};
    list<token> T = F;
    list<token> E = T;
    list<token> R = E;
    list<token> ET = {t_equals, t_carrot, t_less, t_greater, t_lessE, t_greaterE};
    list<token> TT = {t_add, t_sub};
    list<token> FT = {t_mul, t_div};
    list<token> ro = {t_equals, t_carrot, t_less, t_greater, t_lessE, t_greaterE};
    list<token> ao = {t_add, t_sub};
    list<token> mo = {t_mul, t_div};

    FIRST.insert({"P", P});
    FIRST.insert({"SL", SL});
    FIRST.insert({"S", S});
    FIRST.insert({"R", R});
    FIRST.insert({"E", E});
    FIRST.insert({"T", T});
    FIRST.insert({"F", F});
    FIRST.insert({"ET", ET});
    FIRST.insert({"TT", TT});
    FIRST.insert({"FT", FT});
    FIRST.insert({"ro", ro});
    FIRST.insert({"ao", ao});
    FIRST.insert({"mo", mo});
}

// initializing the FOLLOW SET
void initialize_FOLLOW()
{

    // FOLLOW(P) = empty list
    list<token> P = {};
    // FOLLOW(SL) = {eof, fi, od}
    list<token> SL = {t_eof, t_fi, t_od};

    // FOLLOW(S) = {FIRST(SL) + FOLLOW(SL)}
    list<token> S = {
        // FIRST(SL)
        t_id, t_read, t_write, t_if, t_do, t_check,
        // FOLLOW(SL)
        t_eof, t_fi, t_od};

    // FOLLOW(R) = {), FOLLOW(S), FIRST(SL), FOLLOW(SL)}
    list<token> R = {
        // )
        t_rparen,
        // FOLLOW(S) contains both FIRST(SL) and FOLLOW(SL)
        t_id,
        t_read,
        t_write,
        t_if,
        t_do,
        t_check,
        t_eof,
        t_fi,
        t_od,
    };

    // FOLLOW(ET) = {FOLLOW(R)}
    list<token> ET = {
        // FOLLOW (R)
        t_rparen,
        t_id,
        t_read,
        t_write,
        t_if,
        t_do,
        t_check,
        t_eof,
        t_fi,
        t_od,
    };

    // FOLLOW(E) = {FIRST(ET), FOLLOW(R), FOLLOW(ET)}
    list<token> E = {
        // FIRST(ET)
        t_equals, t_carrot, t_less, t_greater, t_lessE, t_greaterE,
        // FOLLOW(R)
        t_rparen,
        t_id, t_read, t_write, t_if, t_do, t_check,
        t_eof, t_fi, t_od,
        // FOLLOW(ET) is equal to FOLLOW(R)
    };

    // FOLLOW(TT) = {FOLLOW(E)}
    list<token> TT = E;

    // FOLLOW(T) = {FIRST(TT), FOLLOW(E)}
    list<token> T = {
        // FIRST(TT)
        t_add,
        t_sub,
        // FOLLOW(E)
        t_equals,
        t_carrot,
        t_less,
        t_greater,
        t_lessE,
        t_greaterE,
        t_rparen,
        t_id,
        t_read,
        t_write,
        t_if,
        t_do,
        t_check,
        t_eof,
        t_fi,
        t_od,
    };

    // FOLLOW(F) = {FIRST(FT), FOLLOW(T)}
    list<token> F = {
        // FIRST (FT)
        t_mul,
        t_div,
        // FOLLOW (T)
        t_add,
        t_sub,
        t_equals,
        t_carrot,
        t_less,
        t_greater,
        t_lessE,
        t_greaterE,
        t_rparen,
        t_id,
        t_read,
        t_write,
        t_if,
        t_do,
        t_check,
        t_eof,
        t_fi,
        t_od,
    };

    // FOLLOW(FT) = {FOLLOW(T)}
    list<token> FT = T;

    // FOLLOW(ro) = {FIRST(E)}
    list<token> ro = {
        t_lparen, t_id, t_literal};

    // FOLLOW(ao) = {FIRST(T)}
    list<token> ao = {
        t_lparen, t_id, t_literal};

    // FOLLOW(mo) = {FIRST(F)}
    list<token> mo = {
        t_lparen, t_id, t_literal};

    FOLLOW.insert({"P", P});
    FOLLOW.insert({"SL", SL});
    FOLLOW.insert({"S", S});
    FOLLOW.insert({"R", R});
    FOLLOW.insert({"E", E});
    FOLLOW.insert({"T", T});
    FOLLOW.insert({"F", F});
    FOLLOW.insert({"ET", ET});
    FOLLOW.insert({"TT", TT});
    FOLLOW.insert({"FT", FT});
    FOLLOW.insert({"ro", ro});
    FOLLOW.insert({"ao", ao});
    FOLLOW.insert({"mo", mo});
}


// print syntax error, and set no_error_found to false to
// disable printing the AST.
void report_error(string sym)
{
    cout << "found syntax error at " << sym << endl;
    no_error_found = false;
}

//Implementation of Wirths algorithm
void check_for_error(string sym)
{
    bool eps = EPS[sym];
    list<token> first = FIRST[sym];
    list<token> follow = FOLLOW[sym];

    if (!(contains(first, upcoming_token) || eps))
    {
        report_error(sym);
        while (true)
        {
            upcoming_token = scan();
            if (contains(first, upcoming_token) ||
                contains(follow, upcoming_token) ||
                upcoming_token == t_eof)
            {
                break;
            }
        }
    }
}

// Given a current token. We try and consume it. If it's Not a valid token in names, then we have to throw an error.
void match(token expected)
{
    if (upcoming_token == expected)
    {
        upcoming_token = scan();
    }
    else
    {
        report_error("matching");
    }
}

//--------------------------------//

string program();
string stmt_list();
string stmt();
string relation();
string expr();
string term();
string term_tail(string s1);
string factor();
string factor_tail(string s1);
string expr_tail(string s1);
string relation_op();
string add_op();
string mul_op();

//--------------------------------//

string program()
{
    string s = "";
    check_for_error("P");
    switch (upcoming_token)
    {
    case t_id:
    case t_read:
    case t_write:
    case t_if:
    case t_do:
    case t_check:
    case t_eof:
        // predict P -> SL $$
        s = s + "(program \n[" + stmt_list() + "]\n)\n";
        match(t_eof);
        break;
    default:
        report_error("P");
    }
    return s;
}

string stmt_list()
{
    string s = " ";
    check_for_error("SL");
    switch (upcoming_token)
    {
    case t_id:
    case t_read:
    case t_write:
    case t_if:
    case t_do:
    case t_check:
        //predict SL --> S SL
        s = s + stmt();
        s = s + stmt_list();
        break;
    case t_eof:
    case t_fi:
    case t_od:
        //predict SL --> epsilon
        return "";
        break;
    default:
        report_error("SL");
    }
    return s;
}

string stmt()
{
    string s = "(";
    check_for_error("S");
    switch (upcoming_token)
    {
    case t_id:
        //predict S --> id := R
        s = s + ":=" + " \"" + string(token_image) + "\"";
        match(t_id);
        match(t_gets);

        s = s + relation();
        break;
    case t_read:
        //predict S --> read id
        match(t_read);
        s = s + "read " + "\"" + string(token_image) + "\"";

        match(t_id);
        break;
    case t_write:
        //predict S --> write R
        match(t_write);
        s = s + "write" + relation();

        break;
    case t_if:
        //predict S --> if R SL fi
        match(t_if);
        s = s + "if\n";
        s = s + "(" + relation() + ")\n";
        s = s + "[ " + stmt_list() + "]\n";

        match(t_fi);
        break;
    case t_do:
        //predict S --> do SL od
        match(t_do);
        s = s + "do\n";
        s = s + "[ " + stmt_list() + "]\n";
        match(t_od);
        break;
    case t_check:
        //predict S --> check R
        match(t_check);
        s += "check";
        s += " (" + relation() + ")";
        break;
    default:
        report_error("S");
    }
    return s + ")\n";
}

string relation()
{
    string s = "";
    check_for_error("R");
    switch (upcoming_token)
    {
    case t_lparen:
    case t_id:
    case t_literal:
        //predict R --> E ET
        s = s + expr_tail(expr());
        break;
    default:
        report_error("R");
    }
    return s + "";
}

string term()
{
    string s = "";
    check_for_error("T");
    switch (upcoming_token)
    {
    case t_lparen:
    case t_id:
    case t_literal:
        //predict T --> F FT
        s = s + factor_tail(factor());
        break;
    default:
        report_error("T");
    }
    return s;
}
string expr()
{
    string s = " ";
    check_for_error("E");
    switch (upcoming_token)
    {
    case t_lparen:
    case t_id:
    case t_literal:
        //predict expr --> T TT
        s = s + term_tail(term());
        break;
    default:
        report_error("E");
    }
    return s;
}

string factor()
{
    string s = " ";
    check_for_error("F");
    switch (upcoming_token)
    {
    case t_lparen:
        //predict F --> ( R )
        match(t_lparen);
        s = s + relation();
        match(t_rparen);

        break;
    case t_id:
        //predict F --> id
        s = s + "(id" + " \"" + string(token_image) + "\")";
        match(t_id);
        break;
    case t_literal:
        //predict F --> lit
        s = s + "(num" + " \"" + string(token_image) + "\")";
        match(t_literal);
        break;
    default:
        report_error("F");
    }
    return s;
}

string expr_tail(string s1)
{
    string s = "";
    check_for_error("ET");
    switch (upcoming_token)
    {
    case t_equals:
    case t_carrot:
    case t_less:
    case t_greater:
    case t_lessE:
    case t_greaterE:
        //predict ET --> ro E
        s = s + relation_op();
        s = s + s1;
        s = s + expr();
        break;
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_if:
    case t_do:
    case t_check:
    case t_eof:
    case t_fi:
    case t_od:
        //predict ET --> epsilon
        return s1;
        break;
    default:
        report_error("ET");
    }
    return s;
}

string term_tail(string s1)
{
    string s = "(";
    check_for_error("TT");
    switch (upcoming_token)
    {
    case t_add:
    case t_sub:
        //predict TT --> ao T TT
        s = s + add_op();
        s = s + s1;
        s = s + term_tail(term());
        break;
    case t_equals:
    case t_carrot:
    case t_less:
    case t_greater:
    case t_lessE:
    case t_greaterE:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_if:
    case t_do:
    case t_check:
    case t_eof:
    case t_fi:
    case t_od:
        //predict TT --> epsilon
        return s1;
        break;
    default:
        report_error("TT");
    }
    return s + ")";
}

string factor_tail(string s1)
{
    string s = "(";
    check_for_error("FT");
    switch (upcoming_token)
    {
    case t_mul:
    case t_div:
        //predict FT --> mo F FT
        s = s + mul_op();
        s = s + s1;
        s = s + factor_tail(factor());
        break;
    case t_add:
    case t_sub:
    case t_equals:
    case t_carrot:
    case t_less:
    case t_greater:
    case t_lessE:
    case t_greaterE:
    case t_rparen:
    case t_id:
    case t_read:
    case t_write:
    case t_if:
    case t_do:
    case t_check:
    case t_eof:
    case t_fi:
    case t_od:
        //predict FT --> epsilon
        return s1;
        break;
    default:
        report_error("FT");
    }
    return s + ")";
}

string relation_op()
{
    string s = "";
    check_for_error("ro");
    switch (upcoming_token)
    {
    case t_equals:
        //predict ro --> ==
        match(t_equals);
        s = s + "==";
        break;
    case t_carrot:
        //predict ro --> <>
        match(t_carrot);
        s = s + "<>";
        break;
    case t_less:
        //predict ro --> <
        match(t_less);
        s = s + "<";
        break;
    case t_greater:
        //predict ro --> >
        match(t_greater);
        s = s + ">";
        break;
    case t_lessE:
        //predict ro --> <=
        match(t_lessE);
        s = s + "<=";
        break;
    case t_greaterE:
        //predict ro --> >=
        match(t_greaterE);
        s = s + ">=";
        break;
    default:
        report_error("ro");
    }
    return s;
}

string add_op()
{
    string s = "";
    check_for_error("ao");
    switch (upcoming_token)
    {
    case t_add:
        //predict ao --> +
        match(t_add);
        s = s + "+";
        break;
    case t_sub:
        //predict ao --> -
        match(t_sub);
        s = s + "-";
        break;
    default:
        report_error("ao");
    }
    return s;
}

string mul_op()
{
    string s = "";
    check_for_error("mo");

    switch (upcoming_token)
    {
    case t_mul:
        //predict mo --> *
        match(t_mul);
        s = s + "*";

        break;
    case t_div:
        //predict mo --> /
        match(t_div);
        s = s + "/";
        break;
    default:
        report_error("mo");
    }
    return s;
}

int main()
{
    // initializing the three sets for error recovery program
    initialize_EPS();
    initialize_FIRST();
    initialize_FOLLOW();

    // starting our program
    upcoming_token = scan();
    string answer = program();

    // conditionally print the AST if no error was found.
    no_error_found == true
        ? cout << answer
        : cout << "no AST will be printed because error was found";

    return 0;
}
