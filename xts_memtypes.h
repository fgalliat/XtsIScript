#ifndef __XTS_MEMTYPES_H_
#define __XTS_MEMTYPES_H_ 1

 /**
  Xtase - fgalliat @ Apr2019

  Memory data types
 */

  #include "xts_typedef.h"


 // Cf arduino
 #ifdef ARDUINO_BOARD
 // 32b signed integer
 typedef long int number;
 #else
 // 32b signed integer
 typedef int number;
 #endif 

 typedef float decimal;

 static const int ST_NUMBER_SIZE = sizeof(number);
 static const int ST_DECIMAL_SIZE = sizeof(decimal);


#endif