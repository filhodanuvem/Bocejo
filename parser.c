/**
* parser.c
* @author Jonatas Castro
* @date   20 de abril de 2011
*
* Arquivo que implementa o analisador sintático
* cada função do tipo g_ representa um não-terminal.
*
**/
#include "lexical.h"
#include "parser.h"
#include "error.h"
#include "abstract_syntax_tree.h"

static int look_ahead;
static Symbols_table_id      * table_id      = NULL;
static Symbols_table_literal * table_literal = NULL;

/*
* construtor do analisador sintatico
*/


void parser_constructor()
{
    look_ahead    = lex_next_token();
    table_id      = GET_symbol_table_id();
    table_literal = GET_symbol_table_lit();
}

/*
* destrutor do analisador sintatico
*/
void parser_destructor(){
    // criar destrutor do current_scope
}

/*
* metodo que consome o look_ahead ou gera um erro, se necessário
*/
void eat(int token){

    if(PRINT_FLUX)fprintf(stdout,"MATCH ");
    if(PRINT_FLUX) fprintf(stdout,GET_token_name(look_ahead));
    if(PRINT_FLUX) if(look_ahead == T_ID || look_ahead == T_LITERAL || look_ahead == T_NUM_INT || look_ahead == T_NUM_REAL) fprintf(stdout,".%s\n",GET_current_lexeme());
    else{
        if(PRINT_FLUX) fprintf(stdout,"\n");
        if(look_ahead == T_EOF) return ;
    }
    look_ahead = lex_next_token();
}

/*
* Função que tenta casar o token atual com o look a head ou escapa
*/
void eat_or_skip(int token, int* synch ){
    if(token == look_ahead) eat(token);
    else{
        syntax_error(look_ahead,token,GET_current_line());
        skip(synch);
    }
}

/*
* Função que sincroniza o parser caso seja digitado algo inesperado
*/
void skip( int* synch){

    while(look_ahead != T_EOF){
        for(int i=0; synch[i] != T_EOF; i++)
            if(synch[i] == look_ahead) return;
        look_ahead = lex_next_token();
    }
}

/*
* Funções que representam não terminais
*/
static int flag_eof = false; // evita imprimir match T_EOF várias vezes caso ele passe no não terminal várias vezes

node_program* g_program(){

    if(PRINT_FLUX)  fprintf(stdout,"g_program\n");
    node_class_list* class_ = g_class_decl();
    node_class_list* list = g_program_();
    if(class_ != NULL) class_->attr_class_list = list; // arquivo vazio será retornado
    if(!flag_eof && look_ahead == T_EOF) {
        flag_eof = true;
        eat(T_EOF);
    }
    return new node_program(class_);
}

node_class_list* g_program_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_program_\n");
    node_program* aux = NULL;
    if(look_ahead == T_CLASS){
        aux = g_program();
        if(aux != NULL) return aux->attr_class_list;
    }
    return NULL ;
}

static int synch_class_decl[]   = {T_CLASS,T_EOF};
static int synch_class_decl_2[] = {T_PUBLIC, T_PROTECTED,T_PRIVATE,T_INT,T_FLOAT,T_BOOLEAN,T_ID,T_BLOCK_END,T_EOF};
static int synch_class_decl_3[] = {T_EXTENDS,T_BLOCK_BEGIN,T_EOF};
static int synch_class_decl_4[] = {T_ID,T_EOF};

node_class_list* g_class_decl(){

    if(PRINT_FLUX)  fprintf(stdout,"g_class_decl\n");
    Symbol* class_name = NULL;
    node_var_list* var_list = NULL;
    node_method_list* method_list = NULL;

    int line = GET_current_line();
    eat_or_skip(T_CLASS,synch_class_decl_4);
    if(look_ahead == T_ID) class_name = table_id->search(GET_current_lexeme());
    eat_or_skip(T_ID,synch_class_decl_3);
    Symbol* class_name_father  = g_extends();
    eat_or_skip(T_BLOCK_BEGIN,synch_class_decl_2);
    g_local_decl(&var_list,&method_list);
    eat_or_skip(T_BLOCK_END,synch_class_decl);

    return new node_class_list(class_name,var_list,method_list,class_name_father,NULL,line);
}

static int synch_extends[]   = {T_BLOCK_BEGIN,T_EOF};

Symbol* g_extends()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_extends\n");
    Symbol* id_father = NULL;
    int line = GET_current_line();
    if (look_ahead == T_EXTENDS)
    {
        eat(T_EXTENDS);
        if(look_ahead == T_ID){
            id_father = table_id->search(GET_current_lexeme());
        }
        eat_or_skip(T_ID,synch_extends);
    }
    else if (look_ahead == T_ID)  //caso de esquecer o Extends
    {
        syntax_error(T_ID,T_EXTENDS,line);
        eat(T_ID);
    }
    return id_father;
}

