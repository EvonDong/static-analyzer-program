#include <iterator>
#include <sstream>
#include <string>
#include <vector>

#include "DesignExtractor.h"
#include "catch.hpp"

//     procedure raymarch {
// 1       ro = 13;
// 2       rd = 19;
// 3       read depth;
// 4       while (count < steps) {
// 5           print depth;
// 6           po = ro + rd * depth;
// 7           while ((a == b) || (c == d)) {
// 8              a = (depth + (c - (d * (e))) + 2 - (3);
// 9               while (2 == 2) {
// 10                  read cba;
// 11                  print abc;
//                 }
// 12              b = rd;
//             }
//         }
// 13      print depth;
// 14      if (dist < epsilon) then {
// 15          done = depth;
//         } else {
// 16          depth = depth + dist;
//         }
// 17      count = count + 1; }

//     procedure spheresdf {
// 18        dist = x * x + y * y + z * z + 2 - 3;
// 19        print z
//     }

std::vector<std::string> generateTokenList(std::string stmt) {
    std::stringstream ss(stmt);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::vector<std::string> splitted(begin, end);
    return splitted;
}

std::unordered_set<std::string> generateVarSet(std::string stmt) {
    std::stringstream ss(stmt);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    return splitted;
}

std::unordered_set<int> generateStmtNumSet(std::string set) {
    std::stringstream ss(set);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    std::unordered_set<int> intSet;
    for (std::string s : splitted) {
        intSet.insert(std::stoi(s));
    }
    return intSet;
}

std::set<int> generateStmtNumOrderedSet(std::string set) {
    std::stringstream ss(set);
    std::istream_iterator<std::string> begin(ss);
    std::istream_iterator<std::string> end;
    std::unordered_set<std::string> splitted(begin, end);
    std::set<int> intset;
    for (std::string s : splitted) {
        intset.insert(std::stoi(s));
    }
    return intset;
}

SimplifiedAST program01Setup() {
    // raymarch procedure
    StatementWrapper one{1, ASSIGN, generateTokenList("ro = 13 ;")};
    StatementWrapper two{2, ASSIGN, generateTokenList("rd = 19 ;")};
    StatementWrapper three{3, READ, generateTokenList("read depth ;")};
    StatementWrapper four{4, WHILE_STMT, generateTokenList("while ( count < steps )")};
    StatementWrapper five{5, PRINT, generateTokenList("print depth ;")};
    StatementWrapper six{6, ASSIGN, generateTokenList("po = ro + rd * depth ;")};
    StatementWrapper seven{7, WHILE_STMT, generateTokenList("while ( ( a == b ) || ( c == d ) )")};
    StatementWrapper eight{8, ASSIGN, generateTokenList("a = ( depth + ( c - ( d * ( e ) ) ) + 2 - ( 3 ) ) ;")};
    StatementWrapper nine{9, WHILE_STMT, generateTokenList("while ( 2 == 2 )")};
    StatementWrapper ten{10, READ, generateTokenList("read cba ;")};
    StatementWrapper eleven{11, PRINT, generateTokenList("print abc ;")};
    StatementWrapper twelve{12, ASSIGN, generateTokenList("b = rd ;")};
    StatementWrapper thirteen{13, PRINT, generateTokenList("print depth ;")};
    StatementWrapper fourteen{14, IF_STMT, generateTokenList("if ( dist < epsilon )")};
    StatementWrapper fifteen{15, ASSIGN, generateTokenList("done = depth ;")};
    StatementWrapper sixteen{16, ASSIGN, generateTokenList("depth = depth + dist ;")};
    StatementWrapper seventeen{17, ASSIGN, generateTokenList("count = count + 1 ;")};

    // line 9 while stmtList
    std::vector<StatementWrapper> whileStatementLine09;
    whileStatementLine09.push_back(ten);
    whileStatementLine09.push_back(eleven);
    nine.setContainerStatementList(whileStatementLine09);

    // line 7 while statement
    std::vector<StatementWrapper> whileStatementLine07;
    whileStatementLine07.push_back(eight);
    whileStatementLine07.push_back(nine);
    whileStatementLine07.push_back(twelve);
    seven.setContainerStatementList(whileStatementLine07);

    // line 4 while statement
    std::vector<StatementWrapper> whileStatementLine04;
    whileStatementLine04.push_back(five);
    whileStatementLine04.push_back(six);
    whileStatementLine04.push_back(seven);
    four.setContainerStatementList(whileStatementLine04);

    // line 14 if-else statement
    std::vector<StatementWrapper> ifStatementLine14;
    std::vector<StatementWrapper> elseStatementLine14;
    ifStatementLine14.push_back(fifteen);
    elseStatementLine14.push_back(sixteen);
    fourteen.setIfStatementList(ifStatementLine14);
    fourteen.setElseStatementList(elseStatementLine14);

    Procedure procedureOne{"raymarch"};
    procedureOne.addStatement(one);
    procedureOne.addStatement(two);
    procedureOne.addStatement(three);
    procedureOne.addStatement(four);
    procedureOne.addStatement(thirteen);
    procedureOne.addStatement(fourteen);
    procedureOne.addStatement(seventeen);

    // spheresdf procedure
    StatementWrapper eighteen{18, ASSIGN, generateTokenList("dist = x * x + y * y + z * z + 2 - 3 ;")};
    StatementWrapper nineteen{19, PRINT, generateTokenList("print z ;")};

    Procedure procedureTwo{"spheresdf"};
    procedureTwo.addStatement(eighteen);
    procedureTwo.addStatement(nineteen);

    SimplifiedAST ast;
    ast.addProcedure(procedureOne);
    ast.addProcedure(procedureTwo);
    return ast;
}

CallGraph program01CallGraphSetup() {
    CallGraph callGraph{};
    callGraph.addProcedure("raymarch");
    callGraph.addProcedure("spheresdf");
    return callGraph;
}

void runExtractor01() {
    CallGraph cg = program01CallGraphSetup();
    DesignExtractor designExtractor(program01Setup(), cg);
    designExtractor.extract();
}

TEST_CASE("Program 01: Check procedures are added") {
    // only need to run one time
    runExtractor01();
    PKB &pkb = PKB::Instance();

    REQUIRE(pkb.getAllProc() == generateVarSet("raymarch spheresdf"));
}

TEST_CASE("Program 01: Follows relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check basic follows relations") {
        // Use case: Follows/Follows*(stmtNum, stmtNum)
        REQUIRE(pkb.isFollows(1, 2) == true);
        REQUIRE(pkb.isFollows(2, 3) == true);
        REQUIRE(pkb.isFollows(3, 4) == true);
        REQUIRE(pkb.isFollows(4, 13) == true);
        REQUIRE(pkb.isFollows(13, 14) == true);
        REQUIRE(pkb.isFollows(14, 17) == true);

        REQUIRE(pkb.isFollows(5, 6) == true);
        REQUIRE(pkb.isFollows(6, 7) == true);

        REQUIRE(pkb.isFollows(8, 9) == true);
        REQUIRE(pkb.isFollows(9, 12) == true);

        REQUIRE(pkb.isFollows(10, 11) == true);
    }

    SECTION("Check that follows is not followsStar") {
        REQUIRE(pkb.isFollows(1, 3) == false);
        REQUIRE(pkb.isFollows(1, 4) == false);
        REQUIRE(pkb.isFollows(2, 4) == false);
        REQUIRE(pkb.isFollows(1, 13) == false);
        REQUIRE(pkb.isFollows(1, 14) == false);
        REQUIRE(pkb.isFollows(1, 17) == false);
        REQUIRE(pkb.isFollows(2, 13) == false);
        REQUIRE(pkb.isFollows(2, 14) == false);
        REQUIRE(pkb.isFollows(2, 17) == false);
        REQUIRE(pkb.isFollows(3, 13) == false);
        REQUIRE(pkb.isFollows(3, 14) == false);
        REQUIRE(pkb.isFollows(3, 17) == false);
        REQUIRE(pkb.isFollows(4, 14) == false);
        REQUIRE(pkb.isFollows(4, 17) == false);

        REQUIRE(pkb.isFollows(5, 7) == false);
    }

    SECTION("Check that statements in different stmt list don't follow each other") {
        REQUIRE(pkb.isFollows(2, 5) == false);
        REQUIRE(pkb.isFollows(4, 6) == false);
        REQUIRE(pkb.isFollows(4, 8) == false);
        REQUIRE(pkb.isFollows(4, 11) == false);
        REQUIRE(pkb.isFollows(5, 11) == false);
        REQUIRE(pkb.isFollows(3, 12) == false);
        REQUIRE(pkb.isFollows(11, 12) == false);
        REQUIRE(pkb.isFollows(13, 15) == false);
        REQUIRE(pkb.isFollows(15, 16) == false);
        REQUIRE(pkb.isFollows(16, 17) == false);
    }
}

