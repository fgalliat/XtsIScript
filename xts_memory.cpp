#include "xts_memory.h"

bool mainHeapInited = false;

void setupMainMemory() {
    mainHeapInited = false;

    _initHRegister();
    _initHeap();

    mainHeapInited = true;
}

heapAddr heapCursor = 0;
heapAddr hRegisterCursor = 0;

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
    return HEAP_REG_ENTRY_NB-1;
}


// -- heap reg ----

bool binEq(char* s0, char* s1, int len) {
    for(int z=0; z < len; z++) {
        if ( s0[z] != s1[z] ) { return false; }
    }
    return true;
}

// Arrays index are 255 max
void composeVarName(char* name, int index, char* dest) {
   memset(dest, 0x00, HEAP_REG_ENTRY_SIZE_name_ext);
   strcpy(dest, name);
   dest[HEAP_REG_ENTRY_SIZE_name_ext - HEAP_REG_ENTRY_SIZE_name_aidx] = index % 256;
}


// BEWARE w/ array cells

int findVar(char* name, int index=0) {
    for(int i=0; i < HEAP_REG_ENTRY_NB; i++) {
        int addr = i*HEAP_REG_ENTRY_SIZE;
        // reached end of used heap register
        if ( hregister[addr] == 0x80 ) {
            DBUG(" a. reached end of heap-reg (0x01)");
            return -1;
        }

        char entryName[ HEAP_REG_ENTRY_SIZE_name_ext ];
        char reqName[ HEAP_REG_ENTRY_SIZE_name_ext ];
        memset( entryName, 0x00, HEAP_REG_ENTRY_SIZE_name_ext );
        memcpy( entryName, &(hregister[addr]), HEAP_REG_ENTRY_SIZE_name_ext );
        composeVarName(name, index, reqName);

        if ( binEq(entryName, reqName, HEAP_REG_ENTRY_SIZE_name_ext) ) {
            DBUG(" a. found in heap-reg (0x01)", i);
            return i;
        }

    }
    DBUG(" a. reached end of heap-reg (0x02)");
    return -1;
}



heapAddr getVar(char* name, int index) {
    int idx = findVar(name, index);
    if ( idx < 0 ) {
        DBUG(" b. Not found variable in register");
        return HEAP_NOT_FOUND;
    }

    int addr = idx*HEAP_REG_ENTRY_SIZE;
    DBUG(" b. Found variable in register", addr);
    uint8_t add1 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +0 ];
    uint8_t add0 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +1 ];

    heapAddr found = ( add1 << 8 ) + add0;
    return found;
}

void registerVar(char* name, int index, heapAddr vaddr) {
  int idx = findVar(name);
  bool found = idx >= 0;

  if ( vaddr == HEAP_NOT_FOUND ) {
      vaddr = getHeapUse();
  }

  if ( found ) {
    //   reassignVarAddr
    int addr = idx*HEAP_REG_ENTRY_SIZE;
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +0 ] = vaddr >> 8;
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +1 ] = vaddr % 256;
  } else {
    //   addEntry @ hregister end
    idx = getHRegisterUse();
    // BEWARE : w/ overflow detection !!!!
    int addr = idx*HEAP_REG_ENTRY_SIZE;
    memset(hregister, 0x00, HEAP_REG_ENTRY_SIZE_name_ext);
    char reqVar[ HEAP_REG_ENTRY_SIZE_name_ext ];
    composeVarName(name, index, reqVar);
    memcpy( &hregister[addr], reqVar, HEAP_REG_ENTRY_SIZE_name_ext );
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +0 ] = vaddr >> 8;
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name_ext +1 ] = vaddr % 256;
  }
}

// ============= Get Variables =========================

int getArrayLength(char* name) {
    // TMP : TODO : better
    return 255;
}


 number getInt(char* name, int index) {
     heapAddr varAddr = getVar(name, index);
     if ( varAddr == HEAP_NOT_FOUND ) { return -1; }

     char tmp[ HEAP_ST_NUMBER_SIZE ];
     memcpy( tmp, &heap[ varAddr+1 ], HEAP_ST_NUMBER_SIZE );

     int result = (tmp[0]<<24)+(tmp[1]<<16)+(tmp[2]<<8)+(tmp[3]);
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
     DBUG("Var addr : ", varAddr);
     bool existing = varAddr != HEAP_NOT_FOUND;
     bool validAddr = varAddr != MAIN_HEAP_SIZE;

     if ( !validAddr ) {
         // MSG: invalid address
         return ASSIGN_ERROR_ADDR;
     }

     if ( !existing ) {
         // needed too for overflow detection
         varAddr = getHeapUse();
         DBUG(" c. will use end of heap");
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
     } else {
         DBUG(" c. register the var in hreg");
         registerVar(name, aryIdx, varAddr);
     } 

     heap[ varAddr +0 ] = TYPE_NUM;
     // memcpy( &heap[ varAddr + 1 ], &value, HEAP_ST_NUMBER_SIZE );
     heap[ varAddr +1 ] = (value >> 24) % 0xFF;
     heap[ varAddr +2 ] = (value >> 16) % 0xFF;
     heap[ varAddr +3 ] = (value >> 8) % 0xFF;
     heap[ varAddr +4 ] = value % 0xFF;

     return ASSIGN_NOERROR;
 }
