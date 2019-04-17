#include "xts_typedef.h"
#include "xts_syntax.h"
#include "xts_io.h"
#include "xts_memseg.h"

#include "xts_memory.h"

// =====================================

// Unit Tests
void TU_testReadInt();
void TU_heap();

// Routines
void tokenizeExpr(char* expr);

// =====================================

int main(int argc, char** argv) {

  DBUG( "OK now" );

    xvalue num0 = num(12);
  DBUG( "OK .A" ); DBUG(num0);
  num0 = num(256); DBUG( "OK .A" ); DBUG(num0);
  num0 = num(65535); DBUG( "OK .A" ); DBUG(num0);
  num0 = num(65536); DBUG( "OK .A" ); DBUG(num0);
  //DBUG( "that gives " ); DBUG( toNum(num0) ); 
  DBUG( toFloat(num0) );

    xvalue flt0 = flt(15.53);
  DBUG( "OK .B" ); DBUG(flt0);
  //DBUG( "that gives " ); DBUG( toFloat(flt0) ); 
  DBUG( toNum(flt0) );

    xvalue str0 = str("Salut tout le monde");
  DBUG( "OK .C" ); DBUG(str0);

    xvalue str1 = str((char*)NULL);

  DBUG( "OK .D" ); DBUG(str1);

str1 = str("Coucou les gens");

DBUG(str1); DBUG( getScratchPad() );

freeVal(str1);
DBUG(str1); DBUG( getScratchPad() );

str1 = str("222");
DBUG(str1); DBUG( getScratchPad() );

xvalue str2 = str("333");
DBUG(str2); DBUG( getScratchPad() );

freeVal( str1 );
DBUG( getScratchPad() );


  DBUG( "-----------------------------" ); 

  // DONE
  // parse ints & floats (use 4 bytes of xlexem.token)
  // @ tokenize() => distinguish NUM (int) & FLT (float)
  // TEST code : readInteger() => done
  // CHANEL LITERAL parsing ( ex. #13 )
  //  #1 is a chanel descriptor (because pins can be simple int parameter in some function)
  //  -> chanels can be files, I2C, Serial, ........
  //  -> max 256 chanels -> (3 digit but 1 byte)

  // TASKS
  // finish tokenization (w/ var, pin, ?chanel? literals)
  // then start token storage / addressing (for RFU jumps)
  // ...
  // REMEMBER that vars have to be assigned before used in a statement (can have unknown_vars)
  // ...
  // then fetch bitlash syntax/priority parser

  // STILL to PARSE
  // num vars / str vars literals
  // arrays
  // 
  // LITERAL RULES
  // -> var names are 6 chars max (w/o decorators)
  // abc is a num var (flt or int)
  // abc$ is a String var
  // abc[0] is a num array -> beware because can be "abc[(i+2)%3]"
  //  -> so don't store as a single token
  // abc@ is a num array length -> must reference a known array

  // ?? keeps "abc", "abc$", "abc[", "abc@" ?? as token representation ?? 

  // In Progress : 




  // functions

  // STILL to STORE                                123456789012
  // vars / arrays (xlexem.token = "abc", "abc$", "abcdfg[999]\0")
  // prefix : lexems_identifiers[]

  // STILL to code
  // bloc addr jump : if/else , while/break/continue
  // so have to give an addr/storage for lexems 
  // operators

  // if is an identifier : but not a known word,
  // it can be a variable : if <varName> '=' <value>
  // > reference it as a KNOWN variable + assign + type
  // else if used in another expr && ! KNOWN -> undef_var_msg 

  // cmd CAN have lparent before args
  // fct MUST have lparent before args

  char* expr = (char*)">=   abs(123.56); if (true) { print(\"coucou les gens, ca va ?\"); } ";
  tokenizeExpr(expr);

  DBUG( "/////////////" );
  // chanel descriptor
  expr = (char*)"#13";
  tokenizeExpr(expr);
  
  // cmd & vars literals
  // BEWARE : "abc$" is a single token
  expr = (char*)"abs abc abc$ abc$[abc$@+1]";
  tokenizeExpr(expr);

  // TU_testReadInt();

  DBUG( "/////////////////////////////" ); 

  setupMainMemory();
  TU_heap();

  DBUG( "OK exit" ); 

  DBUG_MEM();


  return 0;
}

  // @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

  bool testAssignOneInt(char* name, int index, int value) {
    int rc = assignVar(name, index, (number)value);

    char str[128]; memset( str, 0x00, 128 );
    sprintf(str, "(TU) assign %s[%d]=%d ==> %d \t\t [%s] \n", name, index, value, getInt(name, index), assignErrorMsg[rc] );
    DBUG( (const char*)str );

    return rc == ASSIGN_NOERROR;
  }


  void TU_heap() {
    bool ok;
    
    ok = testAssignOneInt("ab", 0, 3);
    ok = testAssignOneInt("ab", 0, 257);
  }


  void TU_testReadInt() {
    DBUG( "=============================" ); 
    const char* subExpr = "0234";
    int intLen = 0;
    int intValue = 0;

    intValue = readInteger((char*)subExpr, intLen);
    char msg[64];
    sprintf(msg, "test readInteger(%s) => %d (len=%d)", subExpr, intValue, intLen );
    printf("%s\n", msg);

    if ( intValue != 234 ) {
      printf("(!!) FAILED !\n");
    } else {
      printf("(ii) SUCCESS !\n");
    }
  }

  void tokenizeExpr(char* expr) {
    DBUG( "=============================" ); 
    xlexem lex;
    int cpt = 0;

    cleanLex();
    bool hadErrror = false;

    while( ( lex = tokenize(expr) ).type != TK_EOF ) {

      if ( lex.type == TK_UNDEF ) {
        DBUG("Parsing error");
        DBUG(lex);

        hadErrror = true;
        break;
      }

      if ( lex.type != TK_WHITESPACE ) {
        DBUG(lex);

        pushLex( lex );
        cpt++;
      } else {
        // DBUG("-whitespace-");
      }

    }
    DBUG_LOCKED = false;

    if ( lex.type == TK_UNDEF ) {
      DBUG("There was an error during tokenization.");
    } else {
      cpt++; // Cf EOF

      char msg[64];
      sprintf(msg, "tokenization succeeded with %d tokens (%d bytes)", cpt, ( (int)(cpt*sizeof(xlexem)) ) );
      printf("%s\n", msg);
    }
  }

  
// -eof- 
