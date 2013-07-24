/**
* error.c
* @author Jonatas Castro
* @date   3 de abril de 2011
*
* Arquivo que implementa o gerenciador de erros
*
**/
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "error.h"
#include "lexical.h"

 static char* lexeme_tokens[] = {"T_EOF", "'class'", "T_ID", "'extends'", "'public'", "'static'", "'void'", "'int'","'float'","'boolean'","'if'",
                        "'('","')'","'while'","'System'","out","'in'","'println'","read","return","'{'","'}'",
                        "'new'","T_NUM_REAL","T_NUM_INT","'literal'","'length'","'['","']'","true","'false'","'this'","'-'",
                        "'!'","T_PLUS","'=='","'<'","'<='","'>'","'>='","'!='","'|'", "'*'","'/'","'%'",
                        "'&'","'and'","'||'","'.'","','","';'","'='","'else'","'protected'","'private'"
                        };

static int stop_bocejo = 0; // caso pelo menos erro sintatico ocorra, nao faz sentido seguir para a analise semantica

int is_ok(){
    return !stop_bocejo;
}

/*
* Metodo que gerencia erros
*/
void lexical_error(int type_error, int line){
    switch(type_error){
        case E_FILE_NOT_FOUND: fprintf(stderr,"*** Lexical error: File not found"); break;
        case E_FILE_ASM_NOT_FOUND: fprintf(stderr,"*** Lexical error: File .asm not found"); break;
        case E_EOF_IN_LITERAL:fprintf(stderr,"*** Lexical error in line %d: EOF in literal\n",line); break;
        case E_INVALID_CHAR: fprintf(stderr,"*** Lexical error in line %d: Invalid entered char\n",line);break;
        case E_FLOAT_END_SIGNAL:fprintf(stderr,"*** Lexical error in line %d: Float cannot end with signal\n",line);break;
        case E_EOF_IN_COMMENT:fprintf(stderr,"*** Lexical error in line %d: EOF in comment\n",line); break;
    }
}

void syntax_error(int token_found, int token_expected,int line){
    syntax_error(lexeme_tokens[token_found - T_EOF],lexeme_tokens[token_expected - T_EOF],line);
}

void syntax_error(int token_found, const char* token_expected,int line){
    fprintf(stderr,"*** Syntax error in line %d: Expected %s before %s token \n",line,token_expected,lexeme_tokens[token_found - T_EOF]);
    stop_bocejo = 1;
}

void syntax_error(const char* token_found, const char* token_expected,int line){
    fprintf(stderr,"*** Syntax error in line %d: Expected %s before %s token \n",line,token_expected,token_found);
    stop_bocejo = 1;
}

// usado principalmente para expressoes

