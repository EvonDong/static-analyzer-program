#include "FrontEnd.h"

void FrontEnd::start(std::string &filename) {
    Lexer::TokenList tokens = Lexer::tokenize(filename);
    SimplifiedAST ast;
    CallGraph callgraph;

    try {
        ast = Parser::parse(tokens);
    } catch (std::exception &e) {
        std::cerr << "Parser Exception - " << e.what() << "\n";
        exit(1);
    }

    try {
        callgraph = Parser::getCallGraph();
    } catch (std::exception &e) {
        std::cerr << "Calls Exception - " << e.what() << "\n";
        exit(1);
    }

    DesignExtractor designExtractor{ast, callgraph};

    try {
        designExtractor.extract();
    } catch (std::exception &e) {
        std::cerr << "Design Extractor - A number larger than integer given: " << e.what() << "\n";
        exit(1);
    }
}
