#ifndef VIRTUAL_MACHINE_H
#define VIRTUAL_MACHINE_H

#include "icg.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <stack>

struct StackFrame {
    std::string funcName;
    size_t returnIP;
    std::unordered_map<std::string, double> locals;
    double returnValue;
    std::string returnVar;

    StackFrame(std::string name, size_t ip, std::string retVar = "")
        : funcName(name), returnIP(ip), returnValue(0.0), returnVar(retVar) {}
};

class VirtualMachine {
private:
    std::unordered_map<std::string, size_t> labelMap;
    std::unordered_map<std::string, size_t> funcMap;
    std::unordered_map<std::string, double> globals;
    std::stack<StackFrame> callStack;
    std::vector<double> pendingParams;

    double getValue(const std::string& arg);
    void setValue(const std::string& varName, double val);
    bool isNumber(const std::string& s, double& outVal);

public:
    VirtualMachine() {}

    bool execute(const std::vector<Quadruple>& code);
};

#endif // VIRTUAL_MACHINE_H
