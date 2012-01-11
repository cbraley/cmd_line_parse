#include "CommandLineParser.h"
//--
#include <cctype>
#include <iostream>
#include <cassert>
#include <iomanip>
#include <climits>
//--
#include "IncludedArgParsers.h"

//Helper data -----------------------------------------------------------------
static const int NUM_HELP_ARGS = 4;
static const std::string HELP_STRS[NUM_HELP_ARGS] = {"-h", "--h", "-help", "--help"};

//Helper functions ------------------------------------------------------------
static inline bool isWhitespace(const char x){ return isspace((int)x); }

static std::string trimWhitespaceFront(const std::string& s){
    size_t ssstart = std::string::npos;
    for(size_t i = 0; i < s.size(); i++){
        if(! isWhitespace(s[i]) ){
            ssstart = i;
            break;
        }
    }
    return ssstart == std::string::npos ? std::string("") : s.substr(ssstart, std::string::npos);
}

static std::string trimWhitespaceBack(const std::string& s){
    size_t sslen = 0;
    for(size_t i = s.size() - 1; i >= 0; i--){
        if(! isWhitespace(s[i]) ){
            sslen = i;
            break;
        }
    }
    return s.substr(0,sslen+1);
}

/// Return true if "str" is a valid argument string.  These strings
/// can only be composed of alphanumeric characters, dashes, and punctuation.
/// Whitespace characters and control characters are NOT allowed.
bool isValidArgString(const std::string& str){
    for(size_t i = 0; i < str.size(); i++){
        const char curr = str[i];

        //OK characters are printable non-whitespace chars
        bool charIsOK = isprint((int) curr) && (! isspace((int) curr));
        if(! charIsOK){
            return false;
        }
    }
    return true;
}

bool isHelpStr(const std::string& str){
    for(int i = 0; i < NUM_HELP_ARGS; i++){
        if(str == HELP_STRS[i]){
            return true;
        }
    }
    return false;
}


//-----------------------------------------------------------------------------


//Method Implementations-------------------------------------------------------

void CommandLineParser::initParserTable(){
    //TODO: This will be removed in the next revision.
    parsers[0] = new FloatArgParser();
    parsers[1] = new DoubleArgParser();
    parsers[2] = new IntArgParser();
    parsers[3] = new UnsignedIntArgParser();
    parsers[4] = new StringArgParser();
    parsers[5] = new BoolArgParser();
}


const ArgParser* const CommandLineParser::getCommonArgParser(CommonParser parser)const{
    //TODO: This will be removed in the next revision.
    switch(parser){
        case AP_INT    :
            return parsers[2];
        case AP_UINT   :
            return parsers[3];
        case AP_FLOAT  :
            return parsers[0];
        case AP_DOUBLE :
            return parsers[1];
        case AP_STRING :
            return parsers[4];
        case AP_BOOLEAN:
            return parsers[5];
        default        :
            return NULL;
    }
}

bool CommandLineParser::appendArgHelper(void* argVar, const std::string argName,
    const ArgParser* parser, const std::string helpStr, bool optional, bool named){

    //Positional arguments CANT be optional
    assert( named || (!optional && !named) );

    if(!isArgumentNameOK(argName)){
        return false;
    }else{
        struct Arg arg;
        arg.var      = argVar;
        arg.name     = argName;
        arg.parser   = parser;
        arg.helpTxt  = helpStr;
        arg.optional = optional;
        arg.named    = named;

        orderedArgs.push_back(arg);
        usedArgNames.insert(argName);
        return true;
    }
}

CommandLineParser::CommandLineParser(const std::string& binaryName,
    const std::string& helpMessage) : appName(binaryName),
    helpMsg(trimWhitespaceFront(trimWhitespaceBack(helpMessage)))
{
    initParserTable();
}

CommandLineParser::~CommandLineParser(){
    for(int i = 0; i < 6; i++){
        delete parsers[i]; parsers[i] = NULL;
    }
}


CommandLineParser& CommandLineParser::operator=(const CommandLineParser& rhs){
    helpMsg = rhs.helpMsg;
    appName = rhs.appName;
    usedArgNames = rhs.usedArgNames;
    orderedArgs = rhs.orderedArgs;
    initParserTable();
    return *this;
}

