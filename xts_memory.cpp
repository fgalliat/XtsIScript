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

// BEWARE w/ array cells

int findVar(char* name) {
    for(int i=0; i < HEAP_REG_ENTRY_NB; i++) {
        int addr = i*HEAP_REG_ENTRY_SIZE;
        // reached end of used heap register
        if ( hregister[addr] == 0x80 ) {
            return -1;
        }

        char entryName[ HEAP_REG_ENTRY_SIZE_name +1 ];
        memset( entryName, 0x00, HEAP_REG_ENTRY_SIZE_name +1 );
        memcpy( entryName, &(hregister[addr]), HEAP_REG_ENTRY_SIZE_name );

        if ( strcmp(entryName, name) == 0 ) {
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
    uint8_t add1 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name +0 ];
    uint8_t add0 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name +1 ];

    heapAddr found = ( add1 << 8 ) + add0;
    return found;
}

void registerVar(char* name, heapAddr vaddr) {
  int idx = findVar(name);
  bool found = idx >= 0;
  if ( found ) {
    //   reassignVarAddr
    int addr = idx*HEAP_REG_ENTRY_SIZE;
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name +0 ] = vaddr >> 8;
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name +1 ] = vaddr % 256;
  } else {
    //   addEntry @ hregister end
    idx = getHRegisterUse();
    // BEWARE : w/ overflow detection !!!!
    int addr = idx*HEAP_REG_ENTRY_SIZE;
    memset(hregister, 0x00, HEAP_REG_ENTRY_SIZE_name);
    memcpy( &hregister[addr], name, max( strlen(name), HEAP_REG_ENTRY_SIZE_name ) );
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name +0 ] = vaddr >> 8;
    hregister[ addr + HEAP_REG_ENTRY_SIZE_name +1 ] = vaddr % 256;
  }
}



