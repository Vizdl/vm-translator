// convert a document tree into VM code
#include "iobuffer.h"
#include "abstract-syntax-tree.h"
#include "assembler-internal.h"
 
// to make out programs a bit neater
using namespace std ;

using namespace CS_IO_Buffers ;
using namespace Hack_Virtual_Machine ;

// grammer to be parsed:
// vm_class ::= command* eoi
// command ::= vm_operator | vm_jump | vm_func | vm_stack
// vm_operator ::= 'add' | 'and' | 'eq' | 'gt' | 'lt' | 'neg' | 'not' | 'or' | 'sub' | 'return'
// vm_jump ::= jump label
// vm_func ::= func label number
// vm_stack ::= stack segment number

// forward declare parsing functions - one per rule
static void translate_vm_class(ast root) ;
static void translate_vm_command(ast command) ;
static void translate_vm_operator(ast vm_op) ;
static void translate_vm_jump(ast jump) ;
static void translate_vm_func(ast func) ;
static void translate_vm_stack(ast stack) ;



////////////////////////////////////////////////////////////////

/************   PUT YOUR HELPER FUNCTIONS HERE   **************/
#define LABEL "temp_label"
typedef int register_name;
enum RegisterName{
    SP = 0,
    LCL,
    ARG,
    THIS,
    THAT,
    R5,
    R6,
    R7,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15
};
enum CompareToken{
    LT = 100,
    GT,
    EQ
};
// class_name and function_name 
static string class_name = "Unknown";
static string function_name = "unknown";

// counter
static int counter = 0;

// function
static string get_prefix();
static string get_class_name();
static void set_function_name(string func);
static void set_class_name(string cla);
static void set_class_and_function_name(string cla,string func);
static void jmp_label(string label);
static void updata_counter();
static string get_counter();
static void output_function(string label,int number);
static void output_call(string label,int number);
static void op_add();
static void op_sub();
static void push_register(register_name rn);
static void push_0();
static void push_A();
static void push_D ();
static void push_constant(int number);
static void pop_D();
static void pop_A();
static void pop_register(register_name rn);
static string get_temp_label();
static void push_static(int number);
static void op_return();
static void op_or();
static void op_not();
static void op_neg();
static void op_lt();
static void op_gt();
static void op_eq();
static void op_and();
static void two_operands();
static void jmp_register(register_name rn);
static void pop_temp(int number);
static void pop_pointer(int number);
static void push_pointer(int number);
static void push_temp(int number);
static void pop_static(int number);
static void Compare(CompareToken ct);



static string get_prefix(){
    return class_name + "." + function_name + "$";
}
static string get_class_name(){
    return class_name;
}
static void set_function_name(string func){
    function_name = func;
}
static void set_class_name(string cla){
    class_name = cla;
}
static void set_class_and_function_name(string label){
    const char * ch= label.c_str();
    int idx = 0;
    while (ch[idx] != '.'){
        idx++;
    }
    set_class_name(label.substr(0,idx));
    set_function_name(label.substr(idx + 1));
}


// counter function
static void updata_counter(){
    counter++;
}
static string get_counter(){
    return to_string(counter);
}


// label function
static void output_label (string label){
    output_assembler("("+label+")") ; 
}
static void A_instructions(string label){
    output_assembler("@" + label) ; 
}
static void register_to_A(register_name rn){
    A_instructions("R"+to_string(rn)); 
}
static void add_temp_label(){
    output_label(get_temp_label());
}
static string get_temp_label(){
    return get_prefix() + LABEL + get_counter();
}


// op
static void two_operands(){
    // D = *(SP - 1), A = SP - 2
    register_to_A(SP);
	output_assembler("AM=M-1"); 
	output_assembler("D=M"); 
	output_assembler("A=A-1"); 
}
static void op_add(){
    two_operands(); 
	output_assembler("M=D+M"); 
}
static void op_sub(){
    two_operands(); 
	output_assembler("M=M-D"); // *(SP - 2) = *(SP - 2) - *(SP - 1)
}