static int synch_local_decl[] = {T_INT, T_FLOAT, T_BOOLEAN, T_ID,T_PRIVATE, T_PUBLIC, T_PROTECTED, T_BLOCK_END, T_EOF};

void g_local_decl(node_var_list** var_list,node_method_list** method_list)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_local_decl\n");
    node_type* type = NULL;
    node_name_decl* name_decl = NULL;
    switch (look_ahead){

        case T_PUBLIC:
        case T_PROTECTED:
        case T_PRIVATE:
        case T_STATIC: // caso do cara esquecer o public
            // cria ou insere o novo metodo na lista de metodos
            if((*method_list) == NULL) *method_list = g_method_decl();
            else (*method_list)->insert_list(g_method_decl());
            g_local_decl(var_list,method_list);
            break;
        case T_INT:
        case T_FLOAT:
        case T_BOOLEAN:
        case T_ID :
            type = g_type_or_id();
            // se eu passar por aqui e a lista não tiver zerada, preciso inserir nela,
            // é o caso de declarar cada variavel em uma linha diferente como em
            // Tipo var1; Tipo var2 .... isso garante que nesses casos a lista de variaveis declaradas
            // também funcione
            if(*var_list == NULL) *var_list = g_id_list(&type,var_list);
            else (*var_list)->insert_list(g_id_list(&type,NULL));
            eat_or_skip(T_SEMICOLON,synch_local_decl);
            g_local_decl(var_list,method_list);
            break;
    }
}

node_var_list* g_var_decl()
{
    node_var_list* var_list = NULL;
    if(PRINT_FLUX)  fprintf(stdout,"g_var_decl\n");
    node_type* type =  g_type();
    return g_id_list(&type,&var_list);
}

static int synch_id_list[] = {T_COMMA,T_SEMICOLON,T_EOF};

node_var_list* g_id_list(node_type** type,node_var_list** var_list)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_id_list\n");
    Symbol* id = NULL;
    node_name* name = NULL;
    node_assign* assign = NULL;
    node_name_decl* name_decl = NULL;
    if(look_ahead == T_ID){
        id = table_id->search(GET_current_lexeme());
        name = new node_name(id,GET_current_line());
        eat(T_ID);
    }
    else{
        syntax_error(look_ahead,T_ID,GET_current_line());
        skip(synch_id_list);
    }
    assign = g_assign((node_expr**)&name);
    name_decl = new node_name_decl(*type,id,assign,GET_current_line());

    node_var_list* aux = new node_var_list(name_decl,NULL,GET_current_line());
    var_list = &aux;
    if(look_ahead == T_COMMA) (*var_list)->insert_list(g_id_list_(type,var_list));
    return *var_list;
}

node_var_list* g_id_list_(node_type** type, node_var_list** var_list)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_id_list_\n");
    if (look_ahead == T_COMMA){

        eat(T_COMMA);
        return g_id_list(type,NULL);
    }
    return NULL;
}

node_assign* g_assign(node_expr** expr)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_assign\n");
    node_expr* expr_right = NULL;
    if (look_ahead == T_ASSIGN)
    {
        eat(T_ASSIGN);
        expr_right = g_expr();
        return new node_assign(*expr,expr_right,GET_current_line());
    }
    return NULL;
}

static int synch_method_decl_1[] = {T_PUBLIC, T_PROTECTED,T_PRIVATE, T_INT, T_FLOAT, T_BOOLEAN, T_ID, T_STATIC, T_EOF };
static int synch_method_decl_2[] = {T_IF, T_WHILE, T_SYSTEM, T_THIS, T_RETURN, T_BLOCK_BEGIN,
                             T_BLOCK_END, T_INT, T_FLOAT, T_BOOLEAN, T_ID, T_EOF};
static int synch_method_decl_3[] = {T_BLOCK_BEGIN, T_EOF};
static int synch_method_decl_4[] = {T_INT, T_FLOAT, T_BOOLEAN, T_ID,  T_PARENT_R, T_EOF};
static int synch_method_decl_5[] = {T_PARENT_L, T_EOF};
static int synch_method_decl_6[] = {T_STATIC, T_EOF}; // apagamos os tipos de retorno

