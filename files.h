/*
 * files.h
 *
 *  Created on: Feb 21, 2019
 *      Author: Edanz
 */

#ifndef FILES_H_
#define FILES_H_

/*
 * Reads the first line of a file to extract the expected board size.
 * Assumes legal file by instructed format (meaning the first two numbers in the file denote block size).
 *
 * Returns 0 on failure and 1 on success.
 */
int getdim(char* name,int res[2]);

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
int load(char* name, int* arr, int dim);

/*
 * Saves a board into a file by specified format.
 * Expects to receive a 1d array from the toArray form of the game module.
 * Negative values are saved as fixed.
 *
 * returns 0 on error.
 *
 */
int save(char* name, int* arr, int blockw, int blockh, mode m);


#endif /* FILES_H_ */
