#ifndef _XTS_IO_H_
#define _XTS_IO_H_ 1

#include "xts_typedef.h"

extern bool DBUG_LOCKED;


void DBUG(const char* str);

void DBUG(xvalue v);
void DBUG(xlexem v);

void DBUG(long int v);

void DBUG(int v);

void DBUG(float v);

void DBUG_MEM();

void warning(const char* str);

#endif