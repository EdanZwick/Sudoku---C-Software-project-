/*
 * ILP.c
 *
 *  Created on: Feb 17, 2019
 *      Author: Edanz
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "map.h"
#include "gurobi_c.h"
#include "solver.h"

int ILP(int* b, map *m, double* sol, int dim, int blockw, int blockh);
int hint(int* b, int index, int boardw, int boardh);
void fill(int* b, map* m, double* sol);
void fillCell(int* b, map* m, double* sol, int index);
int addCellConst(GRBmodel* model ,map* m, int dim, double* ones);
int addConst(int* b, GRBmodel* model , map* m, int dim, double* ones, int blockw, int blockh, int type);
int getIndex(int i, int k, int dim, int blockw, int blockh, int type);
void allOnes(double* ones, int total);
void fillBinary(char* type,int total);

/*
 * Returns whether the board is solvable or not (1/0), and applies a solution on input array b (if apply==1).
 * ***apply==0 does not promise board will be unchanged, might edit board even if apply flag is turned off**
 *
 * Receives an array representing the board, block dimensions, flag whether to apply a solution if found, and a index cell that
 * denotes a single cell we wish to find a filling for.
 *
 * Assumes that board is in a valid state as supplied, block sizes are valid and cell is a legal index in board.
 *
 * The solution is done by the ILP function (which works with GUROBI).
 * This functions main role is to prime and initialize structures needed for the ILP.
 * To try and save work for the ILP, first assigns all cell which have only a single solution possible.
 *
 * If apply = 1
 * 		Fills the supplied array with the solution or returns 0 if impossible in current state.
 * If apply is negative, doesn't fill the whole array, but just the first cell with a hint for the cell in -(apply+1) index.
 * (this is used by the hint function and saves us the work of filling the entire array just to get 1 cell)
 *
 */
int solveB(int* b, int blockw, int blockh, int apply, int cell){
	map* m;
	int dim = blockw*blockh, total;
	double *sol;
	if (!fullAuto(b, blockw, blockh)){ /*makes all obvious placements, returns 0 if that leads to an erroneous state*/
		return 0;
	}
	m = createMap(b,blockw,blockh);
	if (m == NULL){
		return 0;
	}
	total = m->total;
	assert ((sol =(double*) calloc(total,sizeof(double)))!=NULL && "memory allocation error");
	if (ILP(b,m,sol,dim,blockw,blockh)!=0){
		free(sol);
		destroyMap(m);
		return 0;
	}
	if (apply){
		fill(b,m,sol);
	}
	if (!apply && cell>=0 && cell<dim*dim){
		fillCell(b,m,sol,cell);
	}
	destroyMap(m);
	free(sol);
	return 1;
}


/*
 * Creates the ILP model, runs it and returns the solution in sol array.
 *
 * Receives an array representing the board, an initialized map, array for solution and block dimentions.
 *
 * Uses Auxiliary function to create the constraints in the model.
 *
 */
int ILP(int* b, map *m, double* sol, int dim, int blockw, int blockh){
	int i;
	double *ones;
	GRBenv *env = NULL;
	GRBmodel *model = NULL;
	int error, optimstatus, total = GetNumVar(m);
	char* type;
	/* Create environment */
	error = GRBloadenv(&env, NULL);
	if (error || env == NULL){
		return error;
	}
	/* Create a model with 0 objective function, and |total| #variables*/
	assert((type = malloc(total*sizeof(char)))!=NULL && "Memory allocation error");
	fillBinary(type,total);
	if ((error = GRBsetintparam(env, GRB_INT_PAR_LOGTOCONSOLE, 0)) || (error = GRBnewmodel(env, &model, "suduko", total, NULL, NULL, NULL, type, NULL))){
		free(type);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return error;
	}
	free(type);
	assert((ones = malloc(total*sizeof(double)))!=NULL && "Memory allocation error");
	/*add constraints*/
	allOnes(ones, total);
	if (addCellConst(model,m,dim,ones)){ /*adds all constrains stating that a cell can receive only a single assignment*/
		free(ones);
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}
	for (i=0; i<3 ; i++){ /*each iteration adds the rows, cols and block constraints*/
		if (addConst(b,model, m, dim, ones, blockw, blockh, i)){ /*this function returns a non zero value if fails*/
			free(ones);
			GRBfreemodel(model);
			GRBfreeenv(env);
			return -1;
		}
	}
	free(ones);
	/*solve*/
	error = GRBoptimize(model);
	if (error){
		GRBfreemodel(model);
		GRBfreeenv(env);
		return error;
	}

	/*get solution status*/
	error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &optimstatus);
	if (error){
		GRBfreemodel(model);
		GRBfreeenv(env);
		return error;
	}

	if (optimstatus!=2){
		GRBfreemodel(model);
		GRBfreeenv(env);
		return -1;
	}

	/*get actual solution*/
	error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, total, sol);
	if (error){
		GRBfreemodel(model);
		GRBfreeenv(env);
		return error;
	}

	/* Free model */
	GRBfreemodel(model);

	/* Free environment */
	GRBfreeenv(env);
	return 0;
}

/*
 * Adds all constraints to the model that state we can only assign one value per cell.
 */
