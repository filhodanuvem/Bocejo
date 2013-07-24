#ifndef VISITOR_TRANSLATEASAIR_H_INCLUDED
#define VISITOR_TRANSLATEASAIR_H_INCLUDED

#include "visitors.h"
#include "visitor_fragment.h"

class ir_binop;
class ir_call;
class ir_cjump;
class ir_const;
class ir_constf;
class ir_eseq;
class ir_expr;
class ir_expr_void;
class ir_jump;
class ir_label;
class ir_mem;
class ir_move;
class ir_name;
class ir_seq;
class ir_stmt_list;
class ir_stmt;
class ir_temp;

class temp{
    public:
        temp();
        temp(char* string);
        ~temp();
        char* toString();
        char* attr_string;
        static int attr_num;
        static int attr_mod;
};

class label{
    public:
        label();
        label(char* string);
        ~label();
        char* toString();
        char* attr_string;
        int attr_id;
        static int attr_num;
        static int attr_mod;
};
class LabelList{
    public:
        LabelList(label* l, LabelList* prox);
        ~LabelList();
        label* attr_label;
        LabelList* attr_next;

};

class AcessoLocal{
    public:
        virtual ir_expr* codigoAcesso()=0;
};
class inFrame: public AcessoLocal{
    public:
        inFrame(int offset);
        ~inFrame();
        ir_expr* codigoAcesso();
        ir_expr* codigoEndereco();
        int attr_offset;
        ir_temp* fp;
};
class inReg: public AcessoLocal{
    public:
        inReg(temp* temp);
        ~inReg();
        ir_expr* codigoAcesso();
        temp* attr_temp;
};
class ListaAcesso{
    public:
        ListaAcesso(AcessoLocal* l, ListaAcesso* next);
        ~ListaAcesso();
        AcessoLocal* attr_l;
        ListaAcesso* attr_next;
};

class Frame{
    public:
        virtual AcessoLocal* adicionaParam(bool scape, int tamBytes)=0;
        virtual AcessoLocal* adicionaLocal(bool scape, int tamBytes)=0;

};
class FrameMIPS: public Frame{
    public:
        FrameMIPS(label* l, temp* t, ListaAcesso* la,ListaAcesso* lp);
        ~FrameMIPS();
        AcessoLocal* adicionaParam(bool scape, int tamBytes);
        AcessoLocal* adicionaLocal(bool scape, int tamBytes);

        static       temp* fp;
        static       temp* zero;
        static int   wordSize;
        int          attr_size;
        AcessoLocal* attr_this;
        label*       attr_label;
        temp*        attr_temp;
        ListaAcesso* attr_ListaAcesso;
        ListaAcesso* attr_ListaAcessoParam;

};

class Fragment{
    public:
        Fragment* attr_next;
        virtual void accept(Visitor_fragment* visitor) = 0;
};
class Literal: public Fragment{
    public:
        Literal(char* string);
        ~Literal();
        void accept(Visitor_fragment* visitor);
        char* attr_string;
        label* attr_label;
};
class Procedure: public Fragment{
    public:
        Procedure(Frame* frame, ir_stmt* body);
        ~Procedure();
        void accept(Visitor_fragment* visitor);
        Frame* attr_frame;
        ir_stmt_list* attr_body;
};




class Visitor_Translate_IR: public Visitor{
     public:
        Visitor_Translate_IR();
        void visit(node_assign* node);
        void visit(node_primitive* node);
        void visit(node_call* node);
        void visit(node_class_list* node);
        void visit(node_expr_list* node);
        void visit(node_if* node);
        void visit(node_length* node);
        void visit(node_method_list* node);
        void visit(node_this* node);
        void visit(node_name* node);
        void visit(node_vector* node);
        void visit(node_name_decl* node);
        void visit(node_negation* node);
        void visit(node_new* node);
        void visit(node_op_bitwise* node);
        void visit(node_op_addition* node);
        void visit(node_op_boolean* node);
        void visit(node_op_multiplication* node);
        void visit(node_op_relational* node);
        void visit(node_println* node);
        void visit(node_program* node);
        void visit(node_read* node);
        void visit(node_return* node);
        void visit(node_signal* node);
        void visit(node_stmt_list* node);
        void visit(node_type* node);
        void visit(node_var_list* node);
        void visit(node_while* node);
        Fragment* getFirstFragment();
        Fragment* getCurrentFragment();
        ir_expr* AlocaArray(int quant);
        ir_seq* initObject(ir_expr* base, Symbol_class* classe);
        ir_expr* AlocaObject(Symbol_class* classe);
        ir_seq* initArray(ir_expr* base,int quant);
        label* label_main ;
    private:
        Fragment* first_fragment;
        Fragment* last_fragment;
        Fragment* current_fragment;
        FrameMIPS* current_frame;
        ir_stmt*  stmtGERAL;
        ir_expr*  exprGERAL;
        ir_binop*  exprNAME_LIST;
        Symbol_class* current_class_NAME_LIST;

        Symbol_class* current_class;
        Symbols_table_id* table_class;

};


#endif // VISITOR_TRANSLATEASAIR_H_INCLUDED
