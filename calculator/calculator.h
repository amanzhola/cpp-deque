#pragma once

#include <optional>
#include <string>
#include <cmath>
#include <type_traits>

#include "pow.h"
#include "rational.h"

using Error = std::string;

template<typename Number>
class Calculator {
public:
    void Set(Number number) {
        number_ = number;
    }

    Number GetNumber() const {
        return number_;
    }

    std::optional<Error> Add(Number rhs) {
        number_ += rhs;
        return std::nullopt;
    }

    std::optional<Error> Sub(Number rhs) {
        number_ -= rhs;
        return std::nullopt;
    }

    std::optional<Error> Mul(Number rhs) {
        number_ *= rhs;
        return std::nullopt;
    }

    std::optional<Error> Div(Number rhs) {
        if constexpr (std::is_floating_point_v<Number>) {
            number_ /= rhs;
            return std::nullopt;
        } else {
            if (rhs == 0) {
                return "Integer division by zero";
            }
            number_ /= rhs;
            return std::nullopt;
        }
    }

    std::optional<Error> Pow(Number rhs) {
        if constexpr (std::is_floating_point_v<Number>) {
            number_ = std::pow(number_, rhs);
            return std::nullopt;
        }
        else if constexpr (std::is_integral_v<Number>) {
            if (rhs < 0) return "Integer negative power";
            if (number_ == 0 && rhs == 0) return "Zero power to zero";
            number_ = IntegerPow(number_, rhs);
            return std::nullopt;
        }
        else if constexpr (std::is_same_v<Number, Rational>) {
            if (number_ == Rational(0) && rhs == Rational(0)) return "Zero power to zero";
            if (rhs.GetDenominator() != 1) return "Fractional power is not supported";
            number_ = ::Pow(number_, rhs);
            return std::nullopt;
        }
        else {
            return "Unsupported number type";
        }
    }

    bool GetHasMem() const {
        return memory_.has_value();
    }

    void Save() {
        memory_ = number_;
    }

    std::optional<Error> Load() {
        if (!memory_) return "No saved value";
        number_ = *memory_;
        return std::nullopt;
    }

private:
    Number number_{};
    std::optional<Number> memory_;
};