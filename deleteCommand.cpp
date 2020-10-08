#include "header.h"

void deleteFileCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=1){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string filePath = arguments[0];
    if(isPathRelative(filePath)) //if not the whole path will be provided 
        filePath = relativeToAbsolute(filePath);
    
    if(remove(filePath.c_str()) < 0){
        std::cout<<"error deleting file"<<std::endl;
    }
}