#include "icg.h"
#include <iomanip>

vector<TACInstruction> tacInstructions;
static int tempCount = 1;
static int labelCount = 1;

static string newTemp() {
    return "t" + to_string(tempCount++);
}

static string newLabel() {
    return "L" + to_string(labelCount++);
}

void resetICG() {
    tacInstructions.clear();
    tempCount = 1;
    labelCount = 1;
}

string generateTAC(ASTNode* node) {
    if (!node) return "";

    if (node->type == "Integer" || node->type == "Float" || node->type == "Boolean" || node->type == "Char" || node->type == "String") {
        return node->value;
    }

    if (node->type == "Identifier") {
        return node->value;
    }

    if (node->type == "+" || node->type == "-" || node->type == "*" || node->type == "/" || node->type == "%" ||
        node->type == "==" || node->type == "!=" || node->type == "<" || node->type == ">" || node->type == "<=" || node->type == ">=" ||
        node->type == "&&" || node->type == "||") {
        string left = generateTAC(node->children[0]);
        string right = generateTAC(node->children[1]);
        string temp = newTemp();
        tacInstructions.push_back({node->type, left, right, temp, ""});
        return temp;
    }

    if (node->type == "UnaryMinus") {
        string val = generateTAC(node->children[0]);
        string temp = newTemp();
        tacInstructions.push_back({"NEG", val, "", temp, ""});
        return temp;
    }

    if (node->type == "PostIncrement" || node->type == "PreIncrement") {
        string var = node->children[0]->value;
        string temp = newTemp();
        tacInstructions.push_back({"+", var, "1", temp, ""});
        tacInstructions.push_back({"=", temp, "", var, ""});
        return var;
    }

    if (node->type == "PostDecrement" || node->type == "PreDecrement") {
        string var = node->children[0]->value;
        string temp = newTemp();
        tacInstructions.push_back({"-", var, "1", temp, ""});
        tacInstructions.push_back({"=", temp, "", var, ""});
        return var;
    }

    if (node->type == "VariableDeclaration") {
        string varName = node->children[1]->value;
        if (node->children.size() == 3) {
            string exprRes = generateTAC(node->children[2]);
            tacInstructions.push_back({"=", exprRes, "", varName, "Declare & Assign"});
        } else {
            tacInstructions.push_back({"DECL", node->children[0]->value, "", varName, "Declaration"});
        }
        return varName;
    }

    if (node->type == "Assignment") {
        string varName = node->children[0]->value;
        string exprRes = generateTAC(node->children[1]);
        tacInstructions.push_back({"=", exprRes, "", varName, "Assignment"});
        return varName;
    }

    if (node->type == "Print") {
        if (!node->children.empty()) {
            string exprRes = generateTAC(node->children[0]);
            tacInstructions.push_back({"PRINT", exprRes, "", "", "Print Statement"});
        } else {
            tacInstructions.push_back({"PRINT", "\"\"", "", "", "Print Empty Line"});
        }
        return "";
    }

    if (node->type == "If") {
        string condRes = generateTAC(node->children[0]);
        string endLabel = newLabel();
        tacInstructions.push_back({"IFFALSE", condRes, "", endLabel, "If Condition Check"});
        generateTAC(node->children[1]);
        tacInstructions.push_back({"LABEL", "", "", endLabel, "End If"});
        return "";
    }

    if (node->type == "IfElse") {
        string condRes = generateTAC(node->children[0]);
        string elseLabel = newLabel();
        string endLabel = newLabel();
        tacInstructions.push_back({"IFFALSE", condRes, "", elseLabel, "If Condition Check"});
        generateTAC(node->children[1]);
        tacInstructions.push_back({"GOTO", "", "", endLabel, "Jump to End"});
        tacInstructions.push_back({"LABEL", "", "", elseLabel, "Else Block"});
        generateTAC(node->children[2]);
        tacInstructions.push_back({"LABEL", "", "", endLabel, "End IfElse"});
        return "";
    }

    if (node->type == "While") {
        string startLabel = newLabel();
        string endLabel = newLabel();
        tacInstructions.push_back({"LABEL", "", "", startLabel, "While Header"});
        string condRes = generateTAC(node->children[0]);
        tacInstructions.push_back({"IFFALSE", condRes, "", endLabel, "While Condition Check"});
        generateTAC(node->children[1]);
        tacInstructions.push_back({"GOTO", "", "", startLabel, "Loop Back"});
        tacInstructions.push_back({"LABEL", "", "", endLabel, "End While"});
        return "";
    }

    if (node->type == "DoWhile") {
        string startLabel = newLabel();
        tacInstructions.push_back({"LABEL", "", "", startLabel, "Do-While Header"});
        generateTAC(node->children[0]); // body
        string condRes = generateTAC(node->children[1]); // condition
        tacInstructions.push_back({"IFTRUE", condRes, "", startLabel, "Do-While Loop Back"});
        return "";
    }

    if (node->type == "For") {
        string startLabel = newLabel();
        string endLabel = newLabel();
        // Init
        generateTAC(node->children[0]);
        tacInstructions.push_back({"LABEL", "", "", startLabel, "For Header"});
        // Condition
        string condRes = generateTAC(node->children[1]);
        tacInstructions.push_back({"IFFALSE", condRes, "", endLabel, "For Condition Check"});
        // Body
        generateTAC(node->children[3]);
        // Update
        generateTAC(node->children[2]);
        tacInstructions.push_back({"GOTO", "", "", startLabel, "For Loop Back"});
        tacInstructions.push_back({"LABEL", "", "", endLabel, "End For"});
        return "";
    }

    for (auto child : node->children) {
        generateTAC(child);
    }

    return "";
}

void printTAC() {
    cout << "\n...............\n";
    cout << "      PHASE 4: INTERMEDIATE CODE GENERATION (TAC)      \n";
    cout << "...............\n";
    int index = 1;
    for (const auto& tac : tacInstructions) {
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
            cout << tac.result << " = " << tac.arg1 << endl;
        } else if (tac.op == "PRINT") {
            cout << "PRINT " << tac.arg1 << endl;
        } else if (tac.op == "DECL") {
            cout << tac.arg1 << " " << tac.result << endl;
        } else if (tac.op == "NEG") {
            cout << tac.result << " = -" << tac.arg1 << endl;
        } else {
            cout << tac.result << " = " << tac.arg1 << " " << tac.op << " " << tac.arg2 << endl;
        }
        index++;
    }
    cout << "...............\n";
}
