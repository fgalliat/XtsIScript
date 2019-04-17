#include "xts_memory.h"
#include "xts_memutils.h"

bool mainHeapInited = false;


 void _initHeap(); // fill w/ 0x00
 void _initHRegister(); // fill w/ 0x80 (no space used char)

void setupMainMemory() {
    mainHeapInited = false;

    _initHRegister();
    _initHeap();

    mainHeapInited = true;
}

heapAddr heapCursor = 0;
heapAddr hRegisterCursor = 0;

// TO NOT BE DECLARED as STATIC !!!!!!
uint8_t heap[ MAIN_HEAP_SIZE ]; // 64KB

// if static -> may cause single ref but multiple sates
uint8_t hregister[ HEAP_REG_SIZE ]; // 2KB


uint8_t* getHeap(heapAddr addr) {
  return &heap[addr];
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void _initHeap() {
    memset( heap, 0x00, MAIN_HEAP_SIZE );
} // fill w/ 0x00

// fill w/ 0x80
void _initHRegister() {
    memset(hregister, 0x80, HEAP_REG_SIZE);
}

// in bytes
heapAddr getHeapUse() {
    return heapCursor;
}

// return nb of used entries
int getHRegisterUse() {
    for(int i=0; i < HEAP_REG_ENTRY_NB; i++) {
        int addr = i*HEAP_REG_ENTRY_SIZE;
        // reached end of used heap register
        if ( hregister[addr] == 0x80 ) {
            return i;
        }
    }
    return HEAP_REG_ENTRY_NB;
}

// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

// Arrays index are 254 max (so 255 elems)
void composeVarName(char* name, int index, char* dest) {
   memset(dest, 0x00, HEAP_REG_ENTRY_SIZE_name_ext);
   strcpy(dest, name);
   dest[HEAP_REG_ENTRY_SIZE_name_ext - HEAP_REG_ENTRY_SIZE_name_aidx] = index % 256;
}

// returns an index
int findVar(char* name, int index=0) {
    for(int i=0; i < HEAP_REG_ENTRY_NB; i++) {
        int addr = i*HEAP_REG_ENTRY_SIZE;
        // reached end of used heap register
        if ( hregister[addr] == 0x80 ) {
            // DBUG(" a. reached end of heap-reg (0x01)");
            return -1;
        }

        char entryName[ HEAP_REG_ENTRY_SIZE_name_ext ];
        char reqName[ HEAP_REG_ENTRY_SIZE_name_ext ];
        memset( entryName, 0x00, HEAP_REG_ENTRY_SIZE_name_ext );
        memcpy( entryName, &(hregister[addr]), HEAP_REG_ENTRY_SIZE_name_ext );
        composeVarName(name, index, reqName);

        if ( binEq(entryName, reqName, HEAP_REG_ENTRY_SIZE_name_ext) ) {
            DBUG(" a. found in heap-reg (0x01) @entry#", i);
            return i;
        } else {
            // DBUG( entryName, -61 );
        }

    }
    DBUG(" a. reached end of heap-reg (0x02)");
    return -1;
}

// returns an addr
heapAddr getVar(int entryIndex) {
    int idx = entryIndex;

    int addr = idx*HEAP_REG_ENTRY_SIZE;
    DBUG(" b. Found variable in register", addr);
    uint8_t add1 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +0 ];
    uint8_t add0 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +1 ];

    heapAddr found = ( add1 << 8 ) + add0;
    return found;
}

// returns an addr
heapAddr getVar(char* name, int index) {
    int idx = findVar(name, index);
    if ( idx < 0 ) {
        DBUG(" b. Not found variable in register");
        return HEAP_NOT_FOUND;
    }
    return getVar( idx );
}

// returns a varEntryNum of addr just after >> name[index] <<
int findNextVar(int entryIndex) {
    int idx = entryIndex;
    if ( idx < 0 ) {
        DBUG(" b. Not found variable in register");
        return -1;
    }
    if ( idx >= HEAP_REG_ENTRY_NB ) {
        DBUG(" b. No more variable in register");
        return -1;
    }
    return idx+1;
}

// returns a varEntryNum of addr just after >> name[index] <<
int findNextVar(char* name, int index) {
    int idx = findVar(name, index);
    return findNextVar(idx);
}

// returns an addr of addr just after >> name[index] <<
heapAddr getNextVar(char* name, int index) {
    int idx = findNextVar(name, index);
    if ( idx < 0 ) {
        DBUG(" b. Not found variable in register");
        return HEAP_NOT_FOUND;
    }
    return getVar( idx );
}




// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

