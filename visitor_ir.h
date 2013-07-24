#ifndef VISITOR_IR_H_INCLUDED
#define VISITOR_IR_H_INCLUDED

class ir_binop;
class ir_call;
class ir_cjump;
class ir_const;
class ir_constf;
class ir_eseq;
class ir_expr;
class ir_expr_list;
class ir_expr_void;
class ir_jump;
class ir_label;
class ir_mem;
class ir_move;
class ir_name;
class ir_seq;
class ir_stmt;
class ir_stmt_list;
class ir_temp;
class ir_params;
class FrameMIPS;

class Visitor_IR{
    public:
        virtual ir_expr* visit(ir_binop* node)=0;
        virtual ir_expr* visit(ir_call* node)=0;
        virtual ir_stmt* visit(ir_cjump* node)=0;
        virtual ir_expr* visit(ir_const* node)=0;
        virtual ir_expr* visit(ir_constf* node)=0;
        virtual ir_expr* visit(ir_eseq* node)=0;
        virtual ir_stmt* visit(ir_expr_void* node)=0;
        virtual ir_stmt* visit(ir_jump* node)=0;
        virtual ir_stmt* visit(ir_label* node)=0;
        virtual ir_expr* visit(ir_mem* node)=0;
        virtual ir_stmt* visit(ir_move* node)=0;
        virtual ir_expr* visit(ir_name* node)=0;
        virtual ir_stmt* visit(ir_seq* node)=0;
        virtual ir_expr* visit(ir_temp* node)=0;
        virtual ir_expr* visit(ir_expr_list* node)=0;
        virtual ir_expr* visit(ir_params* node)=0;
        virtual ir_stmt_list* visit(ir_stmt_list* node)=0;
};


class Visitor_Canonic:public Visitor_IR{
    public:
        Visitor_Canonic();
        ir_expr* visit(ir_binop* node);
        ir_expr* visit(ir_call* node);
        ir_stmt* visit(ir_cjump* node);
        ir_expr* visit(ir_const* node);
        ir_expr* visit(ir_constf* node);
        ir_expr* visit(ir_eseq* node);
        ir_stmt* visit(ir_expr_void* node);
        ir_stmt* visit(ir_jump* node);
        ir_stmt* visit(ir_label* node);
        ir_expr* visit(ir_mem* node);
        ir_stmt* visit(ir_move* node);
        ir_expr* visit(ir_name* node);
        ir_stmt* visit(ir_seq* node);
        ir_expr* visit(ir_temp* node);
        ir_expr* visit(ir_expr_list* node);
        ir_expr* visit(ir_params* node);
        ir_stmt_list* visit(ir_stmt_list* node);
        ir_stmt* remove_seq(ir_stmt* node);
        ir_expr* remove_seq(ir_expr_list* node);
        ir_stmt_list* remove_seq(ir_stmt_list* node);
        //ir_expr_list* remove_seq(ir_expr_list* node);
        int has_change;
};


class Visitor_print_ir:public Visitor_IR{
    public:
        Visitor_print_ir();
        ir_expr* visit(ir_binop* node);
        ir_expr* visit(ir_call* node);
        ir_stmt* visit(ir_cjump* node);
        ir_expr* visit(ir_const* node);
        ir_expr* visit(ir_constf* node);
        ir_expr* visit(ir_eseq* node);
        ir_stmt* visit(ir_expr_void* node);
        ir_stmt* visit(ir_jump* node);
        ir_stmt* visit(ir_label* node);
        ir_expr* visit(ir_mem* node);
        ir_stmt* visit(ir_move* node);
        ir_expr* visit(ir_name* node);
        ir_stmt* visit(ir_seq* node);
        ir_expr* visit(ir_temp* node);
        ir_expr* visit(ir_params* node);
        ir_expr* visit(ir_expr_list* node);
        ir_stmt_list*  visit(ir_stmt_list* node);
    private:
        void identation();
        int level;

};


class Visitor_muncher:public Visitor_IR{
    public:
        Visitor_muncher();
        char* munch(ir_binop* node);
        char* munch(ir_call* node);
        char* munch(ir_cjump* node);
        char* munch(ir_const* node);
        char* munch(ir_constf* node);
        char* munch(ir_eseq* node);
        char* munch(ir_expr_void* node);
        char* munch(ir_params* node);
        char* munch(ir_jump* node);
        char* munch(ir_label* node);
        char* munch(ir_mem* node);
        char* munch(ir_move* node);
        char* munch(ir_name* node);
        char* munch(ir_seq* node);
        char* munch(ir_temp* node);
        char* munch(ir_expr* node); // genérico que contem o case
        char* munch(ir_stmt* node); // genérico que contem o case


        ir_expr* visit(ir_binop* node);
        ir_expr* visit(ir_call* node);
        ir_stmt* visit(ir_cjump* node);
        ir_expr* visit(ir_const* node);
        ir_expr* visit(ir_constf* node);
        ir_expr* visit(ir_eseq* node);
        ir_stmt* visit(ir_expr_void* node);
        ir_stmt* visit(ir_jump* node);
        ir_stmt* visit(ir_label* node);
        ir_expr* visit(ir_mem* node);
        ir_stmt* visit(ir_move* node);
        ir_expr* visit(ir_name* node);
        ir_stmt* visit(ir_seq* node);
        ir_expr* visit(ir_temp* node);
        ir_expr* visit(ir_params* node);
        ir_expr* visit(ir_expr_list* node);
        ir_stmt_list* visit(ir_stmt_list* node);
        FrameMIPS* current_frame; // guarda o frame atual
    private:
        int bocejo_helper;
        int current_temp ;
        int dest_temp;

};




#endif // VISITOR_IR_H_INCLUDED
