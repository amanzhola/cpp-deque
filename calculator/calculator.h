#ifndef CALCULATOR_H
#define CALCULATOR_H

class Calculator {
public:
    Calculator();

    void Set(double num);
    double GetNumber() const;

    void Add(double num);
    void Sub(double num);
    void Mul(double num);
    void Div(double num);
    void Pow(double num);

private:
    double number_;
};

#endif
