/*
 * rosa_utils.h
 *
 * Created: 06.12.2020 14:25:32
 *  Author: lukas
 */ 


#ifndef ROSA_UTILS_H_
#define ROSA_UTILS_H_


#include <avr32/io.h>
#include "stdint.h"
#include "rosa_def.h"


void ROSA_prv_extractTaskFromDELAYLIST(tcb * tcbTask);
void ROSA_prv_insertTaskToDELAYLIST(tcb * tcbTask);
int ROSA_prv_isTaskinDELAYLIST(tcb * tcbTask);
int ROSA_prv_isTaskinList(tcb * tcbTask);



#endif /* ROSA_UTILS_H_ */