/*
 * history.h
 *
 * Provides a data structure to document player's move history and an API to use and manipulate it.
 *
 * The history struct itself is consisted of three pointers: the first move made, the last move that was affected the boards current
 * state, and the last move made to the board (might have been undone already).
 *
 * Every move documents: the index of effected cell, the previous value in the cell, new value placed to the cell, pointers to next
 * and previous move, and an ID field that helps seperate between actual changes to the board.
 *
 * If two moves contain the same ID, it means they were both made during the same user command (such as generate or autofill).
 *
 *  Created on: Feb 16, 2019
 *      Author: Edanz
 */

#ifndef HISTORY_H_
#define HISTORY_H_

/*
 * Node of history move list.
 * Since some moves change more than one cell in board, undoing or redoing the requires more than one change to the board.
 * id is used to differentiate commands (since some commands make more than a single change to the board).
 */

typedef struct m{
	int old; /*previous value in cell*/
	int new; /*new value in cell*/
	int index; /*index of the cell that was changed*/
	struct m* prev;
	struct m* next;
	int id;
} move;

typedef struct h{
	move* cur; /*last move that had *already* effected the board*/
	move* head;
	move* tail;
} history;

/*
 * initializes the history struct h.
 */
void createHistory(history* h);

/*
 * Removes and frees all moves in history.
 * Does not free the history struct itself!
 */
void clearHistory(history* h);

/*
 * Clears all history after cur pointer.
 */
void clearTail(history* h);


/*
 *	Records a move in board history.
 *	Receives a pointer to the boards history, metadata of a move that was made.
 *	newCmd - flag stating weather this change to the board is a start of new command or part of a bigger one (such as autofill)
 */
void record(history* h, int new, int old, int index, int newCmd);

/*
 * Returns Id of last move that affected the board in it's current state.
 */
int curId(history* h);

/*
 * Returns the ID of the last undone move
 */
int nextId(history* h);

/*
 * Returns the ID of previous move
 */
int prevId(history* h);

/*
 * Updates history pointer to undo a move and returns needed changes to boards.
 * arr[0] - new value to set, or -1 if there are no relevant moves.
 * arr[1] - index of relevant cell
 */
void past(history* h, int arr[]);

/*
 * Updates history pointer to redo a move and returns needed changes to boards.
 * arr[0] - new value to set, or -1 if there are no relevant moves.
 * arr[1] - index of relevant cell
 */
void future(history* h, int arr[]);


#endif /* HISTORY_H_ */
