/*
 * main.c
 *
 * This module Contains just the one function, that handles input and calls mainAux's functions to react.
 * This module also holds the game stage (represented by an enum “mode”), and pointer to the game’s board (if exists).
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
#include "parser.h"
#include "game.h"
#include "dispatcher.h"

int main (void){
	mode m = init;
	board* b = NULL;
	int cmd[5], mark = 1, finish = 0;
	char name[1024] = {0}, str[COMMAND_LEN+2] = {0};
	puts("Hello! this is a new game of Sudoku, please enter your commands to play");
	while (!finish && fgets(str,COMMAND_LEN+2,stdin)!=NULL){
		if (strlen(str) > COMMAND_LEN){
			puts("Command too long\n");
			skipLine(str); /*so the next call to fgets will start reading from next line*/
			continue;
		}
		name[0] = '\0';
		parse(str,cmd,m,name);
		dispatch(&b,cmd,&m,name, &finish,&mark);
	}
	printf("exiting...\n");
	if (b!=NULL){
		destoryBoard(b);
		b = NULL;
	}
	return 0;
}
