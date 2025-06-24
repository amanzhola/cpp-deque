#pragma once

#include <cmath>
#include <iostream>
#include <limits>

using Number = double;
using Handler = bool (*)(Number&, Number&, bool&);

bool ReadNumber(Number& result);
bool RunCalculatorCycle();

bool Multiply(Number& value, Number& memory, bool& memory_set);
bool Load(Number& value, Number& memory, bool& memory_set);
bool Quit(Number&, Number&, bool&);

template <typename F>
bool BinaryOp(Number& value, F func) {
  Number operand;
  if (!ReadNumber(operand)) {
    return false;
  }
  func(value, operand);
  return true;
}

template <typename F>
bool UnaryOp(Number& value, Number& memory, bool& memory_set, F func) {
  func(value, memory, memory_set);
  return true;
}

template <typename F>
inline bool UnaryShortCall(Number& value, Number&, bool&) {
  static F stored;
  stored(value);
  return true;
}

template <typename F>
inline bool BinaryShortCall(Number& value, Number&, bool&) {
  static F stored;
  return BinaryOp(value, stored);
}

template <typename F>
Handler UnaryShort(F func) {
  static F stored = func;
  return [](Number& value, Number&, bool&) -> bool {
    stored(value);
    return true;
  };
}

template <typename F>
Handler BinaryShort(F func) {
  static F stored = func;
  return [](Number& value, Number&, bool&) -> bool {
    return BinaryOp(value, stored);
  };
}

inline Handler* GetHandlers() {
  static Handler table[256] = {nullptr};

  table['+'] = BinaryShort([](Number& a, Number b) { a += b; });
  table['-'] = BinaryShort([](Number& a, Number b) { a -= b; });
  table['*'] = Multiply;
  table['/'] = BinaryShort([](Number& a, Number b) {
    a = (b == 0 ? std::numeric_limits<Number>::infinity() : a / b);
  });
  table[':'] = BinaryShort([](Number& a, Number b) { a = b; });
  table['='] = UnaryShort([](Number& a) { std::cout << a << '\n'; });
  table['c'] = UnaryShort([](Number& a) { a = 0.0; });
  table['s'] = [](Number& value, Number& memory, bool& memory_set) {
    memory = value;
    memory_set = true;
    return true;
  };
  table['l'] = Load;
  table['q'] = Quit;

  return table;
}
