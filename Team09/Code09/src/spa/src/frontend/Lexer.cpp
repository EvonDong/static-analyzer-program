#include <cctype>

#include "Lexer.h"

namespace Lexer {
Lexer::TokenList tokenize(const std::string &filename) {
    std::ifstream stream(filename);

    Lexer::TokenList tokens;

    while (stream.peek() != EOF) {
        if (std::isalpha(stream.peek())) {
            tokens.push_back(getName(stream));
        } else if (std::isdigit(stream.peek())) {
            tokens.push_back(getInteger(stream));
        } else if (std::ispunct(stream.peek())) {
            tokens.push_back(getPunctuation(stream));
        } else if (std::isspace(stream.peek())) {
            getSpaces(stream);
        } else {
            // invalid character
        }
    }
    stream.close();

    // signify end of program
    std::string value = "";
    Lexer::Token token{Lexer::TokenType::STOP, value};
    tokens.push_back(token);
    return tokens;
}
} // namespace Lexer

namespace {
Lexer::Token getName(std::ifstream &stream) {
    std::string value;

    while (std::isalnum(stream.peek())) {
        value += char(stream.get());
    }

    Lexer::Token token{Lexer::TokenType::NAME, value};
    return token;
}

Lexer::Token getInteger(std::ifstream &stream) {
    std::string value;

    while (std::isdigit(stream.peek())) {
        value += char(stream.get());
    }

    Lexer::Token token{Lexer::TokenType::INTEGER, value};
    return token;
}

Lexer::Token getPunctuation(std::ifstream &stream) {
    std::string value;
    switch (stream.peek()) {
    case '=':
    case '!':
    case '<':
    case '>':
        value += char(stream.get());
        if (stream.peek() == '=') {
            value += char(stream.get());
        }
        break;

    case '&':
    case '|':
        value += char(stream.get());
        if (stream.peek() == value[0]) {
            value += char(stream.get());
        }
        break;
    default:
        value += char(stream.get());
    }

    Lexer::Token token{Lexer::TokenType::SYMBOL, value};
    return token;
}

// get spaces includes \n, \t, \r, \v, \f, whitespace
void getSpaces(std::ifstream &stream) {
    while (std::isspace(stream.peek())) {
        stream.get();
    }
}
} // namespace