#ifndef _FUNCTIONS_H
#define _FUNCTIONS_H

//superBlock
typedef struct{
	unsigned short int dirBegin;
	unsigned short int dirEnd;
	unsigned short int FATBegin;
	unsigned short int FATEnd;
	unsigned short int dataBlockBegin;
	unsigned short int dataBlockEnd;
}superBlock;

//Open file table
typedef struct{
	unsigned short int status;
	int filePtr;
	char* name;
}OFT;

typedef struct{
	int status;
	char* fileName;
	char* fileText;
}singlefile;

//Directory
typedef struct{
	unsigned short int status;
	int FATEntryNum;    //first block of file
	char* fileName;
	unsigned short int fileLength;
}Directory;

//FATEntry
typedef struct{
	unsigned short int status;
	unsigned short int nextBlock;
}FATEntry;

//array
extern FATEntry FATArray[];
extern Directory DirArray[];
extern OFT OFTArray[];
extern singlefile files[];


//functions
int make_fs(char *disk_name);
int mount_fs(char *disk_name);
#endif
