#include "header.h"

void searchHelper(std::string fileName,std::string path,bool& result){
    if(result == true){
        return; // already found somewhere
    }
    std::string name = path.substr(path.find_last_of("/\\")+1,path.size()-path.find_last_of("/\\"));
    if(fileName == name ){
        result=true;
        return;
    }
    else
    {
        DIR *dr = opendir(path.c_str());
        if(dr == NULL){
            die(path.append("could not open for search ").c_str());
            return;
        }
        struct dirent *de;
        while((de = readdir(dr))!=NULL){
            if(std::string(de->d_name)=="." || std::string(de->d_name)=="..")
                continue;
        
            else if(de->d_type==DT_DIR){
                std::string dirInside = path;
                searchHelper(fileName,dirInside.append("/").append(de->d_name),result);
            }
            else{
                if(de->d_name == fileName){
                    result = true;
                    return;
                } 
            }
        }
    }
}

bool searchCommand(){
    std::string input;
    getline(std::cin,input);
    std::vector<std::string> arguments = getArgs(input);
    if(arguments.size()!=1){
        std::cout<<"Number of arguments is wrong"<<std::endl;
    }
    std::string fileName = arguments[0];
    bool result=false;
    std::string path = E.root;
    searchHelper(fileName,path,result);
    return result;
}