/*
 * mainAux.c
 *
 *  Created on: Feb 13, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mode.h"
#include "sizes.h"
#include "files.h"
#include "solver.h"
#include "game.h"
#include "ILP.h"
#include "generator.h"

void printBoard(int arr[], int blockw, int blockh, int mark);
void handlePrint(board *b,int mark);
int validCord(board *b, int *cmd);
void handleGameOver(board *b, mode *m);
int validate(board *b);

/*
 *
 * Prints errors if user command is invalid
 *		res[4] values denote:
 *		 -1 invalid command name
 *		 -2 command not available in mode
 *		 -3 Illegal number of arguments
*/
void handleParse(int* res, mode m){
	switch (res[4]){
		case -1:{
			printf("invalid command name\n");
			break;
		}
		case -2:{
			printf("command not available in mode ");
			switch(m){
				case(init):{
					puts("init - choose to edit or solve");
					break;
				}
				case(edit):{
					puts("edit");
					break;
				}
				case(solve):{
					puts("solve");
					break;
				}
			}
			break;
		}
		case -3:{
			printf("Illegal number of arguments\n");
			if (res[0]==4 || res[0]==6 || res[0]==8 || res[0]==9 || res[0]>=12){
				puts("this command takes no arguments");
				return;
			}
			if (res[0]==1 || res[0]==3 || res[0]==10){
				puts("this command takes exactly 1 argument");
				return;
			}
			if (res[0]==7 || res[0]==11){
				puts("this command takes exactly 2 arguments");
				return;
			}
			if (res[0]==5){
				puts("this command takes exactly 3 arguments");
				return;
			}
			if (res[0]==2){
				puts("this command takes no more than one argument (optional file name to load)");
				return;
			}
			break;
		}
	}
}

/*
 * Sets a new game in solve mode, by loading a board from file "name"
 */
void handleSolve(board **b, char name[257], mode *m, int mark){
	int *arr, size, blockdim[2];
	if (!getdim(name,blockdim)){
		puts("file not found\n");
		return;
	}
	size = blockdim[0] * blockdim[0] * blockdim[1] * blockdim[1];
	assert((arr = (int*) calloc ((size), sizeof(int)))!=NULL && "Memory allocation error");
	if (load(name,arr,size)==-1){
		free(arr);
		printf("file error\n");
		return;
	}
	if (*b!=NULL){
			destoryBoard(*b);
			*b = NULL;
	}
	*m = solve;
	*b =  createBoard(arr,blockdim[0],blockdim[1],*m);
	free(arr);
	printf("Welcome to solve mode. created board from file:\n");
	handlePrint(*b,mark);
}

/*
 * Sets a new game in solve mode, by loading a board from file "name" if it's non empty,
 * Or constructs a default empty board.
 */
void handleEdit(board **b, char name[257], mode *m){
	int* arr, size, blockdim[2];
	if (strlen(name)>0){
		if (!getdim(name,blockdim)){
			puts("file not found");
			return;
		}
		size = blockdim[0] * blockdim[0] * blockdim[1] * blockdim[1];
		assert((arr = (int*) calloc((size) , sizeof(int)))!=NULL && "Memory allocation error");
		if (load(name,arr,size)==-1){
			free(arr);
			puts("file error");
			return;
		}
		printf("created board from file %s\n",name);
	}
	else{
		blockdim[0] = DEF_BLOCK_W;
		blockdim[1] = DEF_BLOCK_H;
		size = blockdim[0]*blockdim[1]*blockdim[0]*blockdim[1];
		assert((arr = (int*) calloc (size,sizeof(int)))!=NULL && "Memory allocation error");
		printf("created new default sized board in block dimensions of %d X %d\n",DEF_BLOCK_W,DEF_BLOCK_H);
	}
	if (*b!=NULL){
			destoryBoard(*b);
			*b = NULL;
	}
	*m = edit;
	*b = createBoard(arr,blockdim[0],blockdim[1],*m);
	free(arr);
	printf("Welcome to edit mode. created board is:\n");
	handlePrint(*b,1);
}

/*
 * Changes the mark errors variable accourding to input.
 */
void handleMark(int cmd[3], int *mark){
	char *c;
	if (cmd[1]<0 || cmd[1]>1){
		puts("Invalid parameter for command, please try again with 0/1 (mark/don't mark)");
		return;
	}
	*mark = cmd[1];
	c = (cmd[1]==0)? "not " : "";
	printf("errors will %sbe marked\n",c);
}

/*
 * Prints game's board
 */
void handlePrint(board *b,int mark){
	int *arr, blockdim[2];
	getBlockDim(b,blockdim);
	assert((arr = (int*) calloc (getSize(b),sizeof(int)))!=NULL && "Memory allocation error");
	toArray(b,arr,0);
	printBoard(arr,blockdim[0],blockdim[1],mark);
	free(arr);
}

/*
 * Apples user set command or prints erorrs if not valid
 */
