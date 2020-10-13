fileExplorer: fileExplorer.o copyCommand.o createFileCommand.o deleteCommand.o createDirCommand.o utilities.o gotoCommand.o moveCommand.o renameCommand.o
	g++ -o fileExplorer fileExplorer.o copyCommand.o createFileCommand.o deleteCommand.o createDirCommand.o utilities.o gotoCommand.o moveCommand.o renameCommand.o

fileExplorer.o: fileExplorer.cpp header.h utilities.cpp
	g++ -c fileExplorer.cpp utilities.cpp

copyCommand.o: copyCommand.cpp header.h utilities.cpp
	g++ -c copyCommand.cpp utilities.cpp

createFileCommand.o: createFileCommand.cpp header.h utilities.cpp
	g++ -c createFileCommand.cpp utilities.cpp

createDirCommand.o: createDirCommand.cpp header.h utilities.cpp
	g++ -c createDirCommand.cpp utilities.cpp

deleteCommand.o: deleteCommand.cpp header.h utilities.cpp
	g++ -c deleteCommand.cpp utilities.cpp

gotoCommand.o: gotoCommand.cpp header.h utilities.cpp
	g++ -c gotoCommand.cpp utilities.cpp

moveCommand.o: moveCommand.cpp header.h utilities.cpp
	g++ -c moveCommand.cpp utilities.cpp

renameCommand.o: renameCommand.cpp header.h utilities.cpp
	g++ -c renameCommand.cpp utilities.cpp
clean:
	rm *.o fileExplorer