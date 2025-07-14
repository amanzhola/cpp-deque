#include "calculator.h"
#include <cmath>

Calculator::Calculator() : number_(0.0) {}

void Calculator::Set(double num) {
    number_ = num;
}

double Calculator::GetNumber() const {
    return number_;
}

void Calculator::Add(double num) {
    number_ += num;
}

void Calculator::Sub(double num) {
    number_ -= num;
}

void Calculator::Mul(double num) {
    number_ *= num;
}

void Calculator::Div(double num) {
    number_ /= num;
}

void Calculator::Pow(double num) {
    number_ = std::pow(number_, num);
}
