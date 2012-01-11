#ifndef COMMAND_LINE_PARSER_H
#define COMMAND_LINE_PARSER_H

#include <string>
#include <list>
#include <set>
#include <vector>
#include <iostream>
//--
#include "ArgParser.h"


/**
 *  Class that parses command line arguments.  Typically, one will create a single
 *  CommandLineParser object in int main(...).  After the CommandLineParser has been created,
 *  one typically uses the "parse" method to process the command line arguments.
 *
 *  CommandLineParsers currently suport two kinds of arguments: positional arguments
 *  and named arguments.  Positional arguments are identified by their position(order)
 *  in argv.  Named arguments are prefixed by a string that identifies them.  For example,
 *  in the "ls" command, the directory whose contents we wish to print would be a
 *  positional argument, since we use ls like this "ls ~/foo".  However, ls also takes
 *  arguments like "-w" which allow you to specify the width of each printed line
 *  as a number.  The argument "-w" to ls would be a named argument.  Named arguments
 *  can be optional or mandatory.
 *
 *  Arguments are added through the append*Argument functions.  The order in which called to
 *  append*Argument matter, since the argument list is ordered.
 *
 *
 *  Custom argument types can be included by subclassing ArgParser.h.
 */
class CommandLineParser{
public:

    /**
     *  Create a parser.  Help message optional.
     */
    CommandLineParser(const std::string& binaryName, const std::string& helpMessage = "");

    /**
     *  Destructor.
     */
    virtual ~CommandLineParser();

    /**
     *  Overloaded assignment operator.  Deep copy.
     */
    CommandLineParser& operator=(const CommandLineParser& rhs);

    /**
     *  Overloaded copy constructor.  Deep copy.
     */
    CommandLineParser(const CommandLineParser& other);

    /**
     *  Status returned from the parse method.  ERROR indicates that
     *  something went wrong, HELP_PRINTED indicates that a help message
     *  was printed(since the user required it via --help or similar), and
     *  SUCCESS indicates that all non-optional arguments were parsed
     *  properly and successfully.
     */
    enum ParseDoneStatus{ SUCCESS, ERROR, HELP_PRINTED };
    /**
     *  Parse a command line.  For any error encountered, a std::string
     *  describing what went wrong is appended to "err."  Returns a ParseDoneStatus
     *  to tell you what happened as a result of the parse.
     *
     *  If the user wants to print a help message(and once was provided) this
     *  method will print to the sream outStream.  outStream defaults to std::cout.
     */
     ParseDoneStatus parse(int argc, char** argv, std::list<std::string>& errs,
        std::ostream& outStream = std::cout)const;

    /**
     *  Enum that can be passed to getCommonArgParser() to allow easy creation of
     *  an ArgParser that is commonly used.
     */
    enum CommonParser{AP_INT, AP_UINT, AP_FLOAT, AP_DOUBLE, AP_STRING, AP_BOOLEAN};

    /**
     *  Return a pointer to a common type of argument parser.
     *  Do NOT delete this pointer(this memory is managed by CommandLineParser
     *  internally).  Typically, this function is used similar to what is shown below:
     *
     *  CommandLineParser parser("bin/foobar");
     *  ...
     *  parser.appendPositionalArgument(ptr, "foo", parser.getCommonArgParser(CommandLineParser::INT));
     *
     *  This is to avoid placing the burden on the end user of dynamically
     *  allocating many ArgParsers for very common types of arguments.
     *
     *  @param parser is the type of parser you wish to get.
     *  @return a pointer to the parser.  Do NOT delete this pointer.
     */
    const ArgParser* const  getCommonArgParser(CommonParser parser)const;


    /**
     *  Append a positional argument to the argument list.
     *  All positional arguments are required.
     *
     *  @param argVar pointer to the data which should be updated by this argument.
     *   This pointer must be casted to void*.
     *  @param argName name of the argument.  isArgumentNameOK(argName) must return true
     *   for this name.  The function will return false if this is not true.
     *  @param parser is a pointer to an ArgParser that can parse the specified type of data.
     *   See getCommonArgParser() to obtain simple ArgParser for common data types.
     *  @param helpStr is an optional argument that should give a description of the argument
     *   being appended.
     *  @return true on success, false on failure.
     */
    bool appendPositionalArgument(void* argVar, const std::string argName,
        const ArgParser* parser, const std::string helpStr = "");

