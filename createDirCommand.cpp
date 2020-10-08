#include "header.h"

void createDirCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=2){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string destinationFolder = arguments.back();
    if(isPathRelative(destinationFolder))
        destinationFolder = relativeToAbsolute(destinationFolder);
    std::string dirName = arguments[0];
    std::string destinationDir =destinationFolder.append("/").append(dirName);
    int status = mkdir(destinationDir.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status < 0){
        std::cout<<"couldn't create dir "<<destinationDir<<std::endl;
        return;
    }
    if(chmod(destinationDir.c_str(),0777)<0){
        std::cout<<"couldn't set mode to dir "<<destinationDir<<std::endl;
        return;
    }
}