/*
 * rosa_utils.c
 *
 */ 

#include "kernel/rosa_utils.h"

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

/***********************************************************
 * isTaskinDELAYLIST
 *
 * Comment:
 * 	searches for a task in the delay list
 * 	
 * RETURNVALS:
 *	1 Yes
 *	0 No
 **********************************************************/

int ROSA_prv_isTaskinDELAYLIST(tcb * tcbTask)
{	
	tcb * tcbTmp;
		
	tcbTmp=DELAYLIST;
	
	while(tcbTmp!=NULL){
	if(tcbTmp==tcbTask){
		return 1;
	}
	
	tcbTmp=tcbTmp->nexttcb;	
	
	}
	return 0;
	
}

/***********************************************************
 * isTaskinDELAYLIST
 *
 * Comment:
 * 	searches for a task in the delay list
 * 	
 * RETURNVALS:
 *	1 Yes
 *	0 No
 **********************************************************/

int ROSA_prv_isTaskinTCBLIST(tcb * tcbTask)
{	
	tcb * tcbTmp;
		
	tcbTmp=TCBLIST;
	
	while(tcbTmp!=NULL){
	if(tcbTmp==tcbTask){
		return 1;
	}
	
	tcbTmp=tcbTmp->nexttcb;	
	
	}
	return 0;
	
}

/***********************************************************
 * extractTaskFromTCBLIST
 *
 * Comment:
 * 	extracts a Task from the TCBLIST
 *
 **********************************************************/

void ROSA_prv_extractTaskFromTCBLIST(tcb * tcbTask)
{
	tcb * tcbTmp;
	
	if(ROSA_prv_isTaskinTCBLIST(tcbTask)){
	if(tcbTask->prevtcb==NULL){
		if(tcbTask->nexttcb==NULL){
			TCBLIST=NULL;
		}
		else{
			tcbTmp=tcbTask->nexttcb;
			TCBLIST=tcbTask->nexttcb;
			tcbTmp->prevtcb=NULL;
		}
		
	}
	else{
		if(tcbTask->nexttcb==NULL){
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=NULL;
		}
		else{
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=tcbTask->nexttcb;
			tcbTmp=tcbTask->nexttcb;
			tcbTask->prevtcb=tcbTmp->prevtcb;
		}
	}
	
	tcbTask->prevtcb=NULL;
	tcbTask->nexttcb=NULL;
	}
	
}
/***********************************************************
 * insertTaskToTCBLIST
 *
 * Comment:
 * 	inserts a Task to the TCBLIST
 *
 **********************************************************/

void ROSA_prv_insertTaskToTCBLIST(tcb * tcbTask)
{
	
	tcb * tcbTmp;
	if(!ROSA_prv_isTaskinTCBLIST(tcbTask)){	
	if(TCBLIST==NULL){
		tcbTask->nexttcb=NULL;
		tcbTask->prevtcb=NULL;
		TCBLIST=tcbTask;
	}
	else{
		tcbTmp=TCBLIST;
		while((tcbTmp->runningpriority<=tcbTask->runningpriority)&&(tcbTmp!=NULL)){
			tcbTask->prevtcb=tcbTmp;
			tcbTmp=tcbTmp->nexttcb;
		}
		if(tcbTmp==NULL){
			tcbTask->nexttcb=NULL;
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=tcbTask;
		}
		else{
			tcbTmp->prevtcb=tcbTask;
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=tcbTask;
		}
	}
	}
	
}

/***********************************************************
 * extractTaskFromDELAYLIST
 *
 * Comment:
 * 	extracts a Task from the DELAYLIST
 *
 **********************************************************/

void ROSA_prv_extractTaskFromDELAYLIST(tcb * tcbTask)
{
	tcb * tcbTmp;
	
	if(ROSA_prv_isTaskinDELAYLIST(tcbTask)){
	if(tcbTask->prevtcb==NULL){
		if(tcbTask->nexttcb==NULL){
			DELAYLIST=NULL;
		}
		else{
			tcbTmp=tcbTask->nexttcb;
			DELAYLIST=tcbTask->nexttcb;
			tcbTmp->prevtcb=NULL;
		}
		
	}
	else{
		if(tcbTask->nexttcb==NULL){
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=NULL;
		}
		else{
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=tcbTask->nexttcb;
			tcbTmp=tcbTask->nexttcb;
			tcbTask->prevtcb=tcbTmp->prevtcb;
		}
	}
	
	tcbTask->prevtcb=NULL;
	tcbTask->nexttcb=NULL;
	}
	
}


/***********************************************************
 * insertTaskToDELAYLIST
 *
 * Comment:
 * 	inserts a Task to the DELAYLIST
 *
 **********************************************************/

void ROSA_prv_insertTaskToDELAYLIST(tcb * tcbTask)
{
	
	tcb * tcbTmp;
	if(!ROSA_prv_isTaskinDELAYLIST(tcbTask)){	
	if(DELAYLIST==NULL){
		tcbTask->nexttcb=NULL;
		tcbTask->prevtcb=NULL;
		DELAYLIST=tcbTask;
	}
	else{
		tcbTmp=DELAYLIST;
		while((tcbTmp->waketime<=tcbTask->waketime)&&(tcbTmp!=NULL)){
			tcbTask->prevtcb=tcbTmp;
			tcbTmp=tcbTmp->nexttcb;
		}
		if(tcbTmp==NULL){
			tcbTask->nexttcb=NULL;
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=tcbTask;
		}
		else{
			tcbTmp->prevtcb=tcbTask;
			tcbTmp=tcbTask->prevtcb;
			tcbTmp->nexttcb=tcbTask;
		}
	}
	}
	
}



/***********************************************************
 * findTaskInLists
 *
 * Comment:
 * 	searches for a task in all lists
 * 	
 * RETURNVALS:
 *	0 No List
 *  1 TCBLIST
 *	2 DELAYLIST
 **********************************************************/

int ROSA_prv_isTaskinList(tcb * tcbTask)
{	
	tcb * tcbTmp;
		
	tcbTmp=tcbTask;
	
	while(tcbTmp!=NULL){
	if(tcbTmp==DELAYLIST){
		return 2;
	}
	if(tcbTmp==TCBLIST){
		return 1;
	}
	
	tcbTmp=tcbTmp->prevtcb;	
	
	}
	return 0;
	
}

