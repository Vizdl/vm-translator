#ifndef HACKVM_AST_H
#define HACKVM_AST_H

#include <string>
#include <vector>

// Abstract Syntax Tree
// All AST nodes have a private implementation
// - the AST values passed around are indexes into a private data structure
// - vector like nodes, AST nodes are immutable
// - every AST node has a 'create' function, eg 'create_while(_condition,_statements)'
// - every field of an AST node has a 'get' function, eg, 'get_while_statements(_while_node)'
// - vector like AST nodes have a 'size_of' function, eg 'size_of_statements(_statements_node);'
// - vector like AST nodes have a 'get' function, eg 'get_statements(_statements_node,2);'

// all errors encountered by AST functions will result in calls to fatal_error() defined in iobuffer.h
// an additional error message will be printed to standard error
// the running program will terminate with an exit status of -1
// errors include
// - passing an ast value that does not identify an AST node
// - passing an ast value that refers to an inappropriate kind of AST node
// - tree traversal errors when print a tree as XML
// - parsing errors when constructing an XML tree

// An Abstract Sytax Tree is language specific and has its own private namespace, eg Hack_Virtual_Machine

// Hack Virtual Machine Syntax - the grammar we are recognising
//
// TERM: DEFINITION
// vm_class ::=    vm_command*
// vm_command ::=  vm_operator | vm_jump | vm_function | vm_stack
// vm_operator ::= operator
// vm_jump ::=     jump label
// vm_function ::= function label number
// vm_stack ::=    stack segment number
//
// TOKEN: DEFINITION
//
// operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// jump ::=     'goto' | 'if-goto' | 'label'
// function ::= 'call' | 'function'
// stack ::=    'push' | 'pop'
// segment ::=  'argument' | 'constant' | 'local' | 'pointer' | 'static' | 'temp' | 'that' | 'this'
// number ::=   '0' to '32767'

// Hack Virtual Machine
namespace Hack_Virtual_Machine
{
    // we use integers to identify AST nodes in the public interface
    typedef int ast ;

    // shorthand to avoid std::
    using namespace std ;

    // enumeration for AST node classes
    // ast_alpha & ast_omega describe the maximum range of ast_kind values
    // ast_oops is for error reporting
    enum ast_kind
    {
        ast_alpha = 2000,       // keep enum ranges unique

                                // Groupings - there are no nodes of these kinds
        ast_vm_command,         // vm command - groups ast_vm_operator, ast_vm_jump, ast_vm_function and ast_vm_stack

        ast_vm_class,           // vm_class
        ast_vm_operator,        // operator command
        ast_vm_jump,            // jump command
        ast_vm_function,        // function command
        ast_vm_stack,           // stack command

        ast_oops,               // for error reporting
        ast_omega               // largest ast_kind value
    };

    /***** AST Functions *****
      * IMPORTANT:
      * all errors encountered by these functions will result in calls to fatal_error()
      * an additional error message will be printed to standard error
      * the running program will terminate with an exit status of -1
    */

    //***** AST node kind functions *****//

    extern string   ast_kind_to_string(ast_kind k) ;        // returns string representation of ast kind k
    extern ast_kind ast_node_kind(ast t) ;                  // what kind of ast node is t?

                                                            // the next two functions recognise grouped kinds
    extern bool     ast_have_kind(ast t,ast_kind k) ;       // can node t be safely used as a node of kind k?
    extern void     ast_mustbe_kind(ast t,ast_kind k) ;     // call fatal_error() if node t cannot be safely used as a node of kind k

    //***** XML printing / parsing functions for Jack AST nodes *****//

    // XML token kinds
    enum xml_token_kind
    {
        xml_opentag,    // an open tag, tag name is in spelling
        xml_closetag,   // a close tag, tag name is in spelling
        xml_text,       // a #text node, text is in spelling
        xml_eoi         // end of xml input or error?
    };

    // an XML token
    struct xml_token
    {
        xml_token_kind kind ;
        string spelling ;
    } ;

    // print tree t as XML, an indent_by > 0 causes pretty printing
    extern void ast_print_as_xml(ast t,int indent_by) ;

    // construct a Jack_Compiler abstract syntax tree by parsing XML from standard input,
    // any pretty printed indents are ignored,
    // the result will be either an ast_class node
    extern ast ast_parse_xml() ;

    //***** AST nodes to represent Hack Virtual Machine language *****//
    // vm_class ::= vm_command*
    // ast_vm_class node - a vector of ast_vm_command nodes
    // commands must be a vector of ast_vm_command nodes
    // an ast_vm_command node is one of ast_vm_operator, ast_vm_jump, ast_vm_function or ast_vm_stack
    extern ast create_vm_class(vector<ast> commands) ;
    extern int size_of_vm_class(ast commands) ;
    extern ast get_vm_class(ast commands,int i) ;

    // vm_operator ::= operator
    // ast_vm_operator node
    // command (operator) must be one of 'add', 'and', 'eq', 'gt', 'lt', 'neg', 'not', 'or', 'sub' or 'return'
    extern ast create_vm_operator(string command) ;
    extern string get_vm_operator_command(ast vm_operator) ;

    // vm_jump ::= jump label
    // command (jump) must be one of 'goto', 'if-goto' or 'label'
    extern ast create_vm_jump(string command,string label) ;
    extern string get_vm_jump_command(ast vm_jump) ;
    extern string get_vm_jump_label(ast vm_jump) ;

    // vm_function ::= function label number
    // command (function) must be one of 'call' or 'function'
    // number must be in the range  0 to 32767
    extern ast create_vm_function(string command,string label,int number) ;
    extern string get_vm_function_command(ast vm_function) ;
    extern string get_vm_function_label(ast vm_function) ;
    extern int get_vm_function_number(ast vm_function) ;

    // vm_stack ::= stack segment number
    // command (stack) must be one of 'push' or 'pop'
    // segment must be one of  'argument', 'constant', 'local', 'pointer', 'static', 'temp', 'that' or 'this'
    // number must be in the range  0 to 32767
    extern ast create_vm_stack(string command,string segment,int offset) ;
    extern string get_vm_stack_command(ast vm_stack) ;
    extern string get_vm_stack_segment(ast vm_stack) ;
    extern int get_vm_stack_offset(ast vm_stack) ;

}

#endif //HACKVM_AST_H

