#include "Parser.h"
#include "DesignExtractor.h"
#include "Parser.cpp"
#include "catch.hpp"
#include <sstream>

std::vector<std::string> getTokenList(std::string s) {
    std::vector<std::string> result;
    std::istringstream iss(s);
    for (std::string s; iss >> s;) {
        result.push_back(s);
    }
    return result;
}

bool wrapperOutputChecker(StatementWrapper output, StatementWrapper expected) {
    int num = output.getStatementNum();
    int num2 = expected.getStatementNum();
    REQUIRE(num == num2);
    bool isSame = true;

    std::vector<std::string> out = output.getStatementTokens();
    std::vector<std::string> exp = expected.getStatementTokens();

    if (out.size() != exp.size()) {
        std::cout << out.size() << ", expected size: " << exp.size() << std::endl;
        isSame = false;
    } else {
        for (unsigned i = 0; i < exp.size(); i++) {
            if (out[i] != exp[i]) {
                std::cout << "expected: " << exp[i] << ", output: " << out[i] << std::endl;
                isSame = false;
            }
        }

        if (output.getContainerStatementList().size() != 0) {
            if (expected.getContainerStatementList().size() == 0) {
                std::cout << "null error" << std::endl;
                isSame = false;
            }
            std::vector<StatementWrapper> outStmt = output.getContainerStatementList();
            std::vector<StatementWrapper> expStmt = expected.getContainerStatementList();

            for (unsigned i = 0; i < outStmt.size(); i++) {
                StatementWrapper wrap1 = outStmt[i];
                StatementWrapper wrap2 = expStmt[i];

                if (wrap1.getIfStatementList().size() != 0) {
                    isSame = isSame && wrapperOutputChecker(wrap1, wrap2);
                }

                if (wrap1.getStatementNum() != wrap2.getStatementNum()) {
                    std::cout << "diff statement num" << std::endl;
                    isSame = false;
                }

                std::vector<std::string> outTokens = wrap1.getStatementTokens();
                std::vector<std::string> expTokens = wrap2.getStatementTokens();
                for (unsigned j = 0; j < outTokens.size(); j++) {
                    if (outTokens[j] != expTokens[j]) {
                        std::cout << "container lists:" << std::endl;
                        std::cout << "expected: " << expTokens[j] << ", output: " << outTokens[j] << std::endl;
                        isSame = false;
                    }
                }
            }
        } else if (output.getIfStatementList().size() != 0) {
            if (expected.getIfStatementList().size() == 0) {
                std::cout << "null error for statement:" << expected.getStatementNum() << std::endl;
                isSame = false;
            }
            std::vector<StatementWrapper> outIfStmt = output.getIfStatementList();
            std::vector<StatementWrapper> expIfStmt = expected.getIfStatementList();

            for (unsigned k = 0; k < outIfStmt.size(); k++) {
                StatementWrapper wrap3 = outIfStmt[k];
                StatementWrapper wrap4 = expIfStmt[k];

                if (wrap3.getIfStatementList().size() != 0) {
                    isSame = isSame && wrapperOutputChecker(wrap3, wrap4);
                }

                if (wrap3.getStatementNum() != wrap4.getStatementNum()) {
                    std::cout << "diff statement num" << std::endl;
                    isSame = false;
                }

                std::vector<std::string> outIfTokens = wrap3.getStatementTokens();
                std::vector<std::string> expIfTokens = wrap4.getStatementTokens();
                for (unsigned l = 0; l < outIfTokens.size(); l++) {
                    if (outIfTokens[l] != expIfTokens[l]) {
                        std::cout << "if lists:" << std::endl;
                        std::cout << "expected: " << expIfTokens[l] << ", output:" << outIfTokens[l] << std::endl;
                        isSame = false;
                    }
                }
            }
            std::vector<StatementWrapper> outElseStmt = output.getElseStatementList();
            std::vector<StatementWrapper> expElseStmt = expected.getElseStatementList();

            for (unsigned m = 0; m < outElseStmt.size(); m++) {
                StatementWrapper wrap5 = outElseStmt[m];
                StatementWrapper wrap6 = expElseStmt[m];

                if (wrap5.getIfStatementList().size() != 0) {
                    isSame = isSame && wrapperOutputChecker(wrap5, wrap6);
                }

                if (wrap5.getStatementNum() != wrap6.getStatementNum()) {
                    std::cout << "diff statement num" << std::endl;
                    isSame = false;
                }

                std::vector<std::string> outElseTokens = wrap5.getStatementTokens();
                std::vector<std::string> expElseTokens = wrap6.getStatementTokens();
                for (unsigned n = 0; n < outElseTokens.size(); n++) {
                    if (outElseTokens[n] != expElseTokens[n]) {
                        std::cout << "else lists:" << std::endl;
                        std::cout << "expected: " << expElseTokens[n] << ", output: " << outElseTokens[n] << std::endl;
                        isSame = false;
                    }
                }
            }
        }
    }

    return isSame;
}

bool astChecker(SimplifiedAST output, SimplifiedAST expected) {
    /*if (output.getNumProcedures() != expected.getNumProcedures()) {
        return false;
    }*/

    std::vector<Procedure> out = output.getProcedureList();
    std::vector<Procedure> exp = expected.getProcedureList();

    bool isSame = true;

    for (int i = 0; i < expected.getNumProcedures(); i++) {
        Procedure outPro = out[i];
        Procedure expPro = exp[i];

        if (outPro.getName() != expPro.getName()) {
            isSame = false;
        }

        std::vector<StatementWrapper> outList = outPro.getStatementList();
        std::vector<StatementWrapper> expList = expPro.getStatementList();

        if (outList.size() != expList.size()) {
            std::cout << outList.size() << ", expected size: " << expList.size() << std::endl;
            return false;
        }

        for (unsigned j = 0; j < expList.size(); j++) {
            isSame = isSame && wrapperOutputChecker(outList[j], expList[j]);
        }
    }

    return isSame;
}

