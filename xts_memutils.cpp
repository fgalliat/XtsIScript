#include "xts_memutils.h"

/**
  Xtase - fgalliat @ Apr2019

  Generic memseg routines 

*/

    // returns true if both memseg are binary equals

    bool binEq(char* s0, char* s1, int len) {
        for(int z=0; z < len; z++) {
            if ( s0[z] != s1[z] ) { return false; }
        }
        return true;
    }


    // Serialize some values into a memseg

    int storeInt(uint8_t* ptr, number value) {
      ptr[0] = (value >> 24) % 256;
      ptr[1] = (value >> 16) % 256;
      ptr[2] = (value >> 8) % 256;
      ptr[3] = value % 256; // 256 : not 0xFF (255)   
      return ST_NUMBER_SIZE;
    }

    int storeDec(uint8_t* ptr, decimal value) {
        memcpy( ptr, &value, ST_DECIMAL_SIZE );
        return ST_DECIMAL_SIZE;
    }

    int storeStr(uint8_t* ptr, char* value, int len, bool storeLastZero) {
        if ( len < 0 ) { len = strlen(value); }
        memcpy( ptr, value, len );
        if (storeLastZero) { ptr[ len ] = 0x00; } 
        return len;
    }

    // ========================

    // DeSerialize some values from a memseg

    decimal peekFloat(uint8_t* ptr) {
        decimal result;
        memcpy( &result, ptr, ST_DECIMAL_SIZE );
        return result;
    }

    number peekInt(uint8_t* ptr) {
        number result = 0;
        result = (ptr[0]<<24)+(ptr[1]<<16)+(ptr[2]<<8)+(ptr[3]);
        return result;
    }

    char* peekStr(uint8_t* ptr, char* dest, int maxLen) {
        int maxPeek = -1;
        for(int i=0; i < 99999; i++) {
            if ( ptr[i] == 0x00 ) {
                maxPeek = i;
                break;
            }
        }

        if ( maxLen < 0 ) {
            maxLen = maxPeek;
        }

        int lenToKeep = min( maxLen, maxPeek );

        if ( dest == NULL ) {
            dest = (char*)malloc( lenToKeep+1 );
        }

        memset(dest, 0x00, lenToKeep+1);

        memcpy(dest, ptr, lenToKeep);
        return dest;
    }