node_method_list* g_method_decl()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_method_decl\n");
    int access = T_PUBLIC;
    // Modificação inserindo outros tipos de acesso
    switch(look_ahead){
        case T_PUBLIC:
            eat(T_PUBLIC);
            access = T_PUBLIC;
            break;
        case T_PRIVATE:
            eat(T_PRIVATE);
            access = T_PRIVATE;
            break;
        case T_PROTECTED:
            eat(T_PROTECTED);
            access = T_PROTECTED;
            break;
        default: skip(synch_method_decl_6);
    }

    node_type* type = g_static_or_type();
    Symbol* id = table_id->search(GET_current_lexeme());
    eat_or_skip(T_ID,synch_method_decl_5);
    eat_or_skip(T_PARENT_L,synch_method_decl_4);
    node_var_list* var_param = NULL;
    var_param = g_formal_list(&var_param);
    eat_or_skip(T_PARENT_R,synch_method_decl_3);
    eat_or_skip(T_BLOCK_BEGIN,synch_method_decl_2);
   // node_var_list* var_list = NULL;
    node_stmt_list* stmt_list = g_stmt_list();
    //node_scope* scope = new node_scope(var_list,stmt_list);
    eat_or_skip(T_BLOCK_END,synch_method_decl_1);

    return new node_method_list(type,id,var_param,stmt_list,NULL,access,GET_current_line());
}

static int synch_static_or_type[] = {T_ID, T_EOF};

node_type* g_static_or_type()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_static_or_type\n");
    node_type* type = NULL;
    switch (look_ahead)
    {
        case T_STATIC:
            eat(T_STATIC);
            if(look_ahead == T_VOID) type = new node_type(T_VOID,GET_current_line());
            eat_or_skip(T_VOID,synch_static_or_type);
            return type;
        case T_VOID: // caso em que o cara não digitou o "static" , erro sintático "mágico"
            syntax_error(look_ahead,T_STATIC,GET_current_line());
            eat(T_VOID);
            type = new node_type(T_VOID,GET_current_line());
            return type;
        case T_ID:
        case T_INT:
        case T_FLOAT:
        case T_BOOLEAN:
            return g_type_or_id();
    }
    return NULL;
}

static int synch_formal_list[] = {T_COMMA, T_PARENT_R, T_BLOCK_BEGIN, T_EOF};

node_var_list* g_formal_list(node_var_list** param_list)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_formal_list\n");
    node_type* type = NULL;
    Symbol* id = NULL;
    node_name_decl* name_decl = NULL;
    node_var_list* aux = NULL;
    switch(look_ahead)
    {
        case T_INT:
        case T_FLOAT:
        case T_BOOLEAN:
        case T_ID:
            type = g_type_or_id();
            id = table_id->search(GET_current_lexeme());
            eat(T_ID);
            name_decl = new node_name_decl(type,id,GET_current_line());
            aux = new node_var_list(name_decl,NULL,GET_current_line());
            param_list = &aux;
            if(look_ahead == T_COMMA) (*param_list)->insert_list(g_formal_list_(param_list));
    }
    return *param_list;
}

node_var_list* g_formal_list_(node_var_list** param_list)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_formal_list_\n");
    if (look_ahead == T_COMMA)
    {
        eat(T_COMMA);
        return g_formal_list(NULL);
    }
    return *param_list; // retorna a ĺista intacta
}


node_type* g_type_or_id()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_type_or_id\n");
    node_type* type = NULL;
    switch(look_ahead)
    {
        case T_INT:
        case T_FLOAT:
        case T_BOOLEAN:
            type = g_type();
            break;
        case T_ID:
            type = new node_type(table_id->search(GET_current_lexeme()),GET_current_line());
            eat(T_ID);
            break;
    }
    return type;
}

node_type* g_type()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_type\n");
    node_type* type = NULL;
    switch (look_ahead)
    {
        case T_INT:
            eat(T_INT);
            type = new node_type(T_INT,g_bracket(),GET_current_line()); //g_bracket retorna um boolean que afirma ser ou não um array
            break;
        case T_FLOAT:
            eat(T_FLOAT);
            type = new node_type(T_FLOAT,g_bracket(),GET_current_line());
            break;
        case T_BOOLEAN:
            eat(T_BOOLEAN);
            type = new node_type(T_BOOLEAN,GET_current_line());
            break;
    }
    return type;
}

static int synch_bracket[] = {T_ID, T_EOF};

bool g_bracket()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_bracket\n");
    if (look_ahead == T_BRACKET_L) // decide se é foi intenção do usuário criar ou não um array
    {
        eat(T_BRACKET_L);
        eat_or_skip(T_BRACKET_R, synch_bracket);
        return true;
    }
    return false;
}

node_stmt_list* g_stmt_list()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_stmt_list\n");
    switch(look_ahead)
    {
        case T_IF:
        case T_WHILE:
        case T_SYSTEM:
        case T_RETURN:
        case T_BLOCK_BEGIN:
        case T_INT:
        case T_FLOAT:
        case T_BOOLEAN:
        case T_THIS:
        case T_ID:{
            node_stmt* stmt                 = g_stmt();
            node_stmt_list* stmt_list_next  = g_stmt_list();
            if(stmt == NULL) return stmt_list_next;
            else             return new node_stmt_list(stmt,stmt_list_next,GET_current_line());
        }
    }
    return NULL;
}