TEST_CASE("Program 01: FollowsStar relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check basic followsStar relations") {
        // normal follows are also follow*
        REQUIRE(pkb.isFollows(1, 2, true) == true);
        REQUIRE(pkb.isFollows(14, 17, true) == true);
        REQUIRE(pkb.isFollows(5, 6, true) == true);
        REQUIRE(pkb.isFollows(6, 7, true) == true);

        REQUIRE(pkb.isFollows(1, 3, true) == true);
        REQUIRE(pkb.isFollows(1, 4, true) == true);
        REQUIRE(pkb.isFollows(2, 4, true) == true);
        REQUIRE(pkb.isFollows(1, 13, true) == true);
        REQUIRE(pkb.isFollows(1, 14, true) == true);
        REQUIRE(pkb.isFollows(1, 17, true) == true);
        REQUIRE(pkb.isFollows(2, 13, true) == true);
        REQUIRE(pkb.isFollows(2, 14, true) == true);
        REQUIRE(pkb.isFollows(2, 17, true) == true);
        REQUIRE(pkb.isFollows(3, 13, true) == true);
        REQUIRE(pkb.isFollows(3, 14, true) == true);
        REQUIRE(pkb.isFollows(3, 17, true) == true);
        REQUIRE(pkb.isFollows(4, 13, true) == true);
        REQUIRE(pkb.isFollows(4, 14, true) == true);
        REQUIRE(pkb.isFollows(4, 17, true) == true);

        REQUIRE(pkb.isFollows(5, 7, true) == true);
    }

    SECTION("Check that statements in different stmt list don't followsStar each other") {
        REQUIRE(pkb.isFollows(2, 5, true) == false);
        REQUIRE(pkb.isFollows(4, 6, true) == false);
        REQUIRE(pkb.isFollows(4, 8, true) == false);
        REQUIRE(pkb.isFollows(4, 11, true) == false);
        REQUIRE(pkb.isFollows(5, 11, true) == false);
        REQUIRE(pkb.isFollows(3, 12, true) == false);
        REQUIRE(pkb.isFollows(11, 12, true) == false);
        REQUIRE(pkb.isFollows(13, 15, true) == false);
        REQUIRE(pkb.isFollows(15, 16, true) == false);
        REQUIRE(pkb.isFollows(16, 17, true) == false);
    }
}

TEST_CASE("Program 01: Parent relation") {
    PKB &pkb = PKB::Instance();
    SECTION("Check basic parent relations") {
        REQUIRE(pkb.isParent(4, 5) == true);
        REQUIRE(pkb.isParent(4, 6) == true);
        REQUIRE(pkb.isParent(4, 7) == true);

        REQUIRE(pkb.isParent(7, 8) == true);
        REQUIRE(pkb.isParent(7, 9) == true);
        REQUIRE(pkb.isParent(7, 12) == true);

        REQUIRE(pkb.isParent(14, 15) == true);
        REQUIRE(pkb.isParent(14, 16) == true);
    }

    SECTION("Check that parent is not parentStar") {
        REQUIRE(pkb.isParent(4, 8) == false);
        REQUIRE(pkb.isParent(4, 9) == false);
        REQUIRE(pkb.isParent(4, 10) == false);
        REQUIRE(pkb.isParent(4, 11) == false);
        REQUIRE(pkb.isParent(4, 12) == false);

        REQUIRE(pkb.isParent(7, 10) == false);
        REQUIRE(pkb.isParent(7, 11) == false);
    }

    SECTION("Check that statements under different parents don't have a wrong parent relation") {
        REQUIRE(pkb.isParent(4, 15) == false);
        REQUIRE(pkb.isParent(4, 16) == false);
        REQUIRE(pkb.isParent(9, 16) == false);
        REQUIRE(pkb.isParent(9, 8) == false);
    }

    SECTION("Check that statements without parent / are not parents should not have the relation") {
        REQUIRE(pkb.isParent(1, 5) == false);
        REQUIRE(pkb.isParent(2, 6) == false);
        REQUIRE(pkb.isParent(3, 7) == false);

        REQUIRE(pkb.isParent(5, 7) == false);
        REQUIRE(pkb.isParent(5, 8) == false);
        REQUIRE(pkb.isParent(8, 10) == false);

        REQUIRE(pkb.isParent(13, 14) == false);
        REQUIRE(pkb.isParent(13, 16) == false);
    }
}

TEST_CASE("Program 01: ParentStar relation") {
    PKB &pkb = PKB::Instance();
    SECTION("Check basic parentStar relations") {
        // normal parent are also parent*
        REQUIRE(pkb.isParent(4, 5, true) == true);
        REQUIRE(pkb.isParent(4, 6, true) == true);
        REQUIRE(pkb.isParent(4, 7, true) == true);
        REQUIRE(pkb.isParent(7, 8, true) == true);
        REQUIRE(pkb.isParent(7, 9, true) == true);
        REQUIRE(pkb.isParent(7, 12, true) == true);
        REQUIRE(pkb.isParent(14, 15, true) == true);
        REQUIRE(pkb.isParent(14, 16, true) == true);

        REQUIRE(pkb.isParent(4, 8, true) == true);
        REQUIRE(pkb.isParent(4, 9, true) == true);
        REQUIRE(pkb.isParent(4, 10, true) == true);
        REQUIRE(pkb.isParent(4, 11, true) == true);
        REQUIRE(pkb.isParent(4, 12, true) == true);

        REQUIRE(pkb.isParent(7, 10, true) == true);
        REQUIRE(pkb.isParent(7, 11, true) == true);
    }

    SECTION("Check that statements under different parents* don't have a wrong parent star relation") {
        REQUIRE(pkb.isParent(4, 15, true) == false);
        REQUIRE(pkb.isParent(4, 16, true) == false);
        REQUIRE(pkb.isParent(9, 16, true) == false);
        REQUIRE(pkb.isParent(9, 8, true) == false);
        REQUIRE(pkb.isParent(14, 10, true) == false);
    }

    SECTION("Check that statements without parent should not have the relation") {
        REQUIRE(pkb.isParent(1, 5, true) == false);
        REQUIRE(pkb.isParent(2, 6, true) == false);
        REQUIRE(pkb.isParent(3, 7, true) == false);

        REQUIRE(pkb.isParent(5, 7, true) == false);
        REQUIRE(pkb.isParent(5, 8, true) == false);
        REQUIRE(pkb.isParent(8, 10, true) == false);

        REQUIRE(pkb.isParent(13, 14, true) == false);
        REQUIRE(pkb.isParent(13, 16, true) == false);
    }
}

TEST_CASE("Program 01: Modifies relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct modifies relation for statements") {
        REQUIRE(pkb.getModifiesVariables(1) == generateVarSet("ro"));
        REQUIRE(pkb.getModifiesVariables(2) == generateVarSet("rd"));
        REQUIRE(pkb.getModifiesVariables(3) == generateVarSet("depth"));
        REQUIRE(pkb.getModifiesVariables(4) == generateVarSet("po a cba b"));
        REQUIRE(pkb.getModifiesVariables(5) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(6) == generateVarSet("po"));
        REQUIRE(pkb.getModifiesVariables(7) == generateVarSet("a cba b"));
        REQUIRE(pkb.getModifiesVariables(8) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(9) == generateVarSet("cba"));
        REQUIRE(pkb.getModifiesVariables(10) == generateVarSet("cba"));
        REQUIRE(pkb.getModifiesVariables(11) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(12) == generateVarSet("b"));
        REQUIRE(pkb.getModifiesVariables(13) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(14) == generateVarSet("done depth"));
        REQUIRE(pkb.getModifiesVariables(15) == generateVarSet("done"));
        REQUIRE(pkb.getModifiesVariables(16) == generateVarSet("depth"));
        REQUIRE(pkb.getModifiesVariables(17) == generateVarSet("count"));
        REQUIRE(pkb.getModifiesVariables(18) == generateVarSet("dist"));
        REQUIRE(pkb.getModifiesVariables(19) == generateVarSet(""));
    }

    SECTION("Check correct modifies relation for procedures") {
        REQUIRE(pkb.getModifiesVariableFromProc("raymarch") == generateVarSet("ro rd depth po a cba b done depth count"));
        REQUIRE(pkb.getModifiesVariableFromProc("spheresdf") == generateVarSet("dist"));
    }
}

