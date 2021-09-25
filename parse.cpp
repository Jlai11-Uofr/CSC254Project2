/* Complete recursive descent parser for the calculator language.
   Builds on figure 2.16 in the text.  Prints a trace of productions
   predicted and tokens matched.  Does no error recovery: prints
   "syntax error" and dies on invalid input.
   Michael L. Scott, 2008-2021.
*/

// imported by Sizhe
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

/* TODO:
 *  1. we need to calculate the FIRST(.) and FOLLOW(.) sets for all the non-terminals
 *  2. we need to implement the error recovery program explained in the materials from the companion site
 *  */

// TODO: define this
static map<string, bool> EPS;
static map<string, list<token>> FIRST;
static map<string, list<token>> FOLLOW;
static map<token, string> TOK2STR;
// output syntax tree
static string output_st;

// helper function for checking if a list contains a query element
// credit: https://stackoverflow.com/questions/24139428/check-if-element-is-in-the-list-contains
template <class _container, class _Ty>
inline bool contains(_container _C, const _Ty &_Val)
{
    return std::find(_C.begin(), _C.end(), _Val) != _C.end();
}

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

void initialize_TOK2STR()
{
    TOK2STR.insert({t_read, "read"});
    TOK2STR.insert({t_write, "write"});
    TOK2STR.insert({t_id, "id"});
    TOK2STR.insert({t_literal, "lit"});
    TOK2STR.insert({t_gets, ":="});
    TOK2STR.insert({t_add, "+"});
    TOK2STR.insert({t_sub, "-"});
    TOK2STR.insert({t_mul, "*"});
    TOK2STR.insert({t_div, "/"});
    TOK2STR.insert({t_lparen, "("});
    TOK2STR.insert({t_rparen, ")"});
    TOK2STR.insert({t_eof, "$$"});
    TOK2STR.insert({t_if, "if"});
    TOK2STR.insert({t_fi, "fi"});
    TOK2STR.insert({t_do, "do"});
    TOK2STR.insert({t_od, "od"});
    TOK2STR.insert({t_check, "check"});
    TOK2STR.insert({t_equals, "=="});
    TOK2STR.insert({t_carrot, "<>"});
    TOK2STR.insert({t_greater, ">"});
    TOK2STR.insert({t_less, "<"});
    TOK2STR.insert({t_greaterE, ">="});
    TOK2STR.insert({t_lessE, "<="});
}

void error()
{
    cerr << "Got your self a syntax Error! \n";
}

void report_error(string sym)
{
    cout << "found syntax error at " << sym << endl;
}

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
      //  cout << "matched " << names[upcoming_token];
        if (upcoming_token == t_id || upcoming_token == t_literal)
        //    cout << ": " << token_image;
        cout << "\n";
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
    string s= " ";
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
        //cout << "predict P -> SL $$ \n";
        s = s+ "(program [" + stmt_list() + "]\n";
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
       // cout << "predict SL --> S SL \n";
        s = s + stmt ();
        s = s+  stmt_list ();
        //stmt();
    //    stmt_list();
        break;
    case t_eof:
    case t_fi:
    case t_od:
       // cout << "predict SL --> epsilon \n";
        return "";
        break;
    default:
        report_error("SL");
    }
    return s;
}

string stmt()
{
    string s ="(";
    check_for_error("S");
    switch (upcoming_token)
    {
    case t_id:
      //  cout << "predict S --> id := R \n";
           s = s + ":= " + " \"" +  string(token_image) + "\" ";
         match(t_id);
        match(t_gets);
   
     
      
        s = s + relation();
        break;
    case t_read:
      //  cout << "predict S --> read id \n";
          match(t_read);
        s = s + "read " + "\"" + string(token_image) + "\"";
      
        match(t_id);
        break;
    case t_write:
      //  cout << "predict S --> write R \n";
              match(t_write);
        s = s + "write" + relation();

      //  relation();
        break;
    case t_if:
     //   cout << "predict S --> if R SL fi\n";
             match(t_if);
         s = s + "if\n";
        s = s + "(" + relation () + ")\n";
        s = s +  "[ " + stmt_list () + "]\n";

       // relation();
        //stmt_list();
        match(t_fi);
        break;
    case t_do:
      //  cout << "predict S --> do SL od \n";
        match(t_do);
         s = s + "do\n";
        s = s +  "[ " + stmt_list () + "]\n";
        //stmt_list();
        match(t_od);
        break;
    case t_check:
     //   cout << "predict S --> check R \n";
        match(t_check);
        s = s+ "check";
        s =s +  relation();
        break;
    default:
        report_error("S");
    }
    return s + ")\n";
}

string relation()
{
    string s = "( ";
    check_for_error("R");
    switch (upcoming_token)
    {
    case t_lparen:
    case t_id:
    case t_literal:
       // cout << "predict R --> E ET \n";
        s = s + expr_tail (expr());
        //expr();
        //expr_tail();
        break;
    default:
        report_error("R");
    }
    return s + " )";
}

