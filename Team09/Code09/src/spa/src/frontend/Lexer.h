#pragma once
#include <fstream>
#include <string>
#include <vector>

namespace Lexer {
enum class TokenType { NAME, INTEGER, SYMBOL, STOP };

struct Token {
    const TokenType type;
    const std::string value;
};

typedef std::vector<Token> TokenList;

// API
// tokenize tokenizes the SIMPLE program given from a file name into a TokenList
Lexer::TokenList tokenize(const std::string &filename);

}; // namespace Lexer

// private functions for Lexer::tokenize
namespace {
Lexer::Token getName(std::ifstream &stream);
Lexer::Token getInteger(std::ifstream &stream);
Lexer::Token getPunctuation(std::ifstream &stream);
void getSpaces(std::ifstream &stream);
}; // namespace