TEST_CASE("Program 01: Uses relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct uses relation for statements") {
        REQUIRE(pkb.getUsesVariables(1) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(2) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(3) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(4) == generateVarSet("count steps depth ro rd a b c d e abc"));
        REQUIRE(pkb.getUsesVariables(5) == generateVarSet("depth"));
        REQUIRE(pkb.getUsesVariables(6) == generateVarSet("ro rd depth"));
        REQUIRE(pkb.getUsesVariables(7) == generateVarSet("a b c d e depth abc rd"));
        REQUIRE(pkb.getUsesVariables(8) == generateVarSet("depth c d e"));
        REQUIRE(pkb.getUsesVariables(9) == generateVarSet("abc"));
        REQUIRE(pkb.getUsesVariables(10) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(11) == generateVarSet("abc"));
        REQUIRE(pkb.getUsesVariables(12) == generateVarSet("rd"));
        REQUIRE(pkb.getUsesVariables(13) == generateVarSet("depth"));
        REQUIRE(pkb.getUsesVariables(14) == generateVarSet("dist epsilon depth"));
        REQUIRE(pkb.getUsesVariables(15) == generateVarSet("depth"));
        REQUIRE(pkb.getUsesVariables(16) == generateVarSet("depth dist"));
        REQUIRE(pkb.getUsesVariables(17) == generateVarSet("count"));
        REQUIRE(pkb.getUsesVariables(18) == generateVarSet("x y z"));
        REQUIRE(pkb.getUsesVariables(19) == generateVarSet("z"));
    }

    SECTION("Check correct uses relation for procedures") {
        REQUIRE(pkb.getUsesVariableFromProc("raymarch") ==
                generateVarSet("count steps depth ro rd a b c d e abc abc rd dist epsilon count"));
        REQUIRE(pkb.getUsesVariableFromProc("spheresdf") == generateVarSet("x y z"));
    }
}

TEST_CASE("Program 01: Calls relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct calls relation for statements") {
        REQUIRE(pkb.getProceduresCalledBy("raymarch") == generateVarSet(""));
        REQUIRE(pkb.getProceduresCalledBy("spheresdf") == generateVarSet(""));
    }
}

TEST_CASE("Program 01: Populate PKB with Statements - and their attributes") {
    PKB &pkb = PKB::Instance();

    SECTION("Check that PKB is populated with correct variables in a statement") {
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("1")) == generateVarSet("ro"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("2")) == generateVarSet("rd"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("3")) == generateVarSet("depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("4")) == generateVarSet("count steps"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("5")) == generateVarSet("depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("6")) == generateVarSet("po ro rd depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("7")) == generateVarSet("a b c d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("8")) == generateVarSet("a depth c d e"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("9")) == generateVarSet(""));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("10")) == generateVarSet("cba"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("11")) == generateVarSet("abc"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("12")) == generateVarSet("b rd"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("13")) == generateVarSet("depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("14")) == generateVarSet("dist epsilon"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("15")) == generateVarSet("done depth"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("16")) == generateVarSet("depth dist"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("17")) == generateVarSet("count"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("18")) == generateVarSet("dist x y z"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("19")) == generateVarSet("z"));
    }
}

TEST_CASE("Program 01: Populate PKB with pattern") {
    PKB &pkb = PKB::Instance();

    SECTION("Check that assignment statements have the correct pattern stored") {
        REQUIRE(pkb.getStatementsWithPattern("ro", "13", true, true) == generateStmtNumSet("1"));
        REQUIRE(pkb.getStatementsWithPattern("rd", "19", true, true) == generateStmtNumSet("2"));
        REQUIRE(pkb.getStatementsWithPattern("po", "ro + rd * depth", true, true) == generateStmtNumSet("6"));
        REQUIRE(pkb.getStatementsWithPattern("a", "( depth + ( c - ( d * ( e ) ) ) + 2 - ( 3 ) )", true, true) ==
                generateStmtNumSet("8"));
        REQUIRE(pkb.getStatementsWithPattern("b", "rd", true, true) == generateStmtNumSet("12"));
        REQUIRE(pkb.getStatementsWithPattern("done", "depth", true, true) == generateStmtNumSet("15"));
        REQUIRE(pkb.getStatementsWithPattern("depth", "depth + dist", true, true) == generateStmtNumSet("16"));
        REQUIRE(pkb.getStatementsWithPattern("count", "count + 1", true, true) == generateStmtNumSet("17"));
        REQUIRE(pkb.getStatementsWithPattern("dist", "x * x + y * y + z * z + 2 - 3", true, true) == generateStmtNumSet("18"));

        // to add for conditional pattern
    }

    SECTION("Check that conditional patterns have the correct pattern stored") {
        REQUIRE(pkb.getVarMatchConditionStatements("count", WHILE_STMT) == generateStmtNumSet("4"));
        REQUIRE(pkb.getVarMatchConditionStatements("steps", WHILE_STMT) == generateStmtNumSet("4"));
        REQUIRE(pkb.getVarMatchConditionStatements("a", WHILE_STMT) == generateStmtNumSet("7"));
        REQUIRE(pkb.getVarMatchConditionStatements("b", WHILE_STMT) == generateStmtNumSet("7"));
        REQUIRE(pkb.getVarMatchConditionStatements("c", WHILE_STMT) == generateStmtNumSet("7"));
        REQUIRE(pkb.getVarMatchConditionStatements("d", WHILE_STMT) == generateStmtNumSet("7"));

        REQUIRE(pkb.getVarMatchConditionStatements("dist", IF_STMT) == generateStmtNumSet("14"));
        REQUIRE(pkb.getVarMatchConditionStatements("epsilon", IF_STMT) == generateStmtNumSet("14"));
    }
}

// TEST_CASE("Program 01: Populate PKB with Procedure information") {
//    PKB& pkb = PKB::Instance();
//
//    SECTION("Check that procedure have the correct name and start lines") {
//
//    }
//}

TEST_CASE("Program 01: Test CFG") {
    CallGraph cg = program01CallGraphSetup();
    DesignExtractor designExtractor(program01Setup(), cg);
    std::unordered_map<int, std::set<int>> cfg = designExtractor.createCFG(program01Setup());

    SECTION("Check that adjacency list has correct values") {
        REQUIRE(cfg.find(1)->second == generateStmtNumOrderedSet("2"));
        REQUIRE(cfg.find(2)->second == generateStmtNumOrderedSet("3"));
        REQUIRE(cfg.find(3)->second == generateStmtNumOrderedSet("4"));
        REQUIRE(cfg.find(4)->second == generateStmtNumOrderedSet("5 13"));
        REQUIRE(cfg.find(5)->second == generateStmtNumOrderedSet("6"));
        REQUIRE(cfg.find(6)->second == generateStmtNumOrderedSet("7"));
        REQUIRE(cfg.find(7)->second == generateStmtNumOrderedSet("4 8"));
        REQUIRE(cfg.find(8)->second == generateStmtNumOrderedSet("9"));
        REQUIRE(cfg.find(9)->second == generateStmtNumOrderedSet("10 12"));
        REQUIRE(cfg.find(10)->second == generateStmtNumOrderedSet("11"));
        REQUIRE(cfg.find(11)->second == generateStmtNumOrderedSet("9"));
        REQUIRE(cfg.find(12)->second == generateStmtNumOrderedSet("7"));
        REQUIRE(cfg.find(13)->second == generateStmtNumOrderedSet("14"));
        REQUIRE(cfg.find(14)->second == generateStmtNumOrderedSet("15 16"));
        REQUIRE(cfg.find(15)->second == generateStmtNumOrderedSet("17"));
        REQUIRE(cfg.find(16)->second == generateStmtNumOrderedSet("17"));
        REQUIRE(cfg.find(17) == cfg.end());
        REQUIRE(cfg.find(18)->second == generateStmtNumOrderedSet("19"));
        REQUIRE(cfg.find(19) == cfg.end());
    }
}

