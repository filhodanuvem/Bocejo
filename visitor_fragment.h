#ifndef VISITOR_PRINT_FRAGMENT_H_INCLUDED
#define VISITOR_PRINT_FRAGMENT_H_INCLUDED

#include "visitor_ir.h"
class Fragment;
class Procedure;
class Literal;
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
class ir_stmt;
class ir_temp;
class ir_stmt_list;
class label;

class Visitor_fragment{
    public:
        virtual Fragment* visit(Procedure* fragment)=0;
        virtual void visit(Literal* literal)=0;
};


class Visitor_print_fragment:public Visitor_fragment{
    public:
        Visitor_print_fragment();
        Fragment* visit(Procedure* fragment);
        void visit(Literal* literal);
    private:
        void identation();
        int level;
};

class Visitor_mips:public Visitor_fragment {
    public:
        Visitor_mips(label* label_main);
        Fragment* visit(Procedure* fragment);
        void visit(Literal* literal);
    private:
        Visitor_muncher* muncher;
        label* label_main;
};

#endif // VISITOR_PRINT_FRAGMENT_H_INCLUDED