TEST_CASE("Parser Test 1: While Statements") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "while"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token three = {Lexer::TokenType::NAME, "x"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "=="};
    Lexer::Token six = {Lexer::TokenType::NAME, "y"};
    Lexer::Token seven = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token nine = {Lexer::TokenType::NAME, "a"};
    Lexer::Token ten = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token eleven = {Lexer::TokenType::NAME, "c"};
    Lexer::Token twelve = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token thirteen = {Lexer::TokenType::NAME, "b"};
    Lexer::Token fourteen = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token fifteen = {Lexer::TokenType::NAME, "c"};
    Lexer::Token sixteen = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token seventeen = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,    two,    three,    four,     six,     seven,   eight,     nine, ten,
                                  eleven, twelve, thirteen, fourteen, fifteen, sixteen, seventeen, stop};
    std::string type = "while";
    std::vector<std::string> tokens{"while", "(", "x", "==", "y", ")"};
    std::vector<StatementWrapper> stmtLst;
    std::vector<std::string> inner{"a", "=", "c", ";"};
    std::vector<std::string> inner2{"b", "=", "c", ";"};
    StatementWrapper wrap{2, type, inner};
    StatementWrapper wrap2{3, type, inner2};

    stmtLst.push_back(wrap);
    stmtLst.push_back(wrap2);

    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};
    expected.setContainerStatementList(stmtLst);
    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 2.1: Read Statements") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "read"};
    Lexer::Token two = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token three = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one, two, three, stop};
    std::string type = "read";
    std::vector<std::string> tokens{"read", "variable", ";"};
    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};
    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 2.2: Read Statements Error") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "read"};
    Lexer::Token two = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token three = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one, two, three, four, stop};
    try {
        StatementWrapper output = testParse(tokenlist);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Parser 2.3: Print Statement") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "print"};
    Lexer::Token two = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token three = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one, two, three, stop};
    std::string type = PRINT;
    std::vector<std::string> tokens{"print", "variable", ";"};
    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};
    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 3.1: If Statement mult cond") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "if"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token two_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token two_six = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token three = {Lexer::TokenType::NAME, "x"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "!="};
    Lexer::Token six = {Lexer::TokenType::NAME, "y"};
    Lexer::Token six_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token six_six = {Lexer::TokenType::SYMBOL, "||"};
    Lexer::Token seven = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token seven_one = {Lexer::TokenType::NAME, "x"};
    Lexer::Token seven_two = {Lexer::TokenType::SYMBOL, "=="};
    Lexer::Token seven_four = {Lexer::TokenType::NAME, "y"};
    Lexer::Token seven_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token seven_six = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight_one = {Lexer::TokenType::SYMBOL, "&&"};
    Lexer::Token eight_three = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token eight_four = {Lexer::TokenType::NAME, "x"};
    Lexer::Token eight_five = {Lexer::TokenType::SYMBOL, "=="};
    Lexer::Token eight_seven = {Lexer::TokenType::NAME, "y"};
    Lexer::Token eight_eight = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight_nine = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "then"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "a"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token twelve = {Lexer::TokenType::NAME, "c"};
    Lexer::Token thirteen = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token fourteen = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token fifteen = {Lexer::TokenType::NAME, "else"};
    Lexer::Token sixteen = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token seventeen = {Lexer::TokenType::NAME, "b"};
    Lexer::Token eighteen = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token nineteen = {Lexer::TokenType::NAME, "3"};
    Lexer::Token twenty = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyone = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,         two,        two_five,   two_six,     three,       four,       six,       six_five,
                                  six_six,     seven,      seven_one,  seven_two,   seven_four,  seven_five, seven_six, eight_one,
                                  eight_three, eight_four, eight_five, eight_seven, eight_eight, eight_nine, eight,     nine,
                                  ten,         eleven,     twelve,     thirteen,    fourteen,    fifteen,    sixteen,   seventeen,
                                  eighteen,    nineteen,   twenty,     twentyone,   stop};
    std::string type = "if";
    std::vector<std::string> tokens{"if", "(", "(", "(", "x",  "!=", "y", ")",  "||", "(", "x",
                                    "==", "y", ")", ")", "&&", "(",  "x", "==", "y",  ")", ")"};
    std::vector<StatementWrapper> ifStmtLst;
    std::vector<StatementWrapper> elseStmtLst;

    std::vector<std::string> ifLst{"a", "=", "c", ";"};
    std::vector<std::string> elseLst{"b", "=", "3", ";"};

    StatementWrapper wrap{2, type, ifLst};
    StatementWrapper wrap2{3, type, elseLst};

    ifStmtLst.push_back(wrap);
    elseStmtLst.push_back(wrap2);
    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};
    expected.setIfStatementList(ifStmtLst);
    expected.setElseStatementList(elseStmtLst);

    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 3.2: If Statement with multiple statements in container") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "if"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token three = {Lexer::TokenType::NAME, "x"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "!="};
    Lexer::Token six = {Lexer::TokenType::NAME, "y"};
    Lexer::Token seven = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "then"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "a"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token twelve = {Lexer::TokenType::NAME, "c"};
    Lexer::Token thirteen = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token fourteen = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token fifteen = {Lexer::TokenType::NAME, "else"};
    Lexer::Token sixteen = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token seventeen = {Lexer::TokenType::NAME, "b"};
    Lexer::Token eighteen = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token nineteen = {Lexer::TokenType::NAME, "c"};
    Lexer::Token twenty = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyone = {Lexer::TokenType::NAME, "print"};
    Lexer::Token twentytwo = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token twentythree = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyfour = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,      two,      three,  four,      six,       seven,       eight,      nine,
                                  ten,      eleven,   twelve, thirteen,  fourteen,  fifteen,     sixteen,    seventeen,
                                  eighteen, nineteen, twenty, twentyone, twentytwo, twentythree, twentyfour, stop};
    std::string type = "if";
    std::vector<std::string> tokens{"if", "(", "x", "!=", "y", ")"};
    std::vector<StatementWrapper> ifStmtLst;
    std::vector<StatementWrapper> elseStmtLst;

    std::vector<std::string> ifLst{"a", "=", "c", ";"};
    std::vector<std::string> elseLstOne{"b", "=", "c", ";"};
    std::vector<std::string> elseLstTwo{"print", "variable", ";"};

    StatementWrapper wrap{2, type, ifLst};
    StatementWrapper wrap2{3, type, elseLstOne};
    StatementWrapper wrap3{4, type, elseLstTwo};

    ifStmtLst.push_back(wrap);
    elseStmtLst.push_back(wrap2);
    elseStmtLst.push_back(wrap3);
    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};
    expected.setIfStatementList(ifStmtLst);
    expected.setElseStatementList(elseStmtLst);

    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 3.3: If Statement with unnecessary brackets in condition") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "if"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token two_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token three = {Lexer::TokenType::NAME, "x"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "!="};
    Lexer::Token six = {Lexer::TokenType::NAME, "y"};
    Lexer::Token seven = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token seven_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "then"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "a"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token twelve = {Lexer::TokenType::NAME, "c"};
    Lexer::Token thirteen = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token fourteen = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token fifteen = {Lexer::TokenType::NAME, "else"};
    Lexer::Token sixteen = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token seventeen = {Lexer::TokenType::NAME, "b"};
    Lexer::Token eighteen = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token nineteen = {Lexer::TokenType::NAME, "c"};
    Lexer::Token twenty = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyone = {Lexer::TokenType::NAME, "print"};
    Lexer::Token twentytwo = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token twentythree = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyfour = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,        two,       two_five,    three,      four,     six,      seven,
                                  seven_five, eight,     nine,        ten,        eleven,   twelve,   thirteen,
                                  fourteen,   fifteen,   sixteen,     seventeen,  eighteen, nineteen, twenty,
                                  twentyone,  twentytwo, twentythree, twentyfour, stop};
    try {
        StatementWrapper output = testParse(tokenlist);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Parser 3.4: If Statement with missing comparison in condition") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "if"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token two_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token three = {Lexer::TokenType::NAME, "x"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "!="};
    Lexer::Token six = {Lexer::TokenType::NAME, "y"};
    Lexer::Token seven = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token seven_one = {Lexer::TokenType::SYMBOL, "||"};
    Lexer::Token seven_two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token seven_three = {Lexer::TokenType::NAME, "true"};
    Lexer::Token seven_four = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token seven_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "then"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "a"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token twelve = {Lexer::TokenType::NAME, "c"};
    Lexer::Token thirteen = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token fourteen = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token fifteen = {Lexer::TokenType::NAME, "else"};
    Lexer::Token sixteen = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token seventeen = {Lexer::TokenType::NAME, "b"};
    Lexer::Token eighteen = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token nineteen = {Lexer::TokenType::NAME, "c"};
    Lexer::Token twenty = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyone = {Lexer::TokenType::NAME, "print"};
    Lexer::Token twentytwo = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token twentythree = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyfour = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,       two,         two_five,   three,       four,       six,       seven,    seven_one,
                                  seven_two, seven_three, seven_four, seven_five,  eight,      nine,      ten,      eleven,
                                  twelve,    thirteen,    fourteen,   fifteen,     sixteen,    seventeen, eighteen, nineteen,
                                  twenty,    twentyone,   twentytwo,  twentythree, twentyfour, stop};
    try {
        StatementWrapper output = testParse(tokenlist);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Parser 3.4: If Statement with missing connector in condition") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "if"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token seven_two = {Lexer::TokenType::NAME, "true"};
    Lexer::Token seven_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "then"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "a"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token twelve = {Lexer::TokenType::NAME, "c"};
    Lexer::Token thirteen = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token fourteen = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token fifteen = {Lexer::TokenType::NAME, "else"};
    Lexer::Token sixteen = {Lexer::TokenType::SYMBOL, "{"};
    Lexer::Token seventeen = {Lexer::TokenType::NAME, "b"};
    Lexer::Token eighteen = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token nineteen = {Lexer::TokenType::NAME, "c"};
    Lexer::Token twenty = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyone = {Lexer::TokenType::NAME, "print"};
    Lexer::Token twentytwo = {Lexer::TokenType::NAME, "variable"};
    Lexer::Token twentythree = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token twentyfour = {Lexer::TokenType::SYMBOL, "}"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,    two,       seven_two, seven_five,  eight,      nine,      ten,      eleven,
                                  twelve, thirteen,  fourteen,  fifteen,     sixteen,    seventeen, eighteen, nineteen,
                                  twenty, twentyone, twentytwo, twentythree, twentyfour, stop};
    try {
        StatementWrapper output = testParse(tokenlist);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Parser 4.1: Assign Statement") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "a"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token three = {Lexer::TokenType::NAME, "b"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "+"};
    Lexer::Token four_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token four_six = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token five = {Lexer::TokenType::NAME, "c"};
    Lexer::Token six = {Lexer::TokenType::SYMBOL, "-"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "d"};
    Lexer::Token eight_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight_six = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "*"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "e"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,   two,        three,     four, four_five, four_six, five, six,
                                  eight, eight_five, eight_six, nine, ten,       eleven,   stop};
    std::string type = "assign";
    std::vector<std::string> tokens{"a", "=", "b", "+", "(", "(", "c", "-", "d", ")", ")", "*", "e", ";"};

    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};

    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 4.2: Assign Statement") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "a"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token two_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token three = {Lexer::TokenType::NAME, "b"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, "/"};
    Lexer::Token four_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token five = {Lexer::TokenType::NAME, "c"};
    Lexer::Token six = {Lexer::TokenType::SYMBOL, "+"};
    Lexer::Token eight = {Lexer::TokenType::NAME, "d"};
    Lexer::Token eight_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight_six = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token nine = {Lexer::TokenType::SYMBOL, "*"};
    Lexer::Token ten = {Lexer::TokenType::NAME, "e"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one,   two,        two_five,  three, four, four_five, five, six,
                                  eight, eight_five, eight_six, nine,  ten,  eleven,    stop};
    std::string type = "assign";
    std::vector<std::string> tokens{"a", "=", "(", "b", "/", "(", "c", "+", "d", ")", ")", "*", "e", ";"};

    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};

    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Parser 4.3: Assign Statement") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "a"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token two_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token four_five = {Lexer::TokenType::SYMBOL, "("};
    Lexer::Token eight_five = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eight_six = {Lexer::TokenType::SYMBOL, ")"};
    Lexer::Token eleven = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one, two, two_five, four_five, eight_five, eight_six, eleven, stop};
    try {
        StatementWrapper output = testParse(tokenlist);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Parser 4.4: Assign Statement with reserved word") {
    bool exceptionThrown = false;
    Lexer::Token one = {Lexer::TokenType::NAME, "print"};
    Lexer::Token two = {Lexer::TokenType::SYMBOL, "="};
    Lexer::Token three = {Lexer::TokenType::INTEGER, "1"};
    Lexer::Token four = {Lexer::TokenType::SYMBOL, ";"};
    Lexer::Token stop = {Lexer::TokenType::STOP, ""};

    Lexer::TokenList tokenlist = {one, two, three, four, stop};
    std::string type = "assign";
    std::vector<std::string> tokens{"print", "=", "1", ";"};

    StatementWrapper output = testParse(tokenlist);
    StatementWrapper expected{1, type, tokens};

    REQUIRE(wrapperOutputChecker(output, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Integration Test 1: Basic procedure with one statement") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "main";
    Procedure pro(name);

    std::vector<std::string> token{"a", "=", "2", ";"};
    std::string type = "assign";
    StatementWrapper stmt{1, type, token};

    pro.addStatement(stmt);
    expected.addProcedure(pro);

    std::string filename = "./ParserTestFiles/Test-1.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Integration Test 1.2: Extra ;") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-1_2.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Integration Test 1.3: Extra ; outside procedure") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-1_3.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Integration Test 2: 2 procedures") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "main";
    Procedure pro(name);

    std::vector<std::string> token{"a", "=", "2", ";"};
    std::string type = "assign";
    StatementWrapper stmt{1, type, token};

    pro.addStatement(stmt);
    expected.addProcedure(pro);

    std::string name2 = "side";
    Procedure pro2(name2);

    std::vector<std::string> token2{"print", "variable", ";"};
    std::string type2 = "print";
    StatementWrapper stmt2{2, type2, token2};

    pro2.addStatement(stmt2);
    expected.addProcedure(pro2);

    std::string filename = "./ParserTestFiles/Test-2.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Integration Test 3: Basic if statement") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "main";
    Procedure pro(name);
    std::vector<StatementWrapper> ifStmtLst;
    std::vector<StatementWrapper> elseStmtLst;

    std::vector<std::string> token{"if", "(", "(", "x", "==", "y", ")", "&&", "(", "z", "==", "3", ")", ")"};
    std::string type = "if_stmt";
    StatementWrapper stmt{1, type, token};

    std::vector<std::string> token2{"read", "var", ";"};
    std::string type2 = "read";
    StatementWrapper stmt2{2, type2, token2};

    ifStmtLst.push_back(stmt2);

    std::vector<std::string> token3{"a", "=", "c", ";"};
    std::string type3 = "assign";
    StatementWrapper stmt3{3, type3, token3};

    elseStmtLst.push_back(stmt3);

    stmt.setIfStatementList(ifStmtLst);
    stmt.setElseStatementList(elseStmtLst);

    pro.addStatement(stmt);
    expected.addProcedure(pro);

    std::string filename = "./ParserTestFiles/Test-3.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Integration Test 4: Nested if statements") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "main";
    Procedure pro(name);
    std::vector<StatementWrapper> ifStmtLst1;
    std::vector<StatementWrapper> elseStmtLst1;
    std::vector<StatementWrapper> ifStmtLst2;
    std::vector<StatementWrapper> elseStmtLst2;
    std::vector<StatementWrapper> ifStmtLst3;
    std::vector<StatementWrapper> elseStmtLst3;
    std::vector<StatementWrapper> ifStmtLst4;
    std::vector<StatementWrapper> elseStmtLst4;
    std::vector<StatementWrapper> ifStmtLst5;
    std::vector<StatementWrapper> elseStmtLst5;

    std::vector<std::string> token1{"if", "(", "x", "==", "y", ")"};
    std::string type1 = "if_stmt";
    StatementWrapper stmt1{1, type1, token1};

    std::vector<std::string> token2{"if", "(", "z", "==", "5", ")"};
    std::string type2 = "if_stmt";
    StatementWrapper stmt2{2, type2, token2};

    std::vector<std::string> token3{"a", "=", "2", ";"};
    std::string type3 = "assign";
    StatementWrapper stmt3{3, type3, token3};

    std::vector<std::string> token4{"b", "=", "2", ";"};
    std::string type4 = "assign";
    StatementWrapper stmt4{4, type4, token4};

    std::vector<std::string> token5{"if", "(", "y", "==", "x", ")"};
    std::string type5 = "if_stmt";
    StatementWrapper stmt5{5, type5, token5};

    std::vector<std::string> token6{"if", "(", "b", "==", "c", ")"};
    std::string type6 = "if_stmt";
    StatementWrapper stmt6{6, type6, token6};

    std::vector<std::string> token7{"if", "(", "c", "==", "3", ")"};
    std::string type7 = "if_stmt";
    StatementWrapper stmt7{7, type7, token7};

    std::vector<std::string> token8{"print", "var", ";"};
    std::string type8 = "print";
    StatementWrapper stmt8{8, type8, token8};

    std::vector<std::string> token9{"b", "=", "2", ";"};
    std::string type9 = "assign";
    StatementWrapper stmt9{9, type9, token9};

    std::vector<std::string> token10{"read", "next", ";"};
    std::string type10 = "read";
    StatementWrapper stmt10{10, type10, token10};

    std::vector<std::string> token11{"a", "=", "3", ";"};
    std::string type11 = "assign";
    StatementWrapper stmt11{11, type11, token11};

    std::vector<std::string> token12{"a", "=", "b", ";"};
    std::string type12 = "assign";
    StatementWrapper stmt12{12, type12, token12};

    ifStmtLst5.push_back(stmt8);
    elseStmtLst5.push_back(stmt9);

    stmt7.setIfStatementList(ifStmtLst5);
    stmt7.setElseStatementList(elseStmtLst5);

    ifStmtLst4.push_back(stmt7);
    elseStmtLst4.push_back(stmt10);
    elseStmtLst4.push_back(stmt11);
    elseStmtLst3.push_back(stmt12);

    stmt6.setIfStatementList(ifStmtLst4);
    stmt6.setElseStatementList(elseStmtLst4);

    ifStmtLst3.push_back(stmt6);

    stmt5.setIfStatementList(ifStmtLst3);
    stmt5.setElseStatementList(elseStmtLst3);

    elseStmtLst1.push_back(stmt5);
    ifStmtLst2.push_back(stmt3);
    elseStmtLst2.push_back(stmt4);

    stmt2.setIfStatementList(ifStmtLst2);
    stmt2.setElseStatementList(elseStmtLst2);

    ifStmtLst1.push_back(stmt2);

    stmt1.setIfStatementList(ifStmtLst1);
    stmt1.setElseStatementList(elseStmtLst1);

    pro.addStatement(stmt1);
    expected.addProcedure(pro);

    std::string filename = "./ParserTestFiles/Test-4.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Stress Test 1") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name1 = "main";
    Procedure main(name1);
    std::string name2 = "raymarch";
    Procedure raymarch(name2);
    std::string name3 = "spheresdf";
    Procedure spheresdf(name3);
    std::vector<StatementWrapper> ifStmtLst;
    std::vector<StatementWrapper> elseStmtLst;
    std::vector<StatementWrapper> whileStmtLst1;
    std::vector<StatementWrapper> whileStmtLst2;

    StatementWrapper stmt1{1, READ, getTokenList("read steps ;")};
    StatementWrapper stmt2{2, READ, getTokenList("read orange ;")};
    StatementWrapper stmt3{3, PRINT, getTokenList("print depth ;")};
    StatementWrapper stmt4{4, ASSIGN, getTokenList("ro = 13 ;")};
    StatementWrapper stmt5{5, ASSIGN, getTokenList("rd = 19 ;")};
    StatementWrapper stmt6{6, READ, getTokenList("read depth ;")};
    StatementWrapper stmt7{7, WHILE_STMT, getTokenList("while ( count < steps )")};
    StatementWrapper stmt8{8, PRINT, getTokenList("print depth ;")};
    StatementWrapper stmt9{9, ASSIGN, getTokenList("po = ro + rd * depth ;")};
    StatementWrapper stmt10{10, READ, getTokenList("read apple ;")};
    StatementWrapper stmt11{11, IF_STMT, getTokenList("if ( dist < epsilon )")};
    StatementWrapper stmt12{12, ASSIGN, getTokenList("done = depth ;")};
    StatementWrapper stmt13{13, ASSIGN, getTokenList("depth = depth + dist ;")};
    StatementWrapper stmt14{14, ASSIGN, getTokenList("count = count + 1 ;")};
    StatementWrapper stmt15{15, ASSIGN, getTokenList("dist = x * x + y * y + z * z ;")};
    StatementWrapper stmt16{16, ASSIGN, getTokenList("x = dist ;")};
    StatementWrapper stmt17{17, ASSIGN, getTokenList("depth = depth ;")};
    StatementWrapper stmt18{18, READ, getTokenList("read p ;")};
    StatementWrapper stmt19{19, WHILE_STMT, getTokenList("while ( x != p )")};
    StatementWrapper stmt20{20, ASSIGN, getTokenList("p = x ;")};
    StatementWrapper stmt21{21, ASSIGN, getTokenList("x = ( dist / x + x ) / 2 ;")};
    StatementWrapper stmt22{22, ASSIGN, getTokenList("dist = x - 1 ;")};
    StatementWrapper stmt23{23, ASSIGN, getTokenList("x = x * x + y * y / 2 ;")};

    ifStmtLst.push_back(stmt12);
    elseStmtLst.push_back(stmt13);

    stmt11.setIfStatementList(ifStmtLst);
    stmt11.setElseStatementList(elseStmtLst);

    whileStmtLst1.push_back(stmt8);
    whileStmtLst1.push_back(stmt9);
    whileStmtLst1.push_back(stmt10);
    whileStmtLst1.push_back(stmt11);
    whileStmtLst1.push_back(stmt14);

    stmt7.setContainerStatementList(whileStmtLst1);

    whileStmtLst2.push_back(stmt20);
    whileStmtLst2.push_back(stmt21);

    stmt19.setContainerStatementList(whileStmtLst2);

    main.addStatement(stmt1);
    main.addStatement(stmt2);
    main.addStatement(stmt3);

    raymarch.addStatement(stmt4);
    raymarch.addStatement(stmt5);
    raymarch.addStatement(stmt6);
    raymarch.addStatement(stmt7);

    spheresdf.addStatement(stmt15);
    spheresdf.addStatement(stmt16);
    spheresdf.addStatement(stmt17);
    spheresdf.addStatement(stmt18);
    spheresdf.addStatement(stmt19);
    spheresdf.addStatement(stmt22);
    spheresdf.addStatement(stmt23);

    expected.addProcedure(main);
    expected.addProcedure(raymarch);
    expected.addProcedure(spheresdf);

    std::string filename = "./ParserTestFiles/program1.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Stress Test 4") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "TestC";
    Procedure testC(name);

    std::vector<StatementWrapper> ifStmtLst;
    std::vector<StatementWrapper> elseStmtLst;
    std::vector<StatementWrapper> whileStmtLst1;
    std::vector<StatementWrapper> whileStmtLst2;
    std::vector<StatementWrapper> whileStmtLst3;
    std::vector<StatementWrapper> whileStmtLst4;
    std::vector<StatementWrapper> whileStmtLst5;
    std::vector<StatementWrapper> whileStmtLst6;
    std::vector<StatementWrapper> whileStmtLst7;
    std::vector<StatementWrapper> whileStmtLst8;

    StatementWrapper stmt1{1, READ, getTokenList("read x ;")};
    StatementWrapper stmt2{2, IF_STMT, getTokenList("if ( b < 15 )")};
    StatementWrapper stmt3{3, WHILE_STMT, getTokenList("while ( c <= x )")};
    StatementWrapper stmt4{4, ASSIGN, getTokenList("x = ( x + a ) + ( ( b ) + a ) + a + b + a + ( c ) ;")};
    StatementWrapper stmt5{5, ASSIGN, getTokenList("c = 890123 ;")};
    StatementWrapper stmt6{6, WHILE_STMT,
                           getTokenList("while ( ( ( ( b >= c ) && ( b > a ) ) && ( ( a == 1 ) && ( a != 2 ) ) ) && ( a != ( ( 0 "
                                        "+ ( 0 * 0 ) / 4 ) % 1 ) ) )")};
    StatementWrapper stmt7{7, ASSIGN, getTokenList(" c = 7 * ( a - b ) ;")};
    StatementWrapper stmt8{8, ASSIGN, getTokenList("a = b + c ;")};
    StatementWrapper stmt9{9, ASSIGN, getTokenList("x = c ;")};                   //
    StatementWrapper stmt10{10, ASSIGN, getTokenList("k = x + a - b + a * a ;")}; //
    StatementWrapper stmt11{11, WHILE_STMT, getTokenList("while ( b < 10 )")};    //
    StatementWrapper stmt12{12, WHILE_STMT, getTokenList("while ( b < 5 )")};     //
    StatementWrapper stmt13{13, WHILE_STMT, getTokenList("while ( b < 4 )")};     //
    StatementWrapper stmt14{14, WHILE_STMT, getTokenList("while ( b < 3 )")};     //
    StatementWrapper stmt15{15, WHILE_STMT, getTokenList("while ( b < 2 )")};     //
    StatementWrapper stmt16{16, WHILE_STMT, getTokenList("while ( b < 1 )")};     //
    StatementWrapper stmt17{17, ASSIGN, getTokenList("b = 1 ;")};                 //
    StatementWrapper stmt18{18, ASSIGN,
                            getTokenList("b = 2 * ( 3 + 4 - ( 5 / 6 % ( while / 8 * ( maybe ) ) ) * 3 ) ;")}; //
    StatementWrapper stmt19{19, ASSIGN, getTokenList("b = 3 ;")};                                  //
    StatementWrapper stmt20{20, PRINT, getTokenList("print b ;")};                                 //
    StatementWrapper stmt21{21, ASSIGN, getTokenList("c = c + b ;")};                              //
    StatementWrapper stmt22{22, PRINT, getTokenList("print k ;")};                                 //

    whileStmtLst8.push_back(stmt17);

    stmt16.setContainerStatementList(whileStmtLst8);

    whileStmtLst7.push_back(stmt16);
    whileStmtLst7.push_back(stmt18);

    stmt15.setContainerStatementList(whileStmtLst7);

    whileStmtLst6.push_back(stmt15);

    stmt14.setContainerStatementList(whileStmtLst6);

    whileStmtLst5.push_back(stmt14);
    whileStmtLst5.push_back(stmt19);
    whileStmtLst5.push_back(stmt20);

    stmt13.setContainerStatementList(whileStmtLst5);

    whileStmtLst4.push_back(stmt13);

    stmt12.setContainerStatementList(whileStmtLst4);

    whileStmtLst3.push_back(stmt12);
    whileStmtLst3.push_back(stmt21);

    stmt11.setContainerStatementList(whileStmtLst3);

    whileStmtLst2.push_back(stmt7);
    whileStmtLst2.push_back(stmt8);

    stmt6.setContainerStatementList(whileStmtLst2);

    whileStmtLst1.push_back(stmt4);
    whileStmtLst1.push_back(stmt5);

    stmt3.setContainerStatementList(whileStmtLst1);

    ifStmtLst.push_back(stmt3);
    elseStmtLst.push_back(stmt6);

    stmt2.setIfStatementList(ifStmtLst);
    stmt2.setElseStatementList(elseStmtLst);

    testC.addStatement(stmt1);
    testC.addStatement(stmt2);
    testC.addStatement(stmt9);
    testC.addStatement(stmt10);
    testC.addStatement(stmt11);
    testC.addStatement(stmt22);

    expected.addProcedure(testC);

    std::string filename = "./ParserTestFiles/program4.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Stress Test 5") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "procedure";
    Procedure pro(name);
    std::string name2 = "raymarch";
    Procedure raymarch(name2);

    std::vector<StatementWrapper> ifStmtLst1;
    std::vector<StatementWrapper> elseStmtLst1;
    std::vector<StatementWrapper> ifStmtLst2;
    std::vector<StatementWrapper> elseStmtLst2;
    std::vector<StatementWrapper> ifStmtLst3;
    std::vector<StatementWrapper> elseStmtLst3;
    std::vector<StatementWrapper> ifStmtLst4;
    std::vector<StatementWrapper> elseStmtLst4;
    std::vector<StatementWrapper> ifStmtLst5;
    std::vector<StatementWrapper> elseStmtLst5;
    std::vector<StatementWrapper> ifStmtLst6;
    std::vector<StatementWrapper> elseStmtLst6;
    std::vector<StatementWrapper> whileStmtLst1;
    std::vector<StatementWrapper> whileStmtLst2;
    std::vector<StatementWrapper> whileStmtLst3;
    std::vector<StatementWrapper> whileStmtLst4;
    std::vector<StatementWrapper> whileStmtLst5;
    std::vector<StatementWrapper> whileStmtLst6;
    std::vector<StatementWrapper> whileStmtLst7;
    std::vector<StatementWrapper> whileStmtLst8;

    StatementWrapper stmt1{1, IF_STMT, getTokenList(" if ( a == a )")};
    StatementWrapper stmt2{2, WHILE_STMT, getTokenList("while ( a > 0 )")};
    StatementWrapper stmt3{3, READ, getTokenList("read a ;")};
    StatementWrapper stmt4{4, WHILE_STMT, getTokenList("while ( a != 0 )")};
    StatementWrapper stmt5{5, IF_STMT, getTokenList("if ( b > c )")};
    StatementWrapper stmt6{6, IF_STMT, getTokenList("if ( ( b == c ) && ( b >= 9 ) )")};
    StatementWrapper stmt7{7, WHILE_STMT, getTokenList("while ( (d < a ) || ( c != a ) )")};
    StatementWrapper stmt8{8, WHILE_STMT, getTokenList("while ( ! ( c == 4 ) )")};
    StatementWrapper stmt9{9, PRINT, getTokenList("print a ;")};
    StatementWrapper stmt10{10, IF_STMT, getTokenList("if (f != g )")};
    StatementWrapper stmt11{11, WHILE_STMT, getTokenList("while ( g >= ( a + b * k ) )")};
    StatementWrapper stmt12{12, WHILE_STMT, getTokenList("while ( ( c * g / d % f - c ) != a )")};
    StatementWrapper stmt13{13, WHILE_STMT, getTokenList("while ( ( b - a ) == ( a - b ) )")};
    StatementWrapper stmt14{14, IF_STMT, getTokenList("if (i < j )")};
    StatementWrapper stmt15{15, READ, getTokenList("read i ;")};
    StatementWrapper stmt16{16, WHILE_STMT, getTokenList("while ( 1 == 1 )")};
    StatementWrapper stmt17{17, PRINT, getTokenList("print j ;")};
    StatementWrapper stmt18{18, PRINT, getTokenList("print print ;")};
    StatementWrapper stmt19{19, READ, getTokenList("read read ;")};
    StatementWrapper stmt20{20, PRINT, getTokenList("print read ;")};
    StatementWrapper stmt21{21, READ, getTokenList("read print ;")};
    StatementWrapper stmt22{22, ASSIGN, getTokenList("print = read ;")};
    StatementWrapper stmt23{23, ASSIGN, getTokenList("read = print ;")};
    StatementWrapper stmt24{24, READ, getTokenList("read p ;")};
    StatementWrapper stmt25{25, PRINT, getTokenList("print q ;")};
    StatementWrapper stmt26{26, ASSIGN, getTokenList("a = b + c - d * e / f % g ;")};
    StatementWrapper stmt27{27, ASSIGN, getTokenList("d = b * ( ( g * k ) / f ) ;")};
    StatementWrapper stmt28{28, ASSIGN, getTokenList("j = ( ( k * e ) + h ) / e ;")};
    StatementWrapper stmt29{29, ASSIGN, getTokenList("m = j + ( h - ( i * ( j - k ) / ( f % a ) - k ) + m ) * d ;")};
    StatementWrapper stmt30{30, READ, getTokenList("read f ;")};
    StatementWrapper stmt31{31, READ, getTokenList("read read ;")};
    StatementWrapper stmt32{32, PRINT, getTokenList("print l ;")};
    StatementWrapper stmt33{33, ASSIGN, getTokenList("a = h * k + d * ( k ) ;")};
    StatementWrapper stmt34{34, PRINT, getTokenList("print x ;")};
    StatementWrapper stmt35{35, ASSIGN, getTokenList("j = while * 1 ;")};
    StatementWrapper stmt36{36, ASSIGN, getTokenList("procedure = 341242 * if + 8248 ;")};
    StatementWrapper stmt37{37, IF_STMT,
                            getTokenList("if ( ( ! ( ( procedure > statement ) || ( ( read < print ) && ( print >= call ) ) ) ) "
                                         "&& ( while <= ( ( if - ( k ) % ( 7 ) ) * 0 ) ) )")};
    StatementWrapper stmt38{38, ASSIGN, getTokenList("then = read ;")};
    StatementWrapper stmt39{39, ASSIGN, getTokenList("print = call * k + h - h % j ;")};
    StatementWrapper stmt40{40, PRINT, getTokenList("print print ;")};
    StatementWrapper stmt41{41, READ, getTokenList("read steps ;")};
    StatementWrapper stmt42{42, PRINT, getTokenList("print depth ;")};

    ifStmtLst6.push_back(stmt38);
    ifStmtLst6.push_back(stmt39);
    elseStmtLst6.push_back(stmt40);
    stmt37.setIfStatementList(ifStmtLst6);
    stmt37.setElseStatementList(elseStmtLst6);

    whileStmtLst8.push_back(stmt23);
    whileStmtLst8.push_back(stmt22);
    whileStmtLst8.push_back(stmt21);
    whileStmtLst8.push_back(stmt20);
    whileStmtLst8.push_back(stmt19);
    whileStmtLst8.push_back(stmt18);
    whileStmtLst8.push_back(stmt17);
    stmt16.setContainerStatementList(whileStmtLst8);

    ifStmtLst5.push_back(stmt15);
    elseStmtLst5.push_back(stmt16);
    stmt14.setIfStatementList(ifStmtLst5);
    stmt14.setElseStatementList(ifStmtLst5);

    whileStmtLst7.push_back(stmt14);
    stmt13.setContainerStatementList(whileStmtLst7);

    whileStmtLst6.push_back(stmt13);
    stmt12.setContainerStatementList(whileStmtLst6);

    whileStmtLst5.push_back(stmt12);
    stmt11.setContainerStatementList(whileStmtLst5);

    ifStmtLst4.push_back(stmt11);
    elseStmtLst4.push_back(stmt24);
    elseStmtLst4.push_back(stmt25);
    stmt10.setIfStatementList(ifStmtLst4);
    stmt10.setElseStatementList(elseStmtLst4);

    whileStmtLst4.push_back(stmt9);
    stmt8.setContainerStatementList(whileStmtLst4);

    whileStmtLst3.push_back(stmt8);
    stmt7.setContainerStatementList(whileStmtLst3);

    ifStmtLst3.push_back(stmt7);
    elseStmtLst3.push_back(stmt10);
    stmt6.setIfStatementList(ifStmtLst3);
    stmt6.setElseStatementList(elseStmtLst3);

    ifStmtLst2.push_back(stmt6);
    elseStmtLst2.push_back(stmt29);
    elseStmtLst2.push_back(stmt28);
    elseStmtLst2.push_back(stmt27);
    elseStmtLst2.push_back(stmt26);
    stmt5.setIfStatementList(ifStmtLst2);
    stmt5.setElseStatementList(elseStmtLst2);

    whileStmtLst2.push_back(stmt5);
    stmt4.setContainerStatementList(whileStmtLst2);

    whileStmtLst1.push_back(stmt3);
    stmt2.setContainerStatementList(whileStmtLst1);

    ifStmtLst1.push_back(stmt2);
    elseStmtLst1.push_back(stmt4);
    stmt1.setIfStatementList(ifStmtLst1);
    stmt1.setElseStatementList(elseStmtLst1);

    pro.addStatement(stmt1);
    pro.addStatement(stmt30);
    pro.addStatement(stmt31);
    pro.addStatement(stmt32);
    pro.addStatement(stmt33);
    pro.addStatement(stmt34);
    pro.addStatement(stmt35);
    pro.addStatement(stmt36);
    pro.addStatement(stmt37);

    raymarch.addStatement(stmt41);
    raymarch.addStatement(stmt42);

    std::string filename = "./ParserTestFiles/program5.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Test-5") {
    bool exceptionThrown = false;
    SimplifiedAST expected;
    std::string name = "main";
    Procedure pro(name);

    StatementWrapper stmt1{1, ASSIGN,
                           getTokenList("a = b + ( ( a / c ) + 2 / ( ( c % 5 ) / 3 ) + ( c * 2 ) * ( y ) ) ;")};

    std::string filename = "./ParserTestFiles/Test-5.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);

    SimplifiedAST ast = Parser::parse(output);
    REQUIRE(astChecker(ast, expected) == true);
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Test 6") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-6.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown);
}