TEST_CASE("DesignExtractor Program 01: PKB Teardown") {
    PKB &pkb = PKB::Instance();
    pkb.clear();
}

// ===================================================================================================
// 	procedure procedure {
// 1	if(a == a) then {
// 2			while (a > 0) {
// 3				read a; }
// 		} else {
// 4			while (a != 0) {
// 5				if (b>c) then {
// 6					if ( (b == c) && (b>=9) ) then {
// 7						while ( (d < a) || (c != a) ) {
// 8							while (!(c == 4)) {
// 9								print a;
// 								}
// 							}
// 						} else {
// 10						if (f != g) then {
// 11							while (g >= (a + b * k)) {
// 12								while ((c * g / d % f - c) != a) {
// 13									while ((b- a)==(a -b)) {
// 14										if (i < j) then {
// 15											read i;}
// 											else {
// 16											while (1 == 1){
// 17												print j;}}
// 										}
// 									}
// 								}
// 							} else {
// 18							read p;
// 19							print q;
// 							}
// 						}
// 					} else {
// 20					a=b+c-d*e/f%g;
// 21					d = b * ((g*k)/f);
// 22					j = ((k * e)+h) / e;
// 23					m = j + (h - (i* (j - k)/(f % a)-k)+m) * d;
// 					}
// 				}
// 			}
// 24		read f;
// 25		read read;
// 26		print l;
// 27		a = h*k + d *(k);
// 28		print x;
// 29		j = while*1;
// 30		procedure = 341242 * if + 8248;
// 31		if((!((procedure > statement)||((read < print )&&(print >= call)))) && (while <= ((if - (k) % (7)) * 0)) )then{
// 32			then = read;
// 33			print = call * k +h-h%j;
// 		} else{
// 34			print print;
// 		}
// 	}

// 	procedure test2 {
// 35		a = 1;
// 36		k = h * 3 - 4 + 5;
// 37		check = 5;
// 38		call procedure
// 	}

SimplifiedAST program02Setup() {
    StatementWrapper one{1, IF_STMT, generateTokenList("if ( a == a )")};
    StatementWrapper two{2, WHILE_STMT, generateTokenList("while ( a > 0 )")};
    StatementWrapper three{3, READ, generateTokenList("read a ;")};
    StatementWrapper four{4, WHILE_STMT, generateTokenList("while ( a != 0 )")};
    StatementWrapper five{5, IF_STMT, generateTokenList("if ( b > c )")};
    StatementWrapper six{6, IF_STMT, generateTokenList("if ( ( b == c ) && ( b >= 9 ) )")};
    StatementWrapper seven{7, WHILE_STMT, generateTokenList("while ( ( d < a ) || ( c != a ) )")};
    StatementWrapper eight{8, WHILE_STMT, generateTokenList("while ( ! ( c == 4 ) )")};
    StatementWrapper nine{9, PRINT, generateTokenList("print a ;")};
    StatementWrapper ten{10, IF_STMT, generateTokenList("if ( f != g )")};
    StatementWrapper eleven{11, WHILE_STMT, generateTokenList("while ( g >= ( a + b * k ) )")};
    StatementWrapper twelve{12, WHILE_STMT, generateTokenList("while ( ( c * g / d % f - c ) != a )")};
    StatementWrapper thirteen{13, WHILE_STMT, generateTokenList("while ( ( b - a ) == ( a - b ) )")};
    StatementWrapper fourteen{14, IF_STMT, generateTokenList("if ( i < j )")};
    StatementWrapper fifteen{15, READ, generateTokenList("read i ;")};
    StatementWrapper sixteen{16, WHILE_STMT, generateTokenList("while ( 1 == 1 )")};
    StatementWrapper seventeen{17, PRINT, generateTokenList("print j ;")};
    StatementWrapper eighteen{18, READ, generateTokenList("read p ;")};
    StatementWrapper nineteen{19, PRINT, generateTokenList("print q ;")};
    StatementWrapper twenty{20, ASSIGN, generateTokenList("a = b + c - d * e / f % g ;")};
    StatementWrapper twentyone{21, ASSIGN, generateTokenList("d = b * ( ( g * k ) / f )")};
    StatementWrapper twentytwo{22, ASSIGN, generateTokenList("j = ( ( k * e ) + h ) / e")};
    StatementWrapper twentythree{23, ASSIGN,
                                 generateTokenList("m = j + ( h - ( i * ( j - k ) / ( f % a ) - k ) + m ) * d ;")};
    StatementWrapper twentyfour{24, READ, generateTokenList("read f ;")};
    StatementWrapper twentyfive{25, READ, generateTokenList("read read ;")};
    StatementWrapper twentysix{26, PRINT, generateTokenList("print l ;")};
    StatementWrapper twentyseven{27, ASSIGN, generateTokenList("a = h * k + d * ( k ) ;")};
    StatementWrapper twentyeight{28, PRINT, generateTokenList("print x ;")};
    StatementWrapper twentynine{29, ASSIGN, generateTokenList("j = while * 1 ;")};
    StatementWrapper thirty{30, ASSIGN, generateTokenList("procedure = 341242 * if + 8248 ;")};
    StatementWrapper thirtyone{31, IF_STMT,
                               generateTokenList("if ( ( ! ( ( procedure > statement ) || ( ( read < print ) && ( print >= call "
                                                 ") ) ) ) && ( while <= ( ( if - ( k ) % ( 7 ) ) * 0 ) ) )")};
    StatementWrapper thirtytwo{32, ASSIGN, generateTokenList("then = read ;")};
    StatementWrapper thirtythree{33, ASSIGN, generateTokenList("print = call * k + h - h % j ;")};
    StatementWrapper thirtyfour{34, PRINT, generateTokenList("print print ;")};

    StatementWrapper thirtyfive{35, ASSIGN, generateTokenList("a = 1 ;")};
    StatementWrapper thirtysix{36, ASSIGN, generateTokenList("k = h * 3 - 4 + 5 ;")};
    StatementWrapper thirtyseven{37, ASSIGN, generateTokenList("check = 5 ;")};
    StatementWrapper thirtyeight{38, CALL, generateTokenList("call procedure")};

    // line 16 while
    std::vector<StatementWrapper> whileStatement16;
    whileStatement16.push_back(seventeen);
    sixteen.setContainerStatementList(whileStatement16);

    // line 14 if-else
    std::vector<StatementWrapper> ifStatement14;
    std::vector<StatementWrapper> elseStatement14;
    ifStatement14.push_back(fifteen);
    elseStatement14.push_back(sixteen);
    fourteen.setIfStatementList(ifStatement14);
    fourteen.setElseStatementList(elseStatement14);

    // line 13 while
    std::vector<StatementWrapper> whileStatement13;
    whileStatement13.push_back(fourteen);
    thirteen.setContainerStatementList(whileStatement13);

    // line 12 while
    std::vector<StatementWrapper> whileStatement12;
    whileStatement12.push_back(thirteen);
    twelve.setContainerStatementList(whileStatement12);

    // line 11 while
    std::vector<StatementWrapper> whileStatement11;
    whileStatement11.push_back(twelve);
    eleven.setContainerStatementList(whileStatement11);

    // line 10 if-else
    std::vector<StatementWrapper> ifStatement10;
    std::vector<StatementWrapper> elseStatement10;
    ifStatement10.push_back(eleven);
    elseStatement10.push_back(eighteen);
    elseStatement10.push_back(nineteen);
    ten.setIfStatementList(ifStatement10);
    ten.setElseStatementList(elseStatement10);

    // line 8 while
    std::vector<StatementWrapper> whileStatement08;
    whileStatement08.push_back(nine);
    eight.setContainerStatementList(whileStatement08);

    // line 7 while
    std::vector<StatementWrapper> whileStatement07;
    whileStatement07.push_back(eight);
    seven.setContainerStatementList(whileStatement07);

    // line 6 if-else
    std::vector<StatementWrapper> ifStatement06;
    std::vector<StatementWrapper> elseStatement06;
    ifStatement06.push_back(seven);
    elseStatement06.push_back(ten);
    six.setIfStatementList(ifStatement06);
    six.setElseStatementList(elseStatement06);

    // line 5 if-else
    std::vector<StatementWrapper> ifStatement05;
    std::vector<StatementWrapper> elseStatement05;
    ifStatement05.push_back(six);
    elseStatement05.push_back(twenty);
    elseStatement05.push_back(twentyone);
    elseStatement05.push_back(twentytwo);
    elseStatement05.push_back(twentythree);
    five.setIfStatementList(ifStatement05);
    five.setElseStatementList(elseStatement05);

    // line 4 while
    std::vector<StatementWrapper> whileStatement04;
    whileStatement04.push_back(five);
    four.setContainerStatementList(whileStatement04);

    // line 2 while
    std::vector<StatementWrapper> whileStatement02;
    whileStatement02.push_back(three);
    two.setContainerStatementList(whileStatement02);

    // line 1 if-else
    std::vector<StatementWrapper> ifStatement01;
    std::vector<StatementWrapper> elseStatement01;
    ifStatement01.push_back(two);
    elseStatement01.push_back(four);
    one.setIfStatementList(ifStatement01);
    one.setElseStatementList(elseStatement01);

    // line 31 if-else
    std::vector<StatementWrapper> ifStatement31;
    std::vector<StatementWrapper> elseStatement31;
    ifStatement31.push_back(thirtytwo);
    ifStatement31.push_back(thirtythree);
    elseStatement31.push_back(thirtyfour);
    thirtyone.setIfStatementList(ifStatement31);
    thirtyone.setElseStatementList(elseStatement31);

    Procedure procedure{"procedure"};
    procedure.addStatement(one);
    procedure.addStatement(twentyfour);
    procedure.addStatement(twentyfive);
    procedure.addStatement(twentysix);
    procedure.addStatement(twentyseven);
    procedure.addStatement(twentyeight);
    procedure.addStatement(twentynine);
    procedure.addStatement(thirty);
    procedure.addStatement(thirtyone);

    Procedure test2{"test2"};
    test2.addStatement(thirtyfive);
    test2.addStatement(thirtysix);
    test2.addStatement(thirtyseven);
    test2.addStatement(thirtyeight);

    SimplifiedAST ast;
    ast.addProcedure(procedure);
    ast.addProcedure(test2);
    return ast;
}

