#include "xts_syntax.h"

  int indexOf(char* chars, char ch) {
      int tlen = strlen(chars);
      for(int i=0; i < tlen; i++) {
          if ( chars[i] == ch ) {
              return i;
          }
      }
      return -1;
  }

// find id in PROGMEM wordlist.  result in symval, return true if found.
int _findindex(char *id, const char *wordlist, bool sorted) {
	int symval = 0;
	while ( (wordlist)) {
		int result = strcmp(id, wordlist);
		if (!result) return symval;
		else if (sorted && (result < 0)) break;	// only works if list is sorted!
		else {
			symval++;
			wordlist += strlen(wordlist) + 1;
		}
	}
	return -1;
}



  int indexOfToken(const char* wordlist, char* expr) {
      return _findindex(expr, wordlist, true);
  }

  int getOneOrMoreLen(const char* chars, char* expr) {
      int tlen = 0;
      while ( indexOf( (char*)chars, *expr ) > -1 ) {
          tlen++;
          expr++;
      }
      return tlen;
  }


// get the address of the nth message in the table
const char *getTokenStr(const char* wordlist, uint8_t id) {
    const char *msg = wordlist;
	while (id) { msg += strlen(msg) + 1; id--; }
	return msg;
}

// assumes that expr[0] is ' " '
int readString(char* expr) {
  if ( *expr != '"' ) { return 0; }
  expr++;
  int slen=0;
  while( *expr != '"' ) {
      if ( *expr == 0x00 ) {
          return -1;
      }
      expr++;
      slen++;
  }
  return slen;
}