CommandLineParser::CommandLineParser(const CommandLineParser& other) :
    appName(other.appName), helpMsg(other.helpMsg),
    usedArgNames(other.usedArgNames),
    orderedArgs(other.orderedArgs)
{
    initParserTable();
}


bool CommandLineParser::isArgumentNameOK(const std::string& name)const{
    return
        //Arg name can't be reserved for one of the "--help" keywords,
        //unless we have no help message.
        ((!hasHelpMesage()) || (!isHelpStr(name)))            &&
        //Can't use a argument name more than once.
        (usedArgNames.find(name) == usedArgNames.end())       &&
        //Argument name string can only have valid characters.
        isValidArgString(name);
}

bool CommandLineParser::appendPositionalArgument(void* argVar, const std::string argName,
    const ArgParser* parser, const std::string helpStr){
    return appendArgHelper(argVar, argName, parser, helpStr, false, false);
}

bool CommandLineParser::appendNamedArgument(void* argVar, const std::string argName,
    const ArgParser* parser, bool optional, const std::string helpStr){
    return appendArgHelper(argVar, argName, parser, helpStr, optional, true);
}

CommandLineParser::ParseDoneStatus CommandLineParser::parse(int argc,
    char** argv, std::list<std::string>& errs, std::ostream& os)const{

    //Make a list of arguments
    std::list<std::string> args;
    for(int i = 1; i < argc; i++){ //This is 1(not 0) to skip the program name
        args.push_back(std::string(argv[i]));
    }

    //First check if we should print the help message and be done
    for(std::list<std::string>::iterator itr = args.begin(); itr != args.end(); itr++){
        if(isHelpStr(*itr)){
            printHelpMessage(appName);
            return HELP_PRINTED;
        }
    }

    //Make a mutable list of the ordered arguments
    std::list<struct Arg> mArgList;
    for(size_t i = 0; i < orderedArgs.size(); i++){
        mArgList.push_back(orderedArgs[i]);
    }

    while(args.size() > 0){ //Keep parsing argumuments until none are left

        if(mArgList.size() == 0){
            //This indicates that some argument(s) in args are not matched with anything
            for(std::list<std::string>::iterator it = args.begin(); it != args.end(); it++){
                if(usedArgNames.find(*it) != usedArgNames.end()){
                    errs.push_back("Argument " + *it +
                        " appeared more then once(or in an invalid manner)" +
                        "in the argument list.");
                }else{
                    errs.push_back("Argument " + *it + " is not recognized.");
                }
            }
            return ERROR;
        }

        //Check if we are parsing a single positional argument or a sequence of optional arguments
        struct Arg currParser = Arg(mArgList.front());
        mArgList.pop_front();
        if(! currParser.named){ //We are dealing with a single positional non-named argument
            //Parse one positional argument
            std::string errStr = "";
            const bool success = currParser.parser->parseArg(args, currParser.var, errStr);
            if(!success){
                errs.push_back(errStr);
                return ERROR;
            }

        }else{ //We are dealing with 1 or more named arguments

            //Make a std::set of all the named arguments
            std::set<struct Arg> namedArgParserList;
            namedArgParserList.insert(currParser);
            while(mArgList.size() > 0 && mArgList.front().named){
                namedArgParserList.insert(Arg(mArgList.front()));
                mArgList.pop_front();
            }

            //Keep parsing named arguments until we can't get any more
            bool foundMatch = true;
            while(args.size() >= 2 && foundMatch){

                //Create a dummy argument to search for
                const std::string key = std::string(args.front());
                args.pop_front();
                struct Arg dummy;
                dummy.name = key;

                //Check if we have a match
                std::set<struct Arg>::iterator itr = namedArgParserList.find(dummy);
                foundMatch = itr != namedArgParserList.end();
                if(foundMatch){
                    std::string errStr = "";
                    const bool success = itr->parser->parseArg(args, itr->var, errStr);
                    if(!success){
                        errs.push_back(errStr);
                        return ERROR;
                    }
                    namedArgParserList.erase(itr);
                }
            }

            //Make sure that the only named arguments left are optional
            bool missedAtLeastOneArg = false;
            for(std::set<struct Arg>::iterator leftItr = namedArgParserList.begin();
                leftItr != namedArgParserList.end(); leftItr++){
                if(! leftItr->optional){
                    errs.push_back("No value specified for required named argument " + leftItr->name);
                    missedAtLeastOneArg = true;
                }
            }
            if(missedAtLeastOneArg){
                return ERROR;
            }


        }
    }

    //Make sure we matched all the non-named arguments
    bool noErr = true;
    for(std::list<struct Arg>::iterator it = mArgList.begin(); it != mArgList.end(); it++){
        if(! it->optional){
            noErr = false;
            std::string tstr = it->named ? "named" : "positional";
            errs.push_back("Did not match " + tstr + " argument: " + it->name);
        }
    }
    return noErr ? SUCCESS : ERROR;
}


