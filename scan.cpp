/* Simple ad-hoc scanner for the calculator language.
   Dies on lexical errors.
   Michael L. Scott, 2008-2021.
*/

#include "stdlib.h"
#include "string.h"
#include "ctype.h"

using namespace std;
#include <iostream>
#include "scan.h"

char token_image[MAX_TOKEN_LEN];

token scan() {
    static int c = ' ';
        /* next available char; extra (int) width accommodates EOF */
    int i = 0;              /* index into token_image */

    /* skip white space */
    while (isspace(c)) {
        c = getchar();
    }
    if (c == EOF)
        return t_eof;
    if (isalpha(c)) {
        do {
            token_image[i++] = c;
            if (i >= MAX_TOKEN_LEN) {
                cerr << "max token length exceeded\n";
                exit(1);
            }
            c = getchar();
        } while (isalpha(c) || isdigit(c) || c == '_');
        token_image[i] = '\0';

        //As we have a new grammar, we need to extend our scanner to acccept the new tokens that are introduced with this extended calculator language.


        if (!strcmp(token_image, "read")) return t_read;
        else if (!strcmp(token_image, "if")) return t_if;
        else if (!strcmp(token_image, "fi")) return t_fi;
        else if (!strcmp(token_image, "do")) return t_do;
        else if (!strcmp(token_image, "od")) return t_od;
        else if (!strcmp(token_image, "check")) return t_check;
        else if (!strcmp(token_image, "write")) return t_write;

        else return t_id;
    }
    else if (isdigit(c)) {
        do {
            token_image[i++] = c;
            c = getchar();
        } while (isdigit(c));
        token_image[i] = '\0';
        return t_literal;
    } else switch (c) {
        case ':':
            if ((c = getchar()) != '=') {
                        cerr << "expected '=' after ':', got " << static_cast<char>(c) << "\n";
                        //Lexical error could occur here as well
                        c = getchar();
                        return scan();
                        // exit(1);
            } else {
                c = getchar();
                return t_gets;
            }
            break;
        case '+': c = getchar(); return t_add;
        case '-': c = getchar(); return t_sub;
        case '*': c = getchar(); return t_mul;
        case '/': c = getchar(); return t_div;
        case '(': c = getchar(); return t_lparen;
        case ')': c = getchar(); return t_rparen;
        case '>': 

        // If we see a >, can eitheir be apart of >= so we must scan the next charater to deteremine if it is. If it is, we must consume this = as well.
         if ((c = getchar()) == '=') {
                c = getchar();
                    return t_greaterE;
            } else {
                return t_greater;
            }
            break;
              // If we see a >, can eitheir be apart of <= so we must scan the next charater to deteremine if it is. If it is, we must consume this = as well.
        case '<': 
        if ((c = getchar()) == '=') {
                c=getchar();
                return t_lessE;
            } 
            else if(c == '>') {
                c = getchar();
                return t_carrot;
            }
            else {
                return t_less;
            }
            break;

        //if we see a =, it could be equality so we need to make sure that is is not a equal ==. If it is we consume and return. Else ignore and keep going.
        case '=': 
                 if ((c = getchar()) != '=') {
                        cerr << "expected '=' after '=', got " << static_cast<char>(c) << "\n";
                         c = getchar();
                         return scan();
            } else {
                c = getchar();
                return t_equals;
            }
            break;
        default:
            cout << "unexpected character " <<  static_cast<char>(c)<< "\n";
            c = getchar();
            return scan();
        
    }
}
