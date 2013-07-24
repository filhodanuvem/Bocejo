#ifndef ERROR_H
#define ERROR_H

#define E_FILE_NOT_FOUND 11
// erros léxicos
#define E_EOF_IN_LITERAL 12
#define E_INVALID_CHAR 50
#define E_FLOAT_END_SIGNAL 52
#define E_EOF_IN_COMMENT 53
#define E_BLOCK_END_EXPECTED 60
// erros semanticos
#define E_INVALID_ASSIGN 61
#define E_UNDECLARED_VARIABLE 62
#define E_UNDECLARED_CLASS 63
#define E_INVALID_ADD 64
#define E_INVALID_SUB 65
#define E_INVALID_MULT 66
#define E_INVALID_DIV 67
#define E_INVALID_AND 68
#define E_INVALID_OR 69
#define E_UNDEFINED_ATTR 70
#define E_REDECLARED_VARIABLE 71
#define E_INVALID_NEW 72
#define E_INVALID_EXPRESSION_IF 73
#define E_INVALID_EXPRESSION_WHILE 74
#define E_DIV_BY_ZERO 75
#define E_OBJECT_NOT_INSTANCE 76
#define E_INVALID_LESS_T 77
#define E_INVALID_LESS_E 78
#define E_INVALID_GREATER_T 79
#define E_INVALID_GREATER_E 80
#define E_INVALID_EQUAL 81
#define E_INVALID_NOT_EQUAL 82
#define E_ATTR_IN_PRIMITIVE 83
#define E_CALL_IN_PRIMITIVE 84
#define E_UNDECLARED_CALL 85
#define E_INVALID_LENGTH 86
#define E_INVALID_PARAM 87
#define E_EXPECTED_PARAM 88
#define E_REDECLARED_PARAM 89
#define E_INVALID_RETURN 90
#define E_UNEXPECTED_PARAM 91
#define E_NOT_FOUND_RETURN 92
#define E_INVALID_INHERITANCE 93
#define E_INVALID_SIGNATURE 94
#define E_METH_NOT_FOUND 95
#define E_DUPLICATE_MAIN 96
#define E_INVALID_ASSIGN_ARRAY 97
#define E_INVALID_NEW_ARRAY 98
#define E_INVALID_NEGATION 99
#define E_INVALID_SIGNAL 100
#define E_INVALID_PRINTLN 101
#define E_CALL_IN_ASSIGN 102
#define E_ASSIGN_ATTR 103
#define E_INVALID_OFFSET_ARRAY 104
#define E_INVALID_AND_UNI 105
#define E_INVALID_OR_UNI 106
#define E_ACCESS_PRIVATE 107
#define E_ACCESS_PROTECTED 108
#define E_ATTR_IN_MAIN 109
#define E_TOSTRING_REDECLARED 110

// erro na geração de código mips
#define E_FILE_ASM_NOT_FOUND 200

// erro fatal
#define E_FATAL_ERROR 999


int is_ok();
void lexical_error(int type_error, int line);
void syntax_error(int token_found, int token_expected,int line);
void syntax_error(int token_found, const char* token_expected,int line);
void syntax_error(const char* token_found, const char* token_expected,int line);
void semantic_error(int type_error,int line);
void semantic_error(int type_error,int line,const char* info);
void semantic_error(int type_error, int line,const char* info_1,const char* info_2);
#endif // ERROR_H_INCLUDED