int addCellConst(GRBmodel* model ,map* m, int dim, double* ones){
	int i, j, *cind, NumCells = dim*dim, num, error, startInd;
	assert((cind = (int*) calloc(dim,sizeof(int)))!=NULL);
	for (i = 0; i < NumCells ; i++){
		num = GetNumCell(m, i); /*number of variables in play from this cell*/
		if (num==0){
			continue;
		}
		startInd = GetFirstVar(m,i);
		for(j = 0; j < num; j++) {
			cind[j] = startInd + j; /*build array of all indexes for this cell, the coefficients array is always just filled with 1's*/
		}
		error = GRBaddconstr(model, num, cind, ones, GRB_EQUAL, 1.0, NULL);
		if (error){
			free(cind);
			return error;
		}
	}
	free(cind);
	return 0;
}

/*
 * Adds Sudoku constraints to the model.
 * Since the process of creating constraints for rows, columns and blocks is almost identical (appart from index calculations),
 * this function is generic and takes a flag: type indicating if we are currently working on rows (0), columns (1) or blocks (2)
 *
 * General algorithm:
 * for each dimension (depending if we are working on rows columns or blocks):
 * 		for each possible value:
 * 				creates a constraint that this value can appear once in this dimension.
 * *
 * if during adding this constraint we discover there is no solution (i.e. there is no cell that can take a certain value):
 * 		The function returns -1
 * if we had a GUROBI error, the function will return that error code
 * otherwise: it will return 0;
 */
int addConst(int* b, GRBmodel* model , map* m, int dim, double* ones, int blockw, int blockh, int type){
	int i, j, k, num, error=0, index, *cind, tmp;
	assert ((cind = (int*) malloc(dim*sizeof(int)))!=NULL);
	for (i = 0 ; i<dim ; i++){/*for each dimension*/
		for (j = 1 ; j <= dim ; j++){ /*for each possible placment value*/
			num = 0;
			for (k=0 ; k < dim ; k++){/*for each cell in current dimension*/
				index = getIndex(i, k, dim, blockw, blockh, type); /*we calculate the cells indices according to what dimension we are iterating over*/
				if (b[index]==j){ /*no need to check this value*/
					num = -1; /*it's okay no cells have a variable for this value as it already appears*/
					break;
				}
				if ((tmp = getMapping(m, index, j))>=0){ /*cell[index] has variable for j value that needs to be considered in constraint*/
					cind[num] = tmp;
					num++;
				}
			}
			if (num>0){
				error = GRBaddconstr(model, num, cind, ones, GRB_EQUAL, 1.0, NULL);
			}
			if (error || (!num)){ /*either an error or no cell can take this value*/
				free(cind);
				return (error ? error : -1);
			}
		}
	}
	free(cind);
	return 0;
}

/*
 * Returns a legal assignment for cell index in board, or 0 if none exist.
 * Receives an array representation of the board, index of cell and block sizes.
 *
 * Assumes board is valid and index is legal.
 *
 */
int hint(int* b, int index, int blockw, int blockh){
	if (!solveB(b,blockw,blockh,1,index)){
		return 0;
	}
	return (b[index]);
}

/*
 * fills array b with solution found by the ILP model.
 * Assumes b is in valid size, m is initialized and sol is passed from ILP.
 */
void fill(int* b, map* m, double* sol){
	int i, index=0, total=0;
	total = GetNumVar(m);
	for (i = 0 ; i < total ; i++){
		if (sol[i]!=0){
			index = getCell(m,i);
			if (index<0){
				return;
			}
			b[index] = getVal(m,index,i); /* find what value variable i represents for cell*/
		}
	}
}

/*
 * Returns the solution for specific cell.
 * This lets us giving hints to a user without the work of applying the solution of an entire board.
 *
 * Recieves an array representation of the board, a map, the output of the optimizer, and the cell index.
 */
void fillCell(int* b, map* m, double* sol, int index){
	int var = GetFirstVar(m,index);
	int end = GetLastVar(m,index);
	while (var<=end){
		if (sol[var]!=0){
			b[index] = getVal(m,index,var);
		}
	}
}

/*
 * Fills type char array as GRB Binary.
 * Used to create the initial model.
 */
void fillBinary(char* type,int total){
	int i;
	for (i = 0 ; i<total ; i++){
		type[i] = GRB_BINARY;
	}
}

/*
 * Calculates actual index in board of k'th cell in the i'th dim of type "type".
 * For example- find the index of 8th cell (k=8) in the third block on board (i=3, type=2)
 */
int getIndex(int i, int k, int dim, int blockw, int blockh, int type){
	switch (type){
		case 0: return ((i*dim) + k); /*rows*/
		case 1: return ((k*dim) + i); /*cols*/
		default: return (translateBlockIndex(i, k, blockw, blockh));/*2 - block, from solver module*/
	}
}

/*
 * Receives a double array and size, fills it all with 1's.
 * Used for the Linear optimizer (as our constraints are always boring this way..)
 */
void allOnes(double* ones, int total){
	int i;
	for (i = 0; i < total; i++){
		ones[i] = 1.0;
	}
}
