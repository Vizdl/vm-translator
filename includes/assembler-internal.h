#ifndef _ASSEMBLER_INTERNAL_H
#define _ASSEMBLER_INTERNAL_H

// Interface for the workshop tokeniser
namespace Hack_Virtual_Machine
{
    // shorthand name for the string type
    typedef std::string string ;

    // Internal Assembly - Hack Assembler with a few restrictions on names
    // Labels but in only one of two forms, (<unique function name>) or (<unique label name>)
    // a unique function name is a function name prefixed by its class name and '.', eg Cname.fname
    // a unique label name is a VM language label prefixed by its function's unique name and '$', eg Cname.fname$label
    // class names, function names and VM language labels must start with a letter and can only contain letters, digits and '_'
    // A instructions, eg @43, the number must be in the range 0 to 32767
    // A instructions with unresolved names but only one of four forms, @<static var> or @<unique function name> or @<unique label name> or @<predefined symbol>
    // a static var is the static variable's offset prefixed by its class name and '.', eg Cname.5, the offset must be in the range 0 to 239
    // a predefined symbol is one of SP, LCL, ARG, THIS, THAT, R0 to R15, SCREEN or KBD
    // C instructions
    //
    // NOTE: the first call must be in the form (<unique function name>) or it will fail
    // NOTE: the class name in the unique function name will set the expected class name for this translation
    // NOTE: this function can only be called between a call of start_of_vm_class() and the matching call of end_of_vm_class()
    void output_assembler(string label_or_instr) ;


    // TESTING
    // To assist with testing the following functions ensure that the start and end
    // of each VM command's implementation can be clearly identified in the output.

    // tell the output system we are starting to translate VM commands for a Jack class
    // this must be called only once and before any calls to any other start_*() functions
    void start_of_vm_class() ;

    // tell the output system what kind of VM command we are now trying to implement
    // when called, no other start_*() functions or end_of_vm_class() can be called until after a call to end_of_vm_command()
    void start_of_vm_operator_command(string the_op) ;
    void start_of_vm_jump_command(string jump,string label) ;
    void start_of_vm_func_command(string command,string label,int number) ;
    void start_of_vm_stack_command(string stack,string segment,int number) ;

    // tell the output system that we have just finished trying to implement a VM command
    // this can only be called if the call matches a previous start_of_vm_*_command() call
    void end_of_vm_command() ;

    // tell the output system we have just finished translating VM commands for a Jack class
    // this should be the last function called
    // this will initiate final checks of the instructions generated during the translation
    // no more calls are permitted to output_assembler() or any start_*() or end_*() functions
    void end_of_vm_class() ;

}

#endif //_ASSEMBLER_INTERNAL_H
