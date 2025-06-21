#include "calculator.h"
#include <iostream>
#include <string>
#include <cmath>
#include <limits>

bool ReadNumber(Number& result) {
    if (!(std::cin >> result)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cerr << "Error: Numeric operand expected\n";
        return false;
    }
    return true;
}

bool RunCalculatorCycle() {
    Number value = 0, memory = 0;
    bool memorySet = false;

    if (!ReadNumber(value)) { return false;}

    char cmd;
    while (std::cin >> cmd) {
        if (cmd == '*' && std::cin.peek() == '*') {
            std::cin.get();
            Number x;
            if (!ReadNumber(x)) { return false;}
            value = std::pow(value, x);
            continue;
        }

        switch (cmd) {
            case '+': case '-': case '*': case '/': case ':': {
                Number x;
                if (!ReadNumber(x)) { return false;}
                switch (cmd) {
                    case '+': value += x; break;
                    case '-': value -= x; break;
                    case '*': value *= x; break;
                    case '/': value = (x == 0 ? std::numeric_limits<Number>::infinity() : value / x); break;
                    case ':': value = x; break;
                }
                break;
            }
            case '=': std::cout << value << '\n'; break;
            case 'c': value = 0; break;
            case 's': memory = value; memorySet = true; break;
            case 'l':
                if (!memorySet) {
                    std::cerr << "Error: Memory is empty\n";
                    return false;
                }
                value = memory;
                break;
            case 'q': return true;
            default: {
                std::string token(1, cmd);
                while (std::isalpha(std::cin.peek()))
                    token += std::cin.get();
                std::cerr << "Error: Unknown token " << token << '\n';
                return false;
            }
        }
    }

    return true;
}
