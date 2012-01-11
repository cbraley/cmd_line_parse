#include "IncludedArgParsers.h"

#include <cstring>
#include <cstdlib>


bool FloatArgParser::parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
    if(args.size() == 0){
        err = "Argument not present.";
        return false;
    }else{
        const std::string tmp(args.front());
        args.pop_front();
        const bool ret = convertFromStringToT<float>(tmp, *((float*)placeResultHere));
        err = ret ? err : "Parse error on argument: \"" + tmp + "\"";
        return ret;
    }
}

bool DoubleArgParser::parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
    if(args.size() == 0){
        err = "Argument not present.";
        return false;
    }else{
        const std::string tmp(args.front());
        args.pop_front();
        const bool ret = convertFromStringToT<double>(tmp, *((double*)placeResultHere));
        err = ret ? err : "Parse error on argument: \"" + tmp + "\"";
        return ret;
    }
}
bool IntArgParser::parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
    if(args.size() == 0){
        err = "Argument not present.";
        return false;
    }else{
        const std::string tmp(args.front());
        args.pop_front();
        const bool ret = convertFromStringToT<int>(tmp, *((int*)placeResultHere));
        err = ret ? err : "Parse error on argument: \"" + tmp + "\"";
        return ret;
    }
}
bool UnsignedIntArgParser::parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
    if(args.size() == 0){
        err = "Argument not present.";
        return false;
    }else{
        const std::string tmp(args.front());
        args.pop_front();
        const bool ret = convertFromStringToT<unsigned int>(tmp, *((unsigned int*)placeResultHere));
        err = ret ? err : "Parse error on argument: \"" + tmp + "\"";
        return ret;
    }
}
bool StringArgParser::parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
    if(args.size() == 0){
        err = "Argument not present.";
        return false;
    }else{
        const std::string tmp(args.front());
        args.pop_front();
        *((std::string*)placeResultHere) = tmp;
        return true;
    }
}
bool BoolArgParser::parseArg(std::list<std::string>& args, void* placeResultHere, std::string& err)const{
    if(args.size() == 0){
        err = "Argument not present.";
        return false;
    }else{
        const std::string tmp(args.front());
        args.pop_front();
        const bool ret = convertFromStringToT<bool>(tmp, *((bool*)placeResultHere));
        err = ret ? err : "Parse error on argument: \"" + tmp + "\"";
        return ret;
    }
}

