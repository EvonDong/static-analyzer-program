#include "QueryParser.h"
#include "QueryParserHelper.h"
#include "catch.hpp"

#include <unordered_set>

// -------------------------- Process with clauses --------------------------
TEST_CASE("while w; prog_line n; Select w with n = 10 -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "n";

    QueryParser::InputType input2;
    input2.var = "10";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"n", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "n";
    expectedInput1.entityType = "prog_line";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "10";
    expectedInput2.entityType = "num";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; constant c; Select s with s.stmt# = c.value -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "s.stmt#";

    QueryParser::InputType input2;
    input2.var = "c.value";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"c", "constant"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "s.stmt#";
    expectedInput1.entityType = "stmt";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "c.value";
    expectedInput2.entityType = "constant";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("if f; while w; constant c; Select f with w.stmt# = f.stmt# -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "w.stmt#";

    QueryParser::InputType input2;
    input2.var = "f.stmt#";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"f", "if"});
    inputMap.insert({"w", "while"});
    inputMap.insert({"c", "constant"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "w.stmt#";
    expectedInput1.entityType = "while";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "f.stmt#";
    expectedInput2.entityType = "if";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("stmt s; procedure p; var v; Select v with p.procName = v.varName -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "v.varName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "p.procName";
    expectedInput1.entityType = "procedure";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "v.varName";
    expectedInput2.entityType = "variable";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("stmt s; procedure p; var v; Select v with p.procName = \"getCount\" -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "\"getCount\"";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "p.procName";
    expectedInput1.entityType = "procedure";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "getCount";
    expectedInput2.entityType = "identifier";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; procedure p; var v; Select v with \"getCount\" = r.varName -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "\"getCount\"";

    QueryParser::InputType input2;
    input2.var = "r.varName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"r", "read"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "getCount";
    expectedInput1.entityType = "identifier";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "r.varName";
    expectedInput2.entityType = "read";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; constant c; Select s with 10 = s.stmt# -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "10";

    QueryParser::InputType input2;
    input2.var = "s.stmt#";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"c", "constant"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "10";
    expectedInput1.entityType = "num";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "s.stmt#";
    expectedInput2.entityType = "stmt";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("while w; prog_line n; Select w with 10 = 10 -VALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "10";

    QueryParser::InputType input2;
    input2.var = "10";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"n", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    QueryParser::Clause result = QueryParser::processWith(withClause);

    QueryParser::InputType expectedInput1;
    expectedInput1.var = "10";
    expectedInput1.entityType = "num";

    QueryParser::InputType expectedInput2;
    expectedInput2.var = "10";
    expectedInput2.entityType = "num";

    QueryParser::Clause expectedWithClause;
    expectedWithClause.clauseType = "with";
    expectedWithClause.clauseVariable = "equal";
    expectedWithClause.firstVar = expectedInput1;
    expectedWithClause.secondVar = expectedInput2;

    REQUIRE(result.clauseType == expectedWithClause.clauseType);
    REQUIRE(result.clauseVariable == expectedWithClause.clauseVariable);
    REQUIRE(result.firstVar.var == expectedWithClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == expectedWithClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == expectedWithClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == expectedWithClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ------------------------------- INVALID processWith tests ----------------------------------------------
TEST_CASE("stmt s; prog_line n; var v; Select v with v = 10 -INVALID 1st INPUT TYPE- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "v";

    QueryParser::InputType input2;
    input2.var = "10";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"n", "prog_line"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; prog_line n; var v; Select v with n = \"getCount\" -INVALID INPUT TYPE- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "n";

    QueryParser::InputType input2;
    input2.var = "\"getCount\"";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"n", "prog_line"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; prog_line n; var v; constant c; Select v with p.procName = c.value -INVALID INPUT TYPE- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "20";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"n", "prog_line"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "constant"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; prog_line n; var v; Select v with p.stmt# = 20 -INVALID REFERENCE - processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.stmt#";

    QueryParser::InputType input2;
    input2.var = "20";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"n", "prog_line"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; prog_line n; var v; Select v with 10 = 20 -INVALID nums - processWith Test") {
    QueryParser::InputType input1;
    input1.var = "10";

    QueryParser::InputType input2;
    input2.var = "20";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"n", "prog_line"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; prog_line n; var v; Select v with \"getCount\" = \"sendCount\" -INVALID nums - processWith Test") {
    QueryParser::InputType input1;
    input1.var = "\"getCount\"";

    QueryParser::InputType input2;
    input2.var = "\"sendCount\"";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"n", "prog_line"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; procedure p; var v; Select v with p.procName = \"getCount -INVALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "\"getCount";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("call c; procedure p; var v; Select v with getFirst\" = c.procName -INVALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "getFirst\"";

    QueryParser::InputType input2;
    input2.var = "c.procName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("call c; procedure p; var v; Select v with v.procName = c.procName -INVALID 1st INPUT- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "v.procName";

    QueryParser::InputType input2;
    input2.var = "c.procName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("call c; procedure p; var v; read r; Select v with p.procName = r.procName -INVALID 2nd input- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "r.procName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "call"});
    inputMap.insert({"r", "read"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("MISSING DECLARATION: Select v with p.procName = c.procName -INVALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "c.procName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("WRONG FORMAT: Select v with p = c.procName -INVALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p";

    QueryParser::InputType input2;
    input2.var = "c.procName";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "call"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("INVALID REFERENCE: call c; procedure p; var v; read r; Select v with p.procName = c.proc# -INVALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "p.procName";

    QueryParser::InputType input2;
    input2.var = "c.proc#";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    inputMap.insert({"c", "call"});
    inputMap.insert({"r", "read"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("INVALID REFERENCE: constant c; variable v; Select v with -4 = c.constant -INVALID- processWith Test") {
    QueryParser::InputType input1;
    input1.var = "-4";

    QueryParser::InputType input2;
    input2.var = "c.constant";

    std::map<std::string, std::string> inputMap;
    inputMap.insert({"v", "variable"});
    inputMap.insert({"c", "constant"});
    inputMap.insert({"r", "read"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    REQUIRE_THROWS(QueryParser::processWith(withClause));

    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}



// ******************************    Parse with    ****************
TEST_CASE("while w; prog_line n; Select w with n = 10 -VALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("n    =   10");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"w", "while"});
    inputMap.insert({"n", "prog_line"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::parseWith(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "n";
    input1.entityType = "prog_line";

    QueryParser::InputType input2;
    input2.var = "10";
    input2.entityType = "num";

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    std::vector<QueryParser::Clause> withVec;
    withVec.push_back(withClause);

    REQUIRE(output.size() == withVec.size());
    REQUIRE(result.clauseType == withClause.clauseType);
    REQUIRE(result.clauseVariable == withClause.clauseVariable);
    REQUIRE(result.firstVar.var == withClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == withClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == withClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == withClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("stmt s; constant c; Select s with s.stmt# = c.value  -VALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("s.stmt#  = c.value");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"c", "constant"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::parseWith(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "s.stmt#";
    input1.entityType = "stmt";

    QueryParser::InputType input2;
    input2.var = "c.value";
    input2.entityType = "constant";

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    std::vector<QueryParser::Clause> withVec;
    withVec.push_back(withClause);

    REQUIRE(output.size() == withVec.size());
    REQUIRE(result.clauseType == withClause.clauseType);
    REQUIRE(result.clauseVariable == withClause.clauseVariable);
    REQUIRE(result.firstVar.var == withClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == withClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == withClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == withClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; procedure p; var v; Select v with p.procName = v.varName  -VALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName = v.varName");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::parseWith(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p.procName";
    input1.entityType = "procedure";

    QueryParser::InputType input2;
    input2.var = "v.varName";
    input2.entityType = "variable";

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    std::vector<QueryParser::Clause> withVec;
    withVec.push_back(withClause);

    REQUIRE(output.size() == withVec.size());
    REQUIRE(result.clauseType == withClause.clauseType);
    REQUIRE(result.clauseVariable == withClause.clauseVariable);
    REQUIRE(result.firstVar.var == withClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == withClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == withClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == withClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; procedure p; var v; Select v with p.procName=\"getCount\"  -VALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName   =      \"   getCount \"");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    std::vector<QueryParser::Clause> output = QueryParser::parseWith(input);
    QueryParser::Clause result = output.back();

    QueryParser::InputType input1;
    input1.var = "p.procName";
    input1.entityType = "procedure";

    QueryParser::InputType input2;
    input2.var = "getCount";
    input2.entityType = "identifier";

    QueryParser::Clause withClause;
    withClause.clauseType = "with";
    withClause.clauseVariable = "equal";
    withClause.firstVar = input1;
    withClause.secondVar = input2;

    std::vector<QueryParser::Clause> withVec;
    withVec.push_back(withClause);

    REQUIRE(output.size() == withVec.size());
    REQUIRE(result.clauseType == withClause.clauseType);
    REQUIRE(result.clauseVariable == withClause.clauseVariable);
    REQUIRE(result.firstVar.var == withClause.firstVar.var);
    REQUIRE(result.firstVar.entityType == withClause.firstVar.entityType);
    REQUIRE(result.secondVar.var == withClause.secondVar.var);
    REQUIRE(result.secondVar.entityType == withClause.secondVar.entityType);
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

// ------------------------INVALID parseWith test -----------------------------------------

TEST_CASE("stmt s; procedure p; var v; Select   v with    = p.procName    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("   = p.procName");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("stmt s; procedure p; var v; Select   v with  p.procName  = ""    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName  = \"\"");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("variable v; Select   v with  v.varName  =     -INVALID MISSING INPUT- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("v.varName  = ");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("Wrong format: stmt s; procedure p; var v; Select   v with  p.procName v.varName = -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName  v.varName =");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}


TEST_CASE("Missing equal sign: stmt s; procedure p; var v; Select   v with  p.procName  v.varName    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName  v.varName");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("Double equal sign: stmt s; procedure p; var v; Select   v with  p.procName == v.varName    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName == v.varName");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}

TEST_CASE("Wrong format: stmt s; procedure p; var v; Select   v with  p.procName = v    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName = v");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    REQUIRE_THROWS(QueryParser::parseWith(input));
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
}






// -----BOOLEAN------- INVALID parseWith Semantic checks ------------
TEST_CASE("BOOLEAN   v with  p.procName = v    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName = v");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    REQUIRE_THROWS(QueryParser::parseWith(input));
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN   v with  p.procName = v.procName    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName = v.procName");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    REQUIRE_THROWS(QueryParser::parseWith(input));
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN   v with  p.procName = n    -INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName = n");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    REQUIRE_THROWS(QueryParser::parseWith(input));
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "semantic error");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}

TEST_CASE("BOOLEAN   v with  p.procName = \"invalidd    -SYNTACTIC INVALID- parseWith Test") {
    std::vector<std::string> input;
    input.emplace_back("p.procName = \"invalidd");
    std::map<std::string, std::string> inputMap;
    inputMap.insert({"s", "stmt"});
    inputMap.insert({"v", "variable"});
    inputMap.insert({"p", "procedure"});
    QueryParser::setDeclarationsMap(inputMap);

    QueryParser::setIsBoolean(true);
    QueryParser::setErrorMsg("");

    REQUIRE_THROWS(QueryParser::parseWith(input));
    REQUIRE(QueryParser::getIsBoolean() == true);
    REQUIRE(QueryParser::getErrorMsg() == "");
    std::map<std::string, std::string> cleanMap;
    QueryParser::setDeclarationsMap(cleanMap);
    QueryParser::setIsBoolean(false);
    QueryParser::setErrorMsg("");
}