CallGraph program02CallGraphSetup() {
    CallGraph callGraph{};
    callGraph.addProcedure("procedure");
    callGraph.addProcedure("test2");

    callGraph.addProcedureCall("test2", "procedure");
    return callGraph;
}

void runExtractor02() {
    CallGraph cg = program02CallGraphSetup();
    DesignExtractor designExtractor{program02Setup(), cg};
    designExtractor.extract();
}

TEST_CASE("Program 02: Check procedures are added") {
    // only need to run one time
    runExtractor02();
    PKB &pkb = PKB::Instance();

    REQUIRE(pkb.getAllProc() == generateVarSet("procedure test2"));
}

TEST_CASE("Program 02: Test Follows relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check basic follows relations") {
        // Use case: Follows/Follows*(stmtNum, stmtNum)
        REQUIRE(pkb.isFollows(1, 24) == true);
        REQUIRE(pkb.isFollows(24, 25) == true);
        REQUIRE(pkb.isFollows(25, 26) == true);
        REQUIRE(pkb.isFollows(28, 29) == true);
        REQUIRE(pkb.isFollows(30, 31) == true);

        REQUIRE(pkb.isFollows(18, 19) == true);

        REQUIRE(pkb.isFollows(20, 21) == true);
        REQUIRE(pkb.isFollows(22, 23) == true);

        REQUIRE(pkb.isFollows(32, 33) == true);

        REQUIRE(pkb.isFollows(35, 36) == true);
        REQUIRE(pkb.isFollows(36, 37) == true);
        REQUIRE(pkb.isFollows(37, 38) == true);
    }

    SECTION("Check that follows is not followsStar") {
        REQUIRE(pkb.isFollows(1, 25) == false);
        REQUIRE(pkb.isFollows(1, 26) == false);
        REQUIRE(pkb.isFollows(24, 26) == false);
        REQUIRE(pkb.isFollows(24, 30) == false);
        REQUIRE(pkb.isFollows(26, 31) == false);

        REQUIRE(pkb.isFollows(20, 22) == false);
        REQUIRE(pkb.isFollows(20, 23) == false);
        REQUIRE(pkb.isFollows(21, 23) == false);

        REQUIRE(pkb.isFollows(35, 37) == false);
        REQUIRE(pkb.isFollows(35, 38) == false);
    }

    SECTION("Check that statements in different stmt list don't follow each other") {
        REQUIRE(pkb.isFollows(1, 2) == false);
        REQUIRE(pkb.isFollows(1, 3) == false);
        REQUIRE(pkb.isFollows(1, 4) == false);
        REQUIRE(pkb.isFollows(4, 5) == false);
        REQUIRE(pkb.isFollows(4, 6) == false);
        REQUIRE(pkb.isFollows(5, 6) == false);
        REQUIRE(pkb.isFollows(6, 7) == false);
        REQUIRE(pkb.isFollows(7, 8) == false);
        REQUIRE(pkb.isFollows(8, 9) == false);

        REQUIRE(pkb.isFollows(5, 10) == false);
        REQUIRE(pkb.isFollows(10, 11) == false);
        REQUIRE(pkb.isFollows(11, 12) == false);
        REQUIRE(pkb.isFollows(12, 13) == false);
        REQUIRE(pkb.isFollows(13, 14) == false);
        REQUIRE(pkb.isFollows(14, 15) == false);
        REQUIRE(pkb.isFollows(15, 16) == false);
        REQUIRE(pkb.isFollows(16, 17) == false);

        REQUIRE(pkb.isFollows(6, 18) == false);
        REQUIRE(pkb.isFollows(12, 19) == false);

        REQUIRE(pkb.isFollows(6, 20) == false);

        REQUIRE(pkb.isFollows(20, 24) == false);
        REQUIRE(pkb.isFollows(12, 24) == false);
        REQUIRE(pkb.isFollows(31, 32) == false);
        REQUIRE(pkb.isFollows(33, 34) == false);

        REQUIRE(pkb.isFollows(30, 35) == false);
        REQUIRE(pkb.isFollows(1, 34) == false);
    }
}

TEST_CASE("Program 02: Test FollowsStar relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check basic followsStar relations") {
        // normal follows are also follow*
        REQUIRE(pkb.isFollows(1, 24, true) == true);
        REQUIRE(pkb.isFollows(24, 25, true) == true);
        REQUIRE(pkb.isFollows(25, 26, true) == true);
        REQUIRE(pkb.isFollows(28, 29, true) == true);
        REQUIRE(pkb.isFollows(30, 31, true) == true);
        REQUIRE(pkb.isFollows(18, 19, true) == true);
        REQUIRE(pkb.isFollows(20, 21, true) == true);
        REQUIRE(pkb.isFollows(22, 23, true) == true);
        REQUIRE(pkb.isFollows(32, 33, true) == true);

        REQUIRE(pkb.isFollows(1, 25, true) == true);
        REQUIRE(pkb.isFollows(1, 26, true) == true);
        REQUIRE(pkb.isFollows(1, 27, true) == true);
        REQUIRE(pkb.isFollows(1, 28, true) == true);
        REQUIRE(pkb.isFollows(1, 29, true) == true);
        REQUIRE(pkb.isFollows(1, 30, true) == true);
        REQUIRE(pkb.isFollows(1, 31, true) == true);

        REQUIRE(pkb.isFollows(24, 26, true) == true);
        REQUIRE(pkb.isFollows(24, 27, true) == true);
        REQUIRE(pkb.isFollows(24, 28, true) == true);
        REQUIRE(pkb.isFollows(24, 29, true) == true);
        REQUIRE(pkb.isFollows(24, 30, true) == true);
        REQUIRE(pkb.isFollows(24, 31, true) == true);
        REQUIRE(pkb.isFollows(26, 28, true) == true);
        REQUIRE(pkb.isFollows(26, 29, true) == true);
        REQUIRE(pkb.isFollows(26, 30, true) == true);
        REQUIRE(pkb.isFollows(26, 31, true) == true);
        REQUIRE(pkb.isFollows(28, 30, true) == true);
        REQUIRE(pkb.isFollows(28, 31, true) == true);

        REQUIRE(pkb.isFollows(20, 22, true) == true);
        REQUIRE(pkb.isFollows(20, 23, true) == true);
        REQUIRE(pkb.isFollows(21, 23, true) == true);

        REQUIRE(pkb.isFollows(35, 36, true) == true);
        REQUIRE(pkb.isFollows(35, 37, true) == true);
        REQUIRE(pkb.isFollows(35, 38, true) == true);
        REQUIRE(pkb.isFollows(36, 38, true) == true);
    }

    SECTION("Check that statements in different stmt list don't followsStar each other") {
        REQUIRE(pkb.isFollows(1, 3, true) == false);
        REQUIRE(pkb.isFollows(1, 4, true) == false);
        REQUIRE(pkb.isFollows(4, 6, true) == false);

        REQUIRE(pkb.isFollows(5, 10, true) == false);

        REQUIRE(pkb.isFollows(6, 18, true) == false);
        REQUIRE(pkb.isFollows(12, 19, true) == false);

        REQUIRE(pkb.isFollows(6, 20, true) == false);

        REQUIRE(pkb.isFollows(20, 24, true) == false);
        REQUIRE(pkb.isFollows(12, 24, true) == false);

        REQUIRE(pkb.isFollows(30, 35, true) == false);
        REQUIRE(pkb.isFollows(1, 34, true) == false);
    }
}