static int synch_stmt[]   =  {T_PLUS, T_MINUS, T_NEG, T_PARENT_R, T_LITERAL,
                       T_NUM_INT, T_NUM_REAL, T_TRUE, T_FALSE, T_THIS, T_ID,
                       T_NEW, T_EOF};
static int synch_stmt_2[] =  {T_IF, T_WHILE, T_SYSTEM, T_ID, T_THIS, T_RETURN, T_BLOCK_BEGIN,
                       T_INT, T_FLOAT, T_BOOLEAN, T_EOF};
static int synch_stmt_3[] =  {T_OUT, T_IN, T_EOF};
static int synch_stmt_4[] =  {T_IF, T_WHILE, T_SYSTEM, T_ID, T_THIS, T_RETURN, T_BLOCK_BEGIN,
                       T_INT, T_FLOAT, T_BOOLEAN, T_BLOCK_END, T_ELSE, T_EOF};
static int synch_stmt_5[] =  {T_SEMICOLON,T_BLOCK_END, T_EOF};

node_stmt* g_stmt()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_stmt\n");
    node_expr* expr = NULL;
    node_stmt* stmt = NULL;
    node_stmt* stmt2 = NULL;
    node_stmt_list* stmt_list = NULL;
    node_expr* name = NULL;
    node_expr* name_2 = NULL;
    node_var_list* var_list = NULL;
    int line =GET_current_line();
    Symbol* id = NULL;
    switch (look_ahead)
    {
        case T_IF:
            eat(T_IF);
            eat_or_skip(T_PARENT_L, synch_stmt);
            expr = g_expr();
            eat_or_skip(T_PARENT_R, synch_stmt_2);
            stmt = g_stmt();
            stmt2 = g_else_stmt();
            return new node_if(expr,stmt,stmt2,line);
        case T_WHILE:
            eat(T_WHILE);
            eat_or_skip(T_PARENT_L, synch_stmt);
            expr = g_expr();
            eat_or_skip(T_PARENT_R, synch_stmt_2);
            stmt = g_stmt();
            return new node_while(expr,stmt,line);
        case T_SYSTEM:
            eat(T_SYSTEM);
            eat_or_skip(T_DOT, synch_stmt_3);
            stmt = g_stmt_out_or_in();
            eat_or_skip(T_SEMICOLON, synch_stmt_4);
            return stmt;
        case T_RETURN:
            eat(T_RETURN);
            expr = g_expr();
            eat_or_skip(T_SEMICOLON, synch_stmt_4);
            return new node_return(expr,line);
        case T_BLOCK_BEGIN:{
            eat(T_BLOCK_BEGIN);
          //  node_var_list* var_list_2 = NULL;
            stmt_list = g_stmt_list();
            eat_or_skip(T_BLOCK_END, synch_stmt_4);
            return stmt_list;
        }
        case T_INT:
        case T_FLOAT:
        case T_BOOLEAN:
            stmt = g_var_decl();
            eat_or_skip(T_SEMICOLON, synch_stmt_4);
            return stmt;
        case T_THIS:
            name_2 = new node_this(GET_current_line());
            eat(T_THIS);
            if(look_ahead == T_DOT){
                eat(T_DOT);
                ((node_this*)name_2)->attr_expr = g_dot();
                if(look_ahead == T_ASSIGN){
                    eat(T_ASSIGN);
                    expr = g_expr();
                    stmt = new node_assign(name_2,expr,GET_current_line());
                }
                else stmt = name_2;
            }
            else if(look_ahead == T_ASSIGN){
                 eat(T_ASSIGN);
                 expr = g_expr();
                 stmt = new node_assign(name_2,expr,GET_current_line());
            }
            eat_or_skip(T_SEMICOLON, synch_stmt_4);
            return stmt;
        case T_ID:
            id = table_id->search(GET_current_lexeme());
            eat(T_ID);
            stmt = g_id_compl(&id,&var_list);
            eat_or_skip(T_SEMICOLON, synch_stmt_4);
    }
    return stmt;
}

static int synch_id_compl[] = {T_ASSIGN, T_EOF};

