#include "AffectStorage.h"
#include "catch.hpp"

class TestAffectStorage {

  public:
    std::unordered_map<int, std::set<int>> sampleCfg1{
        {1, std::set<int>{2}},       {2, std::set<int>{3}},       {3, std::set<int>{4}},   {4, std::set<int>{5, 8}},
        {5, std::set<int>{6}},       {6, std::set<int>{7}},       {7, std::set<int>{4}},   {9, std::set<int>{10}},
        {10, std::set<int>{11}},     {11, std::set<int>{12, 20}}, {12, std::set<int>{13}}, {13, std::set<int>{14}},
        {14, std::set<int>{15, 16}}, {15, std::set<int>{17}},     {16, std::set<int>{17}}, {17, std::set<int>{18}},
        {18, std::set<int>{19}},     {19, std::set<int>{11}}};
    std::unordered_map<int, std::set<int>> sampleCfg2{
        {1, std::set<int>{2}},   {2, std::set<int>{3}},      {3, std::set<int>{4, 14}},   {4, std::set<int>{5}},
        {5, std::set<int>{6}},   {6, std::set<int>{7}},      {7, std::set<int>{3, 8}},    {8, std::set<int>{9}},
        {9, std::set<int>{10}},  {10, std::set<int>{7, 11}}, {11, std::set<int>{12, 13}}, {12, std::set<int>{10}},
        {13, std::set<int>{10}}, {14, std::set<int>{15}},    {15, std::set<int>{16}}};
    std::unordered_map<int, std::set<int>> sampleCfg3{
        {1, std::set<int>{2}},   {2, std::set<int>{3}},       {3, std::set<int>{4}},       {4, std::set<int>{5, 15}},
        {5, std::set<int>{6}},   {6, std::set<int>{7, 8}},    {7, std::set<int>{4}},       {8, std::set<int>{9}},
        {9, std::set<int>{10}},  {10, std::set<int>{11, 14}}, {11, std::set<int>{12}},     {12, std::set<int>{13}},
        {13, std::set<int>{10}}, {14, std::set<int>{4}},      {15, std::set<int>{16, 17}}, {16, std::set<int>{19}},
        {17, std::set<int>{18}}, {18, std::set<int>{19}},     {19, std::set<int>{20}}};
    std::unordered_map<int, std::set<int>> sampleCfg4{
        {1, std::set<int>{2}},    {2, std::set<int>{3}},    {3, std::set<int>{4}}, {4, std::set<int>{5, 9}},
        {5, std::set<int>{6, 4}}, {6, std::set<int>{7, 5}}, {7, std::set<int>{8}}, {8, std::set<int>{6}}};

    AffectStorage *aStore = AffectStorage::getInstance();
    NextStorage *nStore = NextStorage::getInstance();
    UseStorage *uStore = UseStorage::getInstance();
    ModifyStorage *mStore = ModifyStorage::getInstance();
    ParentStorage *pStore = ParentStorage::getInstance();
    GeneralStorage *gStore = GeneralStorage::getInstance();
    void setUpProg1();
    void setUpProg2();
    void setUpProg3();
    void setUpProg4();
    void clearAll();
};

//      procedure First {
//  1       read x;
//  2       read z;
//  3       call Second;
//  4       while (apple < 10) {
//  5           grape = orange + 10;
//  6           orange = apple + banana;
//  7           apple = grape + 30;}
//  8       x = 2 + x;}
//
//      procedure Second {
//  9       x = 0;
// 10       i = 5;
// 11       while (i != 0) {
// 12           x = x + 2 * y;
// 13           call Third;
// 14           if (x == 1) then {
// 15               x = x + 1; }
//              else {
// 16               z = 1;}
// 17           print z;
// 18           print y;
// 19           x = x * y + z;}
// 20       i = i - 1;}
//		procedure Third {
// 21		cat = dog;}