    /**
     *  Append a named argument to the argument list.
     *  Named arguments may be optional or required.
     *
     *  @param argVar pointer to the data which should be updated by this argument.
     *   This pointer must be casted to void*.
     *  @param argName name of the argument.  isArgumentNameOK(argName) must return true
     *   for this name.  The function will return false if this is not true.
     *  @param parser is a pointer to an ArgParser that can parse the specified type of data.
     *   See getCommonArgParser() to obtain simple ArgParser for common data types.
     *  @param optional should be true if this argument is not required to be found, or false
     *   if the argument must be present.  By default, named arguments are optional.
     *  @param helpStr is an optional argument that should give a description of the argument
     *   being appended.
     *  @return true on success, false on failure.
     */
    bool appendNamedArgument(void* argVar, const std::string argName,
        const ArgParser* parser, bool optional = true, const std::string helpStr = "");

    /**
     *  Check if a given argument name is OK to be used.
     *  Names are ok if they satisfy the following properties:
     *    -Not already used.
     *    -Not -h, -help, --help, or --h.
     *    -They contain only printable ASCII chars and no whitespace in the name.
     *  Typically this is not needed to be called by the client code.
     *  This predicate returns true for all argNames passed into appendNamedArgument
     *  and appendPositionalArgument.
     */
    bool isArgumentNameOK(const std::string& name)const;

    /**
     *  Print the applications help message. Usually not needed to be called by the end user.
     *  @param appName is the name of the binary.  Should usually be argv[0].
     *  @param os is the stream to print on.  Defaults to std::cout.
     */
    void printHelpMessage(const std::string& appName, std::ostream& os = std::cout)const;


private: //--------------------------------------------------------------------
    std::string appName;
    std::string helpMsg;
    inline bool hasHelpMesage()const{ return ! helpMsg.empty(); }

    std::set<std::string> usedArgNames;
    struct Arg{
        void* var;
        std::string name;
        const ArgParser* parser;
        std::string helpTxt;
        bool optional;
        bool named;

        //We use this to sort arguments
        inline bool operator<(const struct Arg& rhs)const{ return name < rhs.name; }

    };
    std::vector<struct Arg> orderedArgs;

    void initParserTable();

    //Table of default parsers
    //TODO: Having this table here of commonly used parsers is ugly and necessitates
    //having a deep-copy in the copy c-tor and the assignment operator.  Instead, I should
    //refactor this code to use a static method in ArgParse to gain easy access to each of the
    //commonly used parsers.
    //
    //The initial rationale behind this table was to allow the end user, who us likely creating
    //an CommandLineParser inside of main, to avoid having to dynamically allocate simple things
    //like FloatArgParsers and IntArgParsers.
    ArgParser* parsers[6];

    bool appendArgHelper(void* argVar, const std::string argName,
        const ArgParser* parser, const std::string helpStr, bool optional, bool
        named);
};

//Note:
//I also considered designing CommandLineParser as follows:
//
//template based design:
//    template<typename T>
//    bool appendArgument(T* ptr, const std::string& argName, const std::string& helpStr = "");
//
//    This would remove the void pointers(more type safety) at the expense of making the system
//    more complex.  For this to work, all T types would have to have an overloaded stream insertion
//    operator, and the code would not compile with a convoluted template error message if an end
//    user tried to use a type T without a stream insertion operator << defined properly.  I rejected
//    this design for that reason.
//
//    See Generic_Parser<T> in IncludedArgParsers.h for some template stuff...
//
//bool appendArgument(void* ptr, const std::string& argName, (void* (*func_ptr)(std::string str) )
//    Rejected this design because function pointers are unfamiliar to many C++ programmers,
//    whereas inheritance is familiar to nearly everyone.  The function pointer based design might
//    be better in the future once C++0x lambdas are widely used and function pointers are more common.


#endif//COMMAND_LINE_PARSER_H

