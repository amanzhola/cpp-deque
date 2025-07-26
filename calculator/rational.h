#pragma once

#include <numeric>
#include <stdexcept>
#include <iostream>

class Rational {
public:
    Rational(int numerator = 0, int denominator = 1)
        : numerator_(numerator), denominator_(denominator) {
        if (denominator_ == 0) {
            throw std::invalid_argument("Denominator cannot be zero.");
        }
        Normalize();
    }

    int GetNumerator() const { return numerator_; }
    int GetDenominator() const { return denominator_; }

    // Арифметические операции
    Rational operator+(const Rational& other) const {
        return Rational(numerator_ * other.denominator_ + other.numerator_ * denominator_,
                        denominator_ * other.denominator_);
    }

    Rational operator-(const Rational& other) const {
        return Rational(numerator_ * other.denominator_ - other.numerator_ * denominator_,
                        denominator_ * other.denominator_);
    }

    Rational operator*(const Rational& other) const {
        return Rational(numerator_ * other.numerator_, denominator_ * other.denominator_);
    }

    Rational operator/(const Rational& other) const {
        if (other.numerator_ == 0) {
            throw std::invalid_argument("Division by zero.");
        }
        return Rational(numerator_ * other.denominator_, denominator_ * other.numerator_);
    }

    Rational operator-() const {
        return Rational(-numerator_, denominator_);
    }
    
    Rational operator+() const {
        return *this;
    }

    // Compound assignment
    Rational& operator+=(const Rational& other) {
        *this = *this + other;
        return *this;
    }

    Rational& operator-=(const Rational& other) {
        *this = *this - other;
        return *this;
    }

    Rational& operator*=(const Rational& other) {
        *this = *this * other;
        return *this;
    }

    Rational& operator/=(const Rational& other) {
        *this = *this / other;
        return *this;
    }

    // Сравнение
    bool operator==(const Rational& other) const {
        return numerator_ == other.numerator_ && denominator_ == other.denominator_;
    }
    bool operator!=(const Rational& other) const { return !(*this == other); }
    bool operator<(const Rational& other) const {
        return numerator_ * other.denominator_ < other.numerator_ * denominator_;
    }
    bool operator<=(const Rational& other) const { return *this < other || *this == other; }
    bool operator>(const Rational& other) const { return !(*this <= other); }
    bool operator>=(const Rational& other) const { return !(*this < other); }

    // Ввод/вывод
    friend std::ostream& operator<<(std::ostream& os, const Rational& r) {
        os << r.numerator_;
        if (r.denominator_ != 1) {
            os << " / " << r.denominator_;
        }
        return os;
    }

    friend std::istream& operator>>(std::istream& is, Rational& r) {
        int num = 0, denom = 1;
        char sep = 0;
        if (!(is >> num)) return is;

        if (is >> sep && sep == '/') {
            if (!(is >> denom)) {
                is.setstate(std::ios::failbit);
                return is;
            }
        } else {
            is.putback(sep);
        }
        try {
            r = Rational(num, denom);
        } catch (...) {
            is.setstate(std::ios::failbit);
        }
        return is;
    }

private:
    int numerator_;
    int denominator_;

    void Normalize() {
        if (denominator_ < 0) {
            numerator_ = -numerator_;
            denominator_ = -denominator_;
        }
        int gcd = std::gcd(numerator_, denominator_);
        numerator_ /= gcd;
        denominator_ /= gcd;
    }
};