static void op_return(){
    // FRAME = LCL
	register_to_A(LCL);
	output_assembler("D=M"); 
	register_to_A(R14);
	output_assembler("M=D");
    // RET = *(FRAME - 5)
    A_instructions("5");
    output_assembler("D=D-A");
    output_assembler("A=D");
    output_assembler("D=M");
    register_to_A(R13);
    output_assembler("M=D");
    // *ARG = Pop()
    pop_D();
    register_to_A(ARG);
    output_assembler("A=M"); // A = ARG
    output_assembler("M=D");
    // SP = ARG + 1 
    register_to_A(ARG); // A = &ARG
	output_assembler("D=M+1"); // D = ARG + 1
	register_to_A(SP); // A = &SP
	output_assembler("M=D"); // SP = ARG + 1
    for (int i = 1; i <= 4; i++){
        register_to_A(R14); 
        output_assembler("D=M"); 
        A_instructions(to_string(i));
        output_assembler("A=D-A");
        output_assembler("D=M"); 
        register_to_A((register_name)(5 - i));
        output_assembler("M=D"); 
    }
    jmp_register(R13);
}
void change_stack_top_true(){
    A_instructions("32767");
	output_assembler("AD=A");
	output_assembler("D=D+A");
    output_assembler("D=D+1"); 
	register_to_A(SP);
	output_assembler("A=M-1");
	output_assembler("M=D");
}
void change_stack_top_false(){
    A_instructions("0");
	output_assembler("D=A");
	register_to_A(SP); 
	output_assembler("A=M-1");
	output_assembler("M=D");
}

static void op_or(){
    two_operands();
    output_assembler("M=D|M"); 
}
static void op_not(){
    register_to_A(SP); 
	output_assembler("A=M-1");
	output_assembler("M=!M"); 
}
static void op_neg(){
    register_to_A(SP); 
	output_assembler("A=M-1"); 
	output_assembler("M=-M");
}
static void op_lt(){
    Compare(LT);
}
static void op_gt(){
    Compare(GT);
}
static void op_eq(){
    Compare(EQ);
}
static void op_and(){
    two_operands();
    output_assembler("M=D&M");
}
static void Compare(CompareToken ct){
    two_operands();
    // D = *(SP - 1), A = SP - 2
    output_assembler("D=M-D"); 
    A_instructions(get_temp_label());
    if (ct == LT){
        output_assembler("D;JLT");
    }else if (ct == GT){
        output_assembler("D;JGT");
    }else{// eq
        output_assembler("D;JEQ");
    }

    // if true
    change_stack_top_false();
    jmp_label(get_prefix() + LABEL + to_string(counter + 1));

    add_temp_label();
    updata_counter();
    // if false
    change_stack_top_true();
    add_temp_label();
    updata_counter();
}
// jmp
static void output_if_goto(string label){
    pop_D();
    A_instructions(get_prefix()+label);
    output_assembler("D;JNE") ;
}
static void output_goto(string label){
    jmp_label(get_prefix()+label);
}
static void jmp_label(string label){
    A_instructions(label) ;
    output_assembler("0;JMP") ;
}
static void jmp_register(register_name rn){
    A_instructions("R"+to_string(rn)); 
    output_assembler("A=M"); 
	output_assembler("0;JMP");
}
// push 
static void push_register(register_name rn){
    A_instructions("R"+to_string(rn)); 
	output_assembler("D=M"); 
	push_D();
}
static void push_0(){
    // push 0
    register_to_A(SP);
	output_assembler("D=A");
	output_assembler("AM=M+1");
	output_assembler("A=A-1");
	output_assembler("M=D");
}
static void push_A(){
    output_assembler("D=A");
    push_D();
}
static void push_D (){
    register_to_A(SP);
	output_assembler("AM=M+1");
	output_assembler("A=A-1");
	output_assembler("M=D");
}
static void push_address_offset_value(register_name rn, int offset){
	register_to_A(rn);
	output_assembler("A=M"); // A = ARG
	output_assembler("D=A");  // D = ARG
	A_instructions(to_string(offset));
	output_assembler("A=D+A"); // A = ARG + offset 
	output_assembler("D=M"); 
	push_D(); 
}

