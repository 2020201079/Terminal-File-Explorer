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
    std::string fileName = source.substr(source.find_last_of("/\\")+1,source.size()-source.find_last_of("/\\"));
    std::string destinationFolder =destination.append("/").append(fileName);
    int status = mkdir(destinationFolder.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if(status < 0){
        std::cout<<"couldn't create dir "<<destinationFolder<<std::endl;
        return;
    }
    if(chmod(destinationFolder.c_str(),0777)<0){
        std::cout<<"couldn't set mode to dir "<<destinationFolder<<std::endl;
        return;
    }

    DIR *dr = opendir(source.c_str());

    if(dr == NULL){
        die(source.append("could not open for copy ").c_str());
        return;
    }
    struct dirent *de;
    while((de = readdir(dr))!=NULL){
        if(std::string(de->d_name)=="." || std::string(de->d_name)=="..")
            continue;
        
        else if(de->d_type==DT_DIR){
                std::string dirInside = source;
                copyDirectory(dirInside.append("/").append(de->d_name),destinationFolder);
        }
        else{
            std::string filePath = source; 
            copyFile(filePath.append("/").append(de->d_name),destinationFolder);
        }
    }
}

void copyCommand(){
    E.status = "copy failed";
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()<2){
        E.status = "Number of arguments is less for copy";
        std::cout<<"Number of arguments is less"<<std::endl;
        return;
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
        }
        else{
            copyFile(source,destination);
        }
    }
    E.status = " copy completed ";
}