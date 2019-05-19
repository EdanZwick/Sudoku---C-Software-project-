/*
 * dispacher.c
 *
 *	Receives parsed command and dispatches the relevant MainAux functions.
 *	Separated from both main, and mainAux as to not clutter the modules and make them cumbersome
 *
 *  Created on: Apr 26, 2019
 *      Author: Edanz
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mainAux.h"
#include "mode.h"
#include "game.h"

void dispatch(board **b,int cmd[],mode *m,char *name,int *finish,int *mark){
	if (cmd[4]<0){ /*no legal command was detected, print relevant error and get next command*/
		handleParse(cmd,*m);
		return;
	}
	switch(cmd[0]){
		case 1:{ /*solve X*/
			handleSolve(b,name,m,*mark);
			break;
		}
		case 2:{ /*edit*/
			handleEdit(b,name,m);
			break;
		}
		case 3:{ /*mark errors*/
			handleMark(cmd,mark);
			break;
		}
		case 4:{ /*print board*/
			handlePrint(*b,*mark);
			break;
		}
		case 5:{ /*Set*/
			handleSet(*b,cmd,*mark,m);
			break;
		}
		case 6:{ /*validate*/
			handleVali(*b);
			break;
		}
		case 7:{ /*Generate*/
			handleGen(*b,cmd);
			break;
		}
		case 8:{ /*undo*/
			handleDo(*b,*mark,1,*m);
			break;
		}
		case 9:{ /*redo*/
			handleDo(*b,*mark,0,*m);/*same function as undo, different arguments*/
			break;
		}
		case 10:{ /*save*/
			handleSave(*b,*m,name);
			break;
		}
		case 11:{/*hint*/
			handleHint(*b,cmd);
			break;
		}
		case 12:{/*num solutions*/
			handleNum(*b);
			break;
		}
		case 13:{/*Autofill*/
			handleAuto(*b,*mark,m);
			break;
		}
		case 14:{ /*reset*/
			reset(*b);
			handlePrint(*b,*mark);
			break;
		}
		case 15:{ /*exit*/
			*finish=1;
		}
	}
}
