/*
 * files.c
 *
 *	This module is in charge of handling all file related tasks: saving and loading
 *
 *
 *  Created on: Feb 14, 2019
 *      Author: Edanz
 */


#include "mode.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


/*
 * Reads the first line of a file to extract the expected board size.
 * Assumes legal file by instructed format (meaning the first two numbers in the file denote block size).
 *
 * Returns 0 on failure and 1 on success.
 */
int getdim(char* name,int res[2]){
	FILE* fp;
	if ((fp = fopen(name,"r"))==NULL){
		return 0;
	}
	if (!fscanf(fp,"%d",res+1) || !fscanf(fp,"%d",res)){
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

/*
 * Receives the name of a file containing a board, a pointer to an array that will contain the board and the board's block dimensions.
 * Reads the file and fills the array accordingly.
 *
 * The array is filled with the convention that set cells receive negative value.
 * If a negative value appearers in the file, it is ignored.
 *
 * If  a file contains less numbers, this function will assume 0's in the rest of the board.
 * The function returns 1 on success, 0 if the file contained less numbers and -1 on error.
 *
 */
int load(char* name, int* arr, int size){
	FILE* fp;
	int index = 0,tmp;
	if ((fp = fopen(name,"r"))==NULL){
		return -1;
	}
	fscanf(fp,"%d",&tmp); /*update file pointer to skip two first integers- dimensions were already read*/
	fscanf(fp,"%d",&tmp);
	while ((index < size) && (fscanf(fp,"%d",&tmp)>0)){
		if (tmp<0){
			continue;
		}
		arr[index] = tmp;
		if (getc(fp)=='.'){
			arr[index] *= -1;
		}
		index++;
	}
	fclose(fp);
	return (!(index - size));
}

/*
 * Saves a board into a file by specified format.
 * Expects to receive a 1d array from the toArray form of the game module.
 * Negative values are saved as fixed.
 *
 * returns 0 on error.
 *
 */
int save(char* name, int* arr, int blockw, int blockh, mode m){
	FILE* fp;
	int len = blockh* blockw, i, j, index = 0, tmp;
	char* s = "",*space;
	if ((fp=fopen(name,"w"))==NULL){
		return 0;
	}
	if (fprintf(fp,"%d %d\n",blockh,blockw)<=0){/*first line- block dimensions*/
		fclose(fp);
		return 0;
	}
	for (i = 0 ; i<len ; i++){
		space = " ";
		for (j = 0; j<len ; j++){
			if (j==len-1){
				space = "";
			}
			tmp = arr[index];
			tmp = (tmp > len) ? (tmp - len -1) : tmp; /*invalid cells are not printed out as such*/
			s = ((tmp<0) || (m==edit && tmp!=0)) ? "." : "";
			tmp = abs(tmp);
			if (fprintf(fp,"%d%s%s",(tmp),s,space)<=0){
				fclose(fp);
				return 0;
			}
			index++;
		}
		if (fprintf(fp,"\n")<=0){
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return 1;
}