TEST_CASE("Test 7") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-7.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown);
}

TEST_CASE("Test 8") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-8.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown);
}

TEST_CASE("Test 9") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-9.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown);
}

TEST_CASE("Test 10") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-10.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown);
}

TEST_CASE("Test 11") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/Test-11.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(exceptionThrown);
}

TEST_CASE("Test") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/program6.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(!exceptionThrown);
}

TEST_CASE("Test3") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/program3.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 1") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-1.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 2") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-2.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 3") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-3.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 4") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-4.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 5") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-5.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 6") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-6.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 7") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-7.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 8") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-8.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 9") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-9.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 10") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-10.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 11") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-11.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 12") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-12.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 13") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-13.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 14") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-14.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::exception) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 15") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-15.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 16") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-16.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 17") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-17.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 18") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-18.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 19") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-19.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 20") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-20.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test 21") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-21.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("Cond Test 22") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-22.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("Cond Test") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cond-test.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CallGraph Test") {
    CallGraph call;
    call.addProcedure("main");
    call.addProcedure("main2");

    call.addProcedureCall("main", "test1");
    call.addProcedureCall("main", "test2");
    call.addProcedureCall("main", "test3");
    call.addProcedureCall("main", "test4");
    call.addProcedureCall("main2", "test1");

    REQUIRE(call.getProcedureList("main").find("test1") != call.getProcedureList("main").end());
    REQUIRE(call.getProcedureList("main").find("test2") != call.getProcedureList("main").end());
    REQUIRE(call.getProcedureList("main").find("test3") != call.getProcedureList("main").end());
    REQUIRE(call.getProcedureList("main").find("test4") != call.getProcedureList("main").end());
    REQUIRE(call.getProcedureList("main2").find("test1") != call.getProcedureList("main2").end());
    REQUIRE(call.getProcedureList("main2").find("test2") == call.getProcedureList("main2").end());
}