void registerVar(char* name, int index, heapAddr vaddr) {
  int idx = findVar(name, index);
  bool found = idx >= 0;

  if ( vaddr == HEAP_NOT_FOUND ) {
      vaddr = getHeapUse();
  }

  int addr;

  if ( found ) {
    //   reassignVarAddr
    addr = idx*HEAP_REG_ENTRY_SIZE;
  } else {
    //   addEntry @ hregister end
    idx = getHRegisterUse();
    // BEWARE : w/ overflow detection !!!!
    addr = idx*HEAP_REG_ENTRY_SIZE;
    char reqVar[ HEAP_REG_ENTRY_SIZE_name_ext ];
    composeVarName(name, index, reqVar);
    memcpy( &hregister[addr], reqVar, HEAP_REG_ENTRY_SIZE_name_ext );
  }
  hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +0 ] = vaddr >> 8;
  hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +1 ] = vaddr % 256;
}

// ============= Array Variables =======================

#define ARRAY_MAX_LENGTH 255
#define ARRAY_LENGTH_POSITION 255

// max is 254 (255 elems)
// array[255] is the array descriptor & length variable
int getArrayLength(char* name) {
    int len = getInt( name, ARRAY_LENGTH_POSITION );
    if ( len < 0 ) {
        len = ARRAY_MAX_LENGTH;
    }

    return len;
}

int dimArrayVar(char* name, int length) {
    if ( length < 0 || length >= ARRAY_MAX_LENGTH ) {
        return ASSIGN_ERROR_OVERFLOW;
    }

    return assignVar(name, ARRAY_LENGTH_POSITION, length);
}

// ============= Get Variables =========================

 number getInt(char* name, int index) {
     heapAddr varAddr = getVar(name, index);
     if ( varAddr == HEAP_NOT_FOUND ) { return -1; }

     int result;

     if ( heap[ varAddr ] == TYPE_NUM ) {
        result = peekInt( &heap[ varAddr+1 ] );
     } else if ( heap[ varAddr ] == TYPE_FLOAT ) {
         return (number)getFloat(name, index);
     } else {
         DBUG("(!!) Wrong type (0xtfgghf)");
         return -1;
     }

    return result;
 }

 decimal getFloat(char* name, int index) {
     heapAddr varAddr = getVar(name, index);
     if ( varAddr == HEAP_NOT_FOUND ) { return -1.0F; }

     decimal result;

     if ( heap[ varAddr ] == TYPE_FLOAT ) {
        result = peekFloat( &heap[ varAddr+1 ] );
     } else if ( heap[ varAddr ] == TYPE_NUM ) {
         return (decimal)getInt( name, index );
     } else {
         DBUG("(!!) Wrong type (0xdflh)");
         return -1.0F;
     }

    return result;
 }

 // ============= Set Variables =========================


 int assignVar(char* name, int index, number value) {
     int aryIdx = index;
     if ( aryIdx < 0 || aryIdx > getArrayLength(name) ) {
         // MSG : invalid array index
         return ASSIGN_ERROR_INDEX;
     }

     heapAddr varAddr = getVar(name, aryIdx);
     DBUG(" c. Var addr : ", varAddr);
     bool existing = varAddr != HEAP_NOT_FOUND;
     bool validAddr = varAddr != MAIN_HEAP_SIZE;

     if ( !validAddr ) {
         // MSG: invalid address
         return ASSIGN_ERROR_ADDR;
     }

     if ( !existing ) {
         // needed too for overflow detection
         varAddr = getHeapUse();
         DBUG(" c. will use end of heap", varAddr);
     }

     if ( varAddr + HEAP_ST_NUMVAL_SIZE >= MAIN_HEAP_SIZE ) {
         // MSG: overflow address
         return ASSIGN_ERROR_OVERFLOW;
     }

     if ( existing ) {
         DBUG(" c. var was found");
         if ( heap[ varAddr ] != TYPE_NUM && heap[ varAddr ] != TYPE_FLOAT ) {
             // MSG: invalid var type
             return ASSIGN_ERROR_TYPE;
         }

         // Oups : it was a float-typed value
         if ( heap[ varAddr ] == TYPE_FLOAT ) {
             return assignVar(name, index, (float)value);
         }

     } else {
         DBUG(" c. register the var in hreg");
         registerVar(name, aryIdx, varAddr);
     } 

     heap[ varAddr +0 ] = TYPE_NUM;
     storeInt( &heap[ varAddr +1 ], value );

     if (!existing) { heapCursor += HEAP_ST_NUMVAL_SIZE; }
     return ASSIGN_NOERROR;
 }

 int assignVar(char* name, int index, decimal value) {
     int aryIdx = index;
     if ( aryIdx < 0 || aryIdx > getArrayLength(name) ) {
         // MSG : invalid array index
         return ASSIGN_ERROR_INDEX;
     }

     heapAddr varAddr = getVar(name, aryIdx);
     DBUG(" d. Var addr : ", varAddr);
     bool existing = varAddr != HEAP_NOT_FOUND;
     bool validAddr = varAddr != MAIN_HEAP_SIZE;

     if ( !validAddr ) {
         // MSG: invalid address
         return ASSIGN_ERROR_ADDR;
     }

     if ( !existing ) {
         // needed too for overflow detection
         varAddr = getHeapUse();
         DBUG(" d. will use end of heap", varAddr);
     }

     if ( varAddr + HEAP_ST_DECVAL_SIZE >= MAIN_HEAP_SIZE ) {
         // MSG: overflow address
         return ASSIGN_ERROR_OVERFLOW;
     }

     if ( existing ) {
         DBUG(" d. var was found");
         if ( heap[ varAddr ] != TYPE_NUM && heap[ varAddr ] != TYPE_FLOAT ) {
             // MSG: invalid var type
             return ASSIGN_ERROR_TYPE;
         }

         // Oups : it was an int-typed value
         if ( heap[ varAddr ] == TYPE_NUM ) {
             return assignVar(name, index, (number)value);
         }

     } else {
         DBUG(" d. register the var in hreg");
         registerVar(name, aryIdx, varAddr);
     } 

     heap[ varAddr +0 ] = TYPE_FLOAT;
     storeDec( &heap[ varAddr + 1 ], value );

     if (!existing) { heapCursor += HEAP_ST_DECVAL_SIZE; }
     return ASSIGN_NOERROR;
 }

 // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

