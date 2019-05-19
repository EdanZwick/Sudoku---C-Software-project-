/*
 * map.h
 *
 *	This header holds the implementation for the data structure that is used to map between a cell to the variables that were
 *	sent to the optimizer.
 *
 *	The data structure itself wraps an array with the same dimensions as the board.
 *	Each cell of the array is a custom structure:
 *		int field, contains the index of the first variable belonging to this cell in the optimizer.
 *			- this helps us map what variable belongs to what cell more easly.
 *		pointer to a list:
 *			- each node representing a variable in the optimizer
 *	besides the array, the map holds additional helpful meta-data:
 *		- total: number of variables in play.
 *		- size: size of the board that is mapped.
 *
 *  Created on: Apr 18, 2019
 *      Author: Edanz
 */

#ifndef MAP_H_
#define MAP_H_

typedef struct n{
	int val; /*the value this variable represents*/
	struct n* next;
}mnode;

typedef struct c{
	int num; /*number of variables in list*/
	int index; /*index of first variable belonging to this cell*/
	mnode* list;
}cell;

typedef struct m{
	cell* cells; /*array holding lists for each cell*/
	int total; /*total number of variables in this model*/
	int size; /*size of the map (identical to the size of the board*/
}map;

/*
 * Initializes a mapping for board b, and Returns a pointer to the new data structure created.
 *
 * Receives a matrix representation of the board and dimensions.
 * Creates the data structure containing the data needed for each cell- what numbers are possible assignments
 * and index of first variable belonging each cell.
 *
 * If it's obvious during the mapping that no solution is possible, we will return NULL
 *
 */
map* createMap(int* b, int blockw, int blockh);

/*
 * frees all allocate data to this map and it's cells.
 */
void destroyMap(map* m);


/*
 * Returns how many variables this mapping holds.
 * Receives a pointer to the mapping.
 */
int GetNumVar(map* m);
/*
 * Receives a pointer to the mapping and an index of a cell on the board.
 * Returns how many variables this cell contributed to the model.
 */
int GetNumCell(map* m, int index);

/*
 * Returns the last index of a variable in this cell
 * Receives a pointer to the mapping and an index of a cell on the board.
 *
 */
int GetLastVar(map* m, int index);

/*
 * Returns the first index of a variable in this cell
 * Receives a pointer to the mapping and an index of a cell on the board.
 */
int GetFirstVar(map* m, int index);

/*
 *Returns the variable number in the model representing placement of that value in the cell.
 *Receives a pointer to a mapping, index of a cell in the board and a value.
 *If there is no such variable, Returns -1
 */
int getMapping(map* m, int index, int val);

/*
 *	Returns the cell index of the cell variable var belongs to.
 *	Receives a pointer to the map and the number of relevant variable.
 *	Uses a variation on Binary search to economize.
 */
int getCell(map* m, int var);

/*
 * Returns the value represented variable var.
 * If var doesn't belong to cell[index] returns -1
 */
int getVal(map* m, int index, int var);


#endif /* MAP_H_ */
