#include "header.h"

void deleteFileCommandHelper(std::string filePath){
    if(remove(filePath.c_str()) < 0){
        std::cout<<"error deleting file"<<std::endl;
    }
}
void deleteFileCommand(){
    E.status = "Delete file failed";
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=1){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string filePath = arguments[0];
    if(isPathRelative(filePath)) //if not the whole path will be provided 
        filePath = relativeToAbsolute(filePath);
    else{
        auto x = E.root;
        filePath = x.append("/").append(filePath);
    }
    deleteFileCommandHelper(filePath);
    E.status = "File deleted";
}



void deleteDirCommandHelper(std::string dirPath){
    std::cout<<"deleteDirCommandHelper is called"<<std::endl;
    struct dirent *de; //pointer for directory entry
    std::cout<<"Following dir will be deleted "<<std::endl;
    std::cout<<dirPath<<std::endl;
    DIR *dr = opendir(dirPath.c_str());

    if(dr == NULL){
        die(dirPath.append("could not open for deleting ").c_str());
        return;
    }

    while((de = readdir(dr))!=NULL){
        if(std::string(de->d_name)=="." || std::string(de->d_name)=="..")
            continue;
        if(de->d_type==DT_DIR){
            std::string dirInside = dirPath;
            deleteDirCommandHelper(dirInside.append("/").append(de->d_name));
        }
        else{
            std::string filePath = dirPath; 
            deleteFileCommandHelper(filePath.append("/").append(de->d_name));
        }
        //printf("%s\n",de->d_name);
    }
    closedir(dr);
    if(rmdir(dirPath.c_str()) == -1){
        die(dirPath.append(" rmdir failed on this ").c_str());
    }
}

void deleteDirCommand(){
    E.status = "delete dir failed";
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
    else{
        auto x = E.root;
        dirPath = x.append("/").append(dirPath);
    }
    deleteDirCommandHelper(dirPath);
    E.status = "delete dir succesful";
}

