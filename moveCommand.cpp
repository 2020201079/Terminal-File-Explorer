//move is same as copy then delete the copied 
#include "header.h"

void moveCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()<2){
        std::cout<<"Number of arguments is less"<<std::endl;
    }
    std::string destination = arguments.back();
    if(isPathRelative(destination))
        destination = relativeToAbsolute(destination);
    else{
        auto temp = E.root;
        destination = temp.append("/").append(destination);
    }
    arguments.pop_back();
    for(std::string arg : arguments ){
        std::string source = getCompletePath(arg);
        if(isDirectory(source)){
            copyDirectory(source,destination);
            deleteDirCommandHelper(source);
        }
        else{
            copyFile(source,destination);
            deleteFileCommandHelper(source);
        }
    }
}