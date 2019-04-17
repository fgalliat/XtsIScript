#ifndef __XTS_MEMORY_H_
#define __XTS_MEMORY_H_ 1

 #include "xts_typedef.h"
 #include "xts_io.h"

 #include "xts_memtypes.h"
 static const int HEAP_ST_NUMBER_SIZE = ST_NUMBER_SIZE;
 static const int HEAP_ST_DECIMAL_SIZE = ST_DECIMAL_SIZE;
 static const int HEAP_ST_NUMVAL_SIZE = 1+HEAP_ST_NUMBER_SIZE;
 static const int HEAP_ST_DECVAL_SIZE = 1+HEAP_ST_DECIMAL_SIZE;



 #define MAIN_HEAP_SIZE 65536
 typedef uint16_t heapAddr;
 
 // cause no var can use only 1byte of storage
 #define HEAP_NOT_FOUND (MAIN_HEAP_SIZE - 1)
 
 // if static -> may cause single ref but multiple sates
 // static uint8_t heap[ MAIN_HEAP_SIZE ]; // 64KB

 // if static -> may cause single ref but multiple sates
 // static uint8_t hregister[ HEAP_REG_SIZE ]; // 2KB

 void setupMainMemory();
 uint8_t* getHeap(heapAddr addr=0);

 // in bytes
 heapAddr getHeapUse();

 // ======= Assign Ops ============

 #define ASSIGN_NOERROR        0
 #define ASSIGN_ERROR_OVERFLOW 1
 #define ASSIGN_ERROR_TYPE     2
 #define ASSIGN_ERROR_ADDR     3
 #define ASSIGN_ERROR_INDEX    4
 static const char* assignErrorMsg[] = {
     "OK",
     "Memory Overflow",
     "Type mismatch",
     "Invalid addr",
     "Out of Bounds Index"
 };

 int assignVar(char* name, int index, number value);
 int assignVar(char* name, int index, decimal value);
 int assignVar(char* name, int index, char* value);

 static int assignVar(char* name, number value) { return assignVar( name, 0, value ); }
 static int assignVar(char* name, decimal value) { return assignVar( name, 0, value ); }
//  static int assignVar(char* name, char* value) { return assignVar( name, 0, value ); }

 int dimArrayVar(char* name, int length);
 int getArrayLength(char* name);

 // ======= Fetch Ops ============

 number getInt(char* name, int index=0);
 float getFloat(char* name, int index=0);
 char* getString(char* name, int index=0);

 // ====== Heap Register =========

 #define HEAP_REG_ENTRY_SIZE_name_only 5
 #define HEAP_REG_ENTRY_SIZE_name_aidx 1
 #define HEAP_REG_ENTRY_SIZE_name_ext (HEAP_REG_ENTRY_SIZE_name_only + HEAP_REG_ENTRY_SIZE_name_aidx)
 #define HEAP_REG_ENTRY_SIZE_addr sizeof( heapAddr )
 #define HEAP_REG_ENTRY_SIZE ( HEAP_REG_ENTRY_SIZE_name_ext + HEAP_REG_ENTRY_SIZE_addr )
 #define HEAP_REG_ENTRY_NB 256
 #define HEAP_REG_SIZE ( HEAP_REG_ENTRY_SIZE * HEAP_REG_ENTRY_NB )

 void registerVar(char* name, int index = 0, heapAddr addr=HEAP_NOT_FOUND);
 heapAddr getVar(char* name, int index=0);

 // === GC Routines ==============

 bool markForGC(char* name, int index=0); // see later
 heapAddr defragHeap();

 // ==============================
 void debugHRegister(int start=0, int stop=-1);
 void debugHeap(int start=0, int stop=-1, int pageSize=16);

#endif