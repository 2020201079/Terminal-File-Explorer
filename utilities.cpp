#include "header.h"

std::vector<std::string> getArgs(std::string input){
    std::vector<std::string> result;
    std::string word ="";
    for(auto c: input){
        if(c==' '){
            if(word != ""){
                result.push_back(word);
                word="";
            }
        }
        else{
            word = word + c;
        }
    }
    if(word != "")
        result.push_back(word);
    return result;
}

void die(const char *s) {
    write(STDOUT_FILENO, "\x1b[2J", 4); //clear screen
    write(STDOUT_FILENO, "\x1b[H", 3); // set cursor position
    perror(s);
    exit(1);
}

bool isDirectory(std::string path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        die(path.append(" stat failed in isDirectory ").c_str());
    return S_ISDIR(statbuf.st_mode);
}

bool isFile(std::string path) {
    struct stat statbuf;
    if (stat(path.c_str(), &statbuf) != 0)
        die(path.append(" stat failed in isFile ").c_str());
    return S_ISREG(statbuf.st_mode);
}

void enableRawMode(){
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw= E.orig_termios;
    //raw.c_iflag &= ~( ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr");
}

void disableRawMode() {
  if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
    die("tcsetattr");
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

std::string getParent(std::string path){
    if(path=="/")
        return path;
    else{
        return path.substr(0,path.find_last_of("/\\"));
    }
}

std::string getCompletePath(std::string fileName){
    if(fileName == ".")
        return E.root;
    else if(fileName == ".."){
        return getParent(E.root);
    }
    else{
        std::string result=E.root;
        return result.append("/").append(fileName);
    }    
}

bool isPathRelative(std::string path){
    if(path[0] == '~')
        return true;
    else if(path[0] == '.')
        return true;
    return false;
}

std::string relativeToAbsolute(std::string path){
    std::string result;
    if(path[0] == '~'){ // path starts with ~/foobar should remove ~ and replace root
        result=path.substr(1,path.size()-1);
        result=E.root+result;
        return result;
    }
    else if(path[0]=='.' && path[1]=='.'){
        result=path.substr(2,path.size()-1);
        auto parent= getParent(E.root);
        return parent.append(result);
    }
    else if(path[0]=='.'){
        result="";
        if(path.size()>1)
            result=path.substr(1,path.size()-1);
        result=E.root+result;
        return result;
    }
    else{
        die(path.append(" Conversion from rel to absolute failed").c_str());
        return "";
    }
}

void printEntry(std::string e,int lineNo){
        std::string compPath = getCompletePath(e);
        struct stat fileStatus;
        if(stat(compPath.c_str(),&fileStatus)==-1){
            die(compPath.append(" this path failed").c_str());
        }
        std::string permissions="";
        permissions.append((S_ISDIR(fileStatus.st_mode)) ? "d" : "-");
        permissions.append((fileStatus.st_mode & S_IRUSR) ? "r" : "-");
        permissions.append((fileStatus.st_mode & S_IWUSR) ? "w" : "-");
        permissions.append((fileStatus.st_mode & S_IXUSR) ? "x" : "-");
        permissions.append((fileStatus.st_mode & S_IRGRP) ? "r" : "-");
        permissions.append((fileStatus.st_mode & S_IWGRP) ? "w" : "-");
        permissions.append((fileStatus.st_mode & S_IXGRP) ? "x" : "-");
        permissions.append((fileStatus.st_mode & S_IROTH) ? "r" : "-");
        permissions.append((fileStatus.st_mode & S_IWOTH) ? "w" : "-");
        permissions.append((fileStatus.st_mode & S_IXOTH) ? "x" : "-");

        struct passwd *pw = getpwuid(fileStatus.st_uid);
        if(pw == NULL) die("getpwuid failed");
        std::string user = pw->pw_name;

        struct group *gr = getgrgid(fileStatus.st_gid);
        if(gr == NULL) die("getgrgid failed");
        std::string group = gr->gr_name;

        std::string size = std::to_string(fileStatus.st_size);
        size.append("Bytes");

        E.outputBuffer.append(" ").append(std::to_string(lineNo)).append(" ").append(e).append(" ").append(size).append(" ").append(" ").append(user).append(" ").append(group).append(" ").append(permissions).append("\r\n");
        
}
