/*
 * rosa_utils.h
 *
 * Created: 14/12/2020 18:22:24
 *  Author: TEAM RUBUS
 */ 


#ifndef ROSA_UTILS_H_
#define ROSA_UTILS_H_

#include <avr32/io.h>
#include "stdint.h"
#include "rosa_def.h"

int ROSA_prv_insertTaskToWAITINGLIST(tcb * tcbTask);
int ROSA_prv_insertTaskToTCBLIST(tcb * tcbTask);
int ROSA_prv_extractTaskFromLIST(tcb * tcbTask);


#endif /* ROSA_UTILS_H_ */