void TestAffectStorage::setUpProg1() {
    nStore->setAdjacencyListMap(sampleCfg1);
    nStore->setReverseAdjacencyListMap(sampleCfg1);
    aStore->setupAffectStorage();

    nStore->setProcedureRange(1, 8);
    nStore->setProcedureRange(9, 20);
    aStore->setProcedureRange(1, 8);
    aStore->setProcedureRange(9, 20);

    std::unordered_set<std::string> mockVarSet;
    std::unordered_set<int> mockConstSet;
    gStore->storeStmtObj({1, READ, mockVarSet, mockConstSet});
    gStore->storeStmtObj({2, READ, mockVarSet, mockConstSet});
    gStore->storeStmtObj({3, READ, mockVarSet, mockConstSet});
    gStore->storeStmtObj({4, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({5, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({6, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({7, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({8, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({9, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({10, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({11, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({12, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({13, CALL, mockVarSet, mockConstSet});
    gStore->storeStmtObj({14, IF_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({15, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({16, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({17, PRINT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({18, PRINT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({19, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({20, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({21, ASSIGN, mockVarSet, mockConstSet});

    nStore->setStmtToWhileParent(5, 4);
    nStore->setStmtToWhileParent(6, 4);
    nStore->setStmtToWhileParent(7, 4);
    nStore->setStmtToWhileParent(12, 11);
    nStore->setStmtToWhileParent(13, 11);
    nStore->setStmtToWhileParent(17, 11);
    nStore->setStmtToWhileParent(18, 11);
    nStore->setStmtToWhileParent(19, 11);

    uStore->setStmtToVar(4, "apple");
    uStore->setStmtToVar(4, "orange");
    uStore->setStmtToVar(4, "grape");
    uStore->setStmtToVar(5, "orange");
    uStore->setStmtToVar(6, "apple");
    uStore->setStmtToVar(6, "banana");
    uStore->setStmtToVar(7, "grape");
    uStore->setStmtToVar(8, "x");
    uStore->setStmtToVar(3, "x");
    uStore->setStmtToVar(3, "y");
    uStore->setStmtToVar(3, "z");
    uStore->setStmtToVar(3, "i");
    uStore->setStmtToVar(12, "x");
    uStore->setStmtToVar(12, "y");
    uStore->setStmtToVar(13, "dog");
    uStore->setStmtToVar(11, "x");
    uStore->setStmtToVar(11, "y");
    uStore->setStmtToVar(11, "z");
    uStore->setStmtToVar(14, "x");
    uStore->setStmtToVar(15, "x");
    uStore->setStmtToVar(17, "z");
    uStore->setStmtToVar(18, "y");
    uStore->setStmtToVar(19, "x");
    uStore->setStmtToVar(19, "y");
    uStore->setStmtToVar(19, "z");
    uStore->setStmtToVar(20, "i");
    uStore->setStmtToVar(21, "dog");

    uStore->setVarToStmt(4, WHILE_STMT, "apple");
    uStore->setVarToStmt(4, WHILE_STMT, "orange");
    uStore->setVarToStmt(4, WHILE_STMT, "grape");
    uStore->setVarToStmt(5, ASSIGN, "orange");
    uStore->setVarToStmt(6, ASSIGN, "apple");
    uStore->setVarToStmt(6, ASSIGN, "banana");
    uStore->setVarToStmt(7, ASSIGN, "grape");
    uStore->setVarToStmt(8, ASSIGN, "x");
    uStore->setVarToStmt(11, WHILE_STMT, "x");
    uStore->setVarToStmt(11, WHILE_STMT, "y");
    uStore->setVarToStmt(11, WHILE_STMT, "z");
    uStore->setVarToStmt(12, ASSIGN, "x");
    uStore->setVarToStmt(12, ASSIGN, "y");
    uStore->setVarToStmt(13, CALL, "dog");
    uStore->setVarToStmt(14, IF_STMT, "x");
    uStore->setVarToStmt(15, ASSIGN, "x");
    uStore->setVarToStmt(17, PRINT, "z");
    uStore->setVarToStmt(18, PRINT, "y");
    uStore->setVarToStmt(19, ASSIGN, "y");
    uStore->setVarToStmt(19, ASSIGN, "z");
    uStore->setVarToStmt(20, ASSIGN, "i");
    uStore->setVarToStmt(21, ASSIGN, "dog");

    mStore->setStmtToVar(1, "x");
    mStore->setStmtToVar(2, "z");
    mStore->setStmtToVar(3, "x");
    mStore->setStmtToVar(3, "i");
    mStore->setStmtToVar(3, "z");
    mStore->setStmtToVar(4, "grape");
    mStore->setStmtToVar(4, "orange");
    mStore->setStmtToVar(4, "apple");
    mStore->setStmtToVar(5, "grape");
    mStore->setStmtToVar(6, "orange");
    mStore->setStmtToVar(7, "apple");
    mStore->setStmtToVar(8, "x");
    mStore->setStmtToVar(9, "x");
    mStore->setStmtToVar(10, "i");
    mStore->setStmtToVar(11, "x");
    mStore->setStmtToVar(12, "x");
    mStore->setStmtToVar(13, "cat");
    mStore->setStmtToVar(14, "x");
    mStore->setStmtToVar(14, "z");
    mStore->setStmtToVar(15, "x");
    mStore->setStmtToVar(16, "z");
    mStore->setStmtToVar(19, "x");
    mStore->setStmtToVar(20, "i");
    mStore->setStmtToVar(21, "cat");

    mStore->setVarToStmt(1, READ, "x");
    mStore->setVarToStmt(2, READ, "z");
    mStore->setVarToStmt(3, READ, "Second");
    mStore->setVarToStmt(4, WHILE_STMT, "grape");
    mStore->setVarToStmt(4, WHILE_STMT, "orange");
    mStore->setVarToStmt(4, WHILE_STMT, "apple");
    mStore->setVarToStmt(5, ASSIGN, "grape");
    mStore->setVarToStmt(6, ASSIGN, "orange");
    mStore->setVarToStmt(7, ASSIGN, "apple");
    mStore->setVarToStmt(8, ASSIGN, "x");
    mStore->setVarToStmt(9, ASSIGN, "x");
    mStore->setVarToStmt(10, ASSIGN, "i");
    mStore->setVarToStmt(11, WHILE_STMT, "x");
    mStore->setVarToStmt(12, ASSIGN, "x");
    mStore->setVarToStmt(13, CALL, "cat");
    mStore->setVarToStmt(14, IF_STMT, "x");
    mStore->setVarToStmt(14, IF_STMT, "z");
    mStore->setVarToStmt(15, ASSIGN, "x");
    mStore->setVarToStmt(16, ASSIGN, "z");
    mStore->setVarToStmt(19, ASSIGN, "x");
    mStore->setVarToStmt(20, ASSIGN, "i");
    mStore->setVarToStmt(21, ASSIGN, "cat");

    pStore->setParentStar(4, 5);
    pStore->setParentStar(4, 6);
    pStore->setParentStar(4, 7);
    pStore->setParentStar(11, 12);
    pStore->setParentStar(11, 13);
    pStore->setParentStar(11, 14);
    pStore->setParentStar(11, 15);
    pStore->setParentStar(11, 16);
    pStore->setParentStar(11, 17);
    pStore->setParentStar(11, 18);
    pStore->setParentStar(11, 19);
    pStore->setParentStar(14, 15);
    pStore->setParentStar(14, 16);
}

//	procedure Second{
// 1		x = 0;
// 2		i = 5;
// 3		while (i != 0) {
// 4			x = x + 2 * y;
// 5			call Third;
// 6			i = i - 1;
// 7			while (x < 10) {
// 8				x = x + 3;
// 9				y = i * 6;
// 10			while (y < 10) {
// 11				if (x == 1) then {
// 12					x = x + 1;
//					} else {
// 13					z = 1;
//					}
//				}
//			}
//		}
// 14	z = z + x + i;
// 15	y = z + 2;
// 16	x = x * y + z; }
// procedure Third {
// 17	cat = dog;}

void TestAffectStorage::setUpProg2() {
    nStore->setAdjacencyListMap(sampleCfg2);
    nStore->setReverseAdjacencyListMap(sampleCfg2);
    aStore->setupAffectStorage();

    nStore->setProcedureRange(1, 16);
    nStore->setProcedureRange(17, 17);
    aStore->setProcedureRange(1, 16);
    aStore->setProcedureRange(17, 17);

    std::unordered_set<std::string> mockVarSet;
    std::unordered_set<int> mockConstSet;
    gStore->storeStmtObj({1, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({2, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({3, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({4, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({5, CALL, mockVarSet, mockConstSet});
    gStore->storeStmtObj({6, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({7, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({8, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({9, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({10, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({11, IF_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({12, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({13, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({14, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({15, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({16, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({17, ASSIGN, mockVarSet, mockConstSet});

    nStore->setStmtToWhileParent(4, 3);
    nStore->setStmtToWhileParent(5, 3);
    nStore->setStmtToWhileParent(6, 3);
    nStore->setStmtToWhileParent(7, 3);
    nStore->setStmtToWhileParent(8, 7);
    nStore->setStmtToWhileParent(9, 7);
    nStore->setStmtToWhileParent(10, 7);
    nStore->setStmtToWhileParent(11, 10);

    uStore->setStmtToVar(3, "i");
    uStore->setStmtToVar(3, "x");
    uStore->setStmtToVar(3, "dog");
    uStore->setStmtToVar(3, "y");
    uStore->setStmtToVar(4, "x");
    uStore->setStmtToVar(4, "y");
    uStore->setStmtToVar(5, "dog");

    uStore->setStmtToVar(6, "i");
    uStore->setStmtToVar(7, "i");
    uStore->setStmtToVar(7, "x");
    uStore->setStmtToVar(7, "y");
    uStore->setStmtToVar(8, "x");
    uStore->setStmtToVar(9, "i");
    uStore->setStmtToVar(10, "x");
    uStore->setStmtToVar(10, "y");
    uStore->setStmtToVar(11, "x");

    uStore->setStmtToVar(12, "x");
    uStore->setStmtToVar(14, "x");
    uStore->setStmtToVar(14, "z");
    uStore->setStmtToVar(14, "i");
    uStore->setStmtToVar(15, "z");
    uStore->setStmtToVar(16, "z");
    uStore->setStmtToVar(16, "x");
    uStore->setStmtToVar(16, "y");
    uStore->setStmtToVar(17, "dog");

    uStore->setVarToStmt(3, WHILE_STMT, "i");
    uStore->setVarToStmt(3, WHILE_STMT, "x");
    uStore->setVarToStmt(3, WHILE_STMT, "dog");
    uStore->setVarToStmt(3, WHILE_STMT, "y");
    uStore->setVarToStmt(4, ASSIGN, "x");
    uStore->setVarToStmt(4, ASSIGN, "y");
    uStore->setVarToStmt(5, CALL, "dog");
    uStore->setVarToStmt(6, ASSIGN, "i");
    uStore->setVarToStmt(7, WHILE_STMT, "x");
    uStore->setVarToStmt(7, WHILE_STMT, "i");
    uStore->setVarToStmt(8, ASSIGN, "x");
    uStore->setVarToStmt(9, ASSIGN, "y");

    uStore->setVarToStmt(10, WHILE_STMT, "y");
    uStore->setVarToStmt(10, WHILE_STMT, "x");
    uStore->setVarToStmt(11, IF_STMT, "x");
    uStore->setVarToStmt(12, ASSIGN, "x");
    uStore->setVarToStmt(14, ASSIGN, "z");
    uStore->setVarToStmt(14, ASSIGN, "x");
    uStore->setVarToStmt(14, ASSIGN, "i");
    uStore->setVarToStmt(15, ASSIGN, "z");
    uStore->setVarToStmt(16, ASSIGN, "x");
    uStore->setVarToStmt(16, ASSIGN, "y");
    uStore->setVarToStmt(16, ASSIGN, "z");
    uStore->setVarToStmt(17, ASSIGN, "dog");

    mStore->setStmtToVar(1, "x");
    mStore->setStmtToVar(2, "i");
    mStore->setStmtToVar(3, "x");
    mStore->setStmtToVar(3, "i");
    mStore->setStmtToVar(3, "cat");
    mStore->setStmtToVar(3, "z");
    mStore->setStmtToVar(3, "y");
    mStore->setStmtToVar(4, "x");

    mStore->setStmtToVar(5, "cat");
    mStore->setStmtToVar(6, "i");
    mStore->setStmtToVar(7, "x");
    mStore->setStmtToVar(7, "y");
    mStore->setStmtToVar(7, "z");

    mStore->setStmtToVar(8, "x");
    mStore->setStmtToVar(9, "y");
    mStore->setStmtToVar(10, "x");
    mStore->setStmtToVar(10, "z");

    mStore->setStmtToVar(11, "x");
    mStore->setStmtToVar(11, "z");

    mStore->setStmtToVar(12, "x");
    mStore->setStmtToVar(13, "z");
    mStore->setStmtToVar(14, "z");
    mStore->setStmtToVar(15, "y");
    mStore->setStmtToVar(16, "x");
    mStore->setStmtToVar(17, "cat");

    mStore->setVarToStmt(1, ASSIGN, "x");
    mStore->setVarToStmt(2, ASSIGN, "i");
    mStore->setVarToStmt(3, WHILE_STMT, "i");
    mStore->setVarToStmt(3, WHILE_STMT, "x");
    mStore->setVarToStmt(3, WHILE_STMT, "cat");
    mStore->setVarToStmt(3, WHILE_STMT, "y");
    mStore->setVarToStmt(3, WHILE_STMT, "z");

    mStore->setVarToStmt(4, ASSIGN, "x");
    mStore->setVarToStmt(5, CALL, "cat");
    mStore->setVarToStmt(6, ASSIGN, "i");
    mStore->setVarToStmt(7, WHILE_STMT, "x");
    mStore->setVarToStmt(7, WHILE_STMT, "y");
    mStore->setVarToStmt(7, WHILE_STMT, "z");

    mStore->setVarToStmt(8, ASSIGN, "x");
    mStore->setVarToStmt(9, ASSIGN, "y");

    mStore->setVarToStmt(10, WHILE_STMT, "z");
    mStore->setVarToStmt(10, WHILE_STMT, "x");
    mStore->setVarToStmt(11, IF_STMT, "x");
    mStore->setVarToStmt(11, IF_STMT, "z");

    mStore->setVarToStmt(12, ASSIGN, "x");
    mStore->setVarToStmt(13, ASSIGN, "z");

    mStore->setVarToStmt(14, ASSIGN, "z");
    mStore->setVarToStmt(15, ASSIGN, "y");
    mStore->setVarToStmt(16, ASSIGN, "x");
    mStore->setVarToStmt(17, ASSIGN, "cat");

    pStore->setParentStar(3, 4);
    pStore->setParentStar(3, 5);
    pStore->setParentStar(3, 6);
    pStore->setParentStar(3, 7);
    pStore->setParentStar(3, 8);
    pStore->setParentStar(3, 9);
    pStore->setParentStar(3, 10);
    pStore->setParentStar(3, 11);
    pStore->setParentStar(3, 12);
    pStore->setParentStar(3, 13);
    pStore->setParentStar(7, 8);
    pStore->setParentStar(7, 9);
    pStore->setParentStar(7, 10);
    pStore->setParentStar(7, 11);
    pStore->setParentStar(7, 12);
    pStore->setParentStar(7, 13);
    pStore->setParentStar(10, 11);
    pStore->setParentStar(10, 12);
    pStore->setParentStar(10, 13);
    pStore->setParentStar(11, 12);
    pStore->setParentStar(11, 13);
}

//	procedure main{
// 1		count = 0;
// 2		cenX = 0;
// 3		cenY = 0;
// 4		while ((x != 0) && (y != 0)) {
// 5			count = count + 1;
// 6			if (cenX == cenY) then {
// 7				print count;
//			} else {
// 8				a = 1;
// 9				count = a;
// 10			    while (cenX < cenY) {
// 11				    read cenX;
// 12				    read cenY;
// 13				    cenX = cenX + 1;
//				    }
// 14			    count = count + 1;
//			}
//		}
// 15	if (count == 0) then {
// 16		flag = 1;
//		} else {
// 17		cenX = cenX / count;
// 18		cenY = cenY / count;
//		}
// 19	normSq = cenX * cenX + cenY * cenY;
// 20	print normSq;
//	}

void TestAffectStorage::setUpProg3() {
    nStore->setAdjacencyListMap(sampleCfg3);
    nStore->setReverseAdjacencyListMap(sampleCfg3);
    aStore->setupAffectStorage();
    nStore->setProcedureRange(1, 20);
    aStore->setProcedureRange(1, 20);

    nStore->setStmtToWhileParent(5, 4);
    nStore->setStmtToWhileParent(6, 4);
    nStore->setStmtToWhileParent(11, 10);
    nStore->setStmtToWhileParent(12, 10);
    nStore->setStmtToWhileParent(13, 10);

    // it is safe for the mock set to be empty since the data is not being used for this unit test
    std::unordered_set<std::string> mockVarSet;
    std::unordered_set<int> mockConstSet;
    gStore->storeStmtObj({1, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({2, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({3, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({4, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({5, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({6, IF_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({7, PRINT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({8, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({9, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({10, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({11, READ, mockVarSet, mockConstSet});
    gStore->storeStmtObj({12, READ, mockVarSet, mockConstSet});
    gStore->storeStmtObj({13, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({14, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({15, IF_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({16, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({17, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({18, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({19, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({20, PRINT, mockVarSet, mockConstSet});

    uStore->setStmtToVar(4, "x");
    uStore->setStmtToVar(4, "y");
    uStore->setStmtToVar(4, "count");
    uStore->setStmtToVar(4, "cenX");
    uStore->setStmtToVar(4, "cenY");
    uStore->setStmtToVar(4, "a");
    uStore->setStmtToVar(5, "count");
    uStore->setStmtToVar(6, "cenX");
    uStore->setStmtToVar(6, "cenY");
    uStore->setStmtToVar(6, "count");
    uStore->setStmtToVar(6, "a");
    uStore->setStmtToVar(7, "count");
    uStore->setStmtToVar(9, "a");
    uStore->setStmtToVar(10, "cenY");
    uStore->setStmtToVar(10, "cenX");
    uStore->setStmtToVar(13, "cenX");
    uStore->setStmtToVar(14, "count");
    uStore->setStmtToVar(15, "count");
    uStore->setStmtToVar(15, "cenX");
    uStore->setStmtToVar(15, "cenY");
    uStore->setStmtToVar(17, "cenX");
    uStore->setStmtToVar(17, "count");
    uStore->setStmtToVar(18, "cenY");
    uStore->setStmtToVar(18, "count");
    uStore->setStmtToVar(19, "cenX");
    uStore->setStmtToVar(19, "cenY");
    uStore->setStmtToVar(20, "normSq");

    uStore->setVarToStmt(4, WHILE_STMT, "x");
    uStore->setVarToStmt(4, WHILE_STMT, "y");
    uStore->setVarToStmt(4, WHILE_STMT, "count");
    uStore->setVarToStmt(4, WHILE_STMT, "a");
    uStore->setVarToStmt(4, WHILE_STMT, "cenX");
    uStore->setVarToStmt(4, WHILE_STMT, "cenY");

    uStore->setVarToStmt(5, ASSIGN, "count");
    uStore->setVarToStmt(6, IF_STMT, "cenX");
    uStore->setVarToStmt(6, IF_STMT, "cenY");
    uStore->setVarToStmt(6, IF_STMT, "count");
    uStore->setVarToStmt(6, IF_STMT, "a");

    uStore->setVarToStmt(7, PRINT, "count");
    uStore->setVarToStmt(9, ASSIGN, "a");
    uStore->setVarToStmt(10, WHILE_STMT, "cenX");
    uStore->setVarToStmt(10, WHILE_STMT, "cenY");

    uStore->setVarToStmt(13, ASSIGN, "cenX");
    uStore->setVarToStmt(14, ASSIGN, "count");
    uStore->setVarToStmt(15, IF_STMT, "count");
    uStore->setVarToStmt(15, IF_STMT, "cenX");
    uStore->setVarToStmt(15, IF_STMT, "cenY");

    uStore->setVarToStmt(17, ASSIGN, "cenX");
    uStore->setVarToStmt(17, ASSIGN, "count");

    uStore->setVarToStmt(18, ASSIGN, "cenY");
    uStore->setVarToStmt(18, ASSIGN, "count");

    uStore->setVarToStmt(19, ASSIGN, "cenX");
    uStore->setVarToStmt(19, ASSIGN, "cenY");

    uStore->setVarToStmt(20, PRINT, "normSq");

    mStore->setStmtToVar(1, "count");
    mStore->setStmtToVar(2, "cenX");
    mStore->setStmtToVar(3, "cenY");
    mStore->setStmtToVar(4, "count");
    mStore->setStmtToVar(4, "a");
    mStore->setStmtToVar(4, "cenX");
    mStore->setStmtToVar(4, "cenY");
    mStore->setStmtToVar(5, "count");
    mStore->setStmtToVar(6, "a");
    mStore->setStmtToVar(6, "count");
    mStore->setStmtToVar(6, "cenX");
    mStore->setStmtToVar(6, "cenY");

    mStore->setStmtToVar(8, "a");
    mStore->setStmtToVar(9, "count");
    mStore->setStmtToVar(10, "cenX");
    mStore->setStmtToVar(10, "cenY");

    mStore->setStmtToVar(11, "cenX");
    mStore->setStmtToVar(12, "cenY");
    mStore->setStmtToVar(13, "cenX");
    mStore->setStmtToVar(14, "count");
    mStore->setStmtToVar(15, "flag");
    mStore->setStmtToVar(15, "cenX");
    mStore->setStmtToVar(15, "cenY");
    mStore->setStmtToVar(16, "flag");
    mStore->setStmtToVar(17, "cenX");
    mStore->setStmtToVar(18, "cenY");
    mStore->setStmtToVar(19, "normSq");

    mStore->setVarToStmt(1, ASSIGN, "count");
    mStore->setVarToStmt(2, ASSIGN, "cenX");
    mStore->setVarToStmt(3, ASSIGN, "cenY");
    mStore->setVarToStmt(4, WHILE_STMT, "count");
    mStore->setVarToStmt(4, WHILE_STMT, "a");
    mStore->setVarToStmt(4, WHILE_STMT, "cenX");
    mStore->setVarToStmt(4, WHILE_STMT, "cenY");

    mStore->setVarToStmt(5, ASSIGN, "count");
    mStore->setVarToStmt(6, IF_STMT, "cenX");
    mStore->setVarToStmt(6, IF_STMT, "cenY");
    mStore->setVarToStmt(6, IF_STMT, "count");
    mStore->setVarToStmt(6, IF_STMT, "a");

    mStore->setVarToStmt(8, ASSIGN, "a");
    mStore->setVarToStmt(9, ASSIGN, "count");
    mStore->setVarToStmt(10, WHILE_STMT, "cenX");
    mStore->setVarToStmt(10, WHILE_STMT, "cenY");
    mStore->setVarToStmt(11, READ, "cenX");
    mStore->setVarToStmt(13, READ, "cenY");

    mStore->setVarToStmt(13, ASSIGN, "cenX");
    mStore->setVarToStmt(14, ASSIGN, "count");
    mStore->setVarToStmt(15, IF_STMT, "flag");
    mStore->setVarToStmt(15, IF_STMT, "cenX");
    mStore->setVarToStmt(15, IF_STMT, "cenY");
    mStore->setVarToStmt(16, ASSIGN, "flag");

    mStore->setVarToStmt(17, ASSIGN, "cenX");

    mStore->setVarToStmt(18, ASSIGN, "cenY");

    mStore->setVarToStmt(19, ASSIGN, "normSq");

    pStore->setParentStar(4, 5);
    pStore->setParentStar(4, 6);
    pStore->setParentStar(4, 7);
    pStore->setParentStar(4, 8);
    pStore->setParentStar(4, 9);
    pStore->setParentStar(4, 10);
    pStore->setParentStar(4, 11);
    pStore->setParentStar(4, 12);
    pStore->setParentStar(4, 13);
    pStore->setParentStar(4, 14);
    pStore->setParentStar(6, 7);
    pStore->setParentStar(6, 8);
    pStore->setParentStar(6, 9);
    pStore->setParentStar(6, 10);
    pStore->setParentStar(6, 11);
    pStore->setParentStar(6, 12);
    pStore->setParentStar(6, 13);
    pStore->setParentStar(6, 14);
    pStore->setParentStar(10, 11);
    pStore->setParentStar(10, 12);
    pStore->setParentStar(10, 13);
    pStore->setParentStar(15, 16);
    pStore->setParentStar(15, 17);
    pStore->setParentStar(15, 18);
}

//	procedure main{
// 1		count = 0;
// 2		cenX = 0;
// 3		cenY = 0;
// 4		while ((x != 0) && (y != 0)) {
// 5			while(x > 10) {
// 6				while(y < 3) {
// 7					x = count + cenY;
// 8					count = cenX;
//				}
//			}
//		}
// 9		count = count + x;
//	}

void TestAffectStorage::setUpProg4() {
    nStore->setAdjacencyListMap(sampleCfg4);
    nStore->setReverseAdjacencyListMap(sampleCfg4);
    aStore->setupAffectStorage();

    nStore->setProcedureRange(1, 9);
    aStore->setProcedureRange(1, 9);

    std::unordered_set<std::string> mockVarSet;
    std::unordered_set<int> mockConstSet;
    gStore->storeStmtObj({1, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({2, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({3, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({4, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({5, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({6, WHILE_STMT, mockVarSet, mockConstSet});
    gStore->storeStmtObj({7, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({8, ASSIGN, mockVarSet, mockConstSet});
    gStore->storeStmtObj({9, ASSIGN, mockVarSet, mockConstSet});

    nStore->setStmtToWhileParent(5, 4);
    nStore->setStmtToWhileParent(6, 5);
    nStore->setStmtToWhileParent(7, 6);
    nStore->setStmtToWhileParent(8, 6);

    uStore->setStmtToVar(4, "x");
    uStore->setStmtToVar(4, "y");
    uStore->setStmtToVar(5, "x");

    uStore->setStmtToVar(6, "count");
    uStore->setStmtToVar(6, "cenX");
    uStore->setStmtToVar(6, "cenY");
    uStore->setStmtToVar(6, "y");
    uStore->setStmtToVar(7, "count");
    uStore->setStmtToVar(7, "cenY");
    uStore->setStmtToVar(8, "cenX");
    uStore->setStmtToVar(9, "count");
    uStore->setStmtToVar(9, "x");

    uStore->setVarToStmt(4, WHILE_STMT, "x");
    uStore->setVarToStmt(4, WHILE_STMT, "y");
    uStore->setVarToStmt(5, WHILE_STMT, "x");
    uStore->setVarToStmt(6, WHILE_STMT, "count");
    uStore->setVarToStmt(6, WHILE_STMT, "cenX");
    uStore->setVarToStmt(6, WHILE_STMT, "y");
    uStore->setVarToStmt(6, WHILE_STMT, "cenY");
    uStore->setVarToStmt(7, ASSIGN, "cenY");
    uStore->setVarToStmt(7, ASSIGN, "count");
    uStore->setVarToStmt(8, ASSIGN, "cenX");
    uStore->setVarToStmt(9, ASSIGN, "x");
    uStore->setVarToStmt(9, ASSIGN, "count");

    mStore->setStmtToVar(1, "count");
    mStore->setStmtToVar(2, "cenX");
    mStore->setStmtToVar(3, "cenY");
    mStore->setStmtToVar(4, "x");
    mStore->setStmtToVar(4, "count");
    mStore->setStmtToVar(5, "x");
    mStore->setStmtToVar(5, "count");
    mStore->setStmtToVar(6, "x");
    mStore->setStmtToVar(6, "count");

    mStore->setStmtToVar(7, "x");
    mStore->setStmtToVar(8, "count");
    mStore->setStmtToVar(9, "count");

    mStore->setVarToStmt(1, ASSIGN, "count");
    mStore->setVarToStmt(2, ASSIGN, "cenX");
    mStore->setVarToStmt(3, ASSIGN, "cenY");

    mStore->setVarToStmt(4, WHILE_STMT, "x");
    mStore->setVarToStmt(4, WHILE_STMT, "count");
    mStore->setVarToStmt(5, WHILE_STMT, "x");
    mStore->setVarToStmt(5, WHILE_STMT, "count");
    mStore->setVarToStmt(6, WHILE_STMT, "x");
    mStore->setVarToStmt(6, WHILE_STMT, "count");

    mStore->setVarToStmt(7, ASSIGN, "x");
    mStore->setVarToStmt(8, ASSIGN, "count");
    mStore->setVarToStmt(9, ASSIGN, "count");

    pStore->setParentStar(4, 5);
    pStore->setParentStar(5, 6);
    pStore->setParentStar(6, 7);
    pStore->setParentStar(6, 8);
}

void TestAffectStorage::clearAll() {
    nStore->clear();
    mStore->clear();
    uStore->clear();
    nStore->clear();
    aStore->clear();
    gStore->clear();
    pStore->clear();
}

TEST_CASE_METHOD(TestAffectStorage, "Affect Storage: PopulateAffectsMap + isAffectsFromPopulation Test Prog1") {
    TestAffectStorage::setUpProg1();

    REQUIRE(aStore->isAffects(6, 5));
    REQUIRE(aStore->isAffects(7, 6));
    REQUIRE(aStore->isAffects(5, 7));

    REQUIRE(!aStore->isAffects(1, 8));

    REQUIRE(aStore->isAffects(9, 12));
    REQUIRE(aStore->isAffects(10, 20));
    REQUIRE(aStore->isAffects(12, 15));
    REQUIRE(aStore->isAffects(12, 19));

    REQUIRE(aStore->isAffects(15, 19));
    REQUIRE(aStore->isAffects(19, 12));

    REQUIRE_FALSE(aStore->isAffects(9, 11));
    REQUIRE_FALSE(aStore->isAffects(16, 17));
    REQUIRE_FALSE(aStore->isAffects(16, 12));

    REQUIRE(aStore->getStmtsThatAffects(6) == std::unordered_set<int>{7});
    REQUIRE(aStore->getStmtsThatAffects(12) == std::unordered_set<int>{9, 19});
    REQUIRE(aStore->getStmtsThatAffects(20) == std::unordered_set<int>{10});

    REQUIRE(aStore->getStmtsAffectedBy(6) == std::unordered_set<int>{5});
    REQUIRE(aStore->getStmtsAffectedBy(12) == std::unordered_set<int>{15, 19});
    REQUIRE(aStore->getStmtsAffectedBy(19) == std::unordered_set<int>{12});

    REQUIRE(aStore->getAffectsRelationPairing() ==
            IntPairResultSet{std::make_pair(5, 7), std::make_pair(6, 5), std::make_pair(7, 6), std::make_pair(9, 12),
                             std::make_pair(10, 20), std::make_pair(12, 15), std::make_pair(12, 19), std::make_pair(15, 19),
                             std::make_pair(19, 12), std::make_pair(16, 19)});

    REQUIRE(aStore->containsAffectRelation());

    REQUIRE(aStore->isAffectsStar(6, 5));
    REQUIRE(aStore->isAffectsStar(7, 5));
    REQUIRE(aStore->isAffectsStar(15, 12));
    REQUIRE(aStore->isAffectsStar(16, 12));
    REQUIRE_FALSE(aStore->isAffectsStar(11, 20));
    REQUIRE_FALSE(aStore->isAffectsStar(-1, 100));

    REQUIRE(aStore->getStmtsThatAffectsT(6) == std::unordered_set<int>{5, 6, 7});
    REQUIRE(aStore->getStmtsThatAffectsT(12) == std::unordered_set<int>{9, 12, 15, 16, 19});
    REQUIRE(aStore->getStmtsThatAffectsT(19) == std::unordered_set<int>{9, 12, 15, 16, 19});
    REQUIRE(aStore->getStmtsThatAffectsT(21).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(11).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(-1).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(100).empty());

    REQUIRE(aStore->getStmtsAffectedByT(5) == std::unordered_set<int>{5, 6, 7});
    REQUIRE(aStore->getStmtsAffectedByT(10) == std::unordered_set<int>{20});
    REQUIRE(aStore->getStmtsAffectedByT(19) == std::unordered_set<int>{12, 15, 19});
    REQUIRE(aStore->getStmtsAffectedByT(21).empty());
    REQUIRE(aStore->getStmtsAffectedByT(3).empty());
    REQUIRE(aStore->getStmtsAffectedByT(-1).empty());
    REQUIRE(aStore->getStmtsAffectedByT(100).empty());

    REQUIRE(aStore->getAffectsRelationPairingT() ==
            IntPairResultSet{std::make_pair(5, 6),   std::make_pair(5, 7),   std::make_pair(5, 5),   std::make_pair(6, 5),
                             std::make_pair(6, 6),   std::make_pair(6, 7),   std::make_pair(7, 5),   std::make_pair(7, 6),
                             std::make_pair(7, 7),   std::make_pair(9, 12),  std::make_pair(9, 15),  std::make_pair(9, 19),
                             std::make_pair(10, 20), std::make_pair(12, 12), std::make_pair(12, 15), std::make_pair(12, 19),
                             std::make_pair(15, 19), std::make_pair(15, 12), std::make_pair(15, 15), std::make_pair(19, 19),
                             std::make_pair(19, 12), std::make_pair(19, 15), std::make_pair(16, 19), std::make_pair(16, 12),
                             std::make_pair(16, 15)});

    TestAffectStorage::clearAll();
}

TEST_CASE_METHOD(TestAffectStorage,
                 "Affect Storage: PopulateAffectsMap + isAffectsFromPopulation Test Prog2 (Nested while loops)") {

    TestAffectStorage::setUpProg2();

    REQUIRE(aStore->isAffects(1, 4));
    REQUIRE(aStore->isAffects(1, 16));
    REQUIRE(aStore->isAffects(1, 14));

    REQUIRE(aStore->isAffects(2, 6));
    REQUIRE(aStore->isAffects(2, 14));

    REQUIRE(aStore->isAffects(4, 8));
    REQUIRE(aStore->isAffects(4, 14));
    REQUIRE(aStore->isAffects(4, 16));
    REQUIRE(aStore->isAffects(4, 4));

    REQUIRE(aStore->isAffects(6, 14));
    REQUIRE(aStore->isAffects(6, 6));

    REQUIRE(aStore->isAffects(8, 12));
    REQUIRE(aStore->isAffects(8, 16));
    REQUIRE(aStore->isAffects(8, 14));
    REQUIRE(aStore->isAffects(8, 4));
    REQUIRE(aStore->isAffects(8, 8));

    REQUIRE(aStore->isAffects(9, 4));

    REQUIRE(aStore->isAffects(12, 14));
    REQUIRE(aStore->isAffects(12, 16));
    REQUIRE(aStore->isAffects(12, 4));
    REQUIRE(aStore->isAffects(12, 8));
    REQUIRE(aStore->isAffects(12, 12));

    REQUIRE(aStore->isAffects(13, 14));

    REQUIRE(aStore->isAffects(14, 15));
    REQUIRE(aStore->isAffects(14, 16));

    REQUIRE(aStore->isAffects(15, 16));

    REQUIRE_FALSE(aStore->isAffects(9, 16));

    REQUIRE(aStore->getStmtsThatAffects(16) == std::unordered_set<int>{1, 4, 8, 12, 14, 15});
    REQUIRE(aStore->getStmtsThatAffects(14) == std::unordered_set<int>{4, 6, 8, 12, 13, 2, 1});
    REQUIRE(aStore->getStmtsThatAffects(15) == std::unordered_set<int>{14});
    REQUIRE(aStore->getStmtsThatAffects(6) == std::unordered_set<int>{2, 6});
    REQUIRE(aStore->getStmtsThatAffects(4) == std::unordered_set<int>{1, 9, 12, 4, 8});

    REQUIRE(aStore->getStmtsAffectedBy(8) == std::unordered_set<int>{12, 16, 14, 4, 8});
    REQUIRE(aStore->getStmtsAffectedBy(12) == std::unordered_set<int>{14, 16, 4, 8, 12});
    REQUIRE(aStore->getStmtsAffectedBy(14) == std::unordered_set<int>{15, 16});
    REQUIRE(aStore->getStmtsAffectedBy(6) == std::unordered_set<int>{14, 9, 6});
    REQUIRE(aStore->getStmtsAffectedBy(4) == std::unordered_set<int>{8, 14, 16, 4});

    REQUIRE(aStore->containsAffectRelation());

    REQUIRE(aStore->getAffectsRelationPairing() ==
            IntPairResultSet{std::make_pair(1, 4),   std::make_pair(1, 14),  std::make_pair(1, 16),  std::make_pair(2, 6),
                             std::make_pair(2, 14),  std::make_pair(4, 4),   std::make_pair(4, 8),   std::make_pair(4, 14),
                             std::make_pair(4, 16),  std::make_pair(6, 6),   std::make_pair(6, 9),   std::make_pair(6, 14),
                             std::make_pair(8, 12),  std::make_pair(8, 4),   std::make_pair(8, 8),   std::make_pair(8, 14),
                             std::make_pair(8, 16),  std::make_pair(9, 4),   std::make_pair(12, 4),  std::make_pair(12, 8),
                             std::make_pair(12, 12), std::make_pair(12, 14), std::make_pair(12, 16), std::make_pair(13, 14),
                             std::make_pair(14, 15), std::make_pair(14, 16), std::make_pair(15, 16)});

    REQUIRE(aStore->isAffectsStar(2, 9));
    REQUIRE(aStore->isAffectsStar(2, 4));
    REQUIRE(aStore->isAffectsStar(14, 16));
    REQUIRE(aStore->isAffectsStar(9, 8));
    REQUIRE_FALSE(aStore->isAffectsStar(1, 3));
    REQUIRE_FALSE(aStore->isAffectsStar(-1, 100));

    REQUIRE(aStore->getStmtsThatAffectsT(14) == std::unordered_set<int>{1, 2, 4, 6, 8, 9, 12, 13});
    REQUIRE(aStore->getStmtsThatAffectsT(4) == std::unordered_set<int>{1, 2, 4, 6, 8, 9, 12});
    REQUIRE(aStore->getStmtsThatAffectsT(12) == std::unordered_set<int>{1, 2, 4, 6, 8, 9, 12});
    REQUIRE(aStore->getStmtsThatAffectsT(10).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(17).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(-1).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(100).empty());

    REQUIRE(aStore->getStmtsAffectedByT(1) == std::unordered_set<int>{4, 8, 12, 14, 15, 16});
    REQUIRE(aStore->getStmtsAffectedByT(2) == std::unordered_set<int>{6, 9, 4, 8, 12, 14, 15, 16});
    REQUIRE(aStore->getStmtsAffectedByT(13) == std::unordered_set<int>{14, 15, 16});
    REQUIRE(aStore->getStmtsAffectedByT(3).empty());
    REQUIRE(aStore->getStmtsAffectedByT(17).empty());
    REQUIRE(aStore->getStmtsAffectedByT(-1).empty());
    REQUIRE(aStore->getStmtsAffectedByT(100).empty());

    REQUIRE(aStore->getAffectsRelationPairingT() ==
            IntPairResultSet{std::make_pair(1, 4),   std::make_pair(1, 8),   std::make_pair(1, 12),  std::make_pair(1, 14),
                             std::make_pair(1, 15),  std::make_pair(1, 16),  std::make_pair(2, 6),   std::make_pair(2, 9),
                             std::make_pair(2, 4),   std::make_pair(2, 8),   std::make_pair(2, 12),  std::make_pair(2, 14),
                             std::make_pair(2, 15),  std::make_pair(2, 16),  std::make_pair(4, 4),   std::make_pair(4, 8),
                             std::make_pair(4, 12),  std::make_pair(4, 14),  std::make_pair(4, 15),  std::make_pair(4, 16),
                             std::make_pair(6, 6),   std::make_pair(6, 8),   std::make_pair(6, 9),   std::make_pair(6, 4),
                             std::make_pair(6, 12),  std::make_pair(6, 14),  std::make_pair(6, 15),  std::make_pair(6, 16),
                             std::make_pair(8, 12),  std::make_pair(8, 4),   std::make_pair(8, 8),   std::make_pair(8, 14),
                             std::make_pair(8, 15),  std::make_pair(8, 16),  std::make_pair(9, 4),   std::make_pair(9, 8),
                             std::make_pair(9, 12),  std::make_pair(9, 14),  std::make_pair(9, 15),  std::make_pair(9, 16),
                             std::make_pair(12, 4),  std::make_pair(12, 8),  std::make_pair(12, 12), std::make_pair(12, 14),
                             std::make_pair(12, 15), std::make_pair(12, 16), std::make_pair(13, 14), std::make_pair(13, 15),
                             std::make_pair(13, 16), std::make_pair(14, 15), std::make_pair(14, 16), std::make_pair(15, 16)});

    TestAffectStorage::clearAll();
}

TEST_CASE_METHOD(TestAffectStorage,
                 "Affect Storage: PopulateAffectsMap + isAffectsFromPopulation Test Prog3 (While If While nested loops)") {

    TestAffectStorage::setUpProg3();

    REQUIRE(aStore->isAffects(1, 5));
    REQUIRE(aStore->isAffects(8, 9));
    REQUIRE(aStore->isAffects(9, 14));
    REQUIRE(aStore->isAffects(2, 17));
    REQUIRE(aStore->isAffects(1, 17));
    REQUIRE(aStore->isAffects(3, 18));
    REQUIRE(aStore->isAffects(1, 18));
    REQUIRE(aStore->isAffects(13, 17));
    REQUIRE(aStore->isAffects(5, 17));
    REQUIRE(aStore->isAffects(5, 18));
    REQUIRE(aStore->isAffects(5, 5));

    REQUIRE(aStore->isAffects(14, 17));
    REQUIRE(aStore->isAffects(14, 18));
    REQUIRE(aStore->isAffects(14, 5));

    REQUIRE(aStore->isAffects(17, 19));
    REQUIRE(aStore->isAffects(18, 19));
    REQUIRE(aStore->isAffects(13, 19));
    REQUIRE(aStore->isAffects(3, 19));
    REQUIRE(aStore->isAffects(2, 19));

    REQUIRE_FALSE(aStore->isAffects(5, 14));
    REQUIRE_FALSE(aStore->isAffects(13, 13));

    REQUIRE(aStore->getStmtsThatAffects(14) == std::unordered_set<int>{9});
    REQUIRE(aStore->getStmtsThatAffects(19) == std::unordered_set<int>{2, 3, 17, 18, 13});
    REQUIRE(aStore->getStmtsThatAffects(17) == std::unordered_set<int>{2, 1, 5, 14, 13});
    REQUIRE(aStore->getStmtsThatAffects(18) == std::unordered_set<int>{3, 1, 14, 5});

    REQUIRE(aStore->getStmtsAffectedBy(1) == std::unordered_set<int>{5, 17, 18});
    REQUIRE(aStore->getStmtsAffectedBy(5) == std::unordered_set<int>{17, 18, 5});
    REQUIRE(aStore->getStmtsAffectedBy(14) == std::unordered_set<int>{17, 18, 5});
    REQUIRE(aStore->getStmtsAffectedBy(17) == std::unordered_set<int>{19});

    // REQUIRE(aStore->containsAffectRelation());
    REQUIRE(aStore->getAffectsRelationPairing() ==
            IntPairResultSet{std::make_pair(1, 5), std::make_pair(1, 17), std::make_pair(1, 18), std::make_pair(2, 17),
                             std::make_pair(2, 19), std::make_pair(3, 18), std::make_pair(3, 19), std::make_pair(5, 5),
                             std::make_pair(5, 17), std::make_pair(5, 18), std::make_pair(8, 9), std::make_pair(9, 14),
                             std::make_pair(14, 5), std::make_pair(14, 17), std::make_pair(14, 18), std::make_pair(13, 17),
                             std::make_pair(13, 19), std::make_pair(17, 19), std::make_pair(18, 19)});

    REQUIRE(aStore->isAffectsStar(5, 19));
    REQUIRE(aStore->isAffectsStar(8, 5));
    REQUIRE(aStore->isAffectsStar(14, 19));
    REQUIRE_FALSE(aStore->isAffectsStar(1, 13));
    REQUIRE_FALSE(aStore->isAffectsStar(4, 15));
    REQUIRE_FALSE(aStore->isAffectsStar(-1, 100));

    REQUIRE(aStore->getStmtsThatAffectsT(19) == std::unordered_set<int>{1, 2, 3, 5, 8, 9, 13, 14, 17, 18});
    REQUIRE(aStore->getStmtsThatAffectsT(14) == std::unordered_set<int>{8, 9});
    REQUIRE(aStore->getStmtsThatAffectsT(13).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(15).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(-1).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(100).empty());

    REQUIRE(aStore->getStmtsAffectedByT(8) == std::unordered_set<int>{9, 14, 5, 17, 18, 19});
    REQUIRE(aStore->getStmtsAffectedByT(1) == std::unordered_set<int>{5, 17, 18, 19});
    REQUIRE(aStore->getStmtsAffectedByT(13) == std::unordered_set<int>{17, 19});
    REQUIRE(aStore->getStmtsAffectedByT(16).empty());
    REQUIRE(aStore->getStmtsAffectedByT(11).empty());
    REQUIRE(aStore->getStmtsAffectedByT(-1).empty());
    REQUIRE(aStore->getStmtsAffectedByT(100).empty());

    REQUIRE(aStore->getAffectsRelationPairingT() ==
            IntPairResultSet{std::make_pair(1, 5),   std::make_pair(1, 17),  std::make_pair(1, 18),  std::make_pair(1, 19),
                             std::make_pair(2, 17),  std::make_pair(2, 19),  std::make_pair(3, 18),  std::make_pair(3, 19),
                             std::make_pair(5, 5),   std::make_pair(5, 17),  std::make_pair(5, 18),  std::make_pair(5, 19),
                             std::make_pair(8, 9),   std::make_pair(8, 14),  std::make_pair(8, 5),   std::make_pair(8, 17),
                             std::make_pair(8, 18),  std::make_pair(8, 19),  std::make_pair(9, 14),  std::make_pair(9, 5),
                             std::make_pair(9, 17),  std::make_pair(9, 18),  std::make_pair(9, 19),  std::make_pair(13, 17),
                             std::make_pair(13, 19), std::make_pair(14, 5),  std::make_pair(14, 17), std::make_pair(14, 18),
                             std::make_pair(14, 19), std::make_pair(17, 19), std::make_pair(18, 19)});

    TestAffectStorage::clearAll();
}

TEST_CASE_METHOD(TestAffectStorage,
                 "Affect Storage: PopulateAffectsMap + isAffectsFromPopulation Test Prog4 (Nested consecutive while loops)") {

    TestAffectStorage::setUpProg4();

    REQUIRE(aStore->isAffects(2, 8));
    REQUIRE(aStore->isAffects(1, 7));
    REQUIRE(aStore->isAffects(3, 7));
    REQUIRE(aStore->isAffects(8, 9));
    REQUIRE(aStore->isAffects(8, 7));
    REQUIRE(aStore->isAffects(1, 9));

    REQUIRE(aStore->getStmtsThatAffects(7) == std::unordered_set<int>{3, 1, 8});
    REQUIRE(aStore->getStmtsThatAffects(8) == std::unordered_set<int>{2});

    REQUIRE(aStore->getStmtsAffectedBy(1) == std::unordered_set<int>{7, 9});
    REQUIRE(aStore->getStmtsAffectedBy(8) == std::unordered_set<int>{7, 9});

    REQUIRE(aStore->containsAffectRelation());

    REQUIRE(aStore->getAffectsRelationPairing() ==
            IntPairResultSet{std::make_pair(1, 7), std::make_pair(1, 9), std::make_pair(2, 8), std::make_pair(3, 7),
                             std::make_pair(8, 7), std::make_pair(7, 9), std::make_pair(8, 9)});

    REQUIRE(aStore->isAffectsStar(2, 9));
    REQUIRE(aStore->isAffectsStar(2, 7));
    REQUIRE_FALSE(aStore->isAffectsStar(1, 8));
    REQUIRE_FALSE(aStore->isAffectsStar(4, 5));
    REQUIRE_FALSE(aStore->isAffectsStar(-1, 100));

    REQUIRE(aStore->getStmtsThatAffectsT(9) == std::unordered_set<int>{1, 2, 3, 7, 8});
    REQUIRE(aStore->getStmtsThatAffectsT(7) == std::unordered_set<int>{1, 2, 3, 8});
    REQUIRE(aStore->getStmtsThatAffectsT(4).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(-1).empty());
    REQUIRE(aStore->getStmtsThatAffectsT(100).empty());

    REQUIRE(aStore->getStmtsAffectedByT(1) == std::unordered_set<int>{7, 9});
    REQUIRE(aStore->getStmtsAffectedByT(8) == std::unordered_set<int>{7, 9});
    REQUIRE(aStore->getStmtsAffectedByT(9).empty());
    REQUIRE(aStore->getStmtsAffectedByT(6).empty());
    REQUIRE(aStore->getStmtsAffectedByT(-1).empty());
    REQUIRE(aStore->getStmtsAffectedByT(100).empty());

    REQUIRE(aStore->getAffectsRelationPairingT() ==
            IntPairResultSet{std::make_pair(1, 7), std::make_pair(1, 9), std::make_pair(2, 8), std::make_pair(2, 7),
                             std::make_pair(2, 9), std::make_pair(3, 7), std::make_pair(3, 9), std::make_pair(7, 9),
                             std::make_pair(8, 7), std::make_pair(8, 9)});

    TestAffectStorage::clearAll();
}
