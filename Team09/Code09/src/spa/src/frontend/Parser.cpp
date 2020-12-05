#include "Parser.h"
#include <regex>
#include <sstream>

namespace Parser {
SimplifiedAST parse(Lexer::TokenList tl) {
    return parseTokens(tl);
}

CallGraph getCallGraph() {
    return getGraph();
}
} // namespace Parser

namespace {
Parser::Symbol sym;
int stmtNum;
CallGraph call;
std::string currProcedure;
std::vector<std::string> tokens;
std::vector<Lexer::Token>::iterator it;

void next() {
    if (sym != Parser::Symbol::stop) {
        ++it;
    }
}

bool accept(Parser::Symbol s) {
    if (s != Parser::Symbol::name && sym == Parser::Symbol::name) {
        getNameSym(s);
    }
    if (sym == s) {
        nextSym();
        return true;
    }
    return false;
}

bool expect(Parser::Symbol s) {
    if (accept(s)) {
        return true;
    }
    std::string errorMessage = "Err: received unexpected token: " + (--it)->value + "in stmt " + std::to_string(--stmtNum);
    stmtNum++;
    it++;
    throw std::invalid_argument(errorMessage);
    return false;
}

bool acceptAndPush(Parser::Symbol s) {
    if (s != Parser::Symbol::name && sym == Parser::Symbol::name) {
        getNameSym(s);
    }
    if (sym == s) {
        std::string val = (--it)->value;
        it++;
        tokens.push_back(val);
        nextSym();
        return true;
    }
    return false;
}

bool expectAndPush(Parser::Symbol s) {
    if (acceptAndPush(s)) {
        return true;
    }
    std::string errorMessage = "Err: received unexpected token: " + (--it)->value + "in stmt " + std::to_string(--stmtNum);
    stmtNum++;
    it++;
    throw std::invalid_argument(errorMessage);
    return false;
}

CallGraph getGraph() {
    if (call.isEmpty()) {
        std::string errorMessage = "Err: no procedures were found.";
        throw std::invalid_argument(errorMessage);
    } else if (call.isCyclic()) {
        std::string errorMessage = "Err: cyclic call detected.";
        throw std::invalid_argument(errorMessage);
    }
    return call;
}

SimplifiedAST parseTokens(Lexer::TokenList tl) {
    SimplifiedAST ast;
    call = CallGraph();
    tokens.clear();
    stmtNum = 1;
    it = tl.begin();

    nextSym();
    while (!accept(Parser::Symbol::stop)) {
        expect(Parser::Symbol::procedure);
        Procedure pro = procedure();
        ast.addProcedure(pro);
    }
    return ast;
}

StatementWrapper testParse(Lexer::TokenList tl) {
    tokens.clear();
    stmtNum = 1;
    it = tl.begin();
    nextSym();
    return statement();
}

void getNameSym(Parser::Symbol s) {
    std::string value = (--it)->value;
    it++;
    if (s == Parser::Symbol::procedure && value == "procedure") {
        sym = Parser::Symbol::procedure;
    } else if (s == Parser::Symbol::else_stmt && value == "else") {
        sym = Parser::Symbol::else_stmt;
    } else if (s == Parser::Symbol::then && value == "then") {
        sym = Parser::Symbol::then;
    }
}

void nextSym() {
    std::string val = it->value;
    Lexer::TokenType type = it->type;
    switch (type) {
    case Lexer::TokenType::STOP:
        sym = Parser::Symbol::stop;
        break;
    case Lexer::TokenType::INTEGER:
        if (val.at(0) == '0' && val.size() > 1) {
            std::string errorMessage = "Integers greater than 0 cannot start with 0.";
            throw std::invalid_argument(errorMessage);
        }
        sym = Parser::Symbol::number;
        break;
    case Lexer::TokenType::NAME:
        sym = Parser::Symbol::name;
        break;
    case Lexer::TokenType::SYMBOL:
        if (val == "{") {
            sym = Parser::Symbol::open_brace;
        } else if (val == "}") {
            sym = Parser::Symbol::close_brace;
        } else if (val == "(") {
            sym = Parser::Symbol::open_bracket;
        } else if (val == ")") {
            sym = Parser::Symbol::close_bracket;
        } else if (val == "==") {
            sym = Parser::Symbol::equal;
        } else if (val == "=") {
            sym = Parser::Symbol::assign;
        } else if (val == "+" || val == "-" || val == "/" || val == "*" || val == "%") {
            sym = Parser::Symbol::arithmetic;
        } else if (val == ">" || val == "<" || val == ">=" || val == "<=") {
            sym = Parser::Symbol::compare;
        } else if (val == "!") {
            sym = Parser::Symbol::nots;
        } else if (val == "!=") {
            sym = Parser::Symbol::neq;
        } else if (val == "||") {
            sym = Parser::Symbol::ors;
        } else if (val == "&&") {
            sym = Parser::Symbol::ands;
        } else if (val == ";") {
            sym = Parser::Symbol::end;
        } else {
            std::string errorMessage = "Err: Received an invalid symbol. Received: " + val;
            throw std::invalid_argument(errorMessage);
        }
        break;
    }
    next();
}

Procedure procedure() {
    tokens.clear();
    std::string name = (--it)->value;
    it++;
    if (accept(Parser::Symbol::name)) {
        Procedure pro(name);
        currProcedure = name;
        call.addProcedure(name);
        expect(Parser::Symbol::open_brace);

        StatementWrapper stmt = statement();
        pro.addStatement(stmt);

        while (!accept(Parser::Symbol::close_brace)) {
            StatementWrapper stmt = statement();
            pro.addStatement(stmt);
        }

        return pro;
    } else {
        std::string errorMessage = "Err: did not receive a procedure name. Received: " + (--it)->value;
        it++;
        throw std::invalid_argument(errorMessage);
    }
}

StatementWrapper statement() {
    tokens.clear();
    std::string value = (--it)->value;
    it++;
    expectAndPush(Parser::Symbol::name);

    if (value == "read") {
        if (acceptAndPush(Parser::Symbol::assign)) {
            return assignStatement();
        }
        return readStatement();
    } else if (value == "while") {
        if (acceptAndPush(Parser::Symbol::assign)) {
            return assignStatement();
        }
        return whileStatement();
    } else if (value == "if") {
        if (acceptAndPush(Parser::Symbol::assign)) {
            return assignStatement();
        }
        return ifStatement();
    } else if (value == "print") {
        if (acceptAndPush(Parser::Symbol::assign)) {
            return assignStatement();
        }
        return printStatement();
    } else if (value == "call") {
        if (acceptAndPush(Parser::Symbol::assign)) {
            return assignStatement();
        }
        return callStatement();
    } else {
        expectAndPush(Parser::Symbol::assign);
        return assignStatement();
    }
}

StatementWrapper readStatement() {
    int num = stmtNum++;
    std::string type = READ;

    if (acceptAndPush(Parser::Symbol::name)) {
        expectAndPush(Parser::Symbol::end);
    } else {
        std::string errorMessage = "Err: did not receive variable for read statement.";
        throw std::invalid_argument(errorMessage);
    }
    StatementWrapper stmt{num, type, tokens};
    return stmt;
}

StatementWrapper printStatement() {
    int num = stmtNum++;
    std::string type = PRINT;

    if (acceptAndPush(Parser::Symbol::name)) {
        expectAndPush(Parser::Symbol::end);
    } else {
        std::string errorMessage = "Err: did not receive variable for print statement.";
        throw std::invalid_argument(errorMessage);
    }
    StatementWrapper stmt{num, type, tokens};
    return stmt;
}

StatementWrapper callStatement() {
    int num = stmtNum++;
    std::string type = CALL;

    std::string name = (--it)->value;
    it++;
    if (acceptAndPush(Parser::Symbol::name)) {
        call.addProcedureCall(currProcedure, name);
        expectAndPush(Parser::Symbol::end);
    } else {
        std::string errorMessage = "Err: did not receive a procedure name for call statement.";
        throw std::invalid_argument(errorMessage);
    }
    StatementWrapper stmt{num, type, tokens};
    return stmt;
}

StatementWrapper whileStatement() {
    int num = stmtNum++;
    std::vector<StatementWrapper> stmtLst;
    std::string type = WHILE_STMT;

    cond_expr();
    if (accept(Parser::Symbol::open_brace)) {
        StatementWrapper stmt{num, type, tokens};
        stmtLst.push_back(statement());

        while (!accept(Parser::Symbol::close_brace)) {
            stmtLst.push_back(statement());
        }
        stmt.setContainerStatementList(stmtLst);
        return stmt;
    } else {
        std::string errorMessage = "Err: While statement format error. Did not receive {";
        throw std::invalid_argument(errorMessage);
    }
}

StatementWrapper ifStatement() {
    int num = stmtNum++;
    std::string type = IF_STMT;
    std::vector<StatementWrapper> ifStmtLst;
    std::vector<StatementWrapper> elseStmtLst;

    cond_expr();
    if (accept(Parser::Symbol::then)) {
        expect(Parser::Symbol::open_brace);
        StatementWrapper stmt{num, type, tokens};
        ifStmtLst.push_back(statement());

        while (!accept(Parser::Symbol::close_brace)) {
            ifStmtLst.push_back(statement());
        }

        expectAndPush(Parser::Symbol::else_stmt);
        expect(Parser::Symbol::open_brace);
        elseStmtLst.push_back(statement());
        while (!accept(Parser::Symbol::close_brace)) {
            elseStmtLst.push_back(statement());
        }
        stmt.setIfStatementList(ifStmtLst);
        stmt.setElseStatementList(elseStmtLst);
        return stmt;
    } else {
        std::string errorMessage = "Err: If statement format error. Did not find 'then'.";
        throw std::invalid_argument(errorMessage);
    }
}

StatementWrapper assignStatement() {
    std::string type = ASSIGN;
    expr();
    expectAndPush(Parser::Symbol::end);

    StatementWrapper stmt{stmtNum++, type, tokens};
    return stmt;
}

void cond_expr() {
    expectAndPush(Parser::Symbol::open_bracket);
    int count = 1;
    std::string cond = "(";

    while (count != 0) {
        if (acceptAndPush(Parser::Symbol::close_bracket)) {
            count--;
            cond += ")";
        } else if (acceptAndPush(Parser::Symbol::name) || acceptAndPush(Parser::Symbol::number) ||
                   acceptAndPush(Parser::Symbol::arithmetic) || acceptAndPush(Parser::Symbol::compare) ||
                   acceptAndPush(Parser::Symbol::equal) || acceptAndPush(Parser::Symbol::neq)) {
            cond += (--(--it))->value + " ";
            it++;
            it++;
        } else if (acceptAndPush(Parser::Symbol::ands) || acceptAndPush(Parser::Symbol::ors) ||
                   acceptAndPush(Parser::Symbol::nots)) {
            cond += (--(--it))->value;
            it++;
            it++;
        } else if (acceptAndPush(Parser::Symbol::open_bracket)) {
            count++;
            cond += "(";
        } else {
            std::string errorMessage = "Unexpected character found in cond_expr. Received: " + (--it)->value;
            it++;
            throw std::invalid_argument(errorMessage);
        }
    }
    cond_expr_splitter(cond);
}

/*
 * Counts the diff between open and close brackets to identify the outermost cond_exprs
 * and splits into the two outermost cond_exprs.
 * This is to ensure proper bracket matching when splitting the cond_expr into rel_expr.
 * For example, in "if (((a * m <= p) && ((a + v > p) || (v != p))) || (a > b)) then {",
 * the cond_expr:  (((a * m <= p) && ((a + v > p) || (v != p))) || (a > b))
 * is split into ->((a * m <= p) && ((a + v > p) || (v != p))) and (a > b)
 *				-> (a * m <= p) and ((a + v > p) || (v != p)) and (a > b)
 *				-> (a * m <= p) and (a + v > p) and (v != p) and (a > b)
 */
void cond_expr_splitter(std::string str) {
    int count = 0;
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        if (*it == '(') {
            count++;
        }
        if (*it == ')') {
            count--;
        }
        if (*it == '&' || *it == '|') {
            if (count == 0) {
                std::string errorMessage = "Err: Improper use of brackets in cond_expr";
                throw std::invalid_argument(errorMessage);
            }
            if (count == 1) {
                int index = std::distance(str.begin(), ++it);
                cond_expr_splitter(str.substr(1, (index - 2)));
                std::string second = str.substr(index + 1);
                cond_expr_splitter(second.substr(0, second.length() - 1));
                return;
            }
        }
        if (count < 0) {
            std::string errorMessage = "Err: Improper use of brackets in cond_expr";
            throw std::invalid_argument(errorMessage);
        }
    }

