#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "disk.h"
#include "functions.h"
#include <sstream>
#include <iostream>

using namespace std;
extern int active;
int fd;
int initSuperBlock();
int initFATArray();
int initDirArray();
int initOFTArray();
int initFiles();
int mount_DirArray();
int mount_files();
FATEntry FATArray[32];
Directory DirArray[8];
OFT OFTArray[4];
singlefile files[8];


int make_fs(char *disk_name){
	make_disk(disk_name); 
	fd = open_disk(disk_name);
	printf("%s is opened.\n", disk_name);
	initFATArray();
	initDirArray();
	initSuperBlock();
	initFiles();
	return 0;
}

//initialize
int initSuperBlock(){
	
	superBlock spBlock;
	superBlock* spBptr = &spBlock;
	spBlock.dirBegin = 1;
	spBlock.dirEnd = 8;
	spBlock.FATBegin = 9;
	spBlock.FATEnd = 24;
	spBlock.dataBlockBegin = 32;
	spBlock.dataBlockEnd = 63;
	char spBlockData[12];
	memcpy(spBlockData,spBptr,12);  //the size of struct is n*maxsize
	block_write(0, spBlockData);
	char Data[12];
	block_read(0, Data);
	superBlock spBlockR;
	superBlock* DataStruct = &spBlockR;   //can't been written as superBlock* DataStruct;
	memcpy(DataStruct,Data,12);
	//printf("Data read from disk: dirBegin:%d\n",DataStruct->dirBegin);
	return 0;
}

int initFATArray(){
	int i;
	for(i=0;i<32;i++){
		FATArray[i].status = 0;
		FATArray[i].nextBlock = 0;
	}
	return 0;
}

int initDirArray(){
	int i;
	for(i=0;i<8;i++){
		DirArray[i].status = 0;
		DirArray[i].fileLength = 0;
	}
	//write into disk
	Directory* DirPtr = DirArray;
	char DirData[128];
	memcpy(DirData,DirPtr,128);  //the size of struct is n*maxsize
	
	if (lseek(fd, 1*BLOCK_SIZE, SEEK_SET) < 0) {
    	perror("write: failed to lseek");
    	return -1;
  	}

  	if (write(fd, DirData, 8*BLOCK_SIZE) < 0) {
		perror("write: failed to write");
    	return -1;
  	}
	/*for(i=0;i<8;i++){
		block_write(1+i, DirData+i*16);
	}*/
	return 0;
}

int initFiles(){
	int i=0;
	for(;i<8;i++){
		files[i].status = 0;
		files[i].fileName = NULL;
		files[i].fileText = NULL;
	}
	return 0;
}

int mount_fs(char *disk_name){
	printf("#### mount ####\n");
	if(fd==-1)
		fd = open_disk(disk_name);
	mount_DirArray();
	mount_files();
}

int mount_DirArray(){
	
	
	char MountResult[128];
	if (lseek(fd, 1*BLOCK_SIZE, SEEK_SET) < 0) {
    	perror("write: failed to lseek");
    	return -1;
  	}

  	if (read(fd, MountResult, 8*BLOCK_SIZE) < 0) {
		perror("write: failed to write");
    	return -1;
  	}
	memcpy(DirArray, MountResult,128);
	/*char Data[16];
	int i;
	for(i=0;i<8;i++){
		block_read(i+1,Data);
		memcpy(&DirArray[i],Data,16);
		//printf("Data read from disk: status:%d\n",DirArray[i].status);
	}*/
	int d;
  	/*for(d=0;d<8;d++){
   		if(DirArray[d].fileName!=NULL)
			printf("DirArray[%d].fileName%s\n",d,DirArray[d].fileName);
   }*/
	return 0;
}

int mount_files(){
	char MountResult[192];
	if (lseek(fd, 32*BLOCK_SIZE, SEEK_SET) < 0) {
    	perror("write: failed to lseek");
    	return -1;
  	}

  	if (read(fd, MountResult, 12*BLOCK_SIZE) < 0) {
		perror("write: failed to write");
    	return -1;
  	}
	memcpy(files, MountResult,192);
	//printf("#### after mount file ####\n");
	/*int ii;
	for(ii=0;ii<8;ii++){
	if(files[ii].fileText!=NULL)
		printf("files[%d]: %s\n",ii,files[ii].fileText); 
	}
*/
	//printf("&&&&&&&&&&&&  active is:%d &&&&&&&&&&\n",active);
	return 0;
}

