#include "header.h"

void gotoCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=1){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string destinationFolder = arguments.back();
    if(isPathRelative(destinationFolder))
        destinationFolder = relativeToAbsolute(destinationFolder);
    E.root = destinationFolder;
}