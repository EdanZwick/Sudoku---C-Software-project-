/*
 * parser.h
 *
 * See parser function.
 *  Created on: Feb 13, 2019
 *      Author: Edanz
 */

#ifndef PARSER_H_
#define PARSER_H_

#include "mode.h"

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
void parse(char* command, int res[], mode m, char* fileName);

/*
 * if a command line is longer than allowed command line chars it is considered invalid. this auxilary method makes sure we start reading
 * the next command from a new line.
 *
 * assumes str is not NULL
 */
void skipLine(char str[]);


#endif /* PARSER_H_ */
