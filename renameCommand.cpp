#include "header.h"

void renameDirectory(std::string source,std::string newName){
    //implement later
}
void renameFile(std::string source,std::string newName){
    std::string path = getParent(source);
    path.append("/").append(newName);
    int fdSource = open(source.c_str(),O_RDONLY);
    if(fdSource < 0){
        std::cout<<"couldn't open file "<<source<<std::endl;
        return;
    }
    
    int fdDest = open(path.c_str(),O_WRONLY|O_CREAT);
    if(fdDest < 0){
        std::cout<<"couldn't open file "<<path<<std::endl;
        return;
    }
    char c;
    while(read(fdSource,&c,1)!=0){
        write(fdDest,&c,1);
    }
    struct stat sourceStat,destStat;
    if(stat(path.c_str(),&destStat) == -1){
        die(path.append(" stat failed while copying to this ").c_str());
        return;
    }
    if(stat(source.c_str(),&sourceStat) == -1){
        die(source.append(" stat failed while copying from this ").c_str());
        return;
    }
    if(chown(path.c_str(),sourceStat.st_uid,sourceStat.st_gid) != 0)
        die(path.append(" error changing ownership in copy (chown) ").c_str());
    if(chmod(path.c_str(),sourceStat.st_mode)!=0){
        die(path.append(" error changing mode in copy(chmod) ").c_str());
    }
}

void renameCommand(){
    E.status = "rename failed";
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=2){
        std::cout<<"Number of arguments is less"<<std::endl;
    }
    std::string newName = arguments[1];
    std::string source = getCompletePath(arguments[0]);
    if(isDirectory(source)){
        renameDirectory(source,newName);
        E.status = "directory rename not implemented";
    }
    else{
        renameFile(source,newName);
        deleteFileCommandHelper(source);
        E.status = "file renamed";
    }
}