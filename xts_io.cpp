#include "xts_io.h"

#include "xts_syntax.h"

#include <stdio.h>
#define _printf printf

bool DBUG_LOCKED = false;

void DBUG(const char* str) {
    if (DBUG_LOCKED) { return; }
    _printf( "%s\n", str );
}

void DBUG(const char* str, int num) {
    if (DBUG_LOCKED) { return; }
    _printf( "%s => (%d)\n", str, num );
}

void DBUG(xvalue v) {
    if (DBUG_LOCKED) { return; }
    _printf( "%s(%d) %d,%d,%d,%d", TYPES_STR[v.type], v.type, v.value[0], v.value[1], v.value[2], v.value[3] );
    if ( v.type == TYPE_NUM ) {
        _printf( " => %d", toNum(v) );
    } else if ( v.type == TYPE_FLOAT ) {
        _printf( " => %g", toFloat(v) );
    }
    _printf( "\n");
}

void DBUG(xlexem v) {
    if (DBUG_LOCKED) { return; }
    // printf("LEX:%s %d '%s'\n", TK_TYPES[v.type], v.idx, v.token);
    printf("LEX:%s %d ", TK_TYPES[v.type], v.idx);
    if ( v.type == TK_STR ) {
      char* tmp = getScratchPadFromRegister( v.idx );
      printf("'%s'", tmp );
      free(tmp);
    } else if ( v.type == TK_INT ) {
      int x = 0;
      x = (v.token[0] << 24)+(v.token[1] << 16)+(v.token[2] << 8)+(v.token[3]);
      printf("'%d'", x);  
    } else if ( v.type == TK_FLT ) {
      float x = 0;
      memcpy( &x, v.token, 4 );
      printf("'%g'", x);  
    } else {
      printf("'%s'", v.token);
    }
    printf("\n");
}

void DBUG(long int v) {
    if (DBUG_LOCKED) { return; }
    _printf( "%ld\n", v );
}

void DBUG(int v) {
    if (DBUG_LOCKED) { return; }
    _printf( "%d\n", v );
}

void DBUG(float v) {
    if (DBUG_LOCKED) { return; }
    _printf( "%g\n", v );
}

void DBUG_MEM() {
    _printf( "%ld bytes allocated outside prebuff\n", getUsedMemFloating() );
}

void warning(const char* str) {
    _printf("(!!) %s\n", str);
}