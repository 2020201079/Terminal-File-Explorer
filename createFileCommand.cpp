#include "header.h"

void createFileCommand(){
    E.status = "create file failed";
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=2){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string destinationFolder = arguments.back();
    if(isPathRelative(destinationFolder))
        destinationFolder = relativeToAbsolute(destinationFolder);
    else{
        auto temp = E.root;
        destinationFolder = temp.append("/").append(destinationFolder);
    }
    std::string fileName = arguments[0];
    std::string destinationFile =destinationFolder.append("/").append(fileName);
    int fdDest = open(destinationFile.c_str(),O_RDWR|O_CREAT,0777);
    if(fdDest < 0){
        std::cout<<"couldn't open file "<<destinationFile<<std::endl;
        return;
    }
    if(chmod(destinationFile.c_str(),0777)){
        std::cout<<"couldn't set mode to "<<destinationFile<<std::endl;
        return;
    }
    E.status = "created file ";
}