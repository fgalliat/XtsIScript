#include "xts_memseg.h"
#include "xts_io.h"

// ======================== ScratchPad ==================

char scratchPad[ SCRATCHPAD_SIZE ];
int scratchPadCursor = 0;

xvalue scratchPadRegister[ SCRATCHPAD_REGISTER_SIZE ];
int scratchPadRegisterCursor = 0;

extern void storeString(xvalue &v, char* _str, int addr);

int getScratchPadRegisterLen() { return scratchPadRegisterCursor; }
char* getScratchPadFromRegister(int idx) {
    if ( idx < 0 || idx >= getScratchPadRegisterLen() ) {
        return (char*)"Oups (0xOA)";
    }
    xvalue v = scratchPadRegister[idx];
    return getScratchPad( v );
}





xvalue _appendToScratchPad(xvalue &v, char* _str) {
  if ( _str == NULL ) { return XNULL; }
  int tlen = strlen( _str );
  if ( scratchPadCursor + tlen >= SCRATCHPAD_SIZE ) {
      DBUG("Warning SCRATCHPAD Overflow (prevented) !");
      return XNULL;
  }
  if ( scratchPadRegisterCursor >= SCRATCHPAD_REGISTER_SIZE ) {
      DBUG("Warning SCRATCHPAD Overflow (prevented) !");
      return XNULL;
  }
  memcpy( &scratchPad[scratchPadCursor], _str, tlen );
//   xvalue res = str( _str, scratchPadCursor ); 
  storeString( v, _str, scratchPadCursor ); 
  scratchPadCursor += tlen;
  scratchPadRegister[scratchPadRegisterCursor++] = v;
  return v;
}

void cleanScratchPad() {
    memset(scratchPad, 0x00, SCRATCHPAD_SIZE);
    for(int i=0; i < SCRATCHPAD_REGISTER_SIZE; i++) {
        nullifyVal(scratchPadRegister[i]);
    }
    scratchPadCursor = 0;
    scratchPadRegisterCursor = 0;
}

int findInRegister(xvalue v) {
    for(int i=0; i < SCRATCHPAD_REGISTER_SIZE; i++) {
        if ( scratchPadRegister[i].type == TYPE_NULL ) {
            continue;
        }
        if ( scratchPadRegister[i].value[0] == v.value[0] ) {
            if ( scratchPadRegister[i].value[1] == v.value[1] ) {
                return i;
            }
        }
    }
    return -1;
}

// public version
int findInScrathPadRegister(xvalue v) {
    return findInRegister(v);
}


char* getScratchPad(xvalue v) {
    int a0 = v.value[0];
    int a1 = v.value[1];
    int l0 = v.value[2];
    int l1 = v.value[3];

    int addr = (256*a0)+a1;
    int len  = (256*l0)+l1;

    return getScratchPad(addr, len);
}

void removeFromScratchPad(xvalue v) {
    // TODO : clean theregister
    int idx = findInRegister(v);
    if ( idx < 0 ) {
        return;
    }

    int a0 = scratchPadRegister[idx].value[0];
    int a1 = scratchPadRegister[idx].value[1];
    int l0 = scratchPadRegister[idx].value[2];
    int l1 = scratchPadRegister[idx].value[3];

    int addr = (256*a0)+a1;
    int len  = (256*l0)+l1;

    for(int i=addr; i < addr+len; i++ ) {
        // scratchPad[i] = 0x00;
        scratchPad[i] = '?';
    }

    bool needDefrag = true;
    if ( idx == scratchPadRegisterCursor-1 ) {
        scratchPadRegisterCursor--;
        scratchPadCursor -= len;
        needDefrag = false;
    }

    nullifyVal( scratchPadRegister[idx] );

    if (needDefrag) { defragScratchPad(); }
}

void defragScratchPad() {
  DBUG("Defraging scratchPad");
  // TODO : foreach register that is NULL....
  DBUG("Defraged scratchPad");
}

char* getScratchPad(int addr, int len) {
    if ( len < 0 ) {
      return &scratchPad[addr];
    } else {
        // DBUG_LOCKED = false;
        // DBUG("GetScratchPad @");
        // DBUG(addr);
        // DBUG(len);

        // char str[len+1]; // else result is "(null)"
        char* str = (char*)malloc( len+1 );
        memcpy( str, &scratchPad[addr], len );
        str[len] = 0x00;
        return str;
    }
}

// ======================== Lexem Stack ==================

#define LEXEMS_SIZE 5000
xlexem lexems[LEXEMS_SIZE];
int lexemsCursor = 0;
int lexemsNum = 0;

bool pushLex(xlexem x) {
    if ( lexemsCursor >= LEXEMS_SIZE ) {
        warning("Overflow for lexems");
        return false;
    }

    lexems[lexemsNum++] = x;
    lexemsCursor++;

    return true;
}

bool cleanLex() {
    lexemsCursor = 0;
    lexemsNum = 0;

    return true;
}

xlexem getNextLex() { return lexems[ lexemsNum++ ]; }
void resetNextLex() { lexemsNum = 0; }



