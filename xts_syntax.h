#ifndef _XTS_SYNTAX_H_
#define _XTS_SYNTAX_H_ 1

 #include "xts_typedef.h"
 #include "xts_io.h"
 #include "xts_memseg.h"

  enum STATEMENT_LEX {
      COMMA, // ,
      LPARENT, // (
      RPARENT, // )
      LBRACE, // {
      RBRACE, // }
      SEMICOLON, // ;
      DOT // .
  };

  enum VAR_LEX {
      DOLLAR, // $ String
      SHARP, // # pin
      LBRACKET, // [ array
      RBRACKET, // ] array
      ARROWBASE // @ array/string length
  };

  enum TK_TYPE {
     TK_UNDEF,
     TK_ONE_CHAR,   // >
     TK_TWO_CHAR,   // >=
     TK_INT,        // 15
     TK_FLT,        // 15.36
     TK_STR,        // "Hello World"
     TK_STATEMENT,  // if
     TK_CONSTANT,   // true
     TK_CMD,        // print
     TK_FCT,        // abs(3)
     // ===================================
     TK_CHANEL,     // #1
     TK_NUMVAR,     // abc
     TK_STRVAR,     // abc$
     TK_ARRAY,      // abc[ -or- abc$[
     TK_ARRAYLEN,   // abc@ -or- abc$@
     // ===================================
     TK_WHITESPACE, // " "....
     TK_EOF,
  };  

  static const char* TK_TYPES[] = {
      "TK_UNDEF",
      "TK_1CHAR",
      "TK_2CHAR",
      "TK_INT",
      "TK_FLT",
      "TK_STR",
      "TK_STATEMENT",
      "TK_CONSTANT",
      "TK_CMD",
      "TK_FCT",
      // ===================================
      "TK_CHANEL",
      "TK_NUMVAR",
      "TK_STRVAR",
      "TK_ARRAY",
      "TK_ARRAYLEN",
      // ===================================
      "TK_WHITESPACE",
      "TK_EOF"
  };

  // NOT a BUG : ONLY 1 % in this String
  const char lexems_onechar[] = {
      "()&|*/+-%<>=!,\"[]#@${};"
  };

  enum chars_n_ops {
   CO_LPARENT,
   CO_RPARENT,
   CO_AMP, // CO_AND is 2x CO_AMP
   CO_PIPE,
   CO_MUL,
   CO_DIV,
   CO_PLUS,
   CO_MINUS,
   CO_MOD,
   CO_LT,
   CO_GT,
   CO_EQ,
   CO_NOT,
   CO_COMMA,
   CO_DBLQUOTE,
   CO_LBRACKET,
   CO_RBRACKET,
   CO_SHARP,
   CO_AROBASE,
   CO_DOLLAR,
   CO_LBRACE,
   CO_RBRACE,
   CO_SEMICOLON,
   // ====================
   CO_AND, // &&
   CO_OR, // ||
   CO_PLUSEQ,
   CO_MINUSEQ,
   CO_MULEQ,
   CO_DIVEQ,
   CO_MODEQ,
   CO_NOTEQ,
   CO_LTEQ,
   CO_GTEQ,
   CO_EQEQ // ==
};

  const char lexems_onecharCanBeDouble[] = {
      "&|*/+-%%<>=!" // ex. '&&' or '>='
  };

  const char lexems_nums[] = {
      "0123456789."
  };

  const char lexems_identifiers[] = {
      "abcdefghijklmnopqrstuvwxyz0123456789_$"
  };

  const char lexems_whitespaces[] = {
      " \t\r\n"
  };

  // MUST BE SORTED
  const char lexems_statements[] = {
      "break\0continue\0else\0function\0if\0return\0while\0"
  };

  enum statements {
      STMT_BREAK,
      STMT_CONTINUE,
      STMT_ELSE,
      STMT_FUNCTION,
      STMT_IF,
      STMT_RETURN,
      STMT_WHILE
  };

  // MUST BE SORTED
  const char lexems_constants[] = {
    //   "true\0false\0null\0HIGH\0LOW\0OUTPUT\0INPUT\0PULLUP\0"
      "false\0null\0true\0"
  };

  enum constants {
    KST_FALSE,
    KST_NULL,
    KST_TRUE
  };

  // MUST BE SORTED
  const char lexems_cmds[] = {
      "anaread\0beep\0cls\0digread\0digwrite\0gc\0mem\0pinmode\0print\0"
  };

  enum cmds {
    CMD_ANAREAD,
    CMD_BEEP,
    CMD_CLS,
    CMD_DIGREAD,
    CMD_DIGWRITE,
    CMD_GC,
    CMD_MEM,
    CMD_PINMODE,
    CMD_PRINT
  };

  // MUST BE SORTED
  const char lexems_fcts[] = {
      "abs\0max\0min\0"
  };

  enum fcts {
    FCT_ABS,
    FCT_MAX,
    FCT_MIN,
  };

  xlexem tokenize(char* &expr);
  
  // TODO : check if keep public or not ....
  int readInteger(char* expr, int &readed);

#endif