TEST_CASE("Program 02: Parent relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check basic parent relations") {
        REQUIRE(pkb.isParent(1, 2) == true);
        REQUIRE(pkb.isParent(2, 3) == true);

        REQUIRE(pkb.isParent(1, 4) == true);
        REQUIRE(pkb.isParent(4, 5) == true);
        REQUIRE(pkb.isParent(5, 6) == true);
        REQUIRE(pkb.isParent(6, 7) == true);
        REQUIRE(pkb.isParent(7, 8) == true);
        REQUIRE(pkb.isParent(8, 9) == true);

        REQUIRE(pkb.isParent(6, 10) == true);
        REQUIRE(pkb.isParent(10, 11) == true);
        REQUIRE(pkb.isParent(11, 12) == true);
        REQUIRE(pkb.isParent(12, 13) == true);
        REQUIRE(pkb.isParent(13, 14) == true);
        REQUIRE(pkb.isParent(14, 15) == true);
        REQUIRE(pkb.isParent(14, 16) == true);
        REQUIRE(pkb.isParent(16, 17) == true);
        REQUIRE(pkb.isParent(10, 18) == true);
        REQUIRE(pkb.isParent(10, 19) == true);

        REQUIRE(pkb.isParent(5, 20) == true);
        REQUIRE(pkb.isParent(5, 21) == true);
        REQUIRE(pkb.isParent(5, 22) == true);
        REQUIRE(pkb.isParent(5, 23) == true);

        REQUIRE(pkb.isParent(31, 32) == true);
        REQUIRE(pkb.isParent(31, 33) == true);
        REQUIRE(pkb.isParent(31, 34) == true);
    }

    SECTION("Check that parent is not parentStar") {
        REQUIRE(pkb.isParent(4, 8) == false);
        REQUIRE(pkb.isParent(4, 9) == false);
        REQUIRE(pkb.isParent(4, 10) == false);
        REQUIRE(pkb.isParent(4, 11) == false);
        REQUIRE(pkb.isParent(4, 12) == false);

        REQUIRE(pkb.isParent(7, 10) == false);
        REQUIRE(pkb.isParent(7, 11) == false);
    }

    SECTION("Check that statements under different parents don't have a wrong parent relation") {
        REQUIRE(pkb.isParent(4, 15) == false);
        REQUIRE(pkb.isParent(4, 16) == false);
        REQUIRE(pkb.isParent(9, 16) == false);
        REQUIRE(pkb.isParent(9, 8) == false);
    }

    SECTION("Check that statements without parent / are not parents should not have the relation") {
        REQUIRE(pkb.isParent(1, 24) == false);
        REQUIRE(pkb.isParent(20, 21) == false);
        REQUIRE(pkb.isParent(24, 25) == false);
        REQUIRE(pkb.isParent(26, 28) == false);

        REQUIRE(pkb.isParent(32, 33) == false);
        REQUIRE(pkb.isParent(33, 34) == false);

        REQUIRE(pkb.isParent(34, 35) == false);
        REQUIRE(pkb.isParent(35, 36) == false);
    }
}

TEST_CASE("Program 02: Test ParentStar relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check basic parentStar relations") {
        // normal parent are also parent*
        REQUIRE(pkb.isParent(1, 2, true) == true);
        REQUIRE(pkb.isParent(2, 3, true) == true);
        REQUIRE(pkb.isParent(1, 4, true) == true);
        REQUIRE(pkb.isParent(4, 5, true) == true);
        REQUIRE(pkb.isParent(8, 9, true) == true);
        REQUIRE(pkb.isParent(6, 10, true) == true);
        REQUIRE(pkb.isParent(10, 11, true) == true);
        REQUIRE(pkb.isParent(10, 18, true) == true);
        REQUIRE(pkb.isParent(10, 19, true) == true);
        REQUIRE(pkb.isParent(5, 20, true) == true);
        REQUIRE(pkb.isParent(5, 23, true) == true);
        REQUIRE(pkb.isParent(31, 34, true) == true);

        REQUIRE(pkb.isParent(1, 3, true) == true);
        REQUIRE(pkb.isParent(1, 4, true) == true);
        REQUIRE(pkb.isParent(1, 5, true) == true);
        REQUIRE(pkb.isParent(1, 6, true) == true);
        REQUIRE(pkb.isParent(1, 7, true) == true);
        REQUIRE(pkb.isParent(1, 8, true) == true);
        REQUIRE(pkb.isParent(1, 9, true) == true);
        REQUIRE(pkb.isParent(1, 17, true) == true);

        REQUIRE(pkb.isParent(4, 6, true) == true);
        REQUIRE(pkb.isParent(4, 7, true) == true);
        REQUIRE(pkb.isParent(4, 8, true) == true);
        REQUIRE(pkb.isParent(4, 9, true) == true);
        REQUIRE(pkb.isParent(4, 17, true) == true);

        REQUIRE(pkb.isParent(5, 7, true) == true);
        REQUIRE(pkb.isParent(5, 8, true) == true);
        REQUIRE(pkb.isParent(5, 9, true) == true);
        REQUIRE(pkb.isParent(5, 17, true) == true);

        REQUIRE(pkb.isParent(6, 11, true) == true);
        REQUIRE(pkb.isParent(6, 12, true) == true);
        REQUIRE(pkb.isParent(6, 13, true) == true);
        REQUIRE(pkb.isParent(6, 14, true) == true);
        REQUIRE(pkb.isParent(6, 15, true) == true);
        REQUIRE(pkb.isParent(6, 16, true) == true);
        REQUIRE(pkb.isParent(6, 17, true) == true);

        REQUIRE(pkb.isParent(10, 15, true) == true);
        REQUIRE(pkb.isParent(10, 17, true) == true);
        REQUIRE(pkb.isParent(11, 15, true) == true);
        REQUIRE(pkb.isParent(11, 17, true) == true);
        REQUIRE(pkb.isParent(12, 15, true) == true);
        REQUIRE(pkb.isParent(12, 17, true) == true);
        REQUIRE(pkb.isParent(13, 17, true) == true);
        REQUIRE(pkb.isParent(14, 17, true) == true);
    }

    SECTION("Check that statements under different parents* don't have a wrong parent star relation") {
        REQUIRE(pkb.isParent(1, 32, true) == false);
        REQUIRE(pkb.isParent(1, 33, true) == false);
        REQUIRE(pkb.isParent(10, 32, true) == false);
        REQUIRE(pkb.isParent(13, 20, true) == false);
        REQUIRE(pkb.isParent(13, 34, true) == false);
    }

    SECTION("Check that statements without parent should not have the relation") {
        REQUIRE(pkb.isParent(1, 24, true) == false);
        REQUIRE(pkb.isParent(20, 21, true) == false);
        REQUIRE(pkb.isParent(24, 25, true) == false);
        REQUIRE(pkb.isParent(26, 28, true) == false);

        REQUIRE(pkb.isParent(32, 33, true) == false);
        REQUIRE(pkb.isParent(33, 34, true) == false);

        REQUIRE(pkb.isParent(35, 37, true) == false);
        REQUIRE(pkb.isParent(35, 38, true) == false);
    }
}

