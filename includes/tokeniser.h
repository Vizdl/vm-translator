#ifndef VIRTUAL_MACHINE_TOKENISER_H
#define VIRTUAL_MACHINE_TOKENISER_H

#include <string>
#include <iostream>

// we have one namespace per language, eg Hack_Virtual_Machine

namespace Hack_Virtual_Machine
{
    // to make our code shorter
    typedef std::string string ;

    // the type of a token returned by the tokeniser
    typedef int Token ;

    // *** BNF syntax for tokens
    // * literals inside ' '
    // * grouping ( )
    // * ASCII ranges -
    // * alternatives |
    // * 0 or 1 [ ]
    // * 0 or more *
    // * 1 or more +
    //

    // Hack Virtual Machine Tokeniser Tokens
    // *** TOKENS
    // * digit  ::= '0'-'9'
    // * digit19  ::= '1'-'9'
    // * letter ::= 'a'-'z'|'A'-'Z'|'_'|':'|'.'
    //
    // * vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
    // * vm_jump ::= 'goto' | 'if-goto' | 'label'
    // * vm_func ::= 'call' | 'function'
    // * vm_stack ::= 'pop' | 'push'
    // * vm_segment ::= 'argument' | 'constant' | 'local' | 'pointer' | 'static' | 'temp' | 'that' | 'this'
    //
    // * a_label ::= letter (letter | digit)*
    // * a_number ::= '0' | (digit19 digit*)
    //
    // *** OTHER RULES
    // * between tokens, space ' ', tab '\t', carriage return '\r' and newline '\n' are ignored
    // * between tokens any other character that cannot start a token results in an EOI token
    // * if an EOI token has been returned all future tokens returned are EOI
    //

    // The kinds of token that are recognised
    enum TokenKind
    {
        tk_first = 2000,        // VM token kinds all > 2000
        tk_a_label,             // a label
        tk_a_number,            // a number 0 to 32767

                                // the operator commands - tk_vm_operator
        tk_add,                 // 'add'
        tk_and,                 // 'and'
        tk_eq,                  // 'eq'
        tk_gt,                  // 'gt'
        tk_lt,                  // 'lt'
        tk_neg,                 // 'neg'
        tk_not,                 // 'not'
        tk_or,                  // 'or'
        tk_sub,                 // 'sub'
        tk_return,              // 'return'

                                // jump commands - tk_vm_jump
        tk_goto,                // 'goto'
        tk_if_goto,             // 'if-goto'
        tk_label,               // 'label'

                                // the function commands - tk_vm_func
        tk_call,                // 'call'
        tk_function,            // 'function'

                                // the stack commands - tk_vm_stack
        tk_pop,                 // 'pop'
        tk_push,                // 'push'

                                // the segment names - tk_vm_segment
        tk_argument,            // 'argument'
        tk_constant,            // 'constant'
        tk_local,               // 'local'
        tk_pointer,             // 'pointer'
        tk_static,              // 'static'
        tk_temp,                // 'temp'
        tk_that,                // 'that'
        tk_this,                // 'this'

                                // the following tokens are not produced by the tokeniser
                                // these are provided to describe groups of tokens
        tk_vm_command,          // one of tk_add, tk_and, tk_eq, tk_gt, tk_lt, tk_neg, tk_not, tk_or, tk_sub, tk_return,
                                //        tk_goto, tk_if_goto, tk_label, tk_call, tk_function, tk_pop or tk_push
        tk_vm_operator,         // one of tk_add, tk_and, tk_eq, tk_gt, tk_lt, tk_neg, tk_not, tk_or, tk_sub or tk_return
        tk_vm_jump,             // one of tk_goto, tk_if_goto or tk_label
        tk_vm_func,             // one of tk_call or tk_function
        tk_vm_stack,            // one of tk_pop or tk_push
        tk_vm_segment,          // one of tk_argument, tk_constant, tk_local, tk_pointer, tk_static, tk_temp, tk_that or tk_this

        tk_eoi,                 // end of the input
        tk_oops,                // for error handling
        tk_last                 // the last Hack VM token value
    } ;

    // return a string representation of the given token kind
    extern string token_kind_to_string(TokenKind kind) ;

    // returns the TokenKind that would cause token_kind_to_string to return s, or tk_oops
    extern TokenKind string_to_token_kind(std::string s) ;

    // read the next token from standard input
    extern Token next_token() ;

    // return the current token, ie the last token returned by next_token()
    extern Token current_token() ;

    // each token has an object that internally records
    // a kind - the TokenKind that describes the token
    // a spelling - the characters that make up the specific token
    // an ivalue - the integer value if the token is a number, otherwise 0
    // a line - the line on which the token starts, the first line of the input is line 1
    // a column - the line position at which the token starts, the first character on a line is in column 1
    // a string representation of the above used to implement a to_string function

    // to assist with error reporting each token has an associated context string
    // the context string each token that includes:
    // - the line before the token,
    // - the line containing the token, and
    // - a line with a ^ marking the token's starting position

    // the following functions return the kind, spelling, ivalue, line, column, to_string or context for the current token
    extern TokenKind token_kind() ;
    extern string token_spelling() ;
    extern int token_ivalue() ;
    extern int token_line() ;
    extern int token_column() ;
    extern string token_to_string() ;
    extern string token_context() ;

    // the following functions return the kind, spelling, ivalue, line, column, to_string or context for the given token
    extern TokenKind token_kind(Token token) ;
    extern string token_spelling(Token token) ;
    extern int token_ivalue(Token token) ;
    extern int token_line(Token token) ;
    extern int token_column(Token token) ;
    extern string token_to_string(Token token) ;
    extern string token_context(Token token) ;

    // to assist with parsing, TokenKind has some extra definitions that are never returned as real tokens
    // these extra tokens can be used to check if a token is a member of a particular group
    // for example, tk_statement provides a group of all tokens that can start a statement
    

    // check if we HAVE a current token of the given kind or that belongs to the given grouping?
    extern bool have(TokenKind kind_or_grouping) ;

    // check if the given token is of the given kind or that belongs to the given grouping?
    extern bool have(Token token,TokenKind kind_or_grouping) ;

    // to further assist with parsing, functions have been provided to raise an error if an expected token is missing
    // the MUSTBE functions check a token against an expected kind or grouping
    // if the token matches, the next token is read from the input and the matching token is returned
    // if the token does not match, a fatal_error is called with a suitable message based on the token's context

    // the current token MUSTBE of the given kind or belong to the given grouping?
    extern Token mustbe(TokenKind kind_or_grouping) ;

    // the given token MUSTBE of the given kind or belong to the given grouping?
    extern Token mustbe(Token token,TokenKind kind_or_grouping) ;

}

#endif //VIRTUAL_MACHINE_TOKENISER_H
