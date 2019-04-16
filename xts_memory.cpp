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

// -- heap reg ----

bool binEq(char* s0, char* s1, int len) {
    for(int z=0; z < len; z++) {
        if ( s0[z] != s1[z] ) { return false; }
    }
    return true;
}

// Arrays index are 256 max
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
            return -1;
        }

        char entryName[ HEAP_REG_ENTRY_SIZE_name_ext ];
        char reqName[ HEAP_REG_ENTRY_SIZE_name_ext ];
        memset( entryName, 0x00, HEAP_REG_ENTRY_SIZE_name_ext );
        memcpy( entryName, &(hregister[addr]), HEAP_REG_ENTRY_SIZE_name_ext );
        composeVarName(name, index, reqName);

        if ( binEq(entryName, reqName, HEAP_REG_ENTRY_SIZE_name_ext) ) {
            return i;
        }

    }
    return -1;
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

heapAddr getVar(char* name) {
    int idx = findVar(name);
    if ( idx < 0 ) {
        return HEAP_NOT_FOUND;
    }

    int addr = idx*HEAP_REG_ENTRY_SIZE;
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



