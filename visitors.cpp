    /**
    * visitors.cpp
    * @author Claudson Oliveira
    * @date   29 de abril de 2011
    *
    * Arquivo que implementa as classes do padrão visitor
    *
    **/

    #include "visitors.h"
    #include <stdio.h>
    #include <stdlib.h>
    #include <typeinfo>
    #include <string.h>
    #include "lexical.h"
    #include "semantic.h"
    #include "error.h"
    #include "symbols_tables.h"
    #include "abstract_syntax_tree.h"


 //--------------------------------------- Visitor_semantic ------------------------------------------------
    Visitor_semantic::Visitor_semantic(){
        table_id         = GET_symbol_table_id();
        table_class      = GET_table_class();
        attribute_inherited_int  = 0 ;
        attribute_sintetic_int  = 0 ;
        attribute_sintetic_char = NULL;
        attribute_inherited_char = NULL;
        has_main = 0;
    }

    void Visitor_semantic::visit(node_assign* node){
        if(node != NULL){
            is_call = 0;
            int type_right, type_left, array_left, array_right;
            char* object_left, *object_right;
            attribute_inherited_char = NULL;
            is_array = 0 ;
            int state_attr = is_attr;
            is_attr = 0 ;
            if(node->attr_expr_left != NULL) node->attr_expr_left->accept(this);
            if(is_call)semantic_error(E_CALL_IN_ASSIGN,node->attr_line);
            array_left = is_array;
            type_left = attribute_sintetic_int;
            object_left = GET_type_name(attribute_sintetic_int);
            attribute_sintetic_int = 0 ;
            if(attribute_inherited_int && (attribute_inherited_char != NULL)) object_left = attribute_inherited_char;


            attribute_sintetic_char = NULL;
            is_array = 0;
            is_attr = state_attr;
            if(node->attr_expr_right != NULL) node->attr_expr_right->accept(this);
            if((typeid(*(node->attr_expr_right)) == typeid(node_new))){ //Se for NEW
                if(type_left == 0) node->type_assing = node_assign::INIT_OBJECT; //Se for objeto do lado esquerdo
                else node->type_assing = node_assign::INIT_ARRAY; //Se for array
            }

            array_right = is_array;
            type_right = attribute_sintetic_int;
            object_right = GET_type_name(attribute_sintetic_int);
            if(type_left == T_INT && type_right == T_FLOAT) type_left = type_right;
            else if(type_left == T_FLOAT && type_right == T_INT) type_left = type_right;
            if(type_right != type_left ) semantic_error(E_INVALID_ASSIGN,node->attr_line,object_left,object_right);
            else if(!attribute_sintetic_int){
                scope* current_scope = GET_scope_this();
                Symbol_class* father = GET_class(object_left);
                Symbol_class* p = GET_class(object_right);
                int error = 1 ;
                do{
                    if(p == father){
                        error = 0;
                        break;
                    }
                    p = p->father;
                }
                while(p != NULL);
                if(error){
                    if(typeid(*(node->attr_expr_right)) == typeid(node_new)) semantic_error(E_INVALID_NEW,node->attr_line,object_left,object_right);
                    else semantic_error(E_INVALID_ASSIGN,node->attr_line,object_left,object_right);
                }
            }else{
                if(array_left != array_right) semantic_error(E_INVALID_ASSIGN_ARRAY,node->attr_line);
            }
            attribute_sintetic_int = 0;
            attribute_inherited_char = NULL;
            attribute_inherited_int = 0;
        }
    }
    void Visitor_semantic::visit(node_call* node){
        if(node != NULL){
            scope* current_scope = NULL ;

            if(attribute_inherited_char != NULL){
                Symbol_class* class_ = GET_class(attribute_inherited_char) ;
                if(class_ == NULL) current_scope = GET_current_scope();
                else current_scope = class_->current_scope ;
            }else current_scope = GET_scope_this();

            char* meth_name = table_id->get_lexeme(node->attr_id);
            Symbol_method* meth = current_scope->get_method(meth_name);
            if(attribute_inherited_int > T_EOF) semantic_error(E_CALL_IN_PRIMITIVE,node->attr_line,meth_name,GET_token_name(attribute_inherited_int));
            char* class_name = (attribute_inherited_char != NULL)?attribute_inherited_char:table_class->get_lexeme(current_scope->current_class);
            if(meth == NULL) semantic_error(E_UNDECLARED_CALL,node->attr_line,meth_name,class_name);
            else{
                attribute_inherited_int = meth->return_primitive;
                if(attribute_inherited_int == 0 ) attribute_inherited_char = table_class->get_lexeme(meth->return_object);
                else attribute_inherited_char = NULL;
            }
            char* return_object = attribute_inherited_char;
            int return_primitive = attribute_inherited_int;


            // testando tipos de atributos compatíveis
            Symbol_method* meth_atual = meth;
            node_expr_list* list = node->attr_expr_list;
            int count_expr = 0;
            while(list!= NULL){
                count_expr++;
                list = list->attr_expr_list;
            }
            char** array_returns_object  = (char**)malloc(sizeof(char*) * count_expr);
            int* array_returns_primitive = (int*)malloc(sizeof(int) * count_expr);
            list = node->attr_expr_list;
            int i = 0 ;
            // pego o valor de todas as expressões
            attribute_inherited_char = NULL;
            attribute_inherited_int = 0;
            while(list != NULL){
                attribute_sintetic_char = NULL;
                attribute_sintetic_int = 0 ;
                list->attr_expr->accept(this);
                array_returns_object[i] = attribute_sintetic_char;
                array_returns_primitive[i++] = attribute_sintetic_int;
                list = list->attr_expr_list;
            }
            int its_ok = 1;
            while(meth_atual != NULL){
                its_ok = 1;
                if(meth_atual->num_params == count_expr){
                    for(int j=0; j < count_expr; j++ ){
                        if(array_returns_primitive[j] != meth_atual->params[j]->type_primitive){
                            if(array_returns_object[j] == NULL || meth_atual->params[j]->type_object == NULL) its_ok = 0 ;
                            else if(strcmp(array_returns_object[j],table_class->get_lexeme(meth_atual->params[j]->type_object))){
                                    its_ok = 0;
                                    break;
                                }
                        }else  if(array_returns_object[j] != NULL && meth_atual->params[j]->type_object != NULL)
                               if(strcmp(array_returns_object[j],table_class->get_lexeme(meth_atual->params[j]->type_object))){
                                    its_ok = 0;
                                    break;
                            }
                    }
                }else if(meth_atual->overload == NULL) semantic_error(E_METH_NOT_FOUND,node->attr_line,meth_name);
                      else its_ok = 0;

                if(its_ok) break; // encontrou um método que case com o call
                else if(meth_atual->overload == NULL) semantic_error(E_METH_NOT_FOUND,node->attr_line,meth_name);
                meth_atual = meth_atual->overload;
            }
            node->attr_id = meth_atual;
            // Se o método for private ele só pode ser acessado pela própria classe que o criou
            if(meth_atual->access == T_PRIVATE &&  meth_atual->owner->current_scope != GET_scope_this()) semantic_error(E_ACCESS_PRIVATE,node->attr_line,meth_name);
            else if(meth_atual->access == T_PROTECTED){

                // Se o método for protected ele só pode ser acessado por descendentes ou pela própria classe
                // dona do método
                scope* scp = GET_scope_this();
                int flag_found = 0;
                while(scp != NULL){
                    if(scp == meth_atual->owner->current_scope) break;
                    if(scp->current_class->father != NULL) scp = scp->current_class->father->current_scope;
                    else semantic_error(E_ACCESS_PROTECTED,node->attr_line,meth_name);
                }

            }
            attribute_inherited_char = return_object;
            attribute_inherited_int = return_primitive;

            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            else{
                is_call = true;
                attribute_sintetic_char = attribute_inherited_char;
                attribute_sintetic_int  = attribute_inherited_int;
                if(attribute_inherited_char != NULL ){
                    attribute_inherited_int = 1;
                    attribute_inherited_char = NULL;
                }else{
                    attribute_inherited_int = 0;
                }
            }
        }
        is_array = 0;
    }
    void Visitor_semantic::visit(node_class_list* node){
        attribute_inherited_char = NULL;
        attribute_sintetic_int = 0 ;
        is_attr = 0 ;
        if(node != NULL){
            begin_scope();
            scope* current_scope = NULL;
            if(node->attr_id != NULL){
                Symbol_class* class_ = new Symbol_class();
                char* class_name = table_id->get_lexeme(node->attr_id);
                current_scope = GET_scope_this();
                current_scope->current_class = class_;
                class_->current_scope = current_scope;
                if(node->attr_id_father != NULL){
                    char* class_father_name = table_id->get_lexeme(node->attr_id_father);
                    if(strcmp(class_name,class_father_name) == 0 ) semantic_error(E_INVALID_INHERITANCE,node->attr_line,class_name);
                    if(GET_class(class_father_name) != NULL) class_->father = GET_class(class_father_name);
                    else semantic_error(E_UNDECLARED_CLASS,node->attr_line,class_father_name);
                }else class_->father = NULL;
                put(class_,class_name);
                node->attr_id = class_;
            }
            is_attr = 1;
            if(node->attr_var_list != NULL) node->attr_var_list->accept(this);
            is_attr = 0;
            // contando o numero de atributos
            int num_attr = 0 ;
            node_var_list* aux = node->attr_var_list;
            while(aux){
                num_attr++;
                aux = aux->attr_var_list;
            }
            // adicionndo método implícito toString
            node_method_list* toString = new node_method_list(NULL,NULL,NULL,NULL,NULL,T_PUBLIC,0);
            toString->attr_access = T_PUBLIC;
            toString->attr_line  = 0 ;
            toString->attr_method_list = NULL;
            toString->attr_param_list = NULL;
            toString->attr_stmt_list = NULL;
            toString->attr_type = new node_type(T_BOOLEAN,0);

            Symbol_method* s = create_method(toString);
            toString->attr_id  = s ;
            if(current_scope == NULL)current_scope = GET_scope_this();
            current_scope->put(s,"toString");
            Symbol_variable** attributes = (Symbol_variable**)current_scope->table_variable->getAtributos();
            node_stmt_list* prints = new node_stmt_list(NULL,NULL,0);
            node_expr_list* impress = new node_expr_list(NULL,NULL,0);
            node_expr_list* first_impress = impress;
            int i= 0 ;
            Symbol_variable* attr = NULL;
            Symbols_table_literal* lit = GET_symbol_table_lit();
            lit->insert("\"-\"",new  Symbol());
            node_var_list* attrs = node->attr_var_list;
            current_scope->current_class->num_attributo = num_attr;
            while(i < num_attr){
                char name_attr[100];
                char* aux = current_scope->table_variable->get_lexeme(attrs->attr_name->attr_id);
                sprintf(name_attr,"\"%s:\"",aux);
                impress->attr_expr = new node_primitive(T_LITERAL,name_attr,0);
                impress->attr_expr_list = new node_expr_list(NULL,NULL,0);
                impress = impress->attr_expr_list;
                impress->attr_expr = new node_name(attrs->attr_name->attr_id,0);
                impress->attr_expr_list = new node_expr_list(NULL,NULL,0);
                impress = impress->attr_expr_list;
                impress->attr_expr = new node_primitive(T_LITERAL,"\"\\n\"",0);
                attrs = attrs->attr_var_list;
                i++;
                if(i < num_attr){
                    impress->attr_expr_list = new node_expr_list(NULL,NULL,0);
                }else break;
                impress =  impress->attr_expr_list ;
            }

            node_method_list* meth = node->attr_method_list;
            node_method_list* last = meth;
            if(i > 0 ){
                prints->attr_stmt        = new node_println(first_impress,0);
                prints->attr_stmt_list   = new node_stmt_list(new node_return(new node_primitive(T_BOOLEAN,"1",0),0),NULL,0);
                toString->attr_stmt_list = prints;
                meth = toString;
                meth->attr_method_list = node->attr_method_list;
                node->attr_method_list = meth;
            }
            meth = last;
            while(meth != NULL){
                if(meth->attr_type->attr_primitive == T_VOID && node->attr_var_list != NULL) semantic_error(E_ATTR_IN_MAIN,node->attr_line);
                meth->accept(this);
                if(meth->attr_method_list) meth  = meth->attr_method_list;
                else break;
                last = meth;
            }
            end_scope();
            if(node->attr_class_list != NULL) node->attr_class_list->accept(this);
        }
    }

    void Visitor_semantic::visit(node_expr_list* node){}
    void Visitor_semantic::visit(node_if* node){
        if(node!= NULL){
            attribute_sintetic_int = 0;
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            if(attribute_sintetic_int != T_BOOLEAN) semantic_error(E_INVALID_EXPRESSION_IF,node->attr_line);
            else{
                begin_scope();
                if(node->attr_stmt != NULL) node->attr_stmt->accept(this);
                end_scope();
                begin_scope();
                if(node->attr_stmt_else != NULL) node->attr_stmt_else->accept(this);
                end_scope();
            }
        }
    }
    void Visitor_semantic::visit(node_length* node){
        if(node != NULL){
            if(attribute_inherited_char != NULL && !is_array){
                const char* type = attribute_inherited_char;
                const char* action = "primitive";
                if(type == NULL) type = GET_token_name(attribute_inherited_int);
                else action = "class";
                semantic_error(E_INVALID_LENGTH,node->attr_line,action,type);
            }else{
                attribute_sintetic_int = T_INT;
                attribute_sintetic_char = NULL;
                attribute_inherited_int = 0;
                attribute_sintetic_char = NULL;
            }
        }
    }

    void Visitor_semantic::visit(node_method_list* node){
        int return_valid = 0 ; // variável que vai definir se foi realizado retorno em todos os lugares possíveis
        char* method_name ;
        if(node != NULL){
            method_name = table_id->get_lexeme(node->attr_id);
            scope* current_scope = GET_current_scope();
            Symbol_method* meth = current_scope->get_method(method_name);
            Symbol_method *p = NULL; // auxiliar que representa o metodo mais atual
            if(meth == NULL){
                meth = create_method(node);
                current_scope->put(meth,method_name);
                p = meth;
            }
            else{
                Symbol_method *new_meth = create_method(node);
                p = meth;
                while(p != NULL){
                    if(!is_new_signature(new_meth,p)) semantic_error(E_INVALID_SIGNATURE,node->attr_line,method_name);
                    if(p->overload != NULL) p = p->overload;
                    else break;
                }
                // inserindo o novo metodo
                p->overload = new_meth;
                p = new_meth;
            }
            if(p->return_primitive == T_VOID){
                if(has_main == 0) has_main = 1;
                else semantic_error(E_DUPLICATE_MAIN,node->attr_line);
            }else if(node->attr_type->attr_id != NULL){
                Symbol_class* class_ = GET_class(table_id->get_lexeme(node->attr_type->attr_id));
                if(class_ == NULL)semantic_error(E_UNDECLARED_CLASS,node->attr_line,table_id->get_lexeme(node->attr_type->attr_id));
                p->return_object = class_;
            }

            begin_scope();
            attribute_inherited_char = NULL;
            attribute_sintetic_char = NULL;
            attribute_inherited_int = 0;
            attribute_sintetic_int  = 0;
            current_scope = GET_current_scope();
            node_var_list* var = node->attr_param_list;
            int i= 0;

            // percorrendo os parametros e os adicionando como variaveis locais
            while(var != NULL){
                char* param_name  = table_id->get_lexeme(var->attr_name->attr_id);
                Symbol_variable* variable = (Symbol_variable*)current_scope->table_variable->search(param_name);
                if(variable != NULL) semantic_error(E_REDECLARED_PARAM,node->attr_line,param_name,method_name);
                if(var->attr_name->attr_type->attr_id){
                    // é objeto
                    Symbol_class* class_ =  GET_class(table_id->get_lexeme(var->attr_name->attr_type->attr_id));
                    if(class_ != NULL) p->params[i]->type_object = class_;
                    else semantic_error(E_UNDECLARED_CLASS,node->attr_line,table_id->get_lexeme(var->attr_name->attr_type->attr_id));
                }
                current_scope->put(p->params[i],table_id->get_lexeme(var->attr_name->attr_id));
                var->attr_name->attr_id = p->params[i++];
                var = var->attr_var_list;
            }
            node->attr_id = p;
            if(p->return_object){
                current_return_char = table_class->get_lexeme(p->return_object);
                current_return_int = 0;
            }else{
                current_return_int = p->return_primitive;
                current_return_char = NULL;
            }
            node_stmt_list* list = node->attr_stmt_list;
            while(list != NULL){
                attribute_sintetic_char = NULL;
                attribute_sintetic_int = 0;
                attribute_inherited_int = 0;
                attribute_inherited_char = NULL;
                if(list->attr_stmt != NULL) list->attr_stmt->accept(this);
                if(attribute_sintetic_int == ITS_RETURN ) return_valid = ITS_RETURN;
                list = list->attr_stmt_list;
            }
            // ao sair do escopo do método, testa-se se houve um retorno no nível zero do método
        if(return_valid != ITS_RETURN && meth->return_primitive != T_VOID ) semantic_error(E_NOT_FOUND_RETURN,node->attr_line,method_name);
        }
        end_scope();
       // if(node->attr_method_list != NULL) node->attr_method_list->accept(this);
    }
    void Visitor_semantic::visit(node_this* node){
        if(node != NULL){
            attribute_inherited_int = 0;
            attribute_inherited_char = table_class->get_lexeme(GET_scope_this()->current_class);
            attribute_inherited_int = 0;
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            else{
               attribute_sintetic_char = attribute_inherited_char;
               attribute_sintetic_int  = 0 ;
               attribute_inherited_int = 0;
               attribute_inherited_char = NULL;
            }
        }
    }
    void Visitor_semantic::visit(node_name* node){
        if(node != NULL){
            scope* current_scope = NULL ;

        if(attribute_inherited_char != NULL){
            attribute_inherited_int = 0 ;
            Symbol_class* class_ = GET_class(attribute_inherited_char) ;
            if(class_ == NULL) current_scope = GET_current_scope();
            else current_scope = class_->current_scope ;
        }else current_scope = GET_current_scope();
            char* var_name = table_id->get_lexeme(node->attr_id);

            Symbol_variable* var = current_scope->get_variable(var_name);

            if(var == NULL)semantic_error(E_UNDECLARED_VARIABLE,node->attr_line,var_name);
            node->attr_id = var;
            // acessando um atributo de um tipo primitivo;
            if(attribute_inherited_int > T_EOF ) semantic_error(E_ATTR_IN_PRIMITIVE,node->attr_line,var_name,GET_token_name(attribute_inherited_int));
            if(var->type_object == NULL){
                attribute_inherited_int  = var->type_primitive;
                attribute_inherited_char = NULL;
            }else{
                attribute_inherited_int = 0 ;
                attribute_inherited_char =  table_class->get_lexeme(var->type_object);
            }

            if(is_attr) semantic_error(E_ASSIGN_ATTR,node->attr_line,var_name);
            if(var->array) is_array = 1;
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            else{
                attribute_sintetic_char = attribute_inherited_char;
                attribute_sintetic_int  = attribute_inherited_int;
                if(attribute_inherited_char != NULL ){
                    attribute_inherited_int = 0;
                    attribute_inherited_char = NULL;
                }else{
                    attribute_inherited_int = 0;
                }
            }

        }

    }
    void Visitor_semantic::visit(node_vector* node){
        if(node != NULL){
            scope* current_scope = NULL ;
            if(attribute_inherited_char != NULL){
                attribute_inherited_int = 0 ;
                current_scope = GET_class(attribute_inherited_char)->current_scope ;
            }else current_scope = GET_current_scope();
                char* var_name = "";
            if(attribute_inherited_int == 0 ){
                var_name = table_id->get_lexeme(node->attr_name->attr_id);
                Symbol_variable* var   = current_scope->get_variable(var_name);
                if(!var) semantic_error(E_UNDECLARED_VARIABLE,node->attr_line,var_name);
                attribute_sintetic_int = var->type_primitive;
                attribute_sintetic_char = NULL;
                node->attr_name->attr_id = var;
            }else{
                attribute_sintetic_int = attribute_inherited_int;
                attribute_sintetic_char = NULL;
            }
            attribute_inherited_char = NULL;
            attribute_inherited_int = 0 ;
            if(node->attr_expr) node->attr_expr->accept(this);
            if(attribute_sintetic_int != T_INT) semantic_error(E_INVALID_OFFSET_ARRAY,node->attr_line,var_name,GET_type_name(attribute_sintetic_int));
        }
    }

    void Visitor_semantic::visit(node_name_decl* node){
        if(node != NULL){
            attribute_sintetic_char = NULL;
            attribute_sintetic_int = 0;
            scope* current_scope = GET_current_scope();
            char* var_name = table_id->get_lexeme(node->attr_id);
            if(current_scope->table_variable->search(var_name) != NULL) semantic_error(E_REDECLARED_VARIABLE,node->attr_line,var_name);
            else{
                char* class_name;
                if(node->attr_type->attr_id != NULL) class_name = table_id->get_lexeme(node->attr_type->attr_id);
                node->attr_id = create_variable(node);
                if(node->attr_type->attr_id != NULL) {
                    Symbol_class* class_ = GET_class(class_name);
                    if(class_ == NULL) semantic_error(E_UNDECLARED_CLASS,node->attr_line,class_name);
                    node->attr_type->attr_id = class_ ;
                    ((Symbol_variable*)node->attr_id)->type_object = class_;
                }
                current_scope->put((Symbol_variable*)node->attr_id,var_name);
                if(node->attr_assign != NULL){
                  node->attr_assign->accept(this);
                  attribute_sintetic_char = NULL;
                  attribute_sintetic_int  = 0;
                }
            }
        }
    }

    void Visitor_semantic::visit(node_negation* node){
        if(node != NULL){
            if(node->attr_expr) node->attr_expr->accept(this);
            if(attribute_sintetic_int != T_BOOLEAN) semantic_error(E_INVALID_NEGATION,node->attr_line,GET_type_name(attribute_sintetic_int));
        }
    }
    void Visitor_semantic::visit(node_new* node){
        if(node != NULL){
            if(node->attr_info_type != NULL){ // instanciação de um objeto
                char* class_name = table_id->get_lexeme(node->attr_info_type);
                scope* current_scope = GET_current_scope();
                Symbol_class* class_ = GET_class(class_name);
                if(class_ != NULL){
                    node->attr_info_type = class_;
                    attribute_sintetic_char = class_name;
                }else semantic_error(E_UNDECLARED_CLASS,node->attr_line,class_name);
                attribute_sintetic_int = 0 ;
            }else{
                attribute_sintetic_char = NULL;
                attribute_sintetic_int = node->attr_primitive;
                is_array = 1;
                if(node->attr_expr != NULL) node->attr_expr->accept(this);
                if(attribute_sintetic_int != T_INT ) semantic_error(E_INVALID_NEW_ARRAY,node->attr_line,GET_type_name(attribute_sintetic_int));
            }
            attribute_inherited_int = 0;
        }
    }
    void Visitor_semantic::visit(node_primitive* node){
        if(node != NULL){
            // abaixo 'convertemos' os tokens para realizarmos os testes em expressões
            if(node->attr_token == T_NUM_INT) attribute_sintetic_int = T_INT;
            else if(node->attr_token == T_NUM_REAL) attribute_sintetic_int = T_FLOAT;
            else if(node->attr_token == T_LITERAL) attribute_sintetic_int = T_LITERAL;
            else attribute_sintetic_int = T_BOOLEAN;
        }
        attribute_inherited_int = 0;
    }

    void Visitor_semantic::visit(node_op_bitwise* node){
        if(node != NULL){
            int type_left,type_right;
            char* object_left, *object_right;
            attribute_sintetic_char = NULL;
            if(node->attr_expr_1 != NULL) node->attr_expr_1->accept(this);
            type_left = attribute_sintetic_int;
            object_left = GET_type_name(attribute_sintetic_int);

            attribute_sintetic_char = NULL;
            attribute_sintetic_int = 0 ;
            if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
            type_right = attribute_sintetic_int;
            object_right = GET_type_name(attribute_sintetic_int);
            attribute_sintetic_char = NULL;
            if((type_right  != type_left)|| (attribute_sintetic_int == 0 ))
                if(node->attr_operator == T_AND_UNI) semantic_error(E_INVALID_AND_UNI,node->attr_line,object_left,object_right);
                else semantic_error(E_INVALID_OR_UNI,node->attr_line,object_left,object_right);
        }
        attribute_sintetic_int = T_BOOLEAN;
        attribute_inherited_int = 0;
    }

    void Visitor_semantic::visit(node_op_addition* node){
        if(node != NULL){
            int type_left,type_right;
            char* object_left, *object_right;
            attribute_sintetic_char = NULL;
            if(node->attr_expr_1 != NULL) node->attr_expr_1->accept(this);
            type_left = attribute_sintetic_int;
            object_left = GET_type_name(attribute_sintetic_int);

            attribute_sintetic_char = NULL;
            attribute_sintetic_int = 0 ;
            if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
            type_right = attribute_sintetic_int;
            object_right = GET_type_name(attribute_sintetic_int);
            attribute_sintetic_char = NULL;
            //if(type_left == T_INT && type_right == T_FLOAT) type_left = type_right;
            //else if(type_left == T_FLOAT && type_right == T_INT) type_left = type_right;
            if((type_right != type_left) || (type_right != T_INT && type_right != T_FLOAT) )
                if(node->attr_operator == T_PLUS) semantic_error(E_INVALID_ADD,node->attr_line,object_left,object_right);
                else semantic_error(E_INVALID_SUB,node->attr_line,object_left,object_right);
        }
        attribute_inherited_int = 0;
    }
    void Visitor_semantic::visit(node_op_boolean* node){
        if(node != NULL){
            int type_left,type_right;
            char* object_left, *object_right;
            attribute_sintetic_char = NULL;
            if(node->attr_expr_1 != NULL) node->attr_expr_1->accept(this);
            type_left = attribute_sintetic_int;
            object_left = GET_type_name(attribute_sintetic_int);

            attribute_sintetic_char = NULL;
            attribute_sintetic_int = 0 ;
            if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
            type_right = attribute_sintetic_int;
            object_right = GET_type_name(attribute_sintetic_int);
            attribute_sintetic_char = NULL;
            if((type_right  != type_left)|| (type_right != T_BOOLEAN ))
                if(node->attr_operator == T_AND) semantic_error(E_INVALID_AND,node->attr_line,object_left,object_right);
                else if(node->attr_operator == T_OR) semantic_error(E_INVALID_OR,node->attr_line,object_left,object_right);
                else if(type_right == 0) semantic_error(E_INVALID_EQUAL,node->attr_line,object_left,object_right);
                else attribute_sintetic_int = T_BOOLEAN;
            attribute_inherited_int = 0;
        }
    }
    void Visitor_semantic::visit(node_op_multiplication* node){
        attribute_sintetic_int = 0 ;
        if(node != NULL){
            int type_left,type_right;
            char* object_left, *object_right;
            attribute_sintetic_char = NULL;
            if(node->attr_expr_1 != NULL) node->attr_expr_1->accept(this);
            type_left = attribute_sintetic_int;
            object_left = GET_type_name(attribute_sintetic_int);

            attribute_sintetic_char = NULL;
            attribute_sintetic_int = 0 ;
            if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
            type_right = attribute_sintetic_int;
            object_right = GET_type_name(attribute_sintetic_int);
            attribute_sintetic_char = NULL;
            if(type_left == T_INT && type_right == T_FLOAT) type_left = type_right;
            else if(type_left == T_FLOAT && type_right == T_INT) type_left = type_right;
            // testa se o denominador é zero
            if(node->attr_operator == T_DIV && (typeid(*(node->attr_expr_2)) == typeid(node_primitive)) && atoi(((node_primitive*)node->attr_expr_2)->attr_value) == 0 ) semantic_error(E_DIV_BY_ZERO,node->attr_line);
            if((type_right  != type_left)|| (type_right != T_INT && type_right != T_FLOAT)){
                // faz outros testes de multiplicação/divisão inválida
                if(node->attr_operator == T_MULT) semantic_error(E_INVALID_MULT,node->attr_line,object_left,object_right);
                else semantic_error(E_INVALID_DIV,node->attr_line,object_left,object_right);
            }
            attribute_inherited_int = 0;
        }
    }

    void Visitor_semantic::visit(node_op_relational* node){
        if(node != NULL){
            int type_left,type_right;
            char* object_left, *object_right;
            attribute_sintetic_char = NULL;
            if(node->attr_expr_1 != NULL) node->attr_expr_1->accept(this);
            type_left = attribute_sintetic_int;
            object_left = GET_type_name(attribute_sintetic_int);

            attribute_sintetic_char = NULL;
            attribute_sintetic_int = 0 ;
            attribute_inherited_int = 0;
            if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
            type_right = attribute_sintetic_int;
            object_right = GET_type_name(attribute_sintetic_int);
            attribute_sintetic_char = NULL;
            if((type_right  != type_left) || (type_right != T_INT && type_right != T_FLOAT)){
                switch(node->attr_operator){
                    case T_GREATER_E: semantic_error(E_INVALID_GREATER_E,node->attr_line,object_left,object_right);
                    case T_GREATER_T: semantic_error(E_INVALID_GREATER_T,node->attr_line,object_left,object_right);
                    case T_LESS_E:    semantic_error(E_INVALID_LESS_E,node->attr_line,object_left,object_right);
                    case T_LESS_T:    semantic_error(E_INVALID_LESS_T,node->attr_line,object_left,object_right);
                    case T_EQUAL:     semantic_error(E_INVALID_EQUAL,node->attr_line,object_left,object_right);
                    case T_NOT_EQUAL: semantic_error(E_INVALID_NOT_EQUAL,node->attr_line,object_left,object_right);
                }
            }else attribute_sintetic_int = T_BOOLEAN;
            attribute_inherited_int = 0;
        }
    }
    void Visitor_semantic::visit(node_println* node){
        if(node != NULL){
            node_expr_list* e = node->attr_expr_list;
            while(e != NULL){
                e->attr_expr->accept(this);
                if(attribute_sintetic_int == 0){
                    node_name* obj = dynamic_cast<node_name*>(e->attr_expr);
                    if(obj && attribute_sintetic_char != NULL){
                        Symbol_class* class_ = GET_class(attribute_sintetic_char) ;
                        if(class_ != NULL && class_->num_attributo > 0 ){
                             scope* current_scope = class_->current_scope;
                             Symbol* toString = current_scope->table_method->search("toString");
                             obj->attr_expr = new node_call(toString, NULL,e->attr_expr->attr_line);
                             if(toString) e->attr_expr = obj ;
                        }
                    }

                }
                e = e->attr_expr_list;
            }
        }
    }
    void Visitor_semantic::visit(node_program* node){
        if(node != NULL){
            if(node->attr_class_list != NULL ) node->attr_class_list->accept(this);
        }
    }
    void Visitor_semantic::visit(node_read* node){
        if(node != NULL){
            if(node->attr_expr) node->attr_expr->accept(this);
        }
    }
    void Visitor_semantic::visit(node_return* node){
        if(node != NULL){
            char* return_object = current_return_char;
            int return_primitive = current_return_int;
            if(node->attr_expr != NULL){
                attribute_inherited_char = NULL;
                attribute_inherited_int = 0 ;
                node->attr_expr->accept(this);

                char* expected = (return_object == NULL)?(char*)GET_token_name(return_primitive):return_object;
                if(return_primitive != attribute_sintetic_int) semantic_error(E_INVALID_RETURN,node->attr_line,expected,GET_type_name(attribute_sintetic_int));
                else if(attribute_sintetic_char!= NULL && return_object != NULL && strcmp(return_object,attribute_sintetic_char)!= 0)
                        semantic_error(E_INVALID_RETURN,node->attr_line,expected,GET_type_name(attribute_sintetic_int));
                // testa se o return foi no escopo mais baixo do método, o que é preciso para continuar a compilação
                if(GET_scope_level() == SCOPE_METHOD) attribute_sintetic_int = ITS_RETURN;
                else attribute_sintetic_int = 0;
                attribute_inherited_char = NULL;
                attribute_inherited_int = 0 ;
                attribute_sintetic_char = NULL;
            }
        }
    }
    void Visitor_semantic::visit(node_signal* node){
        if(node != NULL){
            node->attr_expr->accept(this);
            if(attribute_sintetic_int != T_INT && attribute_sintetic_int != T_FALSE) semantic_error(E_INVALID_SIGNAL,node->attr_line,GET_type_name(attribute_sintetic_int));
            attribute_sintetic_char = NULL;
        }
    }
    void Visitor_semantic::visit(node_stmt_list* node){
        if(node != NULL){
            if(node->attr_stmt != NULL) node->attr_stmt->accept(this);
            if(node->attr_stmt_list != NULL) node->attr_stmt_list->accept(this);
        }
    }
    void Visitor_semantic::visit(node_type* node){}
    void Visitor_semantic::visit(node_var_list* node){
        attribute_inherited_char = NULL;
        attribute_sintetic_int = 0;
        if(node != NULL){
            if(node->attr_name  != NULL) node->attr_name->accept(this);
            if(node->attr_var_list  != NULL) node->attr_var_list->accept(this);
        }
    }
    void Visitor_semantic::visit(node_while* node){
         if(node!= NULL){
            attribute_sintetic_int = 0;
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            if(attribute_sintetic_int != T_BOOLEAN) semantic_error(E_INVALID_EXPRESSION_WHILE,node->attr_line);
            else{
                begin_scope();
                if(node->attr_stmt != NULL) node->attr_stmt->accept(this);
                end_scope();
            }
            attribute_inherited_int = 0;
        }
    }

    char* Visitor_semantic::GET_type_name(int token){
        return (char*) (attribute_sintetic_char == NULL)?(char*)GET_token_name(token):attribute_sintetic_char;
    }

