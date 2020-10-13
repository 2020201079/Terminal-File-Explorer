#include "header.h"

/*** nav ***/
std::vector<std::string> navVec(0);
int navVecIndex=-1;
/*** data ***/
struct editorConfig E;

/*** defines ***/

enum editorKey {
  ARROW_UP =2000,
  ARROW_DOWN ,
  ARROW_LEFT,
  ARROW_RIGHT,
  COLON,
  ESCAPE,
  ENTER,
  KEY_K,
  KEY_L,
  BACK_SPACE,
  HOME
};

int editorReadKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    if(c == '\x0a'){
        return ENTER;
    }
    if(c == '\x3a'){
        return COLON;
    }
    if(c == '\x7f'){
        return BACK_SPACE;
    }
    if(c=='k' && E.overflow==true) // only detect k in overflow condition
        return KEY_K;
    if(c=='l' && E.overflow==true) // only detect l in overflow condition
        return KEY_L;
    if(c=='h')
        return HOME;
    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESCAPE;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESCAPE;
        if (seq[0] == '[') {
        switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
            case 'C': return ARROW_RIGHT;
            case 'D': return ARROW_LEFT;
            }
        }
        //return '\x1b';
        return ESCAPE;
    } else {
        return c;
    }
}

/*** output ***/

void readDir(std::string path){
    E.currDirFiles.clear();
    struct dirent *de; //pointer for directory entry

    DIR *dr = opendir(path.c_str());

    if(dr == NULL){
        die("Could not open current directory");
        return;
    }

    while((de = readdir(dr))!=NULL){
        E.currDirFiles.push_back(de->d_name);
    }
    sort(E.currDirFiles.begin(),E.currDirFiles.end());
    if(E.currDirFiles.size() > E.screenrows-1) // at once i am printing (E.screenrows-1)
        E.overflow = true;
    else
        E.overflow = false;
    closedir(dr);
}

void printDir(){ // just prints what ever is present in E.currDirFiles
    E.lineNo = 0;
    E.outputBuffer.clear();
    E.outputBuffer.append("\x1b[2J",4);//clear screen
    E.outputBuffer.append("\x1b[H"); //sets cursor position 
    E.endPosition = E.currDirFiles.begin();
    E.startPosition = E.currDirFiles.begin();
    for(;E.endPosition!=E.currDirFiles.end() && E.lineNo<E.screenrows-1;E.endPosition++){
        E.lineNo++;
        printEntry(*E.endPosition,E.lineNo);
    }
    E.cy=0; // set cursor at the start
}

void displayPrevSection(){
    int lineNo = 0;
    if(E.startPosition == E.currDirFiles.begin())
        return;
    E.lineNo -= (E.endPosition - E.startPosition);
    E.startPosition -= E.screenrows-1;
    E.lineNo -= (E.screenrows-1);
    E.endPosition = E.startPosition;
    for(;E.endPosition!=E.currDirFiles.end() && lineNo<E.screenrows-1;E.endPosition++){
        lineNo++;
        E.lineNo++;
        printEntry(*E.endPosition,E.lineNo);
    }
    E.cy=0; // set cursor at the start
};

void displayNextSection(){
    if(E.endPosition == E.currDirFiles.end())
        return;
    E.startPosition = E.endPosition;
    E.outputBuffer.clear();
    E.outputBuffer.append("\x1b[2J",4);//clear screen
    E.outputBuffer.append("\x1b[H"); //sets cursor position 
    int lineNo=0;    
    for(;E.endPosition!=E.currDirFiles.end() && lineNo<E.screenrows-1;E.endPosition++){
        lineNo++;
        E.lineNo++;
        printEntry(*E.endPosition,E.lineNo); // appends to buffer
    }
    E.cy=0;  
};

void editorRefreshScreen() {
    std::string cursorPositionCmd = "\x1b["+ std::to_string(E.cy+1) + ";" + std::to_string(1)+"H";
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize"); // constantly updating win size
    write(STDOUT_FILENO,E.outputBuffer.c_str(),E.outputBuffer.length());
    write(STDOUT_FILENO,cursorPositionCmd.c_str(),cursorPositionCmd.length());
}

/*** directory paths ***/

std::string getFileName(){ // should return path of cursor location
    if(!E.overflow){
        std::string result=E.root;
        if(E.currDirFiles[E.cy] == "..")
            return getParent(result);
        else if(E.currDirFiles[E.cy] == "."){
            return result;
        }
        else {
            return result.append("/").append(E.currDirFiles[E.cy]);
        }
    }
    else {
        auto currPointer = E.startPosition + E.cy;
        std::string result=E.root;
        if(*currPointer == "..")
            return getParent(result);
        else if(*currPointer == "."){
            return result;
        }
        else {
            return result.append("/").append(*currPointer);
        }

    }
}

