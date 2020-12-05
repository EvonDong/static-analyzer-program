#pragma once

#include <iostream>
#include <string>

#include "CallGraph.h"
#include "DesignExtractor.h"
#include "Lexer.h"
#include "Parser.h"
#include "SimplifiedAST.h"

class FrontEnd {
  private:
  public:
    void start(std::string &filename);
};
