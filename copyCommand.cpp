#include "header.h"

void copyFile(std::string source,std::string destinationFolder){
    std::cout<<"source : "<<source<<std::endl;
    std::cout<<"destination : "<<destinationFolder<<std::endl;
    int fdSource = open(source.c_str(),O_RDONLY);
    if(fdSource < 0){
        std::cout<<"couldn't open file "<<source<<std::endl;
        return;
    }
    std::string fileName = source.substr(source.find_last_of("/\\")+1,source.size()-source.find_last_of("/\\"));
    std::string destinationFile =destinationFolder.append("/").append(fileName);
    
    int fdDest = open(destinationFile.c_str(),O_WRONLY|O_CREAT);
    if(fdDest < 0){
        std::cout<<"couldn't open file "<<destinationFile<<std::endl;
        return;
    }
    char c;
    while(read(fdSource,&c,1)!=0){
        write(fdDest,&c,1);
    }
    struct stat sourceStat,destStat;
    if(stat(destinationFile.c_str(),&destStat) == -1){
        die(destinationFile.append(" stat failed while copying to this ").c_str());
        return;
    }
    if(stat(source.c_str(),&sourceStat) == -1){
        die(source.append(" stat failed while copying from this ").c_str());
        return;
    }
    if(chown(destinationFile.c_str(),sourceStat.st_uid,sourceStat.st_gid) != 0)
        die(destinationFile.append(" error changing ownership in copy (chown) ").c_str());
    if(chmod(destinationFile.c_str(),sourceStat.st_mode)!=0){
        die(destinationFile.append(" error changing mode in copy(chmod) ").c_str());
    }

}

void copyDirectory(std::string source,std::string destination){
    
}

void copyCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()<2){
        std::cout<<"Number of arguments is less"<<std::endl;
    }
    std::string destination = arguments.back();
    if(isPathRelative(destination))
        destination = relativeToAbsolute(destination);
    arguments.pop_back();
    for(std::string arg : arguments ){
        std::string source = getCompletePath(arg);
        if(isDirectory(source)){
            copyDirectory(source,destination);
        }
        else{
            copyFile(source,destination);
        }
    }

    
}