static void push_constant(int number){
    A_instructions(to_string(number));
    push_A();
}
static void push_static(int number){
    A_instructions(get_class_name()+"."+to_string(number));
    output_assembler("A=M");
	push_A();
}
static void push_pointer(int number){
    push_register((register_name)3 + number);
}
static void push_temp(int number){
    push_register((register_name)5 + number);
}
//pop
static void pop_temp(int number){
    pop_register((register_name)5 + number);
}
static void pop_pointer(int number){
    pop_register((register_name)3 + number);
}
static void pop_address_offset_value(register_name rn, int offset){
	register_to_A(rn); // A = &ARG
	output_assembler("A=M"); // A = ARG
	output_assembler("D=A");  // D = ARG
	A_instructions(to_string(offset)); // A = offset
	output_assembler("D=D+A"); // A = ARG + offset
    register_to_A(R13); // A = 
	output_assembler("M=D"); 
    pop_D();
    register_to_A(R13);
    output_assembler("A=M"); 
	output_assembler("M=D"); 
}
static void pop_static(int number){
    pop_D();
    A_instructions(get_class_name()+"."+to_string(number));
    output_assembler("M=D"); 

}
static void pop_D(){
    register_to_A(SP);
	output_assembler("AM=M-1"); 
	output_assembler("D=M"); 
}

static void pop_A(){
    pop_D();
    output_assembler("A=D");
}

static void pop_register(register_name rn){
    pop_D();
    A_instructions("R"+to_string(rn)); 
	output_assembler("M=D"); 
}
// function 
static void output_function(string label,int number){
    output_assembler("// function "+label+" "+to_string(number)) ;
    set_class_and_function_name(label);
    output_label (label);
    for (int i = 1; i <= number; i++){
        push_0();
    }
}

static void output_call(string label,int number){
    output_assembler("// call " + label + " " + to_string(number)) ;
    // push
    A_instructions(get_temp_label());
    push_A();
    push_register(LCL);
    push_register(ARG);
    push_register(THIS);
    push_register(THAT);

    // updata register
    // ARG = SP - n - 5
    register_to_A(SP);
    output_assembler("D=M");// D = SP
    A_instructions("5");
    output_assembler("D=D-A");// D = SP - 5
    A_instructions(to_string(number));
    output_assembler("D=D-A");// D = SP - 5 - n
    register_to_A(ARG);
    output_assembler("M=D");// M = SP - 5 - n
    // LCL = SP
    register_to_A(SP);
    output_assembler("D=M");// D = SP
    register_to_A(LCL);
    output_assembler("M=D");// LCL = SP

    // jmp label
    jmp_label(label);
    // add label
    add_temp_label();
    updata_counter();
}

/************      END OF HELPER FUNCTIONS       **************/

///////////////////////////////////////////////////////////////



// the function translate_vm_class() will be called by the main program
// its is passed the abstract syntax tree constructed by the parser
// it walks the abstract syntax tree and produces the equivalent VM code as output
static void translate_vm_class(ast root)
{
    // assumes we have a "class" node containing VM command nodes
    ast_mustbe_kind(root,ast_vm_class) ;

    // tell the output system we are starting to translate VM commands for a Jack class
    start_of_vm_class() ;

    int ncommands = size_of_vm_class(root) ;
    for ( int i = 0 ; i < ncommands ; i++ )
    {
        translate_vm_command(get_vm_class(root,i)) ;
    }

    // tell the output system we have just finished translating VM commands for a Jack class
    end_of_vm_class() ;

}

// translate the current vm command - a bad node is a fatal error
static void translate_vm_command(ast command)
{
    switch(ast_node_kind(command))
    {
    case ast_vm_operator:
        translate_vm_operator(command) ;
        break ;
    case ast_vm_jump:
        translate_vm_jump(command) ;
        break ;
    case ast_vm_function:
        translate_vm_func(command) ;
        break ;
    case ast_vm_stack:
        translate_vm_stack(command) ;
        break ;
    default:
        fatal_error(0,"// bad node - expected vm_operator, vm_jump, vm_function or vm_stack\n") ;
        break ;
    }
}

