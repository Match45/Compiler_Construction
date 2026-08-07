#include "optimizer.h"
#include <iomanip>
#include <map>
#include <cctype>

vector<TACInstruction> optimizedTac;

static bool isNumeric(const string& str, double& val) {
    if (str.empty()) return false;
    char* p;
    val = strtod(str.c_str(), &p);
    return *p == 0;
}

void optimizeTAC(const vector<TACInstruction>& original) {
    optimizedTac.clear();
    map<string, string> constMap;

    for (auto tac : original) {
        if (constMap.count(tac.arg1)) tac.arg1 = constMap[tac.arg1];
        if (constMap.count(tac.arg2)) tac.arg2 = constMap[tac.arg2];

        double num1, num2;
        bool isNum1 = isNumeric(tac.arg1, num1);
        bool isNum2 = isNumeric(tac.arg2, num2);

        if (isNum1 && isNum2) {
            double res = 0;
            bool folded = false;
            if (tac.op == "+") { res = num1 + num2; folded = true; }
            else if (tac.op == "-") { res = num1 - num2; folded = true; }
            else if (tac.op == "*") { res = num1 * num2; folded = true; }
            else if (tac.op == "/" && num2 != 0) { res = num1 / num2; folded = true; }
            else if (tac.op == "%" && (int)num2 != 0) { res = (int)num1 % (int)num2; folded = true; }
            else if (tac.op == "<") { res = num1 < num2; folded = true; }
            else if (tac.op == ">") { res = num1 > num2; folded = true; }
            else if (tac.op == "<=") { res = num1 <= num2; folded = true; }
            else if (tac.op == ">=") { res = num1 >= num2; folded = true; }
            else if (tac.op == "==") { res = num1 == num2; folded = true; }
            else if (tac.op == "!=") { res = num1 != num2; folded = true; }

            if (folded) {
                string valStr = (res == (int)res) ? to_string((int)res) : to_string(res);
                optimizedTac.push_back({"=", valStr, "", tac.result, "Folded Constant"});
                constMap[tac.result] = valStr;
                continue;
            }
        }

        if (tac.op == "=" && isNum1 && tac.result.find("t") == 0) {
            constMap[tac.result] = tac.arg1;
        }

        optimizedTac.push_back(tac);
    }
}

void printOptimizedTAC() {
    cout << "\n...............\n";
    cout << "             PHASE 5: CODE OPTIMIZATION                \n";
    cout << "...............\n";
    int index = 1;
    for (const auto& tac : optimizedTac) {
        cout << left << setw(4) << to_string(index) + ": ";
        if (tac.op == "LABEL") {
            cout << tac.result << ":" << endl;
        } else if (tac.op == "GOTO") {
            cout << "goto " << tac.result << endl;
        } else if (tac.op == "IFFALSE") {
            cout << "ifFalse " << tac.arg1 << " goto " << tac.result << endl;
        } else if (tac.op == "IFTRUE") {
            cout << "if " << tac.arg1 << " goto " << tac.result << endl;
        } else if (tac.op == "=") {
            cout << tac.result << " = " << tac.arg1 << (tac.comment.empty() ? "" : "  // " + tac.comment) << endl;
        } else if (tac.op == "PRINT") {
            cout << "PRINT " << tac.arg1 << endl;
        } else if (tac.op == "DECL") {
            cout << tac.arg1 << " " << tac.result << endl;
        } else {
            cout << tac.result << " = " << tac.arg1 << " " << tac.op << " " << tac.arg2 << endl;
        }
        index++;
    }
    cout << "...............\n";
}
