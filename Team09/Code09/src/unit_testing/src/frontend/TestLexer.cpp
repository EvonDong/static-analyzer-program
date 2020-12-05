#include "Lexer.h"
// need include for anonymous namespace
#include "Lexer.cpp"
#include "catch.hpp"

TEST_CASE("Lexer: Test tokenize private helper methods in anonymous namespace") {

    SECTION("Lexer 1.1: Test getName: getting a name token") {
        // stream with first token as name, make sure only get first token
        std::string filename = "./LexerTestFiles/Test-1_1.txt";
        std::ifstream stream(filename);

        Lexer::Token output = getName(stream);
        stream.close();
        REQUIRE(output.value == "name1");
    }

    SECTION("Lexer 1.2: Test getInteger: getting a integer token") {
        std::string filename = "./LexerTestFiles/Test-1_2.txt";
        std::ifstream stream(filename);

        Lexer::Token output = getInteger(stream);
        stream.close();
        REQUIRE(output.value == "123");
    }

    SECTION("Lexer 1.3: Test getPunctuation: getting a punctuation token") {
        std::string filename = "./LexerTestFiles/Test-1_3.txt";
        std::ifstream stream(filename);

        Lexer::Token output = getPunctuation(stream);
        stream.close();
        REQUIRE(output.value == "&&");
    }

    SECTION("Lexer 1.4: Test getSpaces: removing spaces") {
        std::string filename = "./LexerTestFiles/Test-1_4.txt";
        std::ifstream stream(filename);

        getSpaces(stream);
        REQUIRE(char(stream.peek()) == 'R');
        stream.close();
    }
}

std::vector<std::string> constructTestList() {
    std::vector<std::string> strList{"procedure", "Example", "{",  "x",    "=", "2", ";",    "z",    "=", "3", ";", "while",
                                     "(",         "i",       "!=", "0",    ")", "{", "x",    "=",    "x", "-", "1", ";",
                                     "if",        "(",       "x",  "==",   "1", ")", "then", "{",    "z", "=", "x", "+",
                                     "1",         ";",       "}",  "else", "{", "y", "=",    "z",    "+", "x", ";", "}",
                                     "i",         "=",       "i",  "-",    "1", ";", "}",    "call", "p", ";", "}", ""};

    return strList;
}

void outputChecker(Lexer::TokenList &outputList) {

    std::vector<std::string> testList{constructTestList()};

    REQUIRE(outputList.size() == testList.size());
    bool isSame = true;

    for (int i = 0; i < outputList.size(); i++) {
        if (outputList[i].value != testList[i]) {
            isSame = false;
        }
    }

    REQUIRE(isSame);
}

// Overloaded to compare the outputList given a checkList.
void outputChecker(Lexer::TokenList &outputList, std::vector<std::string> &checkList) {
    REQUIRE(outputList.size() == checkList.size());

    bool isSame = true;

    for (int i = 0; i < outputList.size(); i++) {
        if (outputList[i].value.compare(checkList[i])) {
            isSame = false;
        }
    }

    REQUIRE(isSame);
}

TEST_CASE("Lexer: Test tokenize") {

    SECTION("Lexer 2.1: Given a file with only integers") {
        // Only positive integers since negative integers include the negative symbol.
        std::vector<std::string> checkList{"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "100", "1000", ""};

        std::string filename = "./LexerTestFiles/Test-2_1.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);

        outputChecker(output, checkList);
    }

    SECTION("Lexer 2.2: Given a file with only names") {
        std::vector<std::string> checkList{"procedure", "name",   "if", "a",     "a2",     "a2345",
                                           "a1234bc",   "abc234", "if", "while", "cs3203", ""};

        std::string filename = "./LexerTestFiles/Test-2_2.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);

        outputChecker(output, checkList);
    }

    SECTION("Lexer 2.3: Given a file with only punctuations") {
        // Includes punctuations not used in SIMPLE because tokenizer does not do validation.
        std::vector<std::string> checkList{
            "!", "<", "=", ">=", "==", "!=", "||", "&&", "==", "!=", "~", "$", "$", "!", "@", "#", ""};

        std::string filename = "./LexerTestFiles/Test-2_3.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);

        outputChecker(output, checkList);
    }

    SECTION("Lexer 2.4: Given a file with only spaces/tabs and its equivalent") {
        std::string filename = "./LexerTestFiles/Test-2_4.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);

        REQUIRE(output.size() == 1);
    }

    SECTION("Lexer 2.5: Given an empty file") {
        std::string filename = "./LexerTestFiles/Test-2_5.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);

        REQUIRE(output.size() == 1);
    }

    // === SECTION 2.6 ~ 2.10 should have the same token list as they are variants of the same SIMPLE program
    SECTION("Lexer 2.6: Given a proper/perfect SIMPLE file with line breaks and correct indentation/spaces between tokens") {
        std::string filename = "./LexerTestFiles/Test-2_6.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);
        outputChecker(output);
    }

    SECTION("Lexer 2.7: Given a SIMPLE file with some line breaks but correct spacing between tokens") {
        std::string filename = "./LexerTestFiles/Test-2_7.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);
        outputChecker(output);
    }

    SECTION("Lexer 2.8: Given a SIMPLE file with line breaks and correct indentation but extra spaces between tokens") {
        std::string filename = "./LexerTestFiles/Test-2_8.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);
        outputChecker(output);
    }

    SECTION("Lexer 2.9: Given a SIMPLE file with line breaks but wrong indentation and extra spaces between tokens") {
        std::string filename = "./LexerTestFiles/Test-2_9.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);
        outputChecker(output);
    }

    SECTION("Lexer 2.10: Given a SIMPLE file with line breaks and correct spacing but wrong indentation") {
        std::string filename = "./LexerTestFiles/Test-2_10.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);
        outputChecker(output);
    }

    SECTION("Lexer 2.11: Given a file with incorrect Integer type") {
        std::vector<std::string> checkList{"0", "a", "=", "1023", "b", ";", "x", "=", "10", "x", ";", ""};

        std::string filename = "./LexerTestFiles/Test-2_11.txt";
        Lexer::TokenList output = Lexer::tokenize(filename);

        outputChecker(output, checkList);
    }
}
