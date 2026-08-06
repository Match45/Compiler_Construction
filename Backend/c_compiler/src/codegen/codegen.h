#ifndef CODEGEN_H
#define CODEGEN_H
#include "../icg/icg.h"
#include <vector>
#include <string>

class CodeGen {
public:
    std::vector<std::string> generate(const std::vector<TAC>& code);
    void print() const;

private:
    std::vector<std::string> asmLines;
    std::string mapOp(const std::string& op);
    void emit(const std::string& line);
};

#endif