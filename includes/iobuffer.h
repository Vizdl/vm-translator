#ifndef CSTOOLS_IOBUFFER_H
#define CSTOOLS_IOBUFFER_H

#include <iostream>
#include <string.h>

// the cstools iobuffer buffers data and errors to be written until explicitly erased or printed
// trace writes can be optionally included in the normal output
// log messages can be optionally included in the error output
// printing buffers following a fatal error is optional
// environment variables can be used to override any options set using the config_* functions

namespace CS_IO_Buffers
{
    // buffer configurations
    enum iob_option
    {
        iob_default,        // apply the default configuration
        iob_enable,         // future write_to_* calls are performed
        iob_disable,        // future write_to_* calls are ignored

                            // the following can only be applied to traces and logs
        iob_not_permitted,  // future write_to_* calls result in a fatal_error, this cannot be changed

                            // the following can only be applied to output and errors
        iob_buffer,         // future write_to_* calls are buffered, erase_* is called by fatal error
        iob_loud_buffer,    // future write_to_* calls are buffered, print_* is called by fatal error
        iob_immediate,      // print buffer, future write_to_* calls are immmediately followed by a print_* call

                            // this option only applies to errors
        iob_buffer_why,     // future write_to_errors calls are buffered, fatal_error calls erase_errors but prints it's message
        iob_context,        // fatal_error will call construct_error_context() if the error message is not ""
        iob_no_context      // fatal_error will not call construct_error_context()
    } ;

    // use the output buffer for all output normally written to std::cout
    extern void write_to_output(std::string s) ;        // adds s to the end of the output buffer
    extern void erase_output() ;                        // erase the output buffer contents, nothing is printed
    extern void print_output() ;                        // print the output buffer contents, then call erase_output()
    extern void config_output(iob_option opt) ;         // configure future behaviour, default is iob_immediate or the
                                                        // contents of the environment variable CSTOOLS_IOBUFFER_OUTPUT

    // use the following for trace prints - these messages are written to the output buffer
    extern void write_to_traces(std::string s) ;        // calls write_to_output(s)
    extern void prefix_traces(std::string prefix) ;     // if prefix is not "", future write_to_traces(s) call write_to_output(prefix+s+"\n")
                                                        // if necessary, an initial "\n" will be written so that the trace message starts a new line
    extern void config_traces(iob_option opt) ;         // configure future behaviour, default is iob_enable or the
                                                        // contents of the environment variable CSTOOLS_IOBUFFER_TRACES

    // use the errors buffer for all output normally written to std::cerr
    extern void write_to_errors(std::string s) ;        // adds s to the end of the error buffer
    extern void write_to_errors(int n) ;                // converts n to a string, then adds it to the end of the error buffer
    extern void erase_errors() ;                        // erase the error buffer contents, nothing is printed
    extern void print_errors() ;                        // print the error buffer contents, then call erase_errors()
    extern void config_errors(iob_option opt) ;         // configure future behaviour, default is iob_immediate and iob_context or the
                                                        // contents of the environment variable CSTOOLS_IOBUFFER_ERRORS

    // use the following for logging - these messages are written to the errors buffer
    extern void write_to_logs(std::string s) ;          // calls write_to_errors(s)
    extern void prefix_logs(std::string prefix) ;       // if prefix is not "", future write_to_logs(s) call write_to_errors(prefix+s+"\n")
                                                        // if necessary, an initial "\n" will be written so that the log message starts a new line
    extern void config_logs(iob_option opt) ;           // configure future behaviour, default is iob_enable or the
                                                        // contents of the environment variable CSTOOLS_IOBUFFER_LOGS

    // use the following to configure error context messages - these messages are used to format fatal_error messages
    extern void new_error_context(std::string s) ;      // empty the error context stack then push_error_context(s)
    extern void push_error_context(std::string s) ;     // push a new error message onto the error context stack, control characters in s are escaped
                                                        // control characters are escaped using \0, \t, \r, \n or \uXXXX
    extern std::string pop_error_context() ;            // pops and returns the top message from the error context stack, if empty returns ""
    extern int depth_error_context() ;                  // return the current depth of the error context stack
    extern void prune_error_context(int depth) ;        // set the current depth of the error context stack, excess messages are popped
    extern std::string construct_error_context() ;      // return "" if the error context stack is empty
                                                        // otherwise, construct a multi-line error context message:
                                                        //  . the first line is "***** Fatal Error!\n"
                                                        //  . then a numbered list of messages in the order they were pushed onto the error context stack, eg
                                                        //    "    0: " + message + "\n"
                                                        //  . the last line is "error: "

    // to exit due to a fatal error
    extern void fatal_error(int status,std::string s) ; // write_to_errors(construct_error_context() + s + "\n"), performs any configured ouput, then exit(status)
                                                        // if s is "", write_to_errors() will not be called
                                                        // if the last character of s is '\n', '\n' will not be added
    extern void fatal_error(int status) ;               // calls fatal_error(status,"")
    extern void fatal_error(std::string s) ;            // calls fatal_error(0,s)


    // ***************   EXAMPLES OF HOW TO USE THE IO BUFFER   *********************

    // To add a line containing "hello" and 45 to the output buffer:
    //     write_to_output("hello " + std::to_string(45) + "\n") ;
    //
    // To throw away everything written to the output buffer so far:
    //     erase_output() ;
    //
    // To write the current contents of the output buffer to cout - and erase the contents:
    //     print_output() ;
    //
    // To write out an error containing "elephants are big":
    //     write_to_errors("elephants are big\n") ;
    //
    // To stop trace writes appearing in the output buffer
    //     config_traces(iob_enable) ;
    //
    // To stop log writes to appearing in the error buffer
    //     config_logs(iob_enable) ;
    //
    // To cause output and trace writes to be printed immediately:
    //     config_output(iob_immediate) ;
    //
    // To stop all output including the final error message printed after a fatal error:
    //     config_output(iob_buffer) ;
    //     config_errors(iob_buffer) ;
    //
    // To force the configuration of the output buffer to iob_buffer using an environment variable:
    //     if using bash:   export CSTOOLS_IOBUFFER_OUTPUT=iob_buffer
    //     if using tcsh:   setenv CSTOOLS_IOBUFFER_OUTPUT iob_buffer
    //     
    //
    // Notes:
    //  .  if you have written too much data to the write_to_* functions, an error message
    //     will be printed and your program will exit. This is intended to catch programs
    //     stuck in infinite loops, a correct program should never be affected.
    //
    //  .  If an error occurs and you wish to exit the program immediately and without explanation:
    //        fatal_error(0,"") ;
    //
    //  .  The web submission system will use the environment variables to change the initial
    //     configuration of the output and errors buffers to be iob_buffer and it will disable
    //     the traces and logs buffers.
    //
    //  .  If a buffer configuration is set to anything using an environment variable, the
    //     config_* function for that buffer will be disabled, that is the program will not be
    //     allowed to change it.
    //
}

#endif //CSTOOLS_IOBUFFER_H