void semantic_error(int type_error, int line,const char* info_1,const char* info_2){
    switch(type_error){
        case E_FATAL_ERROR: fprintf(stderr,"**** Erro fatal: Abortou o Bocejo na análise sintática, vai perder 3 pontos! rs \n ");   break;
        case E_INVALID_ASSIGN: fprintf(stderr,"*** Semantic error in line %d: invalid assign between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_ADD: fprintf(stderr,"*** Semantic error in line %d: invalid addition between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_SUB: fprintf(stderr,"*** Semantic error in line %d: invalid subtration between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_DIV: fprintf(stderr,"*** Semantic error in line %d: invalid division between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_MULT: fprintf(stderr,"*** Semantic error in line %d: invalid multiplication between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_AND: fprintf(stderr,"*** Semantic error in line %d: invalid and operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_AND_UNI: fprintf(stderr,"*** Semantic error in line %d: invalid and unary operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_OR:       fprintf(stderr,"*** Semantic error in line %d: invalid or operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_OR_UNI:       fprintf(stderr,"*** Semantic error in line %d: invalid or unary operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_LESS_E:   fprintf(stderr,"*** Semantic error in line %d: invalid less equal operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_LESS_T:   fprintf(stderr,"*** Semantic error in line %d: invalid less then operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_GREATER_E:fprintf(stderr,"*** Semantic error in line %d: invalid greater equal operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_GREATER_T:fprintf(stderr,"*** Semantic error in line %d: invalid greater then operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_EQUAL:    fprintf(stderr,"*** Semantic error in line %d: invalid equal operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_NOT_EQUAL:fprintf(stderr,"*** Semantic error in line %d: invalid not equal operation between type '%s' and '%s' \n",line, info_1, info_2);   break;
        case E_UNDEFINED_ATTR:   fprintf(stderr,"*** Semantic error in line %d: attribute '%s' not found in class '%s' \n"); break;
        case E_ATTR_IN_PRIMITIVE:fprintf(stderr,"*** Semantic error in line %d: attribute '%s' not found in primitive type '%s' \n",line, info_1, info_2);   break;
        case E_CALL_IN_PRIMITIVE:fprintf(stderr,"*** Semantic error in line %d: method '%s' not found in primitive type '%s' \n",line, info_1, info_2);break;
        case E_UNDECLARED_CALL:  fprintf(stderr,"*** Semantic error in line %d: method '%s' not found in class '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_PARAM:  fprintf(stderr,"*** Semantic error in line %d: found parammeter type '%s' and expected '%s' in method \n",line, info_1, info_2);   break;break;
        case E_EXPECTED_PARAM: fprintf(stderr,"*** Semantic error in line %d: expected parammeter '%s' type in method '%s' \n",line, info_1, info_2);   break;
        case E_REDECLARED_PARAM: fprintf(stderr,"*** Semantic error in line %d: parammeter '%s' redeclared in method '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_RETURN: fprintf(stderr,"*** Semantic error in line %d: return invalid expected type '%s' and found '%s' \n",line, info_1, info_2);   break;
        case E_INVALID_EXPRESSION_IF: fprintf(stderr,"*** Semantic error in line %d: not valid expression in if statement\n "); break;
        case E_INVALID_EXPRESSION_WHILE: fprintf(stderr,"*** Semantic error in line %d: not valid expression in while statement\n "); break;
        case E_UNDECLARED_VARIABLE:  fprintf(stderr,"*** Semantic error in line %d: variable '%s' was not declared in this scope\n", line, info_1); break;
        case E_REDECLARED_VARIABLE:  fprintf(stderr,"*** Semantic error in line %d: variable '%s' redeclared in this scope \n", line, info_1); break;
        case E_UNDECLARED_CLASS:     fprintf(stderr,"*** Semantic error in line %d: class '%s' was not declared\n", line, info_1);    break;
        case E_DIV_BY_ZERO:     fprintf(stderr,"*** Semantic error in line %d: division by zero invalid \n", line, info_1);    break;
        case E_OBJECT_NOT_INSTANCE:     fprintf(stderr,"*** Semantic error in line %d: variable '%s' not instance, use new operator \n", line, info_1);    break;
        case E_INVALID_LENGTH:     fprintf(stderr,"*** Semantic error in line %d: invalid attribute length in %s '%s' \n", line, info_1,info_2);    break;
        case E_UNEXPECTED_PARAM:     fprintf(stderr,"*** Semantic error in line %d: unexpected parammeter %s in method '%s' \n", line, info_1,info_2);    break;
        case E_NOT_FOUND_RETURN:     fprintf(stderr,"*** Semantic error in line %d: missing return statemant in method '%s' \n", line, info_1);    break;
        case E_INVALID_INHERITANCE:     fprintf(stderr,"*** Semantic error in line %d: invalid inheritance between '%s' and '%s' \n", line, info_1,info_1);    break;
        case E_INVALID_SIGNATURE:     fprintf(stderr,"*** Semantic error in line %d: method '%s' redeclared \n", line, info_1);    break;
        case E_METH_NOT_FOUND:     fprintf(stderr,"*** Semantic error in line %d: that method '%s' not found \n", line, info_1);    break;
        case E_DUPLICATE_MAIN:     fprintf(stderr,"*** Semantic error in line %d: duplicate method static void \n", line);    break;
        case E_INVALID_ASSIGN_ARRAY:     fprintf(stderr,"*** Semantic error in line %d: invalid assing envolving a array \n", line);    break;
        case E_INVALID_NEW:     fprintf(stderr,"*** Semantic error in line %d: invalid new operator between '%s' and '%s' \n", line,info_1,info_2);    break;
        case E_INVALID_NEW_ARRAY:     fprintf(stderr,"*** Semantic error in line %d: invalid new array, '%s' not a bound valid  \n", line,info_1);    break;
        case E_INVALID_SIGNAL:     fprintf(stderr,"*** Semantic error in line %d: invalid signal envolving a %s  \n", line,info_1);    break;
        case E_INVALID_NEGATION:     fprintf(stderr,"*** Semantic error in line %d: invalid negation envolving a %s  \n", line,info_1);    break;
        case E_INVALID_PRINTLN:     fprintf(stderr,"*** Semantic error in line %d: invalid println envolving a %s  \n", line,info_1);    break;
        case E_CALL_IN_ASSIGN:     fprintf(stderr,"*** Semantic error in line %d: invalid call as left value  \n", line,info_1);    break;
        case E_ASSIGN_ATTR:     fprintf(stderr,"*** Semantic error in line %d: invalid assign emvolving name '%s' as right value  \n", line,info_1);    break;
        case E_INVALID_OFFSET_ARRAY:     fprintf(stderr,"*** Semantic error in line %d: invalid offset type '%s' to array '%s'  \n", line,info_2,info_1);    break;
        case E_ACCESS_PRIVATE:     fprintf(stderr,"*** Semantic error in line %d: Method '%s' is private \n", line,info_1);    break;
        case E_ACCESS_PROTECTED:     fprintf(stderr,"*** Semantic error in line %d: Method '%s' is protected \n", line,info_1);    break;
        case E_ATTR_IN_MAIN:     fprintf(stderr,"*** Semantic error: Class Main can't have attributes \n", line,info_1);    break;
        case E_TOSTRING_REDECLARED:     fprintf(stderr,"*** Semantic error in line: 'toString' is a implicit method and was redeclared \n", line);    break;
        default: fprintf(stderr,"*** erro desconhecido ") ;
    }
    exit(EXIT_FAILURE);
}

void semantic_error(int type_error,int line,const char* info){
    semantic_error(type_error,line,info,"");
}

void semantic_error(int type_error,int line){
    semantic_error(type_error,line,"");
}

