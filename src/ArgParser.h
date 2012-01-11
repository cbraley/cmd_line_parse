#ifndef ARG_PARSER_H
#define ARG_PARSER_H

#include <string>
#include <list>

/**
 *  Pure-virtual base class with one method, parseArg.  To parse custom types on the command line
 *  you should subclass ArgParser.  However, in many situations it may be simpler to define a stream
 *  extraction operator for your custom type "Foo", and then use a GenericParser<Foo> (see
 *  IncludedArgParsers.h for details).
 *
 *  However, for complex parsing, subclassing ArgParser is the reccomended solution.
 */
class ArgParser{
public:
    /**
     *  This function is called to parse command line arguments.  Any argument that is processed should be REMOVED
     *  from the front of the "args" list.  This list is mutable since its passed by reference.  The data
     *  pointed to at placeResultHere should be modified by this call.  If there is an error, modify the "err"
     *  reference variable.
     *
     *  @param args is a mutable list of the current command line arguments being parsed(left to right).
     *  @param placeResultHere is where the results should go.  It's up to the implementation to cast this pointer
     *   appropriately.
     *  @param err is only modified in the event of an error.  Modify "err" to describe what happened.
     *  @return true on success, false on failure.
     */
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const = 0;
};

#endif //ARG_PARSER_H
