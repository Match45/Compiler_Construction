#include "optimizer.h"
#include <sstream>
#include <cmath>

bool CodeOptimizer::isNumber(const std::string& s, double& outVal) {
    if (s.empty()) return false;
    char* p;
    outVal = strtod(s.c_str(), &p);
    return *p == 0;
}

std::vector<Quadruple> CodeOptimizer::optimize(const std::vector<Quadruple>& inputCode) {
    std::vector<Quadruple> output = inputCode;
    bool changed = true;

    while (changed) {
        changed = false;
        std::vector<Quadruple> pass;

        for (size_t i = 0; i < output.size(); ++i) {
            Quadruple quad = output[i];
            double v1, v2;

            if (isNumber(quad.arg1, v1) && isNumber(quad.arg2, v2)) {
                if (quad.op == "+") {
                    double res = v1 + v2;
                    std::string resStr = (res == (long long)res) ? std::to_string((long long)res) : std::to_string(res);
                    pass.push_back(Quadruple("=", resStr, "", quad.result, quad.line));
                    changed = true;
                    continue;
                } else if (quad.op == "-") {
                    double res = v1 - v2;
                    std::string resStr = (res == (long long)res) ? std::to_string((long long)res) : std::to_string(res);
                    pass.push_back(Quadruple("=", resStr, "", quad.result, quad.line));
                    changed = true;
                    continue;
                } else if (quad.op == "*") {
                    double res = v1 * v2;
                    std::string resStr = (res == (long long)res) ? std::to_string((long long)res) : std::to_string(res);
                    pass.push_back(Quadruple("=", resStr, "", quad.result, quad.line));
                    changed = true;
                    continue;
                } else if (quad.op == "/" && v2 != 0) {
                    double res = v1 / v2;
                    std::string resStr = (res == (long long)res) ? std::to_string((long long)res) : std::to_string(res);
                    pass.push_back(Quadruple("=", resStr, "", quad.result, quad.line));
                    changed = true;
                    continue;
                }
            }

            pass.push_back(quad);
        }

        output = pass;
    }

    return output;
}
