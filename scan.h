/* Definitions the scanner shares with the parser.
   Michael L. Scott, 2008-2021.
*/

typedef enum {t_read,
            t_write, t_id, t_literal, t_gets, t_add,
            t_sub, t_mul, t_div, t_lparen, t_rparen, t_eof,t_if,
            t_fi,t_do,t_od,t_check,
            t_equals,t_carrot,t_greater
            ,t_less,t_greaterE,t_lessE} token;

#define MAX_TOKEN_LEN 200
extern char token_image[MAX_TOKEN_LEN];

extern token scan();
