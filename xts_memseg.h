#ifndef _XTS_MEMSEG_H_
#define _XTS_MEMSEG_H_ 1

#include "xts_typedef.h"

#define SCRATCHPAD_SIZE 2048
#define SCRATCHPAD_REGISTER_SIZE 100

xvalue _appendToScratchPad(xvalue &v, char* str);
void removeFromScratchPad(xvalue v);
void cleanScratchPad();
char* getScratchPad(int addr=0, int len=-1);
char* getScratchPad(xvalue ptr);
void defragScratchPad();

int findInScrathPadRegister(xvalue v);
int getScratchPadRegisterLen();
char* getScratchPadFromRegister(int idx);

// =========== Lexems Storage ==============

bool cleanLex();
bool pushLex(xlexem x);

xlexem getNextLex();
void resetNextLex();


#endif