TEST_CASE("CallGraph Test 1: 1 call") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-2.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }

    REQUIRE(call.getProcedureList("main").find("test") != call.getProcedureList("main").end());
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CallGraph Test 2: No calls") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-1.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(call.getProcedureList("main").empty());
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CallGraph Test 3: Cyclic calls") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-3.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("CallGraph Test 4: Multiple calls") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-4.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CallGraph Test 5") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-5.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CallGraph Test 6") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-6.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("CallGraph Test 7") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-7.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("CallGraph Test 8") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-8.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("CallGraph Test 9") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/call-test-9.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(exceptionThrown);
}

TEST_CASE("CallGraph Test 10") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cyclic-test.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        Parser::parse(output);
        CallGraph call = Parser::getCallGraph();
    } catch (std::invalid_argument) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CFG Test") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cfgBipsTest.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
        CallGraph callgraph = Parser::getCallGraph();
        DesignExtractor designExtractor{ast, callgraph};
        std::unordered_map<int, std::set<int>> cfg = designExtractor.createBipsCFG(ast);
        REQUIRE(cfg.find(1) != cfg.end());
        REQUIRE(cfg.find(2) != cfg.end());
        REQUIRE(cfg.find(3) != cfg.end());
        REQUIRE(cfg.find(4) != cfg.end());
        REQUIRE(cfg.find(5) != cfg.end());
        REQUIRE(cfg.find(6) != cfg.end());
        REQUIRE(cfg.find(7) != cfg.end());
        REQUIRE(cfg.find(8) != cfg.end());
        REQUIRE(cfg.find(9) != cfg.end());
        REQUIRE(cfg.find(10) != cfg.end());
        REQUIRE(cfg.find(11) != cfg.end());
        REQUIRE(cfg.find(12) != cfg.end());
        REQUIRE(cfg.find(13) != cfg.end());
        REQUIRE(cfg.find(14) != cfg.end());
        REQUIRE(cfg.find(15) != cfg.end());
        REQUIRE(cfg.find(16) != cfg.end());
        REQUIRE(cfg.find(17) != cfg.end());
        REQUIRE((*cfg.find(1)).second.size() == 1);
        REQUIRE((*cfg.find(2)).second.size() == 1);
        REQUIRE((*cfg.find(3)).second.size() == 2);
        REQUIRE((*cfg.find(4)).second.size() == 1);
        REQUIRE((*cfg.find(5)).second.size() == 2);
        REQUIRE((*cfg.find(6)).second.size() == 1);
        REQUIRE((*cfg.find(7)).second.size() == 1);
        REQUIRE((*cfg.find(8)).second.size() == 1);
        REQUIRE((*cfg.find(9)).second.size() == 1);
        REQUIRE((*cfg.find(10)).second.size() == 1);
        REQUIRE((*cfg.find(11)).second.size() == 1);
        REQUIRE((*cfg.find(12)).second.size() == 1);
        REQUIRE((*cfg.find(13)).second.size() == 1);
        REQUIRE((*cfg.find(14)).second.size() == 1);
        REQUIRE((*cfg.find(15)).second.size() == 2);
        REQUIRE((*cfg.find(16)).second.size() == 1);
        REQUIRE((*cfg.find(17)).second.size() == 2);
        REQUIRE((*cfg.find(1)).second.find(13) != (*cfg.find(1)).second.end());
        REQUIRE((*cfg.find(2)).second.find(3) != (*cfg.find(2)).second.end());
        REQUIRE((*cfg.find(3)).second.find(4) != (*cfg.find(3)).second.end());
        REQUIRE((*cfg.find(3)).second.find(10) != (*cfg.find(3)).second.end());
        REQUIRE((*cfg.find(4)).second.find(5) != (*cfg.find(4)).second.end());
        REQUIRE((*cfg.find(5)).second.find(6) != (*cfg.find(5)).second.end());
        REQUIRE((*cfg.find(5)).second.find(8) != (*cfg.find(5)).second.end());
        REQUIRE((*cfg.find(6)).second.find(7) != (*cfg.find(6)).second.end());
        REQUIRE((*cfg.find(7)).second.find(15) != (*cfg.find(7)).second.end());
        REQUIRE((*cfg.find(8)).second.find(9) != (*cfg.find(8)).second.end());
        REQUIRE((*cfg.find(9)).second.find(16) != (*cfg.find(9)).second.end());
        REQUIRE((*cfg.find(10)).second.find(11) != (*cfg.find(10)).second.end());
        REQUIRE((*cfg.find(11)).second.find(12) != (*cfg.find(11)).second.end());
        REQUIRE((*cfg.find(12)).second.find(17) != (*cfg.find(12)).second.end());
        REQUIRE((*cfg.find(13)).second.find(15) != (*cfg.find(13)).second.end());
        REQUIRE((*cfg.find(14)).second.find(2) != (*cfg.find(14)).second.end());
        REQUIRE((*cfg.find(15)).second.find(3) != (*cfg.find(15)).second.end());
        REQUIRE((*cfg.find(15)).second.find(14) != (*cfg.find(15)).second.end());
        REQUIRE((*cfg.find(16)).second.find(17) != (*cfg.find(16)).second.end());
        REQUIRE((*cfg.find(17)).second.find(3) != (*cfg.find(17)).second.end());
        REQUIRE((*cfg.find(17)).second.find(10) != (*cfg.find(17)).second.end());
    } catch (std::invalid_argument &e) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}