/*** command mode ***/
void enterCommandMode(){
    E.normalMode = false;
    std::string commandModeOutput = E.outputBuffer;
    std::string cursorPositionCmd = "\x1b["+ std::to_string(E.screenrows-1) + ";" + std::to_string(1)+"H";
    commandModeOutput.append(cursorPositionCmd);//goto last line 
    commandModeOutput.append("\x1b[0K");// clear line from cursor right
     
    commandModeOutput.append("command mode :");
    write(STDOUT_FILENO,commandModeOutput.c_str(),commandModeOutput.length());
    disableRawMode();
    while(E.normalMode == false){
        std::string cmd;
        std::cin>>cmd;
        if(cmd == "copy")
            copyCommand();
        else if(cmd == "move")
            moveCommand();
        else if(cmd == "rename")
            renameCommand();
        else if(cmd=="create_file")
            createFileCommand();
        else if(cmd=="create_dir")
            createDirCommand();
        else if(cmd=="delete_file")
            deleteFileCommand();
        else if(cmd=="delete_dir")
            deleteDirCommand();
        else
            std::cout<<"Enter a valid command "<<std::endl;
    }

}
/*** input ***/

void editorMoveCursor(int key) {
    switch (key) {
        case ARROW_UP:
            if (E.cy != 0) {
                E.cy--;
            }
            break;
        case ARROW_DOWN:
            if(!E.overflow){
                if (E.cy != E.screenrows - 2 && E.cy < E.currDirFiles.size()-1) {
                    E.cy++;
                }
            }
            else{
                auto currPointer = E.startPosition + E.cy;
                if (E.cy != E.screenrows - 2 && E.cy < E.currDirFiles.size()-1 && currPointer != E.currDirFiles.end()-1) {
                    E.cy++;
                }
            }
            break;
  }
}

void editorProcessKeypress() {
  int c = editorReadKey();
  std::string selectedPath;
  switch (c) {
    case 'q':
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        exit(0);
        break;
    
    case ARROW_UP:
    case ARROW_DOWN:
        editorMoveCursor(c);
        break;
    case ARROW_RIGHT:
        if(navVecIndex+1 >= navVec.size()){
            //do nothing
        }
        else{
            navVecIndex++;
            std::string currPath = navVec[navVecIndex];
            E.root = currPath;
            readDir(currPath);
            printDir();
        }
        break;
    case ARROW_LEFT:
        if(navVecIndex <= 0){
            //do nothing
        }
        else{
            navVecIndex--;
            std::string currPath = navVec[navVecIndex];
            E.root = currPath;
            readDir(currPath);
            printDir();
        }
        break;
    case ESCAPE:
    case COLON:
        enterCommandMode();
    case KEY_K:
        displayPrevSection();
        //enterCommandMode();
        break;
    case KEY_L:
        displayNextSection();
        //enterCommandMode();
        break;
    case HOME:
        E.root = E.home;
        readDir(E.home);
        printDir();
        break;
    case BACK_SPACE:
        
        selectedPath = getParent(E.root);
        if(isDirectory(selectedPath)){
            std::vector<std::string> newVec(0);
            for(int i=0;i<=navVecIndex;i++){
                newVec.push_back(navVec[i]);
            }
            navVec = newVec;
            navVec.push_back(selectedPath);
            navVecIndex++;
            E.root = selectedPath;
            readDir(selectedPath);
            printDir();
        }
        break; 
    case ENTER:
        selectedPath =getFileName();
        if(isDirectory(selectedPath)){
            navVec.push_back(selectedPath);
            navVecIndex++;
            E.root = selectedPath;
            readDir(selectedPath);
            printDir();
        } 
        else if(isFile(selectedPath)){
           pid_t id = fork();
           if(id == 0){ // child process
               execlp("xdg-open","xdg-open",selectedPath.c_str(),NULL);
               exit(0);
           }
        }
        else{
            // dunno file type 
        }
        break;
  }
}

/*** init ***/

void initEditor() {
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    E.home= std::string(cwd);
    E.root= std::string(cwd);
    E.cy = 0;
    E.outputBuffer="";
    E.currDirFiles = std::vector<std::string>(0);
    navVec.push_back(E.root);
    navVecIndex++;
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowSize");
}

int main(int argc, char* argv[]){
    initEditor(); //initialize all feilds in E
    if(argc==1){
        readDir(E.root);
        printDir();
    }
    else if(argc == 2){
        readDir(argv[1]);
        printDir();
    }
    else{
        std::cout<<"incorrect number of arguments"<<std::endl;
    }

    enableRawMode();

    while (1){
        if(E.normalMode){
            editorRefreshScreen();
            editorProcessKeypress(); // need to stop this in command mode
        }
    }
    return 0;
}