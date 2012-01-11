#ifndef INCLUDED_ARG_PARSERS_H
#define INCLUDED_ARG_PARSERS_H

#include <string>
#include <list>
#include <sstream>
//--
#include "ArgParser.h"


/**
 *  Template helper function.
 *  Convert an arbitrary string str to an object of type T.
 *  T MUST have a properly overloaded stream extraction operator (<<)
 *  for this to work.
 *
 *  @param str is the string to convert.
 *  @param ret is a reference through which the result is returned.
 *  @eturn true on success, false otherwise.
 */
template<typename T>
inline bool convertFromStringToT(const std::string& str, T& ret){
    std::istringstream ss(str);
    ss >> std::boolalpha; //Allow "true" and "false" for booleans;
    //and DISALLOW "1" and "0"
    ss >> std::ws >> ret >> std::ws;
    return ss.eof();
}

/**
 *  Generic ArgParser class that can parse any type T that has a properly
 *  coded stream extraction operator.
 *
 *  Warning - If you attempt to instantiate a GenericParser
 *  with some type T=Foo where Foo has no stream extraction operator
 *  (GenericParser<Foo>) verbose template errors will result from the compiler!
 */
template<typename T>
class GenericParser : public ArgParser{
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
        if(args.empty()){
            err = "No text given.";
            return false;
        }else{
            const std::string arg = args.front();
            args.pop_front();
            const bool ok = convertFromStringToT<T>(arg, *((T*)placeResultHere));
            if(!ok){
                err = "Error when parsing: " + arg;
                return false;
            }else{
                return true;
            }
        }
    }

    virtual std::string name()const{ return "Generic_Parser"; }
};

class FloatArgParser : public ArgParser{
public:
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const;
};
class DoubleArgParser : public ArgParser{
public:
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const;
};
class IntArgParser : public ArgParser{
public:
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const;
};
class UnsignedIntArgParser : public ArgParser{
public:
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const;
};
class StringArgParser : public ArgParser{
public:
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const;
};
class BoolArgParser : public ArgParser{
public:
    virtual bool parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const;
};

#endif //INCLUDED_ARG_PARSERS_H