TEST_CASE("CFG Test 2") {
    bool exceptionThrown = false;
    std::string filename = "./ParserTestFiles/cfgBipsTest2.txt";
    Lexer::TokenList output = Lexer::tokenize(filename);
    try {
        SimplifiedAST ast = Parser::parse(output);
        CallGraph callgraph = Parser::getCallGraph();
        DesignExtractor designExtractor{ast, callgraph};
        std::unordered_map<int, std::set<int>> cfg = designExtractor.createBipsCFG(ast);
        REQUIRE(cfg.find(1) != cfg.end());
        REQUIRE(cfg.find(2) != cfg.end());
        REQUIRE(cfg.find(3) != cfg.end());
        REQUIRE(cfg.find(4) != cfg.end());
        REQUIRE(cfg.find(5) != cfg.end());
        REQUIRE(cfg.find(6) != cfg.end());
        REQUIRE(cfg.find(7) != cfg.end());
        REQUIRE(cfg.find(8) != cfg.end());
        REQUIRE(cfg.find(9) != cfg.end());
        REQUIRE(cfg.find(10) != cfg.end());
        REQUIRE(cfg.find(11) != cfg.end());
        REQUIRE(cfg.find(12) != cfg.end());
        REQUIRE(cfg.find(13) != cfg.end());
        REQUIRE(cfg.find(14) != cfg.end());
        REQUIRE(cfg.find(15) != cfg.end());
        REQUIRE(cfg.find(16) != cfg.end());
        REQUIRE(cfg.find(17) != cfg.end());
        REQUIRE(cfg.find(18) != cfg.end());
        REQUIRE(cfg.find(19) != cfg.end());
        REQUIRE(cfg.find(20) != cfg.end());
        REQUIRE(cfg.find(21) != cfg.end());
        REQUIRE(cfg.find(22) != cfg.end());
        REQUIRE(cfg.find(23) != cfg.end());
        REQUIRE(cfg.find(24) != cfg.end());
        REQUIRE(cfg.find(25) != cfg.end());
        REQUIRE((*cfg.find(1)).second.size() == 1);
        REQUIRE((*cfg.find(2)).second.size() == 1);
        REQUIRE((*cfg.find(3)).second.size() == 2);
        REQUIRE((*cfg.find(4)).second.size() == 1);
        REQUIRE((*cfg.find(5)).second.size() == 2);
        REQUIRE((*cfg.find(6)).second.size() == 1);
        REQUIRE((*cfg.find(7)).second.size() == 1);
        REQUIRE((*cfg.find(8)).second.size() == 1);
        REQUIRE((*cfg.find(9)).second.size() == 1);
        REQUIRE((*cfg.find(10)).second.size() == 1);
        REQUIRE((*cfg.find(11)).second.size() == 1);
        REQUIRE((*cfg.find(12)).second.size() == 1);
        REQUIRE((*cfg.find(13)).second.size() == 2);
        REQUIRE((*cfg.find(14)).second.size() == 1);
        REQUIRE((*cfg.find(15)).second.size() == 1);
        REQUIRE((*cfg.find(16)).second.size() == 3);
        REQUIRE((*cfg.find(17)).second.size() == 2);
        REQUIRE((*cfg.find(18)).second.size() == 1);
        REQUIRE((*cfg.find(19)).second.size() == 1);
        REQUIRE((*cfg.find(20)).second.size() == 1);
        REQUIRE((*cfg.find(21)).second.size() == 1);
        REQUIRE((*cfg.find(22)).second.size() == 2);
        REQUIRE((*cfg.find(23)).second.size() == 1);
        REQUIRE((*cfg.find(24)).second.size() == 2);
        REQUIRE((*cfg.find(25)).second.size() == 3);
        REQUIRE((*cfg.find(1)).second.find(13) != (*cfg.find(1)).second.end());
        REQUIRE((*cfg.find(2)).second.find(3) != (*cfg.find(2)).second.end());
        REQUIRE((*cfg.find(3)).second.find(4) != (*cfg.find(3)).second.end());
        REQUIRE((*cfg.find(3)).second.find(10) != (*cfg.find(3)).second.end());
        REQUIRE((*cfg.find(4)).second.find(5) != (*cfg.find(4)).second.end());
        REQUIRE((*cfg.find(5)).second.find(6) != (*cfg.find(5)).second.end());
        REQUIRE((*cfg.find(5)).second.find(8) != (*cfg.find(5)).second.end());
        REQUIRE((*cfg.find(6)).second.find(7) != (*cfg.find(6)).second.end());
        REQUIRE((*cfg.find(7)).second.find(16) != (*cfg.find(7)).second.end());
        REQUIRE((*cfg.find(8)).second.find(9) != (*cfg.find(8)).second.end());
        REQUIRE((*cfg.find(9)).second.find(22) != (*cfg.find(9)).second.end());
        REQUIRE((*cfg.find(10)).second.find(11) != (*cfg.find(10)).second.end());
        REQUIRE((*cfg.find(11)).second.find(12) != (*cfg.find(11)).second.end());
        REQUIRE((*cfg.find(12)).second.find(25) != (*cfg.find(12)).second.end());
        REQUIRE((*cfg.find(13)).second.find(2) != (*cfg.find(13)).second.end());
        REQUIRE((*cfg.find(13)).second.find(14) != (*cfg.find(13)).second.end());
        REQUIRE((*cfg.find(14)).second.find(16) != (*cfg.find(14)).second.end());
        REQUIRE((*cfg.find(15)).second.find(13) != (*cfg.find(15)).second.end());
        REQUIRE((*cfg.find(16)).second.find(3) != (*cfg.find(16)).second.end());
        REQUIRE((*cfg.find(16)).second.find(15) != (*cfg.find(16)).second.end());
        REQUIRE((*cfg.find(16)).second.find(17) != (*cfg.find(16)).second.end());
        REQUIRE((*cfg.find(17)).second.find(18) != (*cfg.find(17)).second.end());
        REQUIRE((*cfg.find(17)).second.find(20) != (*cfg.find(17)).second.end());
        REQUIRE((*cfg.find(18)).second.find(22) != (*cfg.find(18)).second.end());
        REQUIRE((*cfg.find(19)).second.find(16) != (*cfg.find(19)).second.end());
        REQUIRE((*cfg.find(20)).second.find(21) != (*cfg.find(20)).second.end());
        REQUIRE((*cfg.find(21)).second.find(25) != (*cfg.find(21)).second.end());
        REQUIRE((*cfg.find(22)).second.find(23) != (*cfg.find(22)).second.end());
        REQUIRE((*cfg.find(22)).second.find(24) != (*cfg.find(22)).second.end());
        REQUIRE((*cfg.find(23)).second.find(25) != (*cfg.find(23)).second.end());
        REQUIRE((*cfg.find(24)).second.find(3) != (*cfg.find(24)).second.end());
        REQUIRE((*cfg.find(24)).second.find(19) != (*cfg.find(24)).second.end());
        REQUIRE((*cfg.find(25)).second.find(10) != (*cfg.find(25)).second.end());
        REQUIRE((*cfg.find(25)).second.find(16) != (*cfg.find(25)).second.end());
        REQUIRE((*cfg.find(25)).second.find(22) != (*cfg.find(25)).second.end());
    } catch (std::invalid_argument &e) {
        exceptionThrown = true;
    }
    REQUIRE(!exceptionThrown);
}
