#ifndef __XTS_MEMUTILS_H_
#define __XTS_MEMUTILS_H_ 1

  #include "xts_memtypes.h"

  // returns true if both memseg are binary equals
  bool binEq(char* s0, char* s1, int len);

  // Serialize some values into a memseg
  int storeInt(uint8_t* ptr, number value);
  int storeDec(uint8_t* ptr, decimal value);
  int storeStr(uint8_t* ptr, char* value, int len=-1, bool storeLastZero=true);


  // DeSerialize some values from a memseg
  decimal peekFloat(uint8_t* ptr);
  number peekInt(uint8_t* ptr);
  char* peekStr(uint8_t* ptr, char* dest=NULL, int maxLen=-1);



#endif