node_stmt* g_id_compl(Symbol** id,node_var_list** var_list){

    if(PRINT_FLUX)  fprintf(stdout,"g_id_compl\n");
    node_type* type = NULL;
    node_expr* name = NULL;
    node_expr* name_2 = NULL;
    node_expr* expr = NULL;
    node_stmt* stmt = NULL;
    if (look_ahead == T_ID)
    {
        // nos casos tipo Tipo var1,var2,var3 nós transformamos essa linha única em três
        // Tipo var1 ; Tipo var3, Tipo var3; logo passamos o type para o fluxo no parsing
        // usuá-lo no caso de colocar uma nova variável na lista de declarações
        // OBS: temos que aqui o Tipo é uma classe, uma declaração de objeto;
        type = new node_type(*id,GET_current_line());
        if(*var_list == NULL) *var_list = g_id_list(&type,var_list);
        else (*var_list)->insert_list(g_id_list(&type,NULL));
        return *var_list;
    }else{
        // se um Id apareceu em um caso que não seja declaração de variável, ele será uma chamada
        // pura de um nome como var; (o que não é erro para o compilador) ou poderá ser uma atribuição
        // (garantindo que o lado esquerdo será um um nome)
        name = new node_name(*id,GET_current_line());
        name_2 = g_struct(&name);
        if(look_ahead == T_ASSIGN){
            eat(T_ASSIGN);
            expr = g_expr();
            stmt = new node_assign(name_2,expr,GET_current_line());
        }else stmt = name_2;
        return stmt;
    }
    return NULL;
}

static int synch_id_compl_dot[] = {T_ASSIGN, T_EOF};

node_stmt* g_id_compl_dot(node_name** name)
{
    if(PRINT_FLUX)  fprintf(stdout,"g_id_comple_dot\n");
    node_expr* expr = NULL;
    switch (look_ahead)
    {
        case T_DOT:
            eat(T_DOT);
            (*name)->attr_expr = g_dot();
            ((node_name*)(*name)->attr_expr)->attr_expr = g_expr_();
            return *name;
        case T_ASSIGN:
            eat(T_ASSIGN);
            expr = g_expr();
            return new node_assign((*name),expr,GET_current_line());
    }
    return NULL;
}

static int synch_stmt_out_or_in[]   = {T_PRINTLN, T_PARENT_L, T_SEMICOLON, T_EOF};
static int synch_stmt_out_or_in_2[] = {T_PARENT_L, T_EOF};
static int synch_stmt_out_or_in_3[] = {T_ID, T_LITERAL, T_NUM_INT, T_NUM_REAL, T_NEW, T_TRUE, T_FALSE, T_PARENT_R, T_EOF};
static int synch_stmt_out_or_in_4[] = {T_SEMICOLON, T_EOF};
static int synch_stmt_out_or_in_5[]   = {T_READ, T_PARENT_L, T_SEMICOLON, T_EOF};

node_stmt* g_stmt_out_or_in()
{
    if(PRINT_FLUX)  fprintf(stdout,"g_stmt_out_or_in\n");
    node_expr* expr = NULL;
    node_expr_list* expr_list = NULL;
    switch (look_ahead)
    {
        case T_OUT:
            eat(T_OUT);
            eat_or_skip(T_DOT, synch_stmt_out_or_in);
            eat_or_skip(T_PRINTLN, synch_stmt_out_or_in_2);
            eat_or_skip(T_PARENT_L, synch_stmt_out_or_in_3);
            expr_list = g_expr_list();
            eat_or_skip(T_PARENT_R, synch_stmt_out_or_in_4);
            return new node_println(expr_list,GET_current_line());
        case T_IN:
            eat(T_IN);
            eat_or_skip(T_DOT, synch_stmt_out_or_in_5);
            eat_or_skip(T_READ, synch_stmt_out_or_in_2);
            eat_or_skip(T_PARENT_L, synch_stmt_out_or_in_3);
            expr = g_expr();
            eat_or_skip(T_PARENT_R, synch_stmt_out_or_in);
            return new node_read(expr,GET_current_line());
    }
    return NULL;
}

node_stmt* g_else_stmt(){

    if(PRINT_FLUX)  fprintf(stdout,"g_else_stmt\n");
    if (look_ahead == T_ELSE){
        eat(T_ELSE);
        return g_stmt();
    }
    return NULL;
}

node_expr_list* g_expr_list(){

    if(PRINT_FLUX)  fprintf(stdout,"g_expr_list\n");
    switch(look_ahead){
        case T_PLUS:
        case T_MINUS:
        case T_NEG:
        case T_PARENT_L:
        case T_LITERAL:
        case T_NUM_INT:
        case T_NUM_REAL:
        case T_TRUE:
        case T_FALSE:
        case T_THIS:
        case T_ID:
        case T_NEW:
            node_expr* expr = g_expr();
            node_expr_list* expr_list = g_expr_list_();
            return new node_expr_list(expr,expr_list,GET_current_line());
    }
    return NULL;
}

node_expr_list* g_expr_list_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_expr_list_\n");
    if (look_ahead == T_COMMA){
        eat(T_COMMA);
        return g_expr_list();
    }
    return NULL;
}