    if (count != 0) {
        std::string errorMessage = "Err: Improper use of brackets in cond_expr";
        throw std::invalid_argument(errorMessage);
    }
    rel_expr(str);
}

void rel_expr(std::string str) {
    if (str.at(0) != '(' || str.at(str.length() - 1) != ')') {
        std::string errorMessage = "Err: Improper use of brackets in cond_expr";
        throw std::invalid_argument(errorMessage);
    }
    if (str.at(1) == '!') {
        if (str.at(2) != '(' || str.at(str.length() - 2) != ')') {
            std::string errorMessage = "Err: Improper use of brackets in cond_expr";
            throw std::invalid_argument(errorMessage);
        }
        // if a ! is used, because of the additional bracket wrap, the counting is compromised,
        // so this substring is sent back to be split further if necessary
        cond_expr_splitter(str.substr(2, str.length() - 3));
    } else {
        int pos = 0;
        str = str.substr(1, str.length() - 2);
        if ((pos = str.find_first_of("<=!>")) != std::string::npos) {
            rel_factor(str.substr(0, pos));
            rel_factor(str.substr(pos + 2));
        } else {
            std::string errorMessage = "Err: Missing comparison in rel_expr";
            throw std::invalid_argument(errorMessage);
        }
    }
}

void rel_factor(std::string str) {
    if (!isBalanced(str)) {
        std::string errorMessage = "Err: Improper use of brackets in cond_expr";
        throw std::invalid_argument(errorMessage);
    }
    std::regex expr("^\\s*[(]*\\s*[[:alnum:]]+\\s*[)]*\\s*([-+*%/]\\s*[(]*\\s*[[:alnum:]]+\\s*[)]*\\s*)*\\s*$");
    if (!regex_match(str, expr)) {
        std::string errorMessage = "Err: Invalid expr";
        throw std::invalid_argument(errorMessage);
    }
}

