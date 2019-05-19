/*
 * parser.c
 *
 * See parser function.
 *
 *  Created on: Feb 13, 2019
 *      Author: Edanz
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include "mode.h"
#include "sizes.h"

int getNum(char* str);
int getCmd(char* command);
int legalArgs(int res, int tmp);
int tokNum(char* command);

/*
 *	"Decodes" user input. Receives a string containing a line of user input and returns an int array:
 *		cell 0 defines the action
 *	 		codes for actions:
 *	 			1.	solve X
 *				2.	edit [X]
 *				3.	mark_errors X - only available in Solve mode.
 *				4.	print_board - only available in Edit and Solve modes
 *				5.	set X Y Z - only available in Edit and Solve modes
 *				6.	validate - only available in Edit and Solve modes
 *				7.	generate X Y -  only available in Edit mode
 *				8.	undo - Edit and Solve modes
 *				9.	redo -  Edit and Solve modes
 *				10.	save X -  Edit and Solve modes
 *				11.	hint X Y - only available in Solve mode
 *				12.	num_solutions - Edit and Solve modes
 *				13.	autofill - only available in Solve mode
 *				14.	reset - only available in Edit and Solve modes
 *				15.	exit
 *	 			 0 no operation
 *	 			 -1 invalid command name
 *	 			 -2 command not available in mode
 *	 			 -3 Illegal number of arguments
 *	 	in res array:
 *	 		cell 1 is the first argument (if needed)
 *			cell 2 is the second argument (if needed)
 *			cell 3 is the third argument (if needed)
 */
void parse(char* command, int res[], mode m, char* fileName){
	int tmp;
	char *token, cpy[COMMAND_LEN + 3] = {0};
	res[4] = 0;
	strncpy(cpy,command,COMMAND_LEN);
	tmp = tokNum(cpy); /*number of words in given command*/
	if (tmp==0){ /*blank line*/
		res[0] = 0;
		return;
	}
	strncpy(cpy,command,COMMAND_LEN);
	res[0] = getCmd(cpy); /*code of given command*/
	if (res[0]==0){
		res[4] = -1;
		return;
	}
	if ((m==init && (res[0]>2) && (res[0]<15))
			|| (m==edit && ((res[0]==3) || (res[0]==11) || (res[0]==13)))|| (m==solve && (res[0]==7))){
		res[4] = -2;
		return;
	}
	if (!(legalArgs(res[0],tmp))){
		res[4]=-3;
		return;
	}
	if (res[0]==1 || (res[0]==2 && tmp==2) || res[0]==10){ /*only other argument for these commands is a filename*/
		strncpy(cpy,command,COMMAND_LEN);
		token = strtok(cpy," \t\r\n");
		token = strtok(NULL," \t\r\n");
		strcpy(fileName, token);
		return;
	}
	strtok(command," \t\r\n"); /*if no more args these values will be ignored by main*/
	res[1] = getNum(strtok(NULL," \t\r\n"));
	res[2] = getNum(strtok(NULL," \t\r\n"));
	res[3] = getNum(strtok(NULL," \t\r\n"));
	return;
}

/*
 * Recieves the first token of input and matches it to a supported command- returning the matching code.
 * If the input does not match- returns 0;
 */
int getCmd(char* command){
	char *token;
	if ((token = strtok(command," \t\r\n")) == NULL){
			return 0;
	}
	if (strcmp(token,"solve")==0){
		return 1;
	}
	if (strcmp(token,"edit")==0){
		return 2;
	}
	if (strcmp(token,"mark_errors")==0){
		return 3;
	}
	if (strcmp(token,"print_board")==0){
		return 4;
	}
	if (strcmp(token,"set")==0){
		return 5;
	}
	if (strcmp(token,"validate")==0){
		return 6;
	}
	if (strcmp(token,"generate")==0){
		return 7;
	}
	if (strcmp(token,"undo")==0){
		return 8;
	}
	if (strcmp(token,"redo")==0){
		return 9;
	}
	if (strcmp(token,"save")==0){
		return 10;
	}
	if (strcmp(token,"hint")==0){
		return 11;
	}
	if (strcmp(token,"num_solutions")==0){
		return 12;
	}
	if (strcmp(token,"autofill")==0){
		return 13;
	}
	if (strcmp(token,"reset")==0){
		return 14;
	}
	if (strcmp(token,"exit")==0){
		return 15;
	}
	return 0;
}

/*
 * Recieves the code of requested command and number of arguments that were given with it. validates that they are a legal combination
 */

int legalArgs(int res, int tmp){
	switch (tmp){ /*make sure number of arguments is legal*/
			case 1:{
				if (!(res==1 || res==3 || res==5 || res==7 || res==10 || res==11)){
					return 1;
				}
			break;
			}
			case 2:{
				if ((res<4 || res==10)){
					return 1;
				}
			break;
			}
			case 3:{
				if ((res==7 || res==11)){
					return 1;
				}
			break;
			}
			case 4:{
				if (res==5){
					return 1;
				}
			break;
			}
		}/*end switch*/
	return 0;
}

/*
 * converts a string input to integers.
 * Returns -1 if not a number
 */
int getNum(char* str){
	int sum = 0, exp = 1, n, i;
	if ((str==NULL) || ((n = strlen(str))==0)){
		return -1;
	}
	for (i=n-1 ; i>=0 ; i--){
		if (str[i] < '0' || str[i] > '9'){
			return -1;
		}
		sum += ((str[i]-'0') * exp);
		exp*=10;
	}
	return sum;
}

/*
 * Counts number of text tokens in command, used in parse method to validate legal input size
 */
int tokNum(char* command){
	int i = 0;
	if (strtok(command," \t\r\n")!=NULL){
		i++;
	}
	while ((strtok(NULL," \t\r\n")) != NULL){
		i++;
	}
	return i;
}

/*
 * if a command line is longer than allowed command line chars it is considered invalid. this auxilary method makes sure we start reading
 * the next command from a new line.
 *
 * this method can't get a null string since it is only called when exactly 257 chars were read.
 */
void skipLine(char str[COMMAND_LEN +1]){
	if (str[COMMAND_LEN-1]=='\n'){
		return;
	}
	fscanf(stdin, "%*[^\n]\n"); /*format taken from stack overflow*/
}

