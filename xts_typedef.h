#ifndef XTS_TYPEDEF_H_
#define XTS_TYPEDEF_H_ 1

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * Xtase - fgalliat @Feb 2019
 * 
 * XtsIScript type definitions
 * 
 * 
 */

// ==============================================
static int max(int a, int b) { return a>b ? a : b; }
static int min(int a, int b) { return a<b ? a : b; }
// static int abs(int a) { return a<0 ? -a : a; }
// ==============================================


// long int on Arduino ????
typedef int numvar;

#define TYPE_UNDEF 0
#define TYPE_NUM 1
#define TYPE_UNUM 2
#define TYPE_STR 3
#define TYPE_FLOAT 4
// to access to scratchPad area
#define TYPE_SCRATCH 5
#define TYPE_NULL 6

static const char* TYPES_STR[] = {
  "UNDEF",
  "NUM",
  "UNUM",
  "STR",
  "FLOAT",
  "SCRATCH",
  "NULL",
};

typedef struct {
  uint8_t type = TYPE_NULL;
  uint8_t value[4]; // can be a 32bit integer, float, string addr
} xvalue;

long int getUsedMemFloating();

static xvalue XNULL;

xvalue num(numvar num);
xvalue flt(float num);
// @ this time : ADDR is ignored
xvalue str(char* _str, int addr=0);
// must be defined AFTER the char* signature else segFault
xvalue str(const char* _str, int addr=0);

void reassignVal(xvalue &v, numvar num);
void reassignVal(xvalue &v, float num);
void reassignVal(xvalue &v, char* _str, int addr=0);

void freeVal(xvalue &v);
void nullifyVal(xvalue &v);

numvar toNum(xvalue v);
float toFloat(xvalue v);

// =====================================

// TODO : ?? preallocate a memSeg ?? 
#define MAX_TOKEN_REPR_SIZE 12
typedef struct {
  uint8_t type = 0; // undefined
  uint8_t idx = -1;
  uint8_t len = 0;
  char token[MAX_TOKEN_REPR_SIZE];
} xlexem;

static xlexem lnull; // undefined lexem


#endif