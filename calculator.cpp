// В этом файле определения функций.
// За основу возьмите решение предыдущей задачи.
#include <iostream>
#include <string>
#include <cmath>
#include <unordered_set>

using Number = double;

bool ReadNumber(Number& result) {
    std::cin >> result;
    if (std::cin.fail()) {
        std::cerr << "Error: Numeric operand expected" << std::endl;
        return false;
    }
    return true;
}

bool RunCalculatorCycle() {
    Number value = 0;
    Number memory = 0;
    bool memorySet = false;

    if (!ReadNumber(value)) return false;

    const std::unordered_set<char> binaryOps = {'+', '-', '*', '/', ':'};
    const std::unordered_set<char> simpleOps = {'=', 'c', 's', 'l', 'q'};

    while (true) {
        char command;
        std::cin >> command;

        if (binaryOps.count(command)) {
            // Обработка **
            if (command == '*' && std::cin.peek() == '*') {
                std::cin.get();  // consume second '*'
                Number operand;
                if (!ReadNumber(operand)) return false;
                value = std::pow(value, operand);
                continue;
            }

            // Обычные бинарные операции
            Number operand;
            if (!ReadNumber(operand)) return false;

            switch (command) {
                case '+': value += operand; break;
                case '-': value -= operand; break;
                case '*': value *= operand; break;
                case '/': value /= operand; break;
                case ':': value = operand; break;
            }
        }
        else if (simpleOps.count(command)) {
            switch (command) {
                case '=':
                    std::cout << value << std::endl;
                    break;
                case 'c':
                    value = 0;
                    break;
                case 's':
                    memory = value;
                    memorySet = true;
                    break;
                case 'l':
                    if (!memorySet) {
                        std::cerr << "Error: Memory is empty" << std::endl;
                        return false;
                    }
                    value = memory;
                    break;
                case 'q':
                    return true;
            }
        }
        else {
            // Нераспознанный токен
            std::string token(1, command);
            while (std::isalpha(std::cin.peek())) {
                token += std::cin.get();
            }
            std::cerr << "Error: Unknown token " << token << std::endl;
            return false;
        }
    }
}
