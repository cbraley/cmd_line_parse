#include <iostream>
#include <string>
#include <list>
#include <cassert>
//--
#include "CommandLineParser.h"
#include "IncludedArgParsers.h"


///  Reverse a string.
std::string getReversedString(const std::string& str){
    std::string ret(str);
    for(size_t i = 0; i < ret.size(); i++){
        ret[i] = str[str.size()-1-i];
    }
    return ret;
}

int main(int argc, char** argv){

    //Initialize a command line parser with a help message
    CommandLineParser parser(argv[0], "Print a word a specified number of times.  The word defaults to \"cat,\" but can be changed.");

    //Declare all of the variables we wish to get from our command line arguments
    int word_count = 0;
    bool reverse   = false;
    std::string word("cat");

    //Add each argument.  Once they are all added, assert that they were added with no problems.
    bool argsAddedOK = true;
    argsAddedOK &= parser.appendPositionalArgument( (void*) &word_count,
        "-word_count", parser.getCommonArgParser(CommandLineParser::AP_INT),
        "Number of times to print the specified word." );
    argsAddedOK &= parser.appendNamedArgument     ( (void*) &reverse  ,
        "-reverse"  , parser.getCommonArgParser(CommandLineParser::AP_BOOLEAN),
        true, //true here indicates an optional argument
        "Should we reverse the word before printing?  Use \"true\" or \"false\"." );
    argsAddedOK &= parser.appendNamedArgument     ( (void*) &word      ,
        "-word"      , parser.getCommonArgParser(CommandLineParser::AP_STRING),
        true, //true here indicates an optional argument
        "What word to print?  Any string is OK." );

    //Make sure all arguments were added OK
    //This assertion will fail if we had an argument name with a space, or an
    //argument name that was used multiple times.
    assert(argsAddedOK);


    //After the call to parse, any errors found are added to this list.
    std::list<std::string> errsRet;

    //Parse the input arguments to this program.
    CommandLineParser::ParseDoneStatus status =
        parser.parse(argc, argv, errsRet, std::cout);

    //See how things went
    if(status == CommandLineParser::HELP_PRINTED){
        //The user requested a help message, which was already
        //printed to std::cout in parser.parse
        return 0;
    }else if(status == CommandLineParser::ERROR){
        //parser.parse encountered one or more errors!  Nothing has been
        //printed yet; lets print something and inform the user.
        std::cerr << "Encountered one or more errors when parsing" <<
            " command line arguments:" << std::endl;
        for(std::list<std::string>::iterator itr = errsRet.begin();
            itr != errsRet.end(); itr++){
            std::cerr << "\t" << *itr << std::endl;
        }
        return 1;
    }else{

        //This is the "core" of the program.

        //Arguments parsed successfully, lets do the computation.
        if(reverse){
            word = getReversedString(word);
        }
        for(int i = 0; i < word_count; i++){
            std::cout << word << std::endl;
        }

        return 0;
    }
}