void CommandLineParser::printHelpMessage(const std::string& appName, std::ostream& os)const{

    //String to print to indicate that something must be specified
    const std::string BLANK_SPOT_STR("___");

    //Spacing string between positional arguments and regions of named arguments
    const std::string EXTRA_SPACING("   ");

    //Determine the max number of padding characters we need so that everything
    //lines up when printed out
    size_t wPad = 0;
    for(size_t i = 0; i < orderedArgs.size(); i++){
        wPad = std::max<size_t>(orderedArgs[i].name.size(), wPad);
    }
    //the iomanip setw function takes an int as a padding size, but string length is given
    //as a size_t type(typically unsigned)
    //the below statement ensures that we convert size_t to int properly, even though it is
    //totally ridiculous to assume that the max string length in our list could ever exceed INT_MAX
    int pad = wPad > INT_MAX ? INT_MAX : (int)wPad;


    //Print basic usage line
    bool prevWasPositional = false;
    int optCount = 0;
    os << "usage: " << appName << " ";
    if(hasHelpMesage()){
        ++optCount;
        os << "[--help] ";
        prevWasPositional = false;
    }
    std::vector<size_t> namedIndices, positionalIndices;
    for(size_t i = 0; i < orderedArgs.size(); i++){

        //Create string to print representing the current argument
        std::string nameStr = orderedArgs[i].named ?
            (orderedArgs[i].name + " " + BLANK_SPOT_STR) :
            (orderedArgs[i].name);
        if(orderedArgs[i].optional){
            nameStr = "[" + nameStr + "]";
        }

        //Print the argument
        os << nameStr << " ";

        //Print extra spacing if we are moving from a region of named
        //arguments to a region of positional arguments or vice versa
        const bool currIsPositional = ! orderedArgs[i].named;
        if(currIsPositional ^ prevWasPositional){
            os << EXTRA_SPACING;
            prevWasPositional = currIsPositional;
        }

        //Add to indices lists
        if(orderedArgs[i].named){
            namedIndices.push_back(i);
        }else{
            positionalIndices.push_back(i);
        }

    }
    os << std::endl << std::endl;

    //Print info on what syntax means
    if(optCount > 0 || ! namedIndices.empty()){
        os << "Notation: " << std::endl;
        if(optCount > 0){
            os << "\t[...] indicates that an argument is optional." << std::endl;
        }
        if(! namedIndices.empty()){
            os << "\t" << BLANK_SPOT_STR << " indicates that a value must be placed here." << std::endl;
        }
        os << std::endl;
    }

    //Print short help message
    if(hasHelpMesage()){
        os << helpMsg << std::endl << std::endl;
    }

    //Print off info for each type of argument
    //    positional arguments
    if(positionalIndices.size() > 0){
        os << "Positional arguments: " << std::endl;
        for(size_t i = 0; i < positionalIndices.size(); i++){
            size_t idx = positionalIndices[i];
            os << "\t" << std::setw(pad) << orderedArgs[idx].name << "\t" <<
                orderedArgs[idx].helpTxt << std::endl;
        }
        os << std::endl;
    }
    //    named arguments
    if(namedIndices.size() > 0){
        os << "Named arguments: " << std::endl;
        for(size_t i = 0; i < namedIndices.size(); i++){
            size_t idx = namedIndices[i];
            os << "\t" << std::setw(pad) << orderedArgs[idx].name << "\t" <<
                orderedArgs[idx].helpTxt <<
                (orderedArgs[idx].optional ? "  Optional." : "  Required.") << std::endl;
        }
        os << std::endl << "\tNote that multiple adjacent named arguments can be " <<
            "specified in any order." << std::endl;
    }
}


