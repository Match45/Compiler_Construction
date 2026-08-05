#include "virtual_machine.h"
#include <iostream>
#include <sstream>
#include <cmath>

bool VirtualMachine::isNumber(const std::string& s, double& outVal) {
    if (s.empty()) return false;
    char* p;
    outVal = strtod(s.c_str(), &p);
    return *p == 0;
}

double VirtualMachine::getValue(const std::string& arg) {
    double val;
    if (isNumber(arg, val)) {
        return val;
    }

    if (!callStack.empty()) {
        auto& locals = callStack.top().locals;
        if (locals.find(arg) != locals.end()) {
            return locals[arg];
        }
    }

    if (globals.find(arg) != globals.end()) {
        return globals[arg];
    }

    return 0.0;
}

void VirtualMachine::setValue(const std::string& varName, double val) {
    if (!callStack.empty()) {
        callStack.top().locals[varName] = val;
    } else {
        globals[varName] = val;
    }
}

bool VirtualMachine::execute(const std::vector<Quadruple>& code) {
    labelMap.clear();
    funcMap.clear();
    globals.clear();
    while (!callStack.empty()) callStack.pop();
    pendingParams.clear();

    for (size_t i = 0; i < code.size(); ++i) {
        if (code[i].op == "LABEL") {
            labelMap[code[i].result] = i;
        } else if (code[i].op == "FUNC_BEGIN") {
            funcMap[code[i].result] = i;
        }
    }

    size_t ip = 0;
    if (funcMap.find("main") != funcMap.end()) {
        ip = funcMap["main"];
        callStack.push(StackFrame("main", code.size(), ""));
    } else {
        callStack.push(StackFrame("global", code.size(), ""));
    }

    while (ip < code.size()) {
        const Quadruple& q = code[ip];

        if (q.op == "=") {
            double val = getValue(q.arg1);
            setValue(q.result, val);
        } else if (q.op == "+") {
            double val = getValue(q.arg1) + getValue(q.arg2);
            setValue(q.result, val);
        } else if (q.op == "-") {
            double val = getValue(q.arg1) - getValue(q.arg2);
            setValue(q.result, val);
        } else if (q.op == "*") {
            double val = getValue(q.arg1) * getValue(q.arg2);
            setValue(q.result, val);
        } else if (q.op == "/") {
            double divisor = getValue(q.arg2);
            if (divisor == 0.0) {
                std::cerr << "Runtime Error on Line " << q.line << ": Division by zero!" << std::endl;
                return false;
            }
            double val = getValue(q.arg1) / divisor;
            setValue(q.result, val);
        } else if (q.op == "%") {
            long long divisor = (long long)getValue(q.arg2);
            if (divisor == 0) {
                std::cerr << "Runtime Error on Line " << q.line << ": Division by zero in modulo!" << std::endl;
                return false;
            }
            long long val = (long long)getValue(q.arg1) % divisor;
            setValue(q.result, val);
        } else if (q.op == "==") {
            setValue(q.result, (getValue(q.arg1) == getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == "!=") {
            setValue(q.result, (getValue(q.arg1) != getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == "<") {
            setValue(q.result, (getValue(q.arg1) < getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == ">") {
            setValue(q.result, (getValue(q.arg1) > getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == "<=") {
            setValue(q.result, (getValue(q.arg1) <= getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == ">=") {
            setValue(q.result, (getValue(q.arg1) >= getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == "&&") {
            setValue(q.result, (getValue(q.arg1) && getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == "||") {
            setValue(q.result, (getValue(q.arg1) || getValue(q.arg2)) ? 1.0 : 0.0);
        } else if (q.op == "!") {
            setValue(q.result, (!getValue(q.arg1)) ? 1.0 : 0.0);
        } else if (q.op == "IF_FALSE") {
            if (getValue(q.arg1) == 0.0) {
                ip = labelMap[q.result];
                continue;
            }
        } else if (q.op == "GOTO") {
            ip = labelMap[q.result];
            continue;
        } else if (q.op == "FUNC_BEGIN") {
            size_t paramIdx = 0;
            size_t nextIp = ip + 1;
            while (nextIp < code.size() && code[nextIp].op == "PARAM_DECL") {
                if (paramIdx < pendingParams.size()) {
                    callStack.top().locals[code[nextIp].result] = pendingParams[paramIdx++];
                }
                nextIp++;
            }
            pendingParams.clear();
            ip = nextIp - 1;
        } else if (q.op == "FUNC_END") {
            if (!callStack.empty()) {
                size_t returnIP = callStack.top().returnIP;
                std::string returnVar = callStack.top().returnVar;
                double retVal = callStack.top().returnValue;
                callStack.pop();

                if (!returnVar.empty() && !callStack.empty()) {
                    setValue(returnVar, retVal);
                }

                if (returnIP >= code.size() || callStack.empty()) {
                    break;
                }
                ip = returnIP;
                continue;
            }
        } else if (q.op == "PARAM") {
            pendingParams.push_back(getValue(q.arg1));
        } else if (q.op == "CALL") {
            std::string funcName = q.arg1;
            std::string targetVar = q.result;
            if (funcMap.find(funcName) != funcMap.end()) {
                callStack.push(StackFrame(funcName, ip + 1, targetVar));
                ip = funcMap[funcName];
                continue;
            } else {
                std::cerr << "Runtime Error: Function " << funcName << " not found!" << std::endl;
                return false;
            }
        } else if (q.op == "RETURN") {
            if (!callStack.empty()) {
                double retVal = 0.0;
                if (!q.arg1.empty()) {
                    retVal = getValue(q.arg1);
                }
                size_t returnIP = callStack.top().returnIP;
                std::string returnVar = callStack.top().returnVar;
                callStack.pop();

                if (!returnVar.empty() && !callStack.empty()) {
                    setValue(returnVar, retVal);
                }

                if (returnIP >= code.size() || callStack.empty()) {
                    break;
                }
                ip = returnIP;
                continue;
            }
        } else if (q.op == "COUT") {
            if (!q.arg1.empty() && q.arg1[0] == '"') {
                std::string str = q.arg1.substr(1, q.arg1.length() - 2);
                std::cout << str;
            } else {
                double val = getValue(q.arg1);
                if (val == (long long)val) {
                    std::cout << (long long)val;
                } else {
                    std::cout << val;
                }
            }
        } else if (q.op == "COUT_ENDL") {
            std::cout << std::endl;
        } else if (q.op == "CIN") {
            double val;
            std::cin >> val;
            setValue(q.result, val);
        }

        ip++;
    }

    return true;
}