string term()
{
    string s = " ";
    check_for_error("T");
    switch (upcoming_token)
    {
        case t_lparen:
        case t_id:
        case t_literal:
          //  cout << "predict T --> F FT \n";
            s = s +  factor_tail(factor());
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
      //  cout << "predict expr --> T TT \n";
        s = s + term_tail(term());
        //term();
       // term_tail();
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
     //   cout << "predict F --> ( R ) \n";
        match(t_lparen);
       // relation();
          s =  s + relation ();
        match(t_rparen);
     
        break;
    case t_id:
    //    cout << "predict F --> id \n";
        s = s + "(id " + " \"" +  string(token_image) + "\")";
        match(t_id);
        break;
    case t_literal:
     //   cout << "predict F --> lit \n";
        s= s + "(num " + " \"" + string(token_image) + "\")";
        match(t_literal);
        break;
    default:
        report_error("F");
    }
    return s;
}

string expr_tail(string s1)
{
    string s = " ";
    check_for_error("ET");
    switch (upcoming_token)
    {
    case t_equals:
    case t_carrot:
    case t_less:
    case t_greater:
    case t_lessE:
    case t_greaterE:
     //   cout << "predict ET --> ro E \n";
         s = s + relation_op ();
        s = s+ s1;
        s = s +  expr ();
      //  relation_op();
        //expr();
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
    //    cout << "predict ET --> epsilon \n";
        return s1;
        break;
    default:
        report_error("ET");
    }
    return s;
}

string term_tail(string s1)
{
    string s = "( ";
    check_for_error("TT");
    switch (upcoming_token)
    {
    case t_add:
    case t_sub:
      //  cout << "predict TT --> ao T TT \n";
          s = s +  add_op ();
          s = s +s1;
          s = s + term_tail(term());
      //  add_op();
        //term();
        //term_tail();
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
      //  cout << "predict TT --> epsilon\n";
        return s1;
        break;
    default:
        report_error("TT");
    }
    return s +" )";
}

string factor_tail(string s1)
{
    string s = "( ";
    check_for_error("FT");
    switch (upcoming_token)
    {
    case t_mul:
    case t_div:
   //     cout << "predict FT --> mo F FT \n";
        s = s + mul_op();
        s = s + s1;
        s = s + factor_tail(factor());
        //mul_op();
        //factor();
        //factor_tail();
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
     //   cout << "predict FT --> epsilon\n";
        return s1;
        break;
    default:
        report_error("FT");
    }
    return s + ")";
}

string relation_op()
{
    string s = " ";
    check_for_error("ro");
    switch (upcoming_token)
    {
    case t_equals:
     //   cout << "predict ro --> == \n";
        match(t_equals);
        s = s + " ==";
        break;
    case t_carrot:
     //   cout << "predict ro --> <> \n";
        match(t_carrot);
        s = s + "<>";
        break;
    case t_less:
      //  cout << "predict ro --> < \n";
        match(t_less);
        s = s + "<";
        break;
    case t_greater:
      //  cout << "predict ro --> > \n";
        match(t_greater);
        s = s + " >";
        break;
    case t_lessE:
   //    cout << "predict ro --> <= \n";
        match(t_lessE);
        s = s + "<=";
        break;
    case t_greaterE:
     //   cout << "predict ro --> >= \n";
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
    string s = " ";
    check_for_error("ao");
    switch (upcoming_token)
    {
    case t_add:
    //    cout << "predict ao --> + \n";
       match(t_add);
        s = s + " + ";
     
        break;
    case t_sub:
    //    cout << "predict ao --> - \n";
        match(t_sub);
        s = s + "- ";
        break;
    default:
        report_error("ao");
    }
    return s;
}

string mul_op()
{
    string s = " ";
    check_for_error("mo");

    switch (upcoming_token)
    {
    case t_mul:
     //   cout << "predict mo --> * \n";
        match(t_mul);
        s = s + " *";
 
        break;
    case t_div:
     //   cout << "predict mo --> / \n";
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
    output_st = "";
    initialize_EPS();
    initialize_FIRST();
    initialize_FOLLOW();
    initialize_TOK2STR();

    // cout << "CHECKING TABLE EPS \n";
    // map<string, bool>::iterator it1;
    // // for debugging
    // for (it1 = EPS.begin(); it1 != EPS.end(); it1++)
    // {
    //     std::cout << it1->first // string (key)
    //               << ':'
    //               << it1->second // string's value
    //               << std::endl;
    // }

    // cout << "--------- CHECKING TABLE FIRST ------------ \n";
    // map<string, list<token>>::iterator it2;
    // // for debugging
    // for (it2 = FIRST.begin(); it2 != FIRST.end(); it2++)
    // {
    //     string name = it2->first;
    //     list<token> items = it2->second;

    //     cout << "[" << name << "]";
    //     for (token &i : items)
    //     {
    //         cout << " " << TOK2STR[i];
    //     }
    //     cout << "\n";
    // }

    // cout << "-------- CHECKING TABLE FOLLOW ----------- \n";
    // map<string, list<token>>::iterator it3;
    // // for debugging
    // for (it3 = FOLLOW.begin(); it3 != FOLLOW.end(); it3++)
    // {
    //     string name = it3->first;
    //     list<token> items = it3->second;

    //     cout << "[" << name << "]";
    //     for (token &i : items)
    //     {
    //         cout << " " << TOK2STR[i];
    //     }
    //     cout << "\n";
    // }

    upcoming_token = scan();
    string answer = program();
    cout << answer << ')';

    return 0;
}
