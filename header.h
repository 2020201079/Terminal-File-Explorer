/*** includes ***/
#include<unistd.h>
#include<ctype.h>
#include<stdio.h>
#include<termios.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/ioctl.h>
#include<sys/stat.h>
#include<string>
#include<string.h>
#include<dirent.h>
#include<iostream>
#include<vector>
#include<algorithm>
#include<pwd.h>
#include<grp.h>
#include<fcntl.h>

/*** global variables ***/
struct editorConfig {
    std::string root;
    int cy;
    int screenrows;
    int screencols;
    struct termios orig_termios;
    std::string outputBuffer;
    std::vector<std::string> currDirFiles;
    bool normalMode = true;
    bool overflow;
    std::string home;
    std::vector<std::string>::iterator endPosition;
    std::vector<std::string>::iterator startPosition;
    int lineNo;
};

extern struct editorConfig E;

/*** global methods ***/
void enterCommandMode();
void updateDisplayCommandMode();
void copyCommand();
void createFileCommand();
void createDirCommand();
void deleteFileCommand();
void deleteDirCommand();
void gotoCommand();
void moveCommand();
void renameCommand();
std::vector<std::string> getArgs(std::string input);
bool isDirectory(std::string path);
bool isFile(std::string path);
void die(const char *s);
void enableRawMode();
void disableRawMode();
int getWindowSize(int *rows, int *cols);
std::string getParent(std::string path); // returns absolute path
std::string getCompletePath(std::string fileName); // returns absolute path until file name
bool isPathRelative(std::string path);//check if it starts with '~' or '.'
std::string relativeToAbsolute(std::string path);
void printEntry(std::string e,int lineNo);