void handleSet(board *b, int *cmd, int mark, mode *m){
	if (!validCord(b,cmd)){return;}
	if (cmd[3]>getMaxVal(b) || cmd[3]<0){
		printf("invalid set value, needs to be an integer between 1 to %d\n",getMaxVal(b));
		return;
	}
	if (*m==solve && isFixed(b,cmd+1)){
		puts("selected cell is fixed!");
		return;
	}
	set(b,cmd);
	if (*m==edit){
		mark = 1;
	}
	handlePrint(b,mark);
	handleGameOver(b,m);
	return;
}

/*
 *Prints the needed output from the validate command.
 *Separated from the validate function bellow, as many actions include a validate step which doesn't require the same output
 */
void handleVali(board *b){
	int tmp = (validate(b));
	if (tmp==-1){
		printf("Board is currently not valid \n");
		return;
	}
	printf("board is ");
	if (tmp == 0){
		printf("un");
	}
	printf("solvable at current state\n");
}

/*
 * Returns 1 if board is solvable, 0 otherwise.
 * Separated from handleVali as this function is a perliminary step in many commands.
 */
int validate(board *b){
	int *arr, tmp,blockdim[2];
	if (!(allValid(b))){
		return -1;
	}
	if (isEmpty(b)){
		return 1;
	}
	if (isSolvable(b)!=0){ /*this board was already checked*/
		return(isSolvable(b)==1);
	}
	assert((arr = (int*) calloc (getSize(b),sizeof(int)))!=NULL && "Memory allocation error");
	toArray(b,arr,1);
	getBlockDim(b,blockdim);
	tmp = solveB(arr, blockdim[0], blockdim[1], 0,-1); /*1 if successful, 0 otherwise*/
	free(arr);
	setSolvable(b,(tmp? 1:-1));
	return tmp;
}

/*
 * Generates a board according to user input:
 *
 * Tries for MAX_GEN_ITERATIONS to fill x cells, and solve the board.
 * Applies new board if found or prints error otherwise.
 *
 */
void handleGen(board *b, int cmd[]){
	int empty = numFree(b), *arr, size, blockdim[2];
	if (cmd[1] < 0){
		puts("parameter 1 can't be negative");
	}
	if (cmd[2] < 0){
		puts("parameter 2 can't be negative");
	}
	if (empty < cmd[1]){
		printf("Board does not contain %d additional cells to fill\n",cmd[1]);
		return;
	}
	size = getSize(b);
	if (size < cmd[2]){
		printf("Board contains less than %d cells\n",cmd[2]);
		return;
	}
	if (validate(b)!=1){
		puts("board is currently not solvable");
		return;
	}
	assert((arr = (int*) calloc (size,sizeof(int)))!=NULL && "Memory allocation error");
	if (cmd[2]!=0){ /*no use to try and solve since y=0 means we'll be erasing all of it*/
		toArray(b,arr,1);
		getBlockDim(b,blockdim);
		if (!generate(arr, cmd[1] , cmd[2], blockdim[0], blockdim[1], empty)){ /*in ILP*/
			puts("We were unsuccessful in generating a board");
			puts("\"I have not failed. I've just found 1,000 ways that won't work.\"\nThomas A. Edison");
			free(arr);
			return;
		}
	}
	applyMatrix(b, arr); /*if we are here- we found a solution and we call this function in game to fill board with it*/
	setSolvable(b,1); /*we know this board is solvable*/
	handlePrint(b,1); /*genrate available only in edit mode, thus the mark errors flag sent to print is always 1*/
	free(arr);
}

/*
 * Undoes or redoes a move on board accourding to "un" flag (1 undo, 0 redo)
 */
void handleDo(board *b, int mark, int un, mode m){
	int tmp, output[4];
	char* s = un? "un" : "re";
	if (un == 1){
		tmp = undo(b,output);
	}
	else{
		tmp = redo(b,output);
	}
	if (tmp==0){
		printf("you have reached the end of the board's history, there are no more relevant moves to %sdo\n",s);
		return;
	}
	printf("%sdid ",s);
	if (output[0]==-1){
		s = (m==edit) ? "generate": "autofill";
		printf("%s move\n",s);
	}
	else{
		printf("move, replaced %d with %d in cell %d %d\n",output[2],output[3],output[0]+1,output[1]+1);
	}
	handlePrint(b, (m==edit) ? 1: mark);
}

/*
 * Saves board to file name according to format, or prints error if not permitable.
 */
void handleSave(board *b,mode m, char *name){
	int tmp, *arr, blockdim[2];
	if (m==edit){
		tmp = validate(b);
		if (tmp == -1){
			puts("Board is currently invalid and not allowed to be saved");
			return;
		}
		if (tmp == 0){
			puts("Board is unsolvable and not allowed to be saved");
			return;
		}
	}
	assert((arr = (int*) calloc (getSize(b),sizeof(int)))!=NULL && "Memory allocation error");
	toArray(b,arr,0);
	getBlockDim(b,blockdim);
	if (!save(name,arr, blockdim[0], blockdim[1], m)){
		puts("file error");
	}
	free(arr);
	printf("Saved puzzle to file:%s\n",name);
}

/*
 * Checks if coordinates are valid for current board.
 */
