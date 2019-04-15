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

heapAddr getVar(char* name) {
    for(int i=0; i < HEAP_REG_ENTRY_NB; i++) {
        int addr = i*HEAP_REG_ENTRY_SIZE;
        // reached end of used heap register
        if ( hregister[addr] == 0x80 ) {
            return HEAP_NOT_FOUND;
        }

        char entryName[ HEAP_REG_ENTRY_SIZE_name +1 ];
        memset( entryName, 0x00, HEAP_REG_ENTRY_SIZE_name +1 );
        memcpy( entryName, &(hregister[addr]), HEAP_REG_ENTRY_SIZE_name );

        if ( strcmp(entryName, name) == 0 ) {
            uint8_t add1 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name ];
            uint8_t add0 = hregister[ addr + HEAP_REG_ENTRY_SIZE_name +1 ];

            heapAddr found = ( add1 << 8 ) + add0;
            return found;
        }

    }
    return HEAP_NOT_FOUND;
}