static int synch_expr[] = {T_PARENT_R,T_SEMICOLON, T_EOF};
node_expr* g_expr(){

    if(PRINT_FLUX)  fprintf(stdout,"g_expr\n");
    switch (look_ahead){

        case T_PLUS:
        case T_MINUS:
        case T_NEG:
        case T_PARENT_L:
        case T_LITERAL:
        case T_NUM_INT:
        case T_NUM_REAL:
        case T_TRUE:
        case T_FALSE:
        case T_THIS:
        case T_ID:
            return g_binop2();
        case T_NEW:
            eat(T_NEW);
            return g_expr_new();
        default: // expressões são obrigatórias
            syntax_error(look_ahead,"expression",GET_current_line());
            skip(synch_expr);
    }
    return NULL;
}

static int synch_expr_new_1[] = {T_PARENT_R, T_EOF};
static int synch_expr_new_2[] = {T_PARENT_R, T_BRACKET_R, T_SEMICOLON, T_EOF};
static int synch_expr_new_3[] = {T_PLUS, T_MINUS, T_NEG, T_PARENT_L, T_LITERAL,
                          T_NUM_INT, T_NUM_REAL, T_TRUE, T_FALSE, T_THIS, T_ID, T_NEW, T_EOF};

node_new* g_expr_new(){

    if(PRINT_FLUX)  fprintf(stdout,"g_expr_new\n");
    node_expr* expr = NULL;
    switch (look_ahead){

        case T_INT:
            eat(T_INT);
            eat_or_skip(T_BRACKET_L,synch_expr_new_3);
            expr = g_expr();
            eat_or_skip(T_BRACKET_R,synch_expr_new_2);
            return new node_new(T_INT,expr,GET_current_line());
        case T_FLOAT:
            eat(T_FLOAT);
            eat_or_skip(T_BRACKET_L,synch_expr_new_3);
            expr = g_expr();
            eat_or_skip(T_BRACKET_R,synch_expr_new_2);
            return new node_new(T_FLOAT,expr,GET_current_line());
        case T_ID:
            eat(T_ID);
            eat_or_skip(T_PARENT_L,synch_expr_new_1);
            eat_or_skip(T_PARENT_R,synch_expr_new_2);
            return new node_new(table_id->search(GET_current_lexeme()),GET_current_line());
    }
    return NULL;
}

node_expr* g_expr_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_expr_\n");
    if (look_ahead == T_ASSIGN){

        eat(T_ASSIGN);
        return g_expr();
    }
    return NULL;
}

node_expr* g_binop2(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop2\n");
    node_expr* expr = g_binop3();
    if(look_ahead == T_OR) return new node_op_boolean(T_OR,expr,g_binop2_(),GET_current_line());
    return expr;

}

node_expr* g_binop2_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop2_\n");
    eat(T_OR);
    node_expr* expr = g_binop3();
    if(look_ahead == T_OR) return new node_op_boolean(T_OR,expr,g_binop2_(),GET_current_line());
    return expr;
}

node_expr* g_binop3(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop3\n");
    node_expr* expr =  g_binop4();
    if(look_ahead == T_AND) return new node_op_boolean(T_AND,expr,g_binop3_(),GET_current_line());
    return expr;
}

node_expr* g_binop3_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop3_\n");
    eat(T_AND);
    node_expr* expr = g_binop4();
    if(look_ahead == T_AND) return new node_op_boolean(T_AND,expr,g_binop3_(),GET_current_line());
    return expr;
}

node_expr* g_binop4(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop4\n");
    node_expr* expr = g_binop5();
    switch(look_ahead){
        case T_EQUAL:
            eat(T_EQUAL);
            return new node_op_relational(T_EQUAL,expr,g_binop4(),GET_current_line());
        case T_NOT_EQUAL:
            eat(T_NOT_EQUAL);
            return new node_op_relational(T_NOT_EQUAL,expr,g_binop4(),GET_current_line());
    }
    return expr;
}

node_expr* g_binop4_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop4_\n");
    node_expr* expr = g_binop5();
    switch (look_ahead){

        case T_EQUAL:
            eat(T_EQUAL);
            return new node_op_boolean(T_EQUAL,expr,g_binop4_(),GET_current_line());
        case T_NOT_EQUAL:
            eat(T_NOT_EQUAL);
            return new node_op_boolean(T_NOT_EQUAL,expr,g_binop4_(),GET_current_line());
    }
    return expr;
}