int dismount_fs(char* disk_name){
	printf("######dismount######%\n");
	//printf("size of files is : %d\n",sizeof(files));
	
	//write Dir into disk
	Directory* DirPtr = DirArray;
	char DirData[128];
	int i;
	memcpy(DirData,DirArray,128);
	if (lseek(fd, 1*BLOCK_SIZE, SEEK_SET) < 0) {
    	perror("write: failed to lseek");
    	return -1;
  	}

  	if (write(fd, DirData, 8*BLOCK_SIZE) < 0) {
		perror("write: failed to write");
    	return -1;
  	}
	/*for(i=0;i<8;i++){
		memcpy(DirData,DirPtr,128);  //the size of struct is n*maxsize
		block_write(1+i, DirData+i*16);
	}*/

	//write files into disk
	char FilesData[192];
	char MountResult[192];
	
	memcpy(FilesData,files,192); 
	if (lseek(fd, 32*BLOCK_SIZE, SEEK_SET) < 0) {
    	perror("write: failed to lseek");
    	return -1;
  	}

  	if (write(fd, FilesData, 12*BLOCK_SIZE) < 0) {
		perror("write: failed to write");
    	return -1;
  	}
	/*if (lseek(fd, 32*BLOCK_SIZE, SEEK_SET) < 0) {
    	perror("write: failed to lseek");
    	return -1;
  	}

  	if (read(fd, MountResult, 12*BLOCK_SIZE) < 0) {
		perror("write: failed to write");
    	return -1;
  	}
	memcpy(files, MountResult,192);
	printf("#### after dismount file ####\n");
	int ii;
	for(ii=0;ii<8;ii++){
	if(files[ii].fileText!=NULL)
		printf("files[%d]: %s\n",ii,files[ii].fileText); 
	}*/
	//clear OFT (open file table)
	for(i=0;i<4;i++){
		if(OFTArray[i].status!=0){
			OFTArray[i].status = 0;
			OFTArray[i].filePtr = 0;
			OFTArray[i].name = NULL;
		}
	}
	close_disk();
	fd = -1;
	return 0;
}


int fs_create(char *name){
	
	//checking name 
	if(strlen(name)>4){
		printf("the file name should be within 4 letters!\n");
		return -1;
	}
	//update Directory 
	int i;
	for(i=0;i<8;i++){
		if (DirArray[i].status==0){
			DirArray[i].status =1;
			DirArray[i].fileName = (char*)malloc(4);
			strcpy(DirArray[i].fileName,name);
			
			//update temp file text in files[]
			files[i].fileName = (char*)malloc(4);
			files[i].fileText = (char*)malloc(1);
			DirArray[i].fileLength = 0;
			strcpy(files[i].fileName,name);
			
		return 0;
				
		}
	}
	//if directory is full
	printf("File system can only support 8 files!\n");	
	return -1;
}

int fs_get_filesize(int fildes){
	//get fileLength from DirArray
	int i;
	for(i=0;i<8;i++){
		if(DirArray[i].fileName!=NULL){
			//printf("DirArray[%d].fileName: %s\n",i,DirArray[i].fileName);
			if(strcmp(OFTArray[fildes].name, DirArray[i].fileName)==0){
				//printf("fileLength in fs_get:%d\n", DirArray[i].fileLength);
				return DirArray[i].fileLength;
			}
		}
	}
	printf("Invalid fildes, can't find size!\n");
	return -1;
}

int fs_lseek(int fildes, off_t offset){
	int fileLength;
	fileLength = fs_get_filesize(fildes);
	//check bound
	if(OFTArray[fildes].filePtr + offset < 0 || 
			OFTArray[fildes].filePtr + offset > fileLength){   //Dir length
		printf("invalid offset!\n");
		return -1;
	}
	OFTArray[fildes].filePtr += offset;
	//printf("file pointer now is:%d.\n",OFTArray[fildes].filePtr);
	return 0;
}

