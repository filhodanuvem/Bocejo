#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "IR.h"

ir_expr::~ir_expr(){}
ir_stmt::~ir_stmt(){}

ir_stmt_list::ir_stmt_list(ir_stmt* stmt, ir_stmt_list* stmt_list){
    attr_stmt = stmt;
    attr_stmt_list = stmt_list;
}

ir_stmt_list::~ir_stmt_list(){
    delete attr_stmt;
    delete attr_stmt_list;
}

ir_expr_list::ir_expr_list(ir_expr* e,ir_expr_list* el){
    attr_expr      = e;
    attr_expr_list = el;
}
ir_expr_list::~ir_expr_list(){
    delete attr_expr;
    delete attr_expr_list;
}

//Extends EXPR
ir_const::ir_const(int i){
    attr_i = i;
}

ir_const::~ir_const(){}

ir_constf::ir_constf(float f){
    attr_f = f;
}

ir_constf::~ir_constf(){}


ir_name::ir_name(label* n){
    attr_name = n;
}

ir_name::~ir_name(){
    delete attr_name;
}

ir_temp::ir_temp(temp* t){
    attr_temp = t;
}

ir_temp::~ir_temp(){
    delete attr_temp;
}


ir_binop::ir_binop(int op, ir_expr* e1, ir_expr* e2){
    attr_op = op;
    attr_exp1 = e1;
    attr_exp2 = e2;
}

ir_binop:: ~ir_binop(){
    delete attr_exp1;
    delete attr_exp2;
}

ir_mem::ir_mem(ir_expr* e){
    attr_expr = e;
}

ir_mem:: ~ir_mem(){
    delete attr_expr;
}

ir_call::ir_call(ir_name* n, ir_expr_list* el){
    attr_name = n;
    attr_el   = el;
    attr_params = NULL;
}
ir_call::~ir_call(){
    delete attr_name;
    delete attr_el;
}

ir_eseq::ir_eseq(ir_stmt* s, ir_expr* e){
    attr_stmt = s;
    attr_expr = e;
}

ir_eseq:: ~ir_eseq(){
    delete attr_stmt;
    delete attr_expr;
}


//Extends STMT
ir_move:: ir_move(ir_expr* e1, ir_expr* e2){
    attr_expr1 = e1;
    attr_expr2 = e2;
}


ir_move:: ~ir_move(){
    delete attr_expr1;
    delete attr_expr2;
}


ir_expr_void:: ir_expr_void(ir_expr* e){
    attr_expr = e;
}

ir_expr_void::~ir_expr_void(){
    delete attr_expr;
}


ir_jump:: ir_jump(ir_expr* e, LabelList* labels){
    attr_expr = e;
    attr_labels = labels;
}

ir_jump:: ~ir_jump(){
    delete attr_expr;
    delete attr_labels;
}

ir_cjump::ir_cjump(int op, ir_expr* e1, ir_expr* e2, label* t, label* f){
    attr_op    = op;
    attr_expr1 = e1;
    attr_expr2 = e2;
    attr_true  = t;
    attr_false = f;
}

ir_cjump:: ~ir_cjump(){
    delete attr_expr1;
    delete attr_expr2;
    delete attr_true;
    delete attr_false;
}

ir_seq::ir_seq(ir_stmt* s1, ir_stmt* s2){
    attr_stmt1 = s1;
    attr_stmt2 = s2;
}

ir_seq:: ~ir_seq(){
    delete attr_stmt1;
    delete attr_stmt2;
}

ir_label:: ir_label(label* n){
    attr_n = n;
}

ir_label:: ~ir_label(){
    delete attr_n;
}

ir_params::ir_params(ir_stmt_list* stmt_list){
    attr_sl = stmt_list;
}

ir_params::~ir_params(){
    delete attr_sl;
}

ir_expr* ir_expr_list::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
    }
ir_expr* ir_binop::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
    }
ir_expr* ir_call::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
    }
ir_stmt* ir_cjump::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_const::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_constf::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_eseq::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_stmt* ir_expr_void::accept(Visitor_IR* visitor){
    if(visitor != NULL )  return visitor->visit(this);
}
ir_stmt* ir_jump::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_stmt* ir_label::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_mem::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_stmt* ir_move::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_name::accept(Visitor_IR* visitor){
    if(visitor != NULL )return visitor->visit(this);
}
ir_stmt* ir_seq::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_temp::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_stmt_list* ir_stmt_list::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}
ir_expr* ir_params::accept(Visitor_IR* visitor){
    if(visitor != NULL ) return visitor->visit(this);
}