node_expr* g_binop5(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop5\n");
    node_expr* expr = g_binop6();
    switch(look_ahead){
        case T_LESS_T:
            eat(T_LESS_T);
            return new node_op_relational(T_LESS_T,expr,g_binop5_(),GET_current_line());
        case T_LESS_E:
            eat(T_LESS_E);
            return new node_op_relational(T_LESS_E,expr,g_binop5_(),GET_current_line());
            break;
        case T_GREATER_T:
            eat(T_GREATER_T);
            return new node_op_relational(T_GREATER_T,expr,g_binop5_(),GET_current_line());
            break;
        case T_GREATER_E:
            eat(T_GREATER_E);
            return new node_op_relational(T_GREATER_E,expr,g_binop5_(),GET_current_line());
            break;
    }
    return expr;
}

node_expr* g_binop5_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_bino5_\n");
    node_expr* expr = g_binop6();
    switch(look_ahead){

        case T_LESS_T:
            eat(T_LESS_T);
            return new node_op_relational(T_LESS_T,expr,g_binop5_(),GET_current_line());
        case T_LESS_E:
            eat(T_LESS_E);
            return new node_op_relational(T_LESS_E,expr,g_binop5_(),GET_current_line());
        case T_GREATER_T:
            eat(T_GREATER_T);
            return new node_op_relational(T_GREATER_T,expr,g_binop5_(),GET_current_line());
        case T_GREATER_E:
            eat(T_GREATER_E);
            return new node_op_relational(T_GREATER_E,expr,g_binop5_(),GET_current_line());
    }
    return expr;
}

node_expr* g_binop6(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop6\n");
    node_expr* expr =  g_binop7();
    int line = GET_current_line();
    switch(look_ahead){
        case T_PLUS:
            eat(T_PLUS);
            return new node_op_addition(T_PLUS,expr,g_binop6_(),line);
        case T_MINUS:
            eat(T_MINUS);
            return new node_op_addition(T_MINUS,expr,g_binop6_(),line);
        case T_OR_UNI:
            eat(T_OR_UNI);
            return new node_op_bitwise(T_OR_UNI,expr,g_binop6_(),line);
    }
    return expr;
}

node_expr* g_binop6_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop6_\n");
    node_expr* expr = g_binop7();
    int line = GET_current_line();
    switch(look_ahead){

        case T_PLUS:
            eat(T_PLUS);
            return new node_op_addition(T_PLUS,expr,g_binop6_(),line);
        case T_MINUS:
            eat(T_MINUS);
            return new node_op_addition(T_MINUS,expr,g_binop6_(),line);
        case T_OR_UNI:
            eat(T_OR_UNI);
            return new node_op_bitwise(T_OR_UNI,expr,g_binop6_(),line);
    }
    return expr;
}

node_expr* g_binop7(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop7\n");
    node_expr* expr =  g_unop();
    int line = GET_current_line();
    switch(look_ahead){

        case T_MULT:
            eat(T_MULT);
            return new node_op_multiplication(T_MULT,expr,g_binop7_(),line);
        case T_DIV:
            eat(T_DIV);
            return new node_op_multiplication(T_DIV,expr,g_binop7_(),line);
        case T_MOD:
            eat(T_MOD);
            return new node_op_multiplication(T_MOD,expr,g_binop7_(),line);
        case T_AND_UNI:
            eat(T_AND_UNI);
            return new node_op_bitwise(T_AND_UNI,expr,g_binop7_(),line);
    }
    return expr;
}

node_expr* g_binop7_(){

    if(PRINT_FLUX)  fprintf(stdout,"g_binop7_\n");
    node_expr* expr =  g_unop();
    int line = GET_current_line();
    switch(look_ahead){

        case T_MULT:
            eat(T_MULT);
            return new node_op_multiplication(T_MULT,expr,g_binop7_(),line);
        case T_DIV:
            eat(T_DIV);
            return new node_op_multiplication(T_DIV,expr,g_binop7_(),line);
        case T_MOD:
            eat(T_MOD);
            return new node_op_multiplication(T_MOD,expr,g_binop7_(),line);
        case T_AND_UNI:
            eat(T_AND_UNI);
            return new node_op_bitwise(T_AND_UNI,expr,g_binop7_(),line);
    }
    return expr;
}

node_expr* g_unop(){

    if(PRINT_FLUX)  fprintf(stdout,"g_unop\n");
    int line = GET_current_line();
    switch(look_ahead){
        case T_MINUS:
            eat(T_MINUS);
            return new node_signal(g_primary(),line);
        case T_NEG:
            eat(T_NEG);
            return new node_negation(g_primary(),line);
        case T_PLUS:
            eat(T_PLUS);
            break;
    }
    return g_primary();
}

static int synch_primary[] = {T_BRACKET_L, T_DOT, T_MULT, T_DIV, T_MOD, T_AND_UNI, T_PLUS, T_MINUS, T_OR_UNI,
                        T_LESS_E, T_LESS_T, T_GREATER_E, T_GREATER_T, T_EQUAL, T_NOT_EQUAL, T_AND, T_OR,
                        T_ASSIGN, T_SEMICOLON, T_EOF};