#define NOT_YET 1

#ifdef NOT_YET
 // returns assignResultCode
 int reassignString(int varIdx, char* value, heapAddr &finalAddr) {
     int segLen = strlen( value );

     heapAddr currentAddr = getVar( varIdx );
     heapAddr nextSibbling = getVar( findNextVar( varIdx ) );

     if ( nextSibbling == HEAP_NOT_FOUND ) {
         // there is NO var nextSibbling
         if ( currentAddr + segLen >= MAIN_HEAP_SIZE ) {
             // too long for remaining Heap
             // TODO : could try to defrag if needed ....
             finalAddr = HEAP_NOT_FOUND;
             return ASSIGN_ERROR_OVERFLOW;
         } else {
             // can append to heap
             storeStr( &heap[ currentAddr ], value, segLen, true );
             finalAddr = currentAddr;
             heapCursor += segLen+1;
         }
     } else {
         // there is a var nextSibbling
         if ( currentAddr + segLen >= nextSibbling ) {
             // too long 'till next value, have to relocate

             // blanking no-more-used area
             memset( &heap[ currentAddr ], 0x00, nextSibbling-currentAddr );

             // place @ end of heap
             currentAddr = getHeapUse();

             if ( currentAddr + segLen >= MAIN_HEAP_SIZE ) {
                // too long for remaining Heap
                // TODO : could try to defrag if needed ....
                finalAddr = HEAP_NOT_FOUND;
                return ASSIGN_ERROR_OVERFLOW;
             } else {
                // append to heap
                storeStr( &heap[ currentAddr ], value, segLen, true );
                finalAddr = currentAddr;
                heapCursor += segLen+1;
             }

         } else {
             // can append before next value
             storeStr( &heap[ currentAddr ], value, segLen, true );
             finalAddr = currentAddr;
             heapCursor += 0;
         }
     }

     return ASSIGN_NOERROR;
 }

 // returns assignResultCode
 int reassignString(char* name, int index, char* value) {
     int curVarIdx = findVar( name, index );

     heapAddr lastAddr = 0;

     if ( curVarIdx < 0 ) {
         registerVar(name, index, lastAddr);
     } else {
         reassignString( curVarIdx, value, lastAddr );
         int subAddr = curVarIdx * HEAP_REG_ENTRY_SIZE;
         hregister[ subAddr + HEAP_REG_ENTRY_SIZE_name_ext + 0 ] = (uint8_t)( lastAddr >> 8 );
         hregister[ subAddr + HEAP_REG_ENTRY_SIZE_name_ext + 1 ] = (uint8_t)( lastAddr % 256 );
     }

 }


#endif

 // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
 // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

 // ====== Debug =================
 char printableChar(uint8_t code) {
     if ( code < 32  ) { return '?'; }
     if ( code > 127 ) { return '?'; }
     return (char)code;
 }

 void debugHRegister(int start, int stop) {
     if ( stop < 0 ) { stop = HEAP_REG_SIZE; }
     int pageSize = HEAP_REG_ENTRY_SIZE;
     for(int i=start; i < stop; i++) {
         printf("0x%02X %c ", hregister[i], printableChar(hregister[i]));
         if ( i % pageSize == pageSize-1 ) { printf("\n"); }
     }
     printf("\n");
 }

 void debugHeap(int start, int stop, int pageSize) {
     if ( stop < 0 ) { stop = MAIN_HEAP_SIZE; }
     for(int i=start; i < stop; i++) {
         printf("0x%02X %c ", heap[i], printableChar(heap[i]));
         if ( i % pageSize == pageSize-1 ) { printf("\n"); }
     }
     printf("\n");
 }