// CERTIFIED CODE
// doesn't alter expr pointer position
// returns the int value & updates 'readed' char counter
// "0234" => 234 w/ len: 4
int readInteger(char* expr, int &readed) {
    if ( *expr == 0x00 ) { return -1; }
    if ( *expr < '0' || *expr > '9' ) {
        return -1;
    }
    char str[12+1];
    memset( str, 0x00, 12+1 );
    str[ readed++ ] = *expr;
    expr++;

    // so 0x00 will exit too ...
    while ( *expr >= '0' && *expr <= '9' ) {
        str[ readed++ ] = *expr;
        expr++;
    }

    int result = atoi( str );
    return result;
}

  xlexem tokenize(char* &expr) {
      bool dbugState = DBUG_LOCKED;
      DBUG_LOCKED = true;

      DBUG("tokenize .1");
      if ( expr == NULL ) { return lnull; }
      char exprCh = *expr;
      xlexem result;

      int idx = -1;
      int tlen = 0;

      if ( exprCh == 0x00 ) {
          result.type = TK_EOF;
          result.idx = 0;
          result.len = 0;

          DBUG_LOCKED = dbugState;
          return result;
      }

      // IS a WhiteSpace ?
      idx = indexOf( (char*)lexems_whitespaces, exprCh );
DBUG("tokenize .22");
      if ( idx > -1 ) {
        // numerical expr.
        result.type = TK_WHITESPACE;
        result.idx = idx;
        result.token[0] = lexems_whitespaces[idx];
        result.token[1] = 0x00;
        tlen++;
DBUG("tokenize .23");
        while ( (idx = indexOf( (char*)lexems_whitespaces, *(expr+tlen) )) > -1 ) {
            result.token[tlen] = lexems_whitespaces[idx];
            result.token[tlen+1] = 0x00;
            tlen++;
        }
        result.len = tlen;
      } else {
    DBUG("tokenize .2");
        // is a SPE ONE CHAR ?
        idx = indexOf( (char*)lexems_onechar, exprCh );
    DBUG("tokenize .3");
        if ( idx > -1 ) {
    DBUG("tokenize .4");
            int idx2 = indexOf( (char*)lexems_onecharCanBeDouble, exprCh );
            DBUG("tokenize .5");
            if ( idx2 > -1 ) {
                DBUG("tokenize .6");
                if ( *( expr+1 ) != 0x00 ) {
                    char exprCh2 = *( expr+1 );
                    DBUG("tokenize .7");
                    int idx3 = indexOf( (char*)lexems_onecharCanBeDouble, exprCh2 );

                    int idx1stChar = indexOf( (char*)lexems_onechar, exprCh );
                    int idx2ndChar = indexOf( (char*)lexems_onechar, exprCh2 );
                    int dblCharIdx = -1;

                    if ( idx1stChar == CO_EQ && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_EQEQ;
                    } else if ( idx1stChar == CO_LT && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_LTEQ;
                    } else if ( idx1stChar == CO_GT && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_GTEQ;
                    } else if ( idx1stChar == CO_NOT && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_NOTEQ;
                    } else if ( idx1stChar == CO_MOD && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_MODEQ;
                    } else if ( idx1stChar == CO_MUL && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_MULEQ;
                    } else if ( idx1stChar == CO_DIV && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_DIVEQ;
                    } else if ( idx1stChar == CO_PLUS && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_PLUSEQ;
                    } else if ( idx1stChar == CO_MINUS && idx2ndChar == CO_EQ ) {
                        dblCharIdx = CO_MINUSEQ;
                    } else if ( idx1stChar == CO_AMP && idx2ndChar == CO_AMP ) {
                        dblCharIdx = CO_AND;
                    } else if ( idx1stChar == CO_PIPE && idx2ndChar == CO_PIPE ) {
                        dblCharIdx = CO_OR;
                    } else {
                        dblCharIdx = -1;
                    }


    DBUG("tokenize .8");
                    result.type = TK_TWO_CHAR;
                    result.idx = dblCharIdx;
                    // result.token[0] = lexems_onecharCanBeDouble[idx2];
                    // result.token[1] = lexems_onecharCanBeDouble[idx3];
                    result.token[0] = lexems_onechar[idx1stChar];
                    result.token[1] = lexems_onechar[idx2ndChar];
                    result.token[2] = 0x00;

                    tlen = 2;
                    result.len = tlen;
                }
            }
    DBUG("tokenize .9");
            if ( result.type == 0 ) {
                DBUG("tokenize .10");

                // IS a String
                if ( idx == CO_DBLQUOTE ) {
                    int slen = readString( expr );
                    if ( slen < 0 ) {
                        DBUG_LOCKED = false;
                        DBUG("Unbalanced String");
                        result.type = TK_UNDEF;
                        return result;
                    }

                    result.type = TK_STR;
                    result.len = slen;
                    memcpy(result.token, (expr+1), max(slen, MAX_TOKEN_REPR_SIZE) );
                    if ( slen < MAX_TOKEN_REPR_SIZE ) {
                        result.token[slen] = 0x00;
                    }

                    char _strv[ slen+1 ];
                    memcpy(_strv, (expr+1), slen );
                    _strv[slen] = 0x00;
                    xvalue _str = str( _strv );
                    // result.idx = getScratchPadRegisterLen()-1; // BEWARE : not ThreadSafe
                    result.idx = findInScrathPadRegister(_str);

                    tlen = 1+slen+1;
                } else if ( idx == CO_SHARP ) {
                    // #26 -> Chanel descriptor [0..255]
                    int nlen = 0;
                    int nval = readInteger( expr+1, nlen );
                    if ( nlen < 0 || nval < 0 || nval > 255 ) {
                        DBUG_LOCKED = false;
                        DBUG("Wrong chanel descriptor");
                        result.type = TK_UNDEF;
                        return result;
                    }
                    result.type = TK_CHANEL;
                    result.len = nlen;
                    // not necessary ....
                    memcpy(result.token, (expr+1), max(nlen, MAX_TOKEN_REPR_SIZE) );
                    if ( nlen < MAX_TOKEN_REPR_SIZE ) {
                        result.token[nlen] = 0x00;
                    }
                    // ....
                    result.idx = nval;

                    tlen = 1 + nlen;
                } else {
                    result.type = TK_ONE_CHAR;
                    result.idx = idx;
                    result.token[0] = lexems_onechar[idx];
                    result.token[1] = 0x00;

                    tlen = 1;
                    result.len = tlen;
                }
            }
    DBUG("tokenize .11");
        } else {
            // IS a numerical expr ?
            idx = indexOf( (char*)lexems_nums, exprCh );
    DBUG("tokenize .12");
            if ( idx > -1 ) {
                // numerical expr.
                result.type = TK_INT;
                result.idx = idx;
                result.token[0] = lexems_nums[idx];
                result.token[1] = 0x00;
                tlen++;
    DBUG("tokenize .13");
                while ( (idx = indexOf( (char*)lexems_nums, *(expr+tlen) )) > -1 ) {
                    // DBUG( (char)lexems_nums[idx] );
                    if ( lexems_nums[idx] == '.' ) {
                        result.type = TK_FLT;
                        // DBUG("ITS A FLOAT");
                    }
                    result.token[tlen] = lexems_nums[idx];
                    result.token[tlen+1] = 0x00;
                    tlen++;
                }

                // HERE : store int or float value
                // in Token char array

                if ( result.type == TK_INT ) {
                    int t = atoi( result.token );
                    memset( result.token, 0x00, MAX_TOKEN_REPR_SIZE );
                    // memcpy(result.token, &t, 4);
                    result.token[0] = (t >> 24) & 0xFF;
                    result.token[1] = (t >> 16) & 0xFF;
                    result.token[2] = (t >> 8) & 0xFF;
                    result.token[3] = t & 0xFF;
                } else {
                    float t = atof( result.token );
                    memset( result.token, 0x00, MAX_TOKEN_REPR_SIZE );
                    memcpy(result.token, &t, 4);
                }

                result.len = tlen;
            } else {
                // IS an Identifier ?
                // DBUG_LOCKED = false;

                int slen = getOneOrMoreLen(lexems_identifiers, expr);
                // DBUG("FOUND AN IDENTIFIER");
                // DBUG(slen);

                if ( slen > 0 ) {
                    // we have an IDENTIFIER !
                    DBUG("FOUND AN IDENTIFIER");

                    char identifier[ slen+1 ];
                    memset( identifier, 0x00, slen+1 ); // was a bug
                    memcpy( identifier, expr, slen );
                    DBUG( identifier );
                    DBUG( slen );

                    // IS a Statement control token ?
                    idx = indexOfToken( lexems_statements, identifier );
                    if ( idx > -1 ) {
                        DBUG( "found a Statement token" );
                        // DBUG( getTokenStr( lexems_statements, idx ) );
                        result.type = TK_STATEMENT;
                        result.idx = idx;
                        result.len = slen;
                        memcpy(result.token, identifier, max(slen, MAX_TOKEN_REPR_SIZE) );
                        if ( slen < MAX_TOKEN_REPR_SIZE ) {
                          result.token[slen] = 0x00;
                        }
                        tlen = slen;
                    } else {
                        // IS a Constant token ?
                        idx = indexOfToken( lexems_constants, identifier );
                        if ( idx > -1 ) {
                            DBUG( "found a Constant token" );
                            result.type = TK_CONSTANT;
                            result.idx = idx;
                            result.len = slen;
                            memcpy(result.token, identifier, max(slen, MAX_TOKEN_REPR_SIZE) );
                            if ( slen < MAX_TOKEN_REPR_SIZE ) {
                                result.token[slen] = 0x00;
                            }
                            tlen = slen;
                        } else {
                            // IS a Command token ?
                            idx = indexOfToken( lexems_cmds, identifier );
                            if ( idx > -1 ) {
                                DBUG( "found a Command token" );
                                result.type = TK_CMD;
                                result.idx = idx;
                                result.len = slen;
                                memcpy(result.token, identifier, max(slen, MAX_TOKEN_REPR_SIZE) );
                                if ( slen < MAX_TOKEN_REPR_SIZE ) {
                                    result.token[slen] = 0x00;
                                }
                                tlen = slen;
                            } else {
                                // IS a Function token ?
                                idx = indexOfToken( lexems_fcts, identifier );
                                if ( idx > -1 ) {
                                    DBUG( "found a Function token" );
                                    result.type = TK_FCT;
                                    result.idx = idx;
                                    result.len = slen;
                                    memcpy(result.token, identifier, max(slen, MAX_TOKEN_REPR_SIZE) );
                                    if ( slen < MAX_TOKEN_REPR_SIZE ) {
                                        result.token[slen] = 0x00;
                                    }
                                    tlen = slen;
                                } else {
									// it's an unknown identifier
									// may be a variable name ...
									
									slen = strlen(identifier);
									
									if ( slen < 1 ) {
										result.type = TK_UNDEF;
										return result; // BEWARE w/ that eject code
									}
									
									char lastChar = *(expr+slen-1);
									char nextChar = *(expr+slen);
									
									if ( nextChar == '[' ) {
										// array
										printf("found an array cell identifier >%s< (%c) \n", identifier, nextChar );
									} else if ( nextChar == '@' ) {
										// array len
										printf("found an array length identifier >%s< (%c) \n", identifier, nextChar );
									}
									
									printf("found an unknown identifier >%s< (%c) \n", identifier, nextChar );
									
									// tmp code : have to specify varType
									result.type = lastChar == '$' ? TK_STRVAR : TK_NUMVAR;
                                    result.idx = 0;
                                    result.len = slen;
                                    
                                    memcpy(result.token, identifier, max(slen, MAX_TOKEN_REPR_SIZE) );
                                    if ( slen < MAX_TOKEN_REPR_SIZE ) {
                                        result.token[slen] = 0x00;
                                    }
                                    tlen = slen;
								}	
                            }
                        }
                    }

                }

                
            }
        }
      }
DBUG("tokenize .14");
      expr += tlen;
DBUG("tokenize .15");
      DBUG_LOCKED = dbugState;
      return result;
  }


