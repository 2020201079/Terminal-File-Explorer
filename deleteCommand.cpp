#include "header.h"

void deleteFileCommandHelper(std::string filePath){
    if(remove(filePath.c_str()) < 0){
        std::cout<<"error deleting file"<<std::endl;
    }
}
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
    deleteFileCommandHelper(filePath);
}



void deleteDirCommandHelper(std::string dirPath){
    std::cout<<"deleteFileCommandHelper is called"<<std::endl;
    struct dirent *de; //pointer for directory entry
    std::cout<<"Following dir will be deleted "<<std::endl;
    std::cout<<dirPath<<std::endl;
    DIR *dr = opendir(dirPath.c_str());

    if(dr == NULL){
        die(dirPath.append("could not open for deleting ").c_str());
        return;
    }

    while((de = readdir(dr))!=NULL){
        if(de->d_name=="." || de->d_name=="..")
            continue;
        if(de->d_type==DT_DIR){
            deleteDirCommandHelper(dirPath.append(de->d_name));
        }
        else{
            deleteFileCommandHelper(dirPath.append(de->d_name));
        }
        //printf("%s\n",de->d_name);
    }
    closedir(dr);
    if(rmdir(dirPath.c_str()) == -1){
        die(dirPath.append(" rmdir failed on this ").c_str());
    }
}

void deleteDirCommand(){
    std::cout<<"Need to implement delete dir command !"<<std::endl;
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=1){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string dirPath = arguments[0];
    if(dirPath == "." || dirPath==".." || dirPath=="~/"){
        std::cout<<"shouldn't delete curr dir"<<std::endl;
        return;
    }
    if(isPathRelative(dirPath)) //if not the whole path will be provided 
        dirPath = relativeToAbsolute(dirPath);
    deleteDirCommandHelper(dirPath);
}

