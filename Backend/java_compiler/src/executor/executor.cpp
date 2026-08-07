#include "executor.h"
#include <map>
#include <sstream>
#include <cmath>

vector<string> executionOutput;
static map<string, double> numMemory;
static map<string, string> strMemory;

static string formatNum(double d) {
    if (d == (long long)d) {
        return to_string((long long)d);
    }
    ostringstream oss;
    oss << d;
    return oss.str();
}

struct EvalResult {
    bool isString;
    double numVal;
    string strVal;
};

static EvalResult evalAST(ASTNode* node) {
    if (!node) return {false, 0, ""};

    if (node->type == "Integer" || node->type == "Float") {
        return {false, atof(node->value.c_str()), ""};
    }

    if (node->type == "Boolean") {
        return {false, (node->value == "true" ? 1.0 : 0.0), ""};
    }

    if (node->type == "String") {
        string s = node->value;
        if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
            s = s.substr(1, s.length() - 2);
        }
        return {true, 0, s};
    }

    if (node->type == "Char") {
        string s = node->value;
        if (s.length() >= 2 && s.front() == '\'' && s.back() == '\'') {
            s = s.substr(1, s.length() - 2);
        }
        return {true, 0, s};
    }

    if (node->type == "Identifier") {
        if (strMemory.count(node->value)) {
            return {true, 0, strMemory[node->value]};
        }
        if (numMemory.count(node->value)) {
            return {false, numMemory[node->value], ""};
        }
        return {false, 0, ""};
    }

    if (node->type == "+") {
        EvalResult left = evalAST(node->children[0]);
        EvalResult right = evalAST(node->children[1]);
        if (left.isString || right.isString) {
            string lStr = left.isString ? left.strVal : formatNum(left.numVal);
            string rStr = right.isString ? right.strVal : formatNum(right.numVal);
            return {true, 0, lStr + rStr};
        }
        return {false, left.numVal + right.numVal, ""};
    }

    if (node->type == "-") return {false, evalAST(node->children[0]).numVal - evalAST(node->children[1]).numVal, ""};
    if (node->type == "*") return {false, evalAST(node->children[0]).numVal * evalAST(node->children[1]).numVal, ""};
    if (node->type == "/") {
        double denom = evalAST(node->children[1]).numVal;
        return {false, denom != 0 ? evalAST(node->children[0]).numVal / denom : 0, ""};
    }
    if (node->type == "%") {
        long long denom = (long long)evalAST(node->children[1]).numVal;
        return {false, denom != 0 ? (double)((long long)evalAST(node->children[0]).numVal % denom) : 0, ""};
    }

    if (node->type == "UnaryMinus") return {false, -evalAST(node->children[0]).numVal, ""};

    if (node->type == "<") return {false, (double)(evalAST(node->children[0]).numVal < evalAST(node->children[1]).numVal), ""};
    if (node->type == ">") return {false, (double)(evalAST(node->children[0]).numVal > evalAST(node->children[1]).numVal), ""};
    if (node->type == "<=") return {false, (double)(evalAST(node->children[0]).numVal <= evalAST(node->children[1]).numVal), ""};
    if (node->type == ">=") return {false, (double)(evalAST(node->children[0]).numVal >= evalAST(node->children[1]).numVal), ""};
    if (node->type == "==") {
        EvalResult l = evalAST(node->children[0]);
        EvalResult r = evalAST(node->children[1]);
        if (l.isString || r.isString) return {false, (double)(l.strVal == r.strVal), ""};
        return {false, (double)(l.numVal == r.numVal), ""};
    }
    if (node->type == "!=") {
        EvalResult l = evalAST(node->children[0]);
        EvalResult r = evalAST(node->children[1]);
        if (l.isString || r.isString) return {false, (double)(l.strVal != r.strVal), ""};
        return {false, (double)(l.numVal != r.numVal), ""};
    }
    if (node->type == "&&") return {false, (double)(evalAST(node->children[0]).numVal && evalAST(node->children[1]).numVal), ""};
    if (node->type == "||") return {false, (double)(evalAST(node->children[0]).numVal || evalAST(node->children[1]).numVal), ""};
    if (node->type == "!") return {false, (double)(!evalAST(node->children[0]).numVal), ""};

    if (node->type == "PostIncrement") {
        string name = node->children[0]->value;
        double cur = numMemory[name];
        numMemory[name] = cur + 1;
        return {false, cur, ""};
    }
    if (node->type == "PreIncrement") {
        string name = node->children[0]->value;
        numMemory[name] += 1;
        return {false, numMemory[name], ""};
    }
    if (node->type == "PostDecrement") {
        string name = node->children[0]->value;
        double cur = numMemory[name];
        numMemory[name] = cur - 1;
        return {false, cur, ""};
    }
    if (node->type == "PreDecrement") {
        string name = node->children[0]->value;
        numMemory[name] -= 1;
        return {false, numMemory[name], ""};
    }

    return {false, 0, ""};
}

