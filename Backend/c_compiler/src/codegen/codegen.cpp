#include "codegen.h"
#include <cstdio>

void CodeGen::emit(const std::string& line) {
    asmLines.push_back(line);
}

std::string CodeGen::mapOp(const std::string& op) {
    if (op == "+") return "ADD";
    if (op == "-") return "SUB";
    if (op == "*") return "MUL";
    if (op == "/") return "DIV";
    if (op == "%") return "MOD";
    if (op == "<") return "CMP_LT";
    if (op == ">") return "CMP_GT";
    if (op == "<=") return "CMP_LE";
    if (op == ">=") return "CMP_GE";
    if (op == "==") return "CMP_EQ";
    if (op == "!=") return "CMP_NE";
    if (op == "&&") return "AND";
    if (op == "||") return "OR";
    if (op == "!") return "NOT";
    if (op == "-u") return "NEG"; // unary minus, if labelled distinctly
    return "OP_" + op;
}

std::vector<std::string> CodeGen::generate(const std::vector<TAC>& code) {
    asmLines.clear();

    for (auto& t : code) {
        if (t.op == "func_begin") {
            emit(t.arg1 + ":");
            emit("  PUSH BP");
            emit("  MOV BP, SP");
        }
        else if (t.op == "func_end") {
            emit("  POP BP");
            emit("  RET");
            emit("");
        }
        else if (t.op == "label") {
            emit(t.result + ":");
        }
        else if (t.op == "goto") {
            emit("  JMP " + t.result);
        }
        else if (t.op == "ifFalse") {
            emit("  MOV R1, " + t.arg1);
            emit("  CMP R1, 0");
            emit("  JE " + t.result);
        }
        else if (t.op == "=") {
            emit("  MOV " + t.result + ", " + t.arg1);
        }
        else if (t.op == "param") {
            emit("  PUSH " + t.arg1);
        }
        else if (t.op == "call") {
            emit("  CALL " + t.arg1);
            emit("  MOV " + t.result + ", RET_REG");
        }
        else if (t.op == "return") {
            if (!t.arg1.empty()) emit("  MOV RET_REG, " + t.arg1);
        }
        else if (t.op == "=[]") {
            emit("  MOV R1, " + t.arg2);
            emit("  LOAD_ARR R2, " + t.arg1 + "[R1]");
            emit("  MOV " + t.result + ", R2");
        }
        else if (t.op == "break" || t.op == "continue") {
            emit("  ; " + t.op + " (unresolved - needs loop context)");
        }
        else if (t.arg2.empty()) {
            // unary op
            emit("  MOV R1, " + t.arg1);
            emit("  " + mapOp(t.op) + " R1");
            emit("  MOV " + t.result + ", R1");
        }
        else {
            // binary op
            emit("  MOV R1, " + t.arg1);
            emit("  MOV R2, " + t.arg2);
            emit("  " + mapOp(t.op) + " R1, R2");
            emit("  MOV " + t.result + ", R1");
        }
    }
    return asmLines;
}

void CodeGen::print() const {
    for (auto& line : asmLines) printf("%s\n", line.c_str());
}