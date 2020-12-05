#pragma once
#include <cctype>
#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>

#include "CallGraph.h"
#include "Lexer.h"
#include "SimplifiedAST.h"

namespace Parser {
enum class Symbol {
    name,
    number,
    read,
    print,
    call,
    while_stmt,
    if_stmt,
    else_stmt,
    then,
    cond_expr,
    rel_expr,
    rel_factor,
    expr,
    term,
    factor,
    open_brace,    //{
    close_brace,   //}
    open_bracket,  //(
    close_bracket, //)
    arithmetic,    //+ | - | * | / | %
    compare,       //  > | < | <= | >=
    ands,          //&&
    ors,           //||
    equal,         //==
    assign,        //=
    procedure,
    end,  // ;
    nots, //!
    neq,  //!=
    stop
};

// API
SimplifiedAST parse(Lexer::TokenList tokens);
CallGraph getCallGraph();
}; // namespace Parser

// private functions for Parser
namespace {
CallGraph getGraph();
SimplifiedAST parseTokens(Lexer::TokenList tokens);
StatementWrapper testParse(Lexer::TokenList tokens);
void getNameSym(Parser::Symbol s);
void nextSym();
bool accept(Parser::Symbol s);
bool expect(Parser::Symbol s);
bool acceptAndPush(Parser::Symbol s);
bool expectAndPush(Parser::Symbol s);
Procedure procedure();
StatementWrapper statement();
StatementWrapper readStatement();
StatementWrapper printStatement();
StatementWrapper callStatement();
StatementWrapper whileStatement();
StatementWrapper ifStatement();
StatementWrapper assignStatement();
/*
 * Description: Processes and stores cond_expr as a string, then calls cond_expr_splitter to begin validation
 */
void cond_expr();
/*
 * Description: Splits the cond_expr received into individual rel_expr
 */
void cond_expr_splitter(std::string str);
/*
 * Description: Processes the rel_expr into rel_factor
 */
void rel_expr(std::string str);
/*
 * Description: Validates the rel_factor
 */
void rel_factor(std::string str);
/*
 * Description: Checks if the string has balanced brackets
 */
bool isBalanced(std::string str);
/*
 * Description: Validates an expr using recursive descent
 */
void expr();
}; // namespace