static void runStatement(ASTNode* node) {
    if (!node) return;

    if (node->type == "VariableDeclaration") {
        string type = node->children[0]->value;
        string name = node->children[1]->value;
        if (node->children.size() == 3) {
            EvalResult res = evalAST(node->children[2]);
            if (res.isString) strMemory[name] = res.strVal;
            else numMemory[name] = res.numVal;
        } else {
            if (type == "String" || type == "char") strMemory[name] = "";
            else numMemory[name] = 0;
        }
        return;
    }

    if (node->type == "Assignment") {
        string name = node->children[0]->value;
        EvalResult res = evalAST(node->children[1]);
        if (res.isString) strMemory[name] = res.strVal;
        else numMemory[name] = res.numVal;
        return;
    }

    if (node->type == "Print") {
        if (!node->children.empty()) {
            EvalResult res = evalAST(node->children[0]);
            if (res.isString) {
                executionOutput.push_back(res.strVal);
            } else {
                executionOutput.push_back(formatNum(res.numVal));
            }
        } else {
            executionOutput.push_back("");
        }
        return;
    }

    if (node->type == "If") {
        if (evalAST(node->children[0]).numVal != 0) {
            runStatement(node->children[1]);
        }
        return;
    }

    if (node->type == "IfElse") {
        if (evalAST(node->children[0]).numVal != 0) {
            runStatement(node->children[1]);
        } else {
            runStatement(node->children[2]);
        }
        return;
    }

    if (node->type == "While") {
        int maxIter = 10000;
        while (evalAST(node->children[0]).numVal != 0 && maxIter-- > 0) {
            runStatement(node->children[1]);
        }
        return;
    }

    if (node->type == "DoWhile") {
        int maxIter = 10000;
        do {
            runStatement(node->children[0]);
        } while (evalAST(node->children[1]).numVal != 0 && maxIter-- > 0);
        return;
    }

    if (node->type == "For") {
        runStatement(node->children[0]); // Init
        int maxIter = 10000;
        while (evalAST(node->children[1]).numVal != 0 && maxIter-- > 0) {
            runStatement(node->children[3]); // Body
            evalAST(node->children[2]);       // Update
        }
        return;
    }

    for (auto child : node->children) {
        runStatement(child);
    }
}

void executeAST(ASTNode* root) {
    executionOutput.clear();
    numMemory.clear();
    strMemory.clear();
    if (root) {
        runStatement(root);
    }
}

void printExecutionOutput() {
    cout << "\n...............\n";
    cout << "          PHASE 6: PROGRAM EXECUTION & OUTPUT          \n";
    cout << "...............\n";
    if (executionOutput.empty()) {
        cout << "[Program executed with no console output]\n";
    } else {
        for (const auto& line : executionOutput) {
            cout << line << endl;
        }
    }
    cout << "...............\n";
}
