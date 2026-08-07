#include "optimizer.h"
#include <cstdlib>
#include <set>
#include <cmath>

bool Optimizer::isNumber(const std::string& s) {
    if (s.empty()) return false;
    size_t i = 0;
    if (s[0] == '-') i = 1;
    if (i >= s.size()) return false;
    bool seenDigit = false;
    for (; i < s.size(); i++) {
        if (isdigit((unsigned char)s[i])) seenDigit = true;
        else if (s[i] != '.') return false;
    }
    return seenDigit;
}

// ---- Optimization 1: Constant Folding ----

std::vector<TAC> Optimizer::constantFolding(const std::vector<TAC>& code) {
    std::vector<TAC> out;
    for (auto& t : code) {
        bool isArith = (t.op == "+" || t.op == "-" || t.op == "*" ||
                         t.op == "/" || t.op == "%");
        if (isArith && isNumber(t.arg1) && isNumber(t.arg2)) {
            double a = atof(t.arg1.c_str());
            double b = atof(t.arg2.c_str());
            double r = 0;
            bool isFloat = (t.arg1.find('.') != std::string::npos ||
                             t.arg2.find('.') != std::string::npos);

            if (t.op == "+") r = a + b;
            else if (t.op == "-") r = a - b;
            else if (t.op == "*") r = a * b;
            else if (t.op == "/") r = (b != 0) ? a / b : 0;
            else if (t.op == "%") r = (b != 0) ? (int)a % (int)b : 0;

            TAC folded;
            folded.op = "=";
            folded.arg1 = isFloat ? std::to_string(r) : std::to_string((int)r);
            folded.arg2 = "";
            folded.result = t.result;
            out.push_back(folded);
        } else {
            out.push_back(t);
        }
    }
    return out;
}

// ---- Optimization 2: Remove Unused Labels ----

std::vector<TAC> Optimizer::removeUnusedLabels(const std::vector<TAC>& code) {
    std::set<std::string> usedLabels;
    for (auto& t : code) {
        if (t.op == "goto" || t.op == "ifFalse") usedLabels.insert(t.result);
    }
    std::vector<TAC> out;
    for (auto& t : code) {
        if (t.op == "label" && usedLabels.find(t.result) == usedLabels.end())
            continue; // drop unused label
        out.push_back(t);
    }
    return out;
}

// ---- Optimization 3: Dead Code Elimination ----

std::vector<TAC> Optimizer::removeDeadCodeAfterJump(const std::vector<TAC>& code) {
    std::vector<TAC> out;
    bool dead = false;
    for (auto& t : code) {
        if (t.op == "label" || t.op == "func_begin" || t.op == "func_end") {
            dead = false;
            out.push_back(t);
            continue;
        }
        if (dead) continue; // skip unreachable instruction
        out.push_back(t);
        if (t.op == "goto" || t.op == "return") dead = true;
    }
    return out;
}

std::vector<TAC> Optimizer::optimize(const std::vector<TAC>& code) {
    std::vector<TAC> step1 = constantFolding(code);
    std::vector<TAC> step2 = removeDeadCodeAfterJump(step1);
    std::vector<TAC> step3 = removeUnusedLabels(step2);
    return step3;
}