//  --- Visitor Print ASA ------------

    Visitor_print_AST::Visitor_print_AST(){
        this->level = 0; // nivel raiz da árvore
    }

    void Visitor_print_AST::identation(){
        for(int i=1;i < level;i++) fprintf(stdout,"    ");
    }

    void Visitor_print_AST::visit(node_assign* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_ASSIGN\n");
            if(node->attr_expr_left  != NULL) node->attr_expr_left->accept(this);
            if(node->attr_expr_right != NULL) node->attr_expr_right->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_primitive* node){
        if(node != NULL){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_PRIMITIVE\n");
            level++;
            this->identation();
            fprintf(stdout,"->%s\n",GET_token_name(node->attr_token));
            if(node->attr_token != T_TRUE && node->attr_token != T_FALSE){
                this->identation();
                fprintf(stdout,"->%s\n",node->attr_value);
            }
            level-=2;
        }
    }

    void Visitor_print_AST::visit(node_call* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_CALL\n");
            level++;
            this->identation();
            Symbols_table_id* table = GET_symbol_table_id();
            level--;
            if(node->attr_id != NULL){
                table->print_symbol(node->attr_id);
                fprintf(stdout,"\n");
            }
            if(node->attr_expr_list != NULL) node->attr_expr_list->accept(this);
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_class_list* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_CLASS_LIST\n");
            Symbols_table_id* table = GET_symbol_table_id();
            level++;
            if(node->attr_id != NULL){
                this->identation();
                if(node->attr_id != NULL) table->print_symbol(node->attr_id);
                fprintf(stdout,"\n");
            }

            if(node->attr_id_father != NULL){
                this->identation();
                if(node->attr_id_father != NULL) table->print_symbol(node->attr_id_father);
                fprintf(stdout,"\n");
            }
            level--;
            if(node->attr_var_list  != NULL) node->attr_var_list->accept(this);
            if(node->attr_method_list  != NULL) node->attr_method_list->accept(this);
            if(node->attr_class_list  != NULL) node->attr_class_list->accept(this);
            level--;
        }
    }


    void Visitor_print_AST::visit(node_expr_list* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_EXPR_LIST\n");
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            if(node->attr_expr_list  != NULL) node->attr_expr_list->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_if* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_IF\n");
            if(node->attr_expr      != NULL) node->attr_expr->accept(this);
            if(node->attr_stmt      != NULL) node->attr_stmt->accept(this);
            if(node->attr_stmt_else != NULL) node->attr_stmt_else->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_length* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_LENGTH\n");
            level--;
        }
    }

    void Visitor_print_AST::visit(node_method_list* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_METHOD_LIST\n");
            if(node->attr_type  != NULL) node->attr_type->accept(this);
            level++;
            if(node->attr_id != NULL){
                Symbols_table_id* table = GET_symbol_table_id();
                this->identation();
                table->print_symbol(node->attr_id);
                fprintf(stdout,"\n");
            }
            level--;
            if(node->attr_param_list  != NULL) node->attr_param_list->accept(this);
            if(node->attr_stmt_list       != NULL) node->attr_stmt_list->accept(this);
            if(node->attr_method_list != NULL) node->attr_method_list->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_this* node){
        if(node != NULL){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_THIS\n");
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_name* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_NAME\n");
            if(node->attr_id != NULL){
                level++;
                this->identation();
                Symbols_table_id* table = GET_symbol_table_id();
                table->print_symbol(node->attr_id);
                fprintf(stdout,"\n");
                level--;
            }
            if(node->attr_expr  != NULL) node->attr_expr->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_vector* node){
        if(node != NULL){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_VECTOR\n");
            if(node->attr_name  != NULL) node->attr_name->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_name_decl* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_NAME_DECL\n");
            if(node->attr_type != NULL) node->attr_type->accept(this);
            if(node->attr_id   != NULL){
                level++;
                this->identation();
                Symbols_table_id* table = GET_symbol_table_id();
                table->print_symbol(node->attr_id);
                fprintf(stdout,"\n");
                level--;
            }
            if(node->attr_assign != NULL) node->attr_assign->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_negation* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_NEGATION\n");
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_new* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_NEW\n");
            level++;
            this->identation();
            if(node->attr_info_type != NULL){
                Symbols_table_id* table = GET_symbol_table_id();
                table->print_symbol(node->attr_info_type);
                fprintf(stdout,"\n");
            }
            else{
                fprintf(stdout,"->%s\n",GET_token_name(node->attr_primitive));
                if(node->attr_expr != NULL) node->attr_expr->accept(this);
            }
            level-=2 ;
        }
    }

    void Visitor_print_AST::visit(node_op_bitwise* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_OP_BITWISE\n");
            level++;
            this->identation();
            fprintf(stdout,"->%s\n",GET_token_name(node->attr_operator));
            level--;
            if(node->attr_expr_1  != NULL)  node->attr_expr_1->accept(this);
            if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_op_addition* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_OP_ADDITION\n");
            level++;
            this->identation();
            fprintf(stdout,"->%s\n", GET_token_name(node->attr_operator));
            level--;
            if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
            if(node->attr_expr_2 != NULL) node->attr_expr_2->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_op_boolean* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_OP_BOOLEAN\n");
            level++;
            this->identation();
            fprintf(stdout,"->%s\n",GET_token_name(node->attr_operator));
            level--;
            if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
            if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
            level--;

        }
    }


    void Visitor_print_AST::visit(node_op_multiplication* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_OP_MULTIPLICATION\n");
            level++;
            this->identation();
            fprintf(stdout,"->%s\n",GET_token_name(node->attr_operator));
            level--;
            if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
            if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_op_relational* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_OP_RELATIONAL\n");
            level++;
            this->identation();
            fprintf(stdout,"->%s\n",GET_token_name(node->attr_operator));
            level--;
            if(node->attr_expr_1  != NULL) node->attr_expr_1->accept(this);
            if(node->attr_expr_2  != NULL) node->attr_expr_2->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_println* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_PRINTLN\n");
            if(node->attr_expr_list != NULL) node->attr_expr_list->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_program* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_PROGRAM\n");
            if(node->attr_class_list != NULL) node->attr_class_list->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_read* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_READ\n");
            if(node->attr_expr != NULL) node->attr_expr->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_return* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_RETURN\n");
            if(node->attr_expr  != NULL) node->attr_expr->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_signal* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_SIGNAL\n");
            if(node->attr_expr  != NULL) node->attr_expr->accept(this);
            level--;
        }
    }



    void Visitor_print_AST::visit(node_stmt_list* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_STMT_LIST\n");
            if(node->attr_stmt  != NULL) node->attr_stmt->accept(this);
            if(node->attr_stmt_list  != NULL) node->attr_stmt_list->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_type* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_TYPE\n");
            level++;
            this->identation();
            if(node->attr_id == NULL){
                fprintf(stdout,"->%s",GET_token_name(node->attr_primitive));
                if(node->attr_array) fprintf(stdout,"[]");
                fprintf(stdout,"\n");
            }
            else{
                Symbols_table_id* table = GET_symbol_table_id();
                table->print_symbol(node->attr_id);
                fprintf(stdout,"\n");
            }
            level-= 2;
        }
    }

    void Visitor_print_AST::visit(node_var_list* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_VAR_LIST\n");
            if(node->attr_name  != NULL) node->attr_name->accept(this);
            if(node->attr_var_list  != NULL) node->attr_var_list->accept(this);
            level--;
        }
    }

    void Visitor_print_AST::visit(node_while* node){
        if(node != NULL ){
            level++;
            this->identation();
            fprintf(stdout,"->NODE_WHILE\n");
            if(node->attr_expr  != NULL) node->attr_expr->accept(this);
            if(node->attr_stmt  != NULL) node->attr_stmt->accept(this);
            level--;
        }
    }
