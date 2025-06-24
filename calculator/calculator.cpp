#include "calculator.h"

bool ReadNumber(Number& result) {
  if (std::cin >> result) {
    return true;
  }

  std::cin.clear();
  std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
  std::cerr << "Error: Numeric operand expected\n";
  return false;
}

bool Multiply(Number& value, Number&, bool&) {
  if (std::cin.peek() == '*') {
    std::cin.get();
    return BinaryOp(value, [](Number& a, Number b) { a = std::pow(a, b); });
  }
  return BinaryOp(value, [](Number& a, Number b) { a *= b; });
}

bool Load(Number& value, Number& memory, bool& memory_set) {
  if (!memory_set) {
    std::cerr << "Error: Memory is empty\n";
    return false;
  }

  return UnaryOp(value, memory, memory_set, [](Number& a, Number& b, bool&) { a = b; });
}

bool Quit(Number&, Number&, bool&) {
  return false;
}

bool RunCalculatorCycle() {
  Number value = 0.0;
  Number memory = 0.0;
  bool memory_set = false;
  char command = '\0';

  if (!ReadNumber(value)) {
    return false;
  }

  while (std::cin >> command) {
    Handler handler = GetHandlers()[static_cast<unsigned char>(command)];

    if (!handler) {
      std::string token(1, command);
      while (std::isalpha(std::cin.peek())) {
        token += static_cast<char>(std::cin.get());
      }
      std::cerr << "Error: Unknown token " << token << '\n';
      return false;
    }

    if (!handler(value, memory, memory_set)) {
      return false;
    }
  }

  return true;
}