TEST_CASE("Program 02: Modifies relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct modifies relation for statements") {
        REQUIRE(pkb.getModifiesVariables(1) == generateVarSet("a i p a d j m"));
        REQUIRE(pkb.getModifiesVariables(2) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(3) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(4) == generateVarSet("a i p a d j m"));
        REQUIRE(pkb.getModifiesVariables(5) == generateVarSet("a i p a d j m"));
        REQUIRE(pkb.getModifiesVariables(6) == generateVarSet("i p"));
        REQUIRE(pkb.getModifiesVariables(7) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(8) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(9) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(10) == generateVarSet("i p"));
        REQUIRE(pkb.getModifiesVariables(11) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(12) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(13) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(14) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(15) == generateVarSet("i"));
        REQUIRE(pkb.getModifiesVariables(16) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(17) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(18) == generateVarSet("p"));
        REQUIRE(pkb.getModifiesVariables(19) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(20) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(21) == generateVarSet("d"));
        REQUIRE(pkb.getModifiesVariables(22) == generateVarSet("j"));
        REQUIRE(pkb.getModifiesVariables(23) == generateVarSet("m"));
        REQUIRE(pkb.getModifiesVariables(24) == generateVarSet("f"));
        REQUIRE(pkb.getModifiesVariables(25) == generateVarSet("read"));
        REQUIRE(pkb.getModifiesVariables(26) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(27) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(28) == generateVarSet(""));
        REQUIRE(pkb.getModifiesVariables(29) == generateVarSet("j"));
        REQUIRE(pkb.getModifiesVariables(30) == generateVarSet("procedure"));
        REQUIRE(pkb.getModifiesVariables(31) == generateVarSet("then print"));
        REQUIRE(pkb.getModifiesVariables(32) == generateVarSet("then"));
        REQUIRE(pkb.getModifiesVariables(33) == generateVarSet("print"));
        REQUIRE(pkb.getModifiesVariables(34) == generateVarSet(""));

        REQUIRE(pkb.getModifiesVariables(35) == generateVarSet("a"));
        REQUIRE(pkb.getModifiesVariables(36) == generateVarSet("k"));
        REQUIRE(pkb.getModifiesVariables(37) == generateVarSet("check"));
        REQUIRE(pkb.getModifiesVariables(38) == generateVarSet("a i p a d j m f read a j procedure then print"));
    }

    SECTION("Check correct modifies relation for procedures") {
        REQUIRE(pkb.getModifiesVariableFromProc("procedure") == generateVarSet("a i p a d j m f read a j procedure then print"));
        REQUIRE(pkb.getModifiesVariableFromProc("test2") ==
                generateVarSet("a i p a d j m f read a j procedure then print a k check"));
    }
}

