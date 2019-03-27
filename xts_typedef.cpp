#include "xts_typedef.h"
#include "xts_memseg.h"

/**
 * Xtase - fgalliat @Feb 2019
 * 
 * XtsIScript type manipulations routines
 * 
 * 
 */

int xvAllocNb = 0;

long int getUsedMemFloating() {
    return xvAllocNb * sizeof(xvalue);
}

// CERTIFIED CODE
void reassignVal(xvalue &v, numvar num) {
    v.type = TYPE_NUM;
    v.value[0] = (num >> 24) & 0xFF;
    v.value[1] = (num >> 16) & 0xFF;
    v.value[2] = (num >> 8) & 0xFF;
    v.value[3] = num & 0xFF;
}

// CERTIFIED CODE
void reassignVal(xvalue &v, float num) {
    v.type = TYPE_FLOAT;
    memcpy( v.value, &num, 4 );
}

// CERTIFIED CODE
// TODO : will Ignore addr @ this time
void reassignVal(xvalue &v, char* _str, int addr) {
    // TODO : check if already a String
    // then if len is <= : reuse addr
    _appendToScratchPad(v, _str);
}

void storeString(xvalue &v, char* _str, int addr) {
    if ( _str == NULL ) {
        v.type = TYPE_NULL;
        return;
    }

    v.type = TYPE_STR;
    int len = strlen(_str);
    v.value[0] = addr/256; // TODO
    v.value[1] = addr%256; // TODO
    v.value[2] = len/256; // TODO
    v.value[3] = len%256; // TODO
}


// CERTIFIED CODE
xvalue num(numvar num) {
    xvalue v;
    xvAllocNb++;
    reassignVal(v, num);
    return v;
}

// CERTIFIED CODE
numvar toNum(xvalue v) {
    if ( v.type == TYPE_FLOAT ) return (int)toFloat(v);
    if ( v.type != TYPE_NUM ) return -1;
    int res = ( v.value[0] << 24 ) +( v.value[1] << 16 ) +( v.value[2] << 8 ) +( v.value[3] );
    return res;
}

// CERTIFIED CODE
xvalue flt(float num) {
    xvalue v;
    xvAllocNb++;
    reassignVal(v, num);
    return v;
}

// CERTIFIED CODE
float toFloat(xvalue v) {
    if ( v.type == TYPE_NUM ) return (float)toNum(v);
    if ( v.type != TYPE_FLOAT ) return -1.0f;
    float res = 0.0f;
    memcpy( &res, v.value, 4 );
    return res;
}

// TODO : will Ignore addr @ this time
xvalue str(char* _str, int addr) {
    xvalue v;
    xvAllocNb++;

    reassignVal(v, _str, addr);
    return v;
}

// must be defined AFTER the char* signature else segFault
xvalue str(const char* _str, int addr) {
    return str( (char*)_str, addr );
}

void freeVal(xvalue &v) {
    if (v.type == TYPE_STR) {
        removeFromScratchPad(v);
    }
    nullifyVal(v);
}

void nullifyVal(xvalue &v) {
    v.type = TYPE_NULL;
}