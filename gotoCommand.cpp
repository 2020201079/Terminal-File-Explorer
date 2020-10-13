#include "header.h"

void gotoCommand(){
    E.status = "goto failed";
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=1){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string destinationFolder = arguments.back();
    if(isPathRelative(destinationFolder))
        destinationFolder = relativeToAbsolute(destinationFolder);
    else{
        auto temp = E.root;
        destinationFolder = temp.append("/").append(destinationFolder);
    }
    if(isDirectory(destinationFolder)){
        std::vector<std::string> newVec(0);
        for(int i=0;i<=navVecIndex;i++){
            newVec.push_back(navVec[i]);
        }
        navVec = newVec;
        navVec.push_back(destinationFolder);
        navVecIndex++;
        E.root = destinationFolder;
    }
    else{
        return;
    }
    E.status =" goto executed ";
}