TEST_CASE("Program 02: Uses relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct uses relation for statements") {
        REQUIRE(
            pkb.getUsesVariables(1) ==
            generateVarSet("a b c b d a c a f g g a b k c g d f c a b a b i j q b c d e f g b g k f k e h e j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(2) == generateVarSet("a"));
        REQUIRE(pkb.getUsesVariables(3) == generateVarSet(""));
        REQUIRE(
            pkb.getUsesVariables(4) ==
            generateVarSet("a b c b d a c a f g g a b k c g d f c a b a b i j q b c d e f g b g k f k e h e j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(5) ==
                generateVarSet("b c b d a c a f g g a b k c g d f c a b a b i j q b c d e f g b g k f k e h e j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(6) == generateVarSet("b d a c a f g g a b k c g d f c a b a b i j q"));
        REQUIRE(pkb.getUsesVariables(7) == generateVarSet("d a c a"));
        REQUIRE(pkb.getUsesVariables(8) == generateVarSet("c a"));
        REQUIRE(pkb.getUsesVariables(9) == generateVarSet("a"));
        REQUIRE(pkb.getUsesVariables(10) == generateVarSet("f g g a b k c g d f c a b a b i j q"));
        REQUIRE(pkb.getUsesVariables(11) == generateVarSet("g a b k c g d f c a b a b i j"));
        REQUIRE(pkb.getUsesVariables(12) == generateVarSet("c g d f c a b a b i j"));
        REQUIRE(pkb.getUsesVariables(13) == generateVarSet("b a b i j"));
        REQUIRE(pkb.getUsesVariables(14) == generateVarSet("i j"));
        REQUIRE(pkb.getUsesVariables(15) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(16) == generateVarSet("j"));
        REQUIRE(pkb.getUsesVariables(17) == generateVarSet("j"));
        REQUIRE(pkb.getUsesVariables(18) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(19) == generateVarSet("q"));
        REQUIRE(pkb.getUsesVariables(20) == generateVarSet("b c d e f g"));
        REQUIRE(pkb.getUsesVariables(21) == generateVarSet("b g k f"));
        REQUIRE(pkb.getUsesVariables(22) == generateVarSet("k e h e"));
        REQUIRE(pkb.getUsesVariables(23) == generateVarSet("j h i k f a m d"));
        REQUIRE(pkb.getUsesVariables(24) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(25) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(26) == generateVarSet("l"));
        REQUIRE(pkb.getUsesVariables(27) == generateVarSet("h k d"));
        REQUIRE(pkb.getUsesVariables(28) == generateVarSet("x"));
        REQUIRE(pkb.getUsesVariables(29) == generateVarSet("while"));
        REQUIRE(pkb.getUsesVariables(30) == generateVarSet("if"));
        REQUIRE(pkb.getUsesVariables(31) == generateVarSet("procedure statement read print call while if k h j"));
        REQUIRE(pkb.getUsesVariables(32) == generateVarSet("read"));
        REQUIRE(pkb.getUsesVariables(33) == generateVarSet("call k h j"));
        REQUIRE(pkb.getUsesVariables(34) == generateVarSet("print"));

        REQUIRE(pkb.getUsesVariables(35) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(36) == generateVarSet("h"));
        REQUIRE(pkb.getUsesVariables(37) == generateVarSet(""));
        REQUIRE(pkb.getUsesVariables(38) ==
                generateVarSet("a b c d e f g h i j k l m q x while procedure statement read print call if"));
    }

    SECTION("Check correct uses relation for procedures") {
        REQUIRE(pkb.getUsesVariableFromProc("procedure") ==
                generateVarSet("a b c d e f g h i j k l m q x while procedure statement read print call if"));
        REQUIRE(pkb.getUsesVariableFromProc("test2") ==
                generateVarSet("a b c d e f g h i j k l m q x while procedure statement read print call if"));
    }
}

TEST_CASE("Program 02: Calls relation") {
    PKB &pkb = PKB::Instance();

    SECTION("Check correct calls relation for statements") {
        REQUIRE(pkb.getProceduresCalledBy("procedure") == generateVarSet(""));
        REQUIRE(pkb.getProceduresCalledBy("test2") == generateVarSet("procedure"));
    }
}

TEST_CASE("Program 02: Populate PKB with Statements - and their attributes") {
    PKB &pkb = PKB::Instance();

    SECTION("Check that PKB is populated with correct variables in a statement") {
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("1")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("2")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("3")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("4")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("5")) == generateVarSet("b c"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("6")) == generateVarSet("b c"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("7")) == generateVarSet("a c d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("8")) == generateVarSet("c"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("9")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("10")) == generateVarSet("f g"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("11")) == generateVarSet("g a b k"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("12")) == generateVarSet("c g d f c a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("13")) == generateVarSet("a b"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("14")) == generateVarSet("i j"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("15")) == generateVarSet("i"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("16")) == generateVarSet(""));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("17")) == generateVarSet("j"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("18")) == generateVarSet("p"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("19")) == generateVarSet("q"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("20")) == generateVarSet("a b c d e f g"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("21")) == generateVarSet("d b g k f"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("22")) == generateVarSet("j k e h"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("23")) == generateVarSet("m j h i j k f a k m d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("24")) == generateVarSet("f"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("25")) == generateVarSet("read"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("26")) == generateVarSet("l"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("27")) == generateVarSet("a h k d"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("28")) == generateVarSet("x"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("29")) == generateVarSet("j while"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("30")) == generateVarSet("procedure if"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("31")) ==
                generateVarSet("if procedure statement read print call while if k"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("32")) == generateVarSet("then read"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("33")) == generateVarSet("print call k h j"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("34")) == generateVarSet("print"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("35")) == generateVarSet("a"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("36")) == generateVarSet("k h"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("37")) == generateVarSet("check"));
        REQUIRE(pkb.getAllVarsInStatement(generateStmtNumSet("38")) == generateVarSet(""));
    }
}

TEST_CASE("Program 02: Populate PKB with Assignment and Conditional pattern") {
    PKB &pkb = PKB::Instance();

    SECTION("Check that assignment statements have the correct pattern stored") {
        REQUIRE(pkb.getStatementsWithPattern("a", "b + c - d * e / f % g", true, true) == generateStmtNumSet("20"));
        REQUIRE(pkb.getStatementsWithPattern("d", "b * ( ( g * k ) / f )", true, true) == generateStmtNumSet("21"));
        REQUIRE(pkb.getStatementsWithPattern("j", "( ( k * e ) + h ) / e", true, true) == generateStmtNumSet("22"));
        REQUIRE(pkb.getStatementsWithPattern("m", "j + ( h - ( i * ( j - k ) / ( f % a ) - k ) + m ) * d", true, true) ==
                generateStmtNumSet("23"));
        REQUIRE(pkb.getStatementsWithPattern("a", "h * k + d * ( k )", true, true) == generateStmtNumSet("27"));
        REQUIRE(pkb.getStatementsWithPattern("procedure", "341242 * if + 8248", true, true) == generateStmtNumSet("30"));
        REQUIRE(pkb.getStatementsWithPattern("then", "read", true, true) == generateStmtNumSet("32"));
        REQUIRE(pkb.getStatementsWithPattern("print", "call * k + h - h % j", true, true) == generateStmtNumSet("33"));
    }

    SECTION("Check that conditional patterns have the correct pattern stored") {
        REQUIRE(pkb.getVarMatchConditionStatements("a", WHILE_STMT) == generateStmtNumSet("2 4 7 11 12 13"));
        REQUIRE(pkb.getVarMatchConditionStatements("b", WHILE_STMT) == generateStmtNumSet("11 13"));
        REQUIRE(pkb.getVarMatchConditionStatements("c", WHILE_STMT) == generateStmtNumSet("7 8 12"));
        REQUIRE(pkb.getVarMatchConditionStatements("d", WHILE_STMT) == generateStmtNumSet("7 12"));
        REQUIRE(pkb.getVarMatchConditionStatements("f", WHILE_STMT) == generateStmtNumSet("12"));
        REQUIRE(pkb.getVarMatchConditionStatements("g", WHILE_STMT) == generateStmtNumSet("11 12"));
        REQUIRE(pkb.getVarMatchConditionStatements("k", WHILE_STMT) == generateStmtNumSet("11"));

        REQUIRE(pkb.getVarMatchConditionStatements("a", IF_STMT) == generateStmtNumSet("1"));
        REQUIRE(pkb.getVarMatchConditionStatements("b", IF_STMT) == generateStmtNumSet("5 6"));
        REQUIRE(pkb.getVarMatchConditionStatements("c", IF_STMT) == generateStmtNumSet("5 6"));
        REQUIRE(pkb.getVarMatchConditionStatements("f", IF_STMT) == generateStmtNumSet("10"));
        REQUIRE(pkb.getVarMatchConditionStatements("g", IF_STMT) == generateStmtNumSet("10"));
        REQUIRE(pkb.getVarMatchConditionStatements("i", IF_STMT) == generateStmtNumSet("14"));
        REQUIRE(pkb.getVarMatchConditionStatements("j", IF_STMT) == generateStmtNumSet("14"));
        REQUIRE(pkb.getVarMatchConditionStatements("procedure", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("statement", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("read", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("print", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("call", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("while", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("if", IF_STMT) == generateStmtNumSet("31"));
        REQUIRE(pkb.getVarMatchConditionStatements("k", IF_STMT) == generateStmtNumSet("31"));
    }
}

// TEST_CASE("Program 02: Populate PKB with Procedure information") {
//    PKB& pkb = PKB::Instance();
//
//    SECTION("Check that procedure have the correct name and start lines") {
//
//    }
//}

TEST_CASE("Program 02: Test CFG") {
    CallGraph cg = program02CallGraphSetup();
    DesignExtractor designExtractor(program02Setup(), cg);
    std::unordered_map<int, std::set<int>> cfg = designExtractor.createCFG(program02Setup());

    SECTION("Check that adjacency list has correct values") {
        REQUIRE(cfg.find(1)->second == generateStmtNumOrderedSet("2 4"));
        REQUIRE(cfg.find(2)->second == generateStmtNumOrderedSet("3 24"));
        REQUIRE(cfg.find(3)->second == generateStmtNumOrderedSet("2"));
        REQUIRE(cfg.find(4)->second == generateStmtNumOrderedSet("5 24"));
        REQUIRE(cfg.find(5)->second == generateStmtNumOrderedSet("6 20"));
        REQUIRE(cfg.find(6)->second == generateStmtNumOrderedSet("7 10"));
        REQUIRE(cfg.find(7)->second == generateStmtNumOrderedSet("4 8"));
        REQUIRE(cfg.find(8)->second == generateStmtNumOrderedSet("9 7"));
        REQUIRE(cfg.find(9)->second == generateStmtNumOrderedSet("8"));
        REQUIRE(cfg.find(10)->second == generateStmtNumOrderedSet("11 18"));
        REQUIRE(cfg.find(11)->second == generateStmtNumOrderedSet("4 12"));
        REQUIRE(cfg.find(12)->second == generateStmtNumOrderedSet("13 11"));
        REQUIRE(cfg.find(13)->second == generateStmtNumOrderedSet("14 12"));
        REQUIRE(cfg.find(14)->second == generateStmtNumOrderedSet("15 16"));
        REQUIRE(cfg.find(15)->second == generateStmtNumOrderedSet("13"));
        REQUIRE(cfg.find(16)->second == generateStmtNumOrderedSet("13 17"));
        REQUIRE(cfg.find(17)->second == generateStmtNumOrderedSet("16"));
        REQUIRE(cfg.find(18)->second == generateStmtNumOrderedSet("19"));
        REQUIRE(cfg.find(19)->second == generateStmtNumOrderedSet("4"));
        REQUIRE(cfg.find(20)->second == generateStmtNumOrderedSet("21"));
        REQUIRE(cfg.find(21)->second == generateStmtNumOrderedSet("22"));
        REQUIRE(cfg.find(22)->second == generateStmtNumOrderedSet("23"));
        REQUIRE(cfg.find(23)->second == generateStmtNumOrderedSet("4"));
        REQUIRE(cfg.find(24)->second == generateStmtNumOrderedSet("25"));
        REQUIRE(cfg.find(25)->second == generateStmtNumOrderedSet("26"));
        REQUIRE(cfg.find(26)->second == generateStmtNumOrderedSet("27"));
        REQUIRE(cfg.find(27)->second == generateStmtNumOrderedSet("28"));
        REQUIRE(cfg.find(28)->second == generateStmtNumOrderedSet("29"));
        REQUIRE(cfg.find(29)->second == generateStmtNumOrderedSet("30"));
        REQUIRE(cfg.find(30)->second == generateStmtNumOrderedSet("31"));
        REQUIRE(cfg.find(31)->second == generateStmtNumOrderedSet("32 34"));
        REQUIRE(cfg.find(32)->second == generateStmtNumOrderedSet("33"));
        REQUIRE(cfg.find(33) == cfg.end());
        REQUIRE(cfg.find(34) == cfg.end());
        REQUIRE(cfg.find(35)->second == generateStmtNumOrderedSet("36"));
        REQUIRE(cfg.find(36)->second == generateStmtNumOrderedSet("37"));
        REQUIRE(cfg.find(37)->second == generateStmtNumOrderedSet("38"));
        REQUIRE(cfg.find(38) == cfg.end());
    }
}

TEST_CASE("Program 02: PKB Teardown") {
    PKB &pkb = PKB::Instance();
    pkb.clear();
}
