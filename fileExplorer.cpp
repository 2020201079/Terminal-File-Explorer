#include "header.h"

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
  ENTER
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
    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return ESCAPE;
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return ESCAPE;
        if (seq[0] == '[') {
        switch (seq[1]) {
            case 'A': return ARROW_UP;
            case 'B': return ARROW_DOWN;
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
    closedir(dr);
}

void printDir(){ // just prints what ever is present in E.currDirFiles
    E.outputBuffer.clear();
    E.outputBuffer.append("\x1b[2J",4);//clear screen
    E.outputBuffer.append("\x1b[H"); //sets cursor position 
    //need to print in chunks of row size and scroll with k or l
    for(auto e:E.currDirFiles){
        printEntry(e);
    }
    E.cy=0; // set cursor at the start
    
}

void editorRefreshScreen() {
    std::string cursorPositionCmd = "\x1b["+ std::to_string(E.cy+1) + ";" + std::to_string(1)+"H";
    write(STDOUT_FILENO,E.outputBuffer.c_str(),E.outputBuffer.length());
    write(STDOUT_FILENO,cursorPositionCmd.c_str(),cursorPositionCmd.length());
}

/*** directory paths ***/



std::string getFileName(){ // should return path of cursor location
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
            if (E.cy != E.screenrows - 1 && E.cy < E.currDirFiles.size()-1) {
                E.cy++;
            }
            break;
  }
}

void editorProcessKeypress() {
  int c = editorReadKey();
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
    case ESCAPE:
    case COLON:
        enterCommandMode();
    case ENTER: // going to parent directory when enter is pressed
        std::string selectedPath =getFileName();
        if(isDirectory(selectedPath)){
            E.root = selectedPath;
            readDir(selectedPath);
            printDir();
        } 
        /*else if(isTextFile(selectedPath)){
            //open via vi
        }*/
        else{
            // should cout at bottom line file not supported to open
        }
        break;
  }
}

/*** init ***/

void initEditor() {
    char cwd[1024];
    getcwd(cwd,sizeof(cwd));
    E.root= std::string(cwd);
    E.cy = 0;
    E.outputBuffer="";
    E.currDirFiles = std::vector<std::string>(0);
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