// translate vm operator command into assembly language
static void translate_vm_operator(ast vm_op)
{
    // extract command specific info from the ast node passed in
    string the_op = get_vm_operator_command(vm_op) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_operator_command(the_op) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    if (the_op == "add"){
		op_add();
	}else if (the_op == "return"){
		op_return();
	} else if (the_op == "and"){
		op_and();
	} else if (the_op == "eq"){
		op_eq();
	} else if (the_op == "gt"){
		op_gt();
	} else if (the_op == "lt"){
		op_lt();
	} else if (the_op == "neg"){
		op_neg();
	} else if (the_op == "not"){
		op_not();
	} else if (the_op == "or"){
		op_or();
	} else{
		op_sub();
	} 

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// translate vm operator command into assembly language
static void translate_vm_jump(ast jump)
{
    // extract command specific info from the ast node passed in
    string command = get_vm_jump_command(jump) ;
    string label = get_vm_jump_label(jump) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_jump_command(command,label) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    if (command == "label"){
		output_label(get_prefix() + label);
	}else if (command == "if-goto"){
		output_if_goto(label);
	}else{ // goto
		output_goto(label);
	}

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// translate vm operator command into assembly language
static void translate_vm_func(ast func)
{
    // extract command specific info from the ast node passed in
    string command = get_vm_function_command(func) ;
    string label = get_vm_function_label(func) ;
    int number = get_vm_function_number(func) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_func_command(command,label,number) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // function ::= 'call' | 'function'
    output_assembler("// "+command+" " + label +" "+to_string(number)) ; 
    if (command == "function"){
        output_function(label,number);
    }else{ // call 
        output_call(label,number);
    }

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// translate vm operator command into assembly language
static void translate_vm_stack(ast stack)
{
    // extract command specific info from the ast node passed in
    string command = get_vm_stack_command(stack) ;
    string segment = get_vm_stack_segment(stack) ;
    int number = get_vm_stack_offset(stack) ;

    // tell the output system what kind of VM command we are now trying to implement
    start_of_vm_stack_command(command,segment,number) ;

    /************   ADD CODE BETWEEN HERE   **************/

    // use the output_assembler() function to implement this VM command in Hack Assembler
    // careful use of helper functions you can define above will keep your code simple
    // ...
    output_assembler("// "+command+" " + segment +" "+to_string(number)) ; 

    if (command == "push"){
        if (segment == "static"){
            push_static(number);
        }else if (segment == "constant"){
            push_constant(number);
        }else if (segment == "temp"){
            push_temp(number);
        }else if (segment == "pointer"){
            push_pointer(number);
        }else if (segment == "local"){
            push_address_offset_value(LCL, number);
        }else if (segment == "argument"){
            push_address_offset_value(ARG, number);     
        }else if (segment == "that"){
            push_address_offset_value(THAT, number);
        }else if (segment == "this"){
            push_address_offset_value(THIS, number);
        }
    }else{ // pop
        if (segment == "static"){
            pop_static(number);
        }else if (segment == "temp"){
            pop_temp(number);
        }else if (segment == "pointer"){
            pop_pointer(number);
        }else if (segment == "local"){
            pop_address_offset_value(LCL, number);
        }else if (segment == "argument"){
            pop_address_offset_value(ARG, number);     
        }else if (segment == "that"){
            pop_address_offset_value(THAT, number);
        }else if (segment == "this"){
            pop_address_offset_value(THIS, number);
        }
    }

    /************         AND HERE          **************/

    // tell the output system that we have just finished trying to implement a VM command
    end_of_vm_command() ;
}

// main program
int main(int argc,char **argv)
{
    // parse abstract syntax tree and pass to the translator
    translate_vm_class(ast_parse_xml()) ;
    // flush output and errors
    print_output() ;
    print_errors() ;
}
