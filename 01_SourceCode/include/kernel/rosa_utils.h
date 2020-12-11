
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

int ROSA_prv_insertTaskToWAITINGLIST(tcb * tcbTask);
int ROSA_prv_insertTaskToTCBLIST(tcb * tcbTask);
int ROSA_prv_extractTaskFromLIST(tcb * tcbTask);


#endif /* ROSA_UTILS_H_ */