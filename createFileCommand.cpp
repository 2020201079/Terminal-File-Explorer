#include "header.h"

void moveCommand(){};
void renameCommand(){};

void createFileCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=2){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string destinationFolder = arguments.back();
    if(isPathRelative(destinationFolder))
        destinationFolder = relativeToAbsolute(destinationFolder);
    std::string fileName = arguments[0];
    std::string destinationFile =destinationFolder.append("/").append(fileName);
    int fdDest = open(destinationFile.c_str(),O_WRONLY|O_CREAT);
    if(fdDest < 0){
        std::cout<<"couldn't open file "<<destinationFile<<std::endl;
        return;
    }

}