node_expr* g_primary(){

    if(PRINT_FLUX)  fprintf(stdout,"g_primary\n");
    node_expr* primary = NULL;
    node_name* name = NULL;
    int line = GET_current_line();
    switch(look_ahead){

        case T_LITERAL:
            primary = new node_primitive(T_LITERAL,GET_current_lexeme(),line);
            eat(T_LITERAL);
            break;
        case T_NUM_INT:
            primary = new node_primitive(T_NUM_INT,GET_current_lexeme(),line);
            eat(T_NUM_INT);
            break;
        case T_NUM_REAL:
            primary = new node_primitive(T_NUM_REAL,GET_current_lexeme(),line);
            eat(T_NUM_REAL);
            break;
        case T_TRUE:
            primary = new node_primitive(T_TRUE,line);
            eat(T_TRUE);
            break;
        case T_FALSE:
            primary = new node_primitive(T_FALSE,line);
            eat(T_FALSE);
            break;
        case T_ID:
            name = new node_name(table_id->search(GET_current_lexeme()),line);
            eat(T_ID);
            return g_struct((node_expr**)&name);
        case T_PARENT_L:
            eat(T_PARENT_L);
            primary = g_expr();
            eat_or_skip(T_PARENT_R,synch_primary);
            break;
        case T_THIS:
            eat(T_THIS);
            primary = new node_this(line);
            if(look_ahead == T_DOT){
                eat(T_DOT);
                ((node_this*)primary)->attr_expr = g_dot();
            }
            return primary;
    }
    return primary;
}

static int synch_struct[] = {T_MULT, T_DIV, T_MOD, T_AND_UNI, T_PLUS, T_MINUS, T_OR_UNI, T_LESS_E,
                       T_LESS_T, T_GREATER_E, T_GREATER_T, T_EQUAL, T_NOT_EQUAL, T_AND, T_OR,
                       T_ASSIGN, T_SEMICOLON, T_EOF};

node_expr* g_struct(node_expr** arg){

    if(PRINT_FLUX)  fprintf(stdout,"g_struct\n");
    node_expr* expr = NULL;
    node_expr_list* expr_list = NULL;
    node_call* call = NULL;
    int line = GET_current_line();
    switch(look_ahead){

        case T_BRACKET_L:
            eat(T_BRACKET_L);
            expr = g_expr();
            eat_or_skip(T_BRACKET_R,synch_struct);
            return new node_vector((node_name*)*arg,expr,line);
        case T_PARENT_L:
            eat(T_PARENT_L);
            expr_list = g_expr_list();
            eat_or_skip(T_PARENT_R,synch_struct);
            call = new node_call(((node_name*)*arg)->attr_id,expr_list,line);
            call->attr_expr = g_struct(&expr);
            return call;
        case T_DOT:
            eat(T_DOT);
            if(*arg == NULL) *arg = g_dot();
            else ((node_name*)*arg)->attr_expr = g_dot();
            return *arg;
    }
    return *arg;
}

static int synch_dot[] = {T_MULT, T_DIV, T_MOD, T_AND_UNI, T_PLUS, T_MINUS, T_OR_UNI, T_LESS_E,
                       T_LESS_T, T_GREATER_E, T_GREATER_T, T_EQUAL, T_NOT_EQUAL, T_AND, T_OR,
                       T_ASSIGN, T_SEMICOLON, T_EOF};
node_expr* g_dot(){

    if(PRINT_FLUX)  fprintf(stdout,"g_dot\n");
    node_name* name = NULL;
    int line = GET_current_line();
    switch(look_ahead){

        case T_LENGTH:
            eat(T_LENGTH);
            return new node_length(line);
        case T_ID:
            name = new node_name(table_id->search(GET_current_lexeme()),line);
            eat(T_ID);
            return g_struct((node_expr**)&name);
        default:
            syntax_error(look_ahead, "Length or ID", line);
            skip(synch_dot);
    }
    return NULL;
}

static int synch_parent[] = {T_DOT, T_ASSIGN, T_SEMICOLON, T_EOF};

void g_parent(){

    if(PRINT_FLUX)  fprintf(stdout,"g_parent\n");
    if(look_ahead == T_PARENT_L){
        eat(T_PARENT_L);
        g_expr_list();
        eat_or_skip(T_PARENT_R,synch_parent);
    }
}

void g_dot_(){
    if(PRINT_FLUX)  fprintf(stdout,"g_dot_\n");
    if(look_ahead == T_DOT){
        eat(T_DOT);
        g_dot();
    }
}