int validCord(board *b, int *cmd){
	int max = getMaxVal(b);
	if (cmd[1] > max || cmd[1] < 1){
		printf("X coordinate invalid. needs to be an integer between 1 to %d \n",max);
		return 0;
	}
	if (cmd[2] > max || cmd[2] < 1){
		printf("Y coordinate invalid. needs to be an integer between 1 to %d \n",max);
		return 0;
	}
	return 1;
}

/*
 * prints a placement to cell cmd[2],cmd[1] that will keep the board solvable,
 * or prints an error if no such placement exists
 */
void handleHint(board *b, int *cmd){
	int *arr, tmp, blockdim[2];
	if (!(validCord(b,cmd))){
		return;
	}
	if (!(allValid(b))){
		puts("board isn't valid. here's a hint: why don't you correct it first?!");
		return;
	}
	if (isFixed(b, cmd+1)){
		puts("Cell is fixed!");
		return;
	}
	if (getCurVal(b,cmd+1)){
		puts("Cell already contains a value!");
		return;
	}
	if (isSolvable(b)==-1){
		puts("Board is not solvable");
		return;
	}
	assert((arr = (int*) calloc (getSize(b),sizeof(int)))!=NULL && "Memory allocation error");
	toArray(b,arr,1);
	getBlockDim(b,blockdim);
	tmp = hint(arr, cordToInd(b,cmd+1), blockdim[0], blockdim[1]);
	free(arr);
	if (!(tmp)){
		puts("Board is not solvable");
		return;
	}
	printf("here's a hint! try setting cell %d %d to %d\n",cmd[1],cmd[2],tmp);
}

/*
 * Prints number of possible solutions to the board, or error if none exist
 */
void handleNum(board *b){
	int *arr,tmp,blockdim[2];
	char *c, *s;
	if (!(allValid(b))){
		printf("board is not valid, there are 0 possible solutions\n");
		return;
	}
	if (isSolvable(b)==-1){
		puts("there are 0 possible solutions to this board");
		return;
	}
	assert((arr = (int*) calloc (getSize(b),sizeof(int)))!=NULL && "Memory allocation error");
	toArray(b,arr,1);
	getBlockDim(b,blockdim);
	tmp = num_solutions(arr, blockdim[0], blockdim[1]);
	s = (tmp>1 || tmp==0) ? "are" : "is";
	c = (tmp>1 || tmp==0) ? "s " : " ";
	printf("there %s %d possible solution%sto this board\n",s,tmp,c);
	setSolvable(b,(tmp>0 ? 1 : -1));
	free(arr);
}

/*
 * Filles every empty cell with only 1 possible value, or prints error if not possible.
 */
void handleAuto(board *b, int mark,mode *m){
	int tmp, *arr1, *arr2, *options, size, blockdim[2];
	if (!(allValid(b))){
		printf("board is not valid please correct and try again");
		return;
	}
	size = getSize(b);
	assert((arr1 = (int*) calloc(size,sizeof(int)))!=NULL && "Memory allocation error");
	toArray(b,arr1,1);
	assert((arr2 = (int*) calloc(size,sizeof(int)))!=NULL && "Memory allocation error");
	assert((options = (int*) malloc(getMaxVal(b)*sizeof(int)))!=NULL && "Memory allocation error");
	getBlockDim(b,blockdim);
	if ((tmp = autofill(arr1,arr2,options,blockdim[0],blockdim[1]))==0){
		free(arr1);
		free(arr2);
		free(options);
		printf("There are no obvious assignments, no changes were made to the board\n");
		return;
	}
	free(arr2);
	free(options);
	printf("%d cells were filled:\n",tmp);
	applyMatrix(b, arr1);
	free(arr1);
	handlePrint(b,mark);
	handleGameOver(b,m);
}

void printdash(int size, int blockh){
	int i = 0, t = 4*size + blockh + 1;
	for ( ; i < t; i++){
		printf("-");
	}
	printf("\n");
}


void printBoard(int arr[], int blockw, int blockh, int mark){
	int i, j, len = blockw*blockh, index = 0, tmp;
	char c;
	for (i = 0 ; i < len ; i++){
		if (i%blockh == 0){
			printdash(len, blockh);
		}
		for (j = 0; j < len ; j++){
			index = (i*len) + j;
			if ((j%blockw)==0){
				printf("|");
			}
			if (arr[index] == 0){
				printf("    ");
				continue;
			}
			c = ' ';
			tmp = arr[index];
			if (tmp>len){
				tmp -= len + 1;
				if (mark == 1){
					c = '*';
				}
			}
			if (tmp<0){
				tmp *= -1;
				c = '.';
			}
			printf(" %2d%c",tmp,c);
		}
		printf("|\n");
	}
	printdash(len, blockh);
}

/*
 * Checks if game has been won, changes state and prints output accourdingly.
 */
void handleGameOver(board *b, mode *m){
	if ((*m == solve) && numFree(b)==0){
		if (allValid(b)){
			puts("Congrats! you have solved the board successfully! you can exit or start again!");
			*m = init;
		}
		else{
			puts("you call this a solution?! you should be ashamed of yourself. undo some moves...");
		}
	}
}

