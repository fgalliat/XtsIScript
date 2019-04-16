#ifndef __XTS_MEMORY_H_
#define __XTS_MEMORY_H_ 1

 #include "xts_typedef.h"


 void setupMainMemory();

 #define MAIN_HEAP_SIZE 655356
 
 typedef uint16_t heapAddr;
 // cause no var can use only 1byte of storage
 #define HEAP_NOT_FOUND (MAIN_HEAP_SIZE - 1)
 
 uint8_t heap[ MAIN_HEAP_SIZE ]; // 64KB
 void _initHeap(); // fill w/ 0x00

 // in bytes
 heapAddr getHeapUse();


 // Cf arduino
 typedef uint32_t num;

 void assignVar(char* name, num value);
 void assignVar(char* name, int value);
 void assignVar(char* name, float value);
 void assignVar(char* name, char* value);

 void dimArrayVar(char* name, int length);

 void assignArray(char* name, int index, num value);
 void assignArray(char* name, int index, int value);
 void assignArray(char* name, int index, float value);
 void assignArray(char* name, int index, char* value);
 
 num getInt(char* name);
 float getFloat(char* name);
 char* getString(char* name);

 int getArrayLength(char* name);

 num getIntFromArray(char* name, int index);
 float getFloatFromArray(char* name, int index);
 char* getStringFromArray(char* name, int index);

 heapAddr defragHeap();

 #define HEAP_REG_ENTRY_SIZE_name_only 5
 #define HEAP_REG_ENTRY_SIZE_name_aidx 1
 #define HEAP_REG_ENTRY_SIZE_name_ext (HEAP_REG_ENTRY_SIZE_name_only + HEAP_REG_ENTRY_SIZE_name_aidx)
 #define HEAP_REG_ENTRY_SIZE_addr sizeof( heapAddr )
 #define HEAP_REG_ENTRY_SIZE ( HEAP_REG_ENTRY_SIZE_name_ext + HEAP_REG_ENTRY_SIZE_addr )
 #define HEAP_REG_ENTRY_NB 256
 #define HEAP_REG_SIZE ( HEAP_REG_ENTRY_SIZE * HEAP_REG_ENTRY_NB )
 uint8_t hregister[ HEAP_REG_SIZE ]; // 2KB

 void _initHRegister(); // fill w/ 0x80 (no space used char)

 void registerVar(char* name, int index = 0, heapAddr addr=HEAP_NOT_FOUND);
 heapAddr getVar(char* name, int index=0);
 bool markForGC(char* name, int index=0); // see later

#endif