int fs_open(char* name){
	int i;
	
	//check if the file has been opened
	for(i=0;i<4;i++){
		
		if(OFTArray[i].name != NULL){
			if(strcmp(name, OFTArray[i].name)==0){
				printf("You can't open one file twice!\n");
				return -1;
			}
		}
	}
	//check if file system has this file
	int j;
	for(j=0;j<8;j++){
		
		if(files[j].fileName!=NULL){
			if(strcmp(name, files[j].fileName)==0){   //this file exists
				
				//find space in OFT and add this file in OFT
				for(i=0;i<4;i++){
						
					if(OFTArray[i].status == 0){
						
						OFTArray[i].status = 1;
						OFTArray[i].filePtr = 0;
						OFTArray[i].name = (char*)malloc(4);
						strcpy(OFTArray[i].name, name);
						//printf("file %s is in OFT %d\n",OFTArray[i].name,i);
						return i;
						
					}
				}
				printf("There are already 4 files, you can't open more.");
				return -1;
			}
		}
	}
	printf("There is no such file!\n");
	return -1;
}
int fs_read(int fildes, void* buf, size_t nbyte){
	int i, fileLength;
	//printf("OFT %d name is %s\n",fildes, OFTArray[fildes].name);
	fileLength = fs_get_filesize(fildes);
	//printf("file[%d] fileLength:%d\n",fildes,fileLength);
	memset(buf,0,80);
	//get fileLength from DirArray
	//printf("fildes:%d\n",fildes);
	for(i=0;i<8;i++){
		//printf(" create in %d loop is segment fault here?\n",i);
		if(files[i].fileName!= NULL){
			
			if(strcmp(OFTArray[fildes].name, files[i].fileName)==0){
				
				if(OFTArray[fildes].filePtr + nbyte > fileLength)
					nbyte = fileLength - OFTArray[fildes].filePtr;
				char readText[80];
				int k=0,j=OFTArray[fildes].filePtr;
				for(;k<nbyte;k++,j++){
					readText[k] = (char)(files[i].fileText)[j];
				}
				
				
				strcpy((char*)buf,readText);
				return nbyte;
			}
		}
	}
	printf("invalid offset!\n");
	return -1;	
}

int fs_write(int fildes, void *buf, size_t nbyte){
	//check filders
	if(fildes>3 || fildes<0 || OFTArray[fildes].name ==NULL){
		printf("Invalid file descriptor!\n");
		return -1;
	}
	int i;
	//check total size of data
	int length;
	length = fs_get_filesize(fildes);
	length += nbyte;
	if(length > 512){
		printf("the size of files will beyond the disk size!\n");
		return -1;
	}
	//find the file and write new text into it
	for(i=0;i<8;i++){
		if(files[i].fileName!=NULL){
			if(strcmp(OFTArray[fildes].name, files[i].fileName)==0){
				char* originalText; 
				originalText= (char*)malloc(length-nbyte+1);
				strcpy(originalText, files[i].fileText);
				files[i].fileText = (char*)malloc(length+1);
				
				strcpy(files[i].fileText, originalText);
				strcat(files[i].fileText,(char*)buf);
				DirArray[i].fileLength +=nbyte;
				//printf("after write: DirArray[%d].fileLength:%d \n", i, DirArray[i].fileLength);
				length = fs_get_filesize(fildes);
				OFTArray[i].filePtr = DirArray[i].fileLength;
				//printf("file's content is: %s\n", files[i].fileText);
				
				return 0;
			}	
		}
	}
}

int fs_truncate(int fildes, off_t length){
	int i;
	for(i=0;i<8;i++){
		if(files[i].fileName!=NULL){
			if(strcmp(OFTArray[fildes].name, files[i].fileName)==0){
				char* temp;
				int fileLength;
				fileLength = fs_get_filesize(fildes);
				if(fileLength<length) return -1;
				temp = (char*)malloc(length);
				strncpy(temp,files[i].fileText,length);
				files[i].fileText = (char*)malloc(length);
				strcpy(files[i].fileText, temp);
				OFTArray[fildes].filePtr = 0;
				return 0;
			}
		}
	}
	printf("No such file!\n");
	return -1;
}

int fs_close(int fildes){
	if(fildes>3 || fildes<0 || OFTArray[fildes].status == 0){
		printf("invalid fildes");
		return -1;
	}
	OFTArray[fildes].status = 0;
	OFTArray[fildes].filePtr = 0;
	OFTArray[fildes].name = NULL;
	return 0;
}

int fs_delete(char* name){
	int i;
	//printf("what I need to close is :%s\n", name);
	for(i=0;i<4;i++){
		if(OFTArray[i].name != NULL){
			if(strcmp(name, OFTArray[i].name)==0){
			printf("This file is still opened.\n");
			return -1;
			}
		}
	}
	for(i=0;i<8;i++){
		if(DirArray[i].fileName != NULL){
			//printf("the %dst file is:%s\n", i, DirArray[i].fileName);
			if(strcmp(name, DirArray[i].fileName)==0){
				files[i].fileText = NULL;
				files[i].status = 0;
				files[i].fileName = NULL;
				DirArray[i].status = 0;
				DirArray[i].fileName = NULL;
				DirArray[i].fileLength = 0;
				return 0;
			}
		}
		else
		printf("the %dst file is NULL\n",i);
	}
	printf("No such file!");
	return -1;
}
int initOFTArray(){
	int i;
	for(i=0;i<4;i++){
		OFTArray[i].status = 0;
		//strcpy(OFTArray[i].name, " ");
	}
	return 0;
}


