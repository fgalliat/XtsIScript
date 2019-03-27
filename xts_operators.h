#ifndef _XTS_OPERATOR_H_
#define _XTS_OPERATOR_H_ 1

  enum UNARY_OPERATOR {
      PLUS_PLUS,
      MINUS_MINUS
  };

  enum ASSIGN_OPERATOR {
      PLUS_EQ,
      MINUS_EQ,
      MUL_EQ,
      DIV_EQ,
      MOD_EQ,

      ASSIGN_EQ
  };

  enum BINARY_OPERATOR {
      PLUS,
      MINUS,
      MUL,
      DIV,
      MOD
  };

  enum BOOL_OPERATOR {
      EQ_EQ,
      NOT_EQ,
      GT,
      LT,
      GT_EQ,
      LT_EQ
  };

  enum BOOL_LINK_OPERATOR {
      AND,
      OR,
      XOR,
      NOT
  };


#endif