bool isBalanced(std::string str) {
    int count = 0;
    for (std::string::iterator it = str.begin(); it != str.end(); ++it) {
        if (*it == '(') {
            count++;
        }
        if (*it == ')') {
            count--;
        }
    }
    return (count == 0);
}

void expr() {
    if (acceptAndPush(Parser::Symbol::name) || acceptAndPush(Parser::Symbol::number)) {
        while (acceptAndPush(Parser::Symbol::arithmetic)) {
            if (acceptAndPush(Parser::Symbol::open_bracket)) {
                expr();
                expectAndPush(Parser::Symbol::close_bracket);
            } else if (!acceptAndPush(Parser::Symbol::name) && !acceptAndPush(Parser::Symbol::number)) {
                std::string errorMessage = "Err: Did not receive variable or constant. Received: " + (--it)->value;
                it++;
                throw std::invalid_argument(errorMessage);
            }
        }
    } else if (acceptAndPush(Parser::Symbol::open_bracket)) {
        expr();
        expectAndPush(Parser::Symbol::close_bracket);
        if (acceptAndPush(Parser::Symbol::arithmetic)) {
            expr();
        }
    } else {
        std::string errorMessage = "Err: Did not receive variable or constant. Received: " + (--it)->value;
        it++;
        throw std::invalid_argument(errorMessage);
    }
}
} // namespace
