#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <typeinfo>

#include "error.h"
#include "lexical.h"
#include "parser.h"
#include "semantic.h"
#include "symbols_tables.h"
#include "abstract_syntax_tree.h"
#include "visitors.h"
#include "visitor_translateAsaIR.h"
#include "visitor_fragment.h"
#include "visitor_ir.h"
#include "mips.h"


int main(int argc, char** argv) {

    //Captação do arquivo fonte
    FILE* file = NULL;
    char* name_of_file = NULL;
    char* mips_file = NULL;
    fprintf(stdout,"----------------------------BoCeJo compiler---------------------------\n");
    if (argc > 1){
        // copiamos o valor do argumento para um enderenco de memoria de nosso dominio
        name_of_file = (char*)malloc(sizeof(char) * strlen(argv[1]));
        strcpy(name_of_file,argv[1]);
        const char ext[]  = ".jmm";
        if( strstr(name_of_file,ext) == NULL ){

            realloc(name_of_file,(strlen(name_of_file) + strlen(ext)) * sizeof(char) );
            strcat(name_of_file,".jmm"); //Concatena o nome do arquivo com a extensão
        }
        file = fopen(name_of_file,"r");
        free(name_of_file);
        if(file == NULL){
            lexical_error(E_FILE_NOT_FOUND,0);
            return EXIT_FAILURE;
        }

        if(argc > 2 ){ // existe um segundo parâmetro
            mips_file = (char*)malloc(sizeof(char) * strlen(argv[2]));
            strcpy(mips_file,argv[2]);
        }else{
            // caso não tenha parâmetro usa-se o mesmo nome do arquivo jmm
            mips_file = (char*)malloc(sizeof(char) * strlen(argv[1]));
            strcpy(mips_file,argv[1]);
        }
        const char ext_asm[] = ".ams";
        if( strstr(mips_file,ext_asm) == NULL ){
            realloc(mips_file,(strlen(mips_file) + strlen(ext_asm)) * sizeof(char) );
            strcat(mips_file,".asm");
        }
    }
    else{
        //Se o usuário não digitar nenhum nome como argumento a entrada padrão é a stdin
        fprintf(stdout,"Digite o codigo fonte a ser compilado \n");
        file = stdin;
        mips_file = "bocejo.asm";
    }
    lex_constructor(file);
    parser_constructor();
    node_program* ast = g_program();

    if(PRINT_FLUX){
        printf("\n\nASA\n");
        Visitor_print_AST* vist = new Visitor_print_AST();
        vist->visit(ast);
    }


    semantic_constructor();
    Visitor_semantic* visit_semantic = new Visitor_semantic();
    visit_semantic->visit(ast);

    Visitor_Translate_IR* visitTranslateRI = new Visitor_Translate_IR();
    visitTranslateRI->visit(ast);

    Visitor_print_fragment* visit_fragments = new Visitor_print_fragment();
    Procedure* list = (Procedure*)visit_fragments->visit((Procedure* )visitTranslateRI->getFirstFragment());
    mips_constructor(mips_file,(Procedure*)visitTranslateRI->getFirstFragment(),visitTranslateRI->label_main);
    mips_destructor();
    semantic_destructor();
    parser_destructor();
    lex_destructor();
    return (EXIT_SUCCESS);
}
