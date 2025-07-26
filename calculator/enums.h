#pragma once

enum class Operation {
    MULTIPLICATION,
    DIVISION,
    SUBTRACTION,
    ADDITION,
    POWER,
};

enum class ControlKey {
    EQUALS,     // Кнопка "Равно".
    CLEAR,      // Кнопка "C".
    MEM_SAVE,   // Кнопка "MS".
    MEM_LOAD,   // Кнопка "ML".
    MEM_CLEAR,  // Кнопка "MC".
    PLUS_MINUS, // Кнопка "+-".
    BACKSPACE,  // Кнопка "Стереть последний символ".
    EXTRA_KEY,  // Дополнительная экстра-кнопка.
};

enum class ControllerType {
    DOUBLE,     // 0
    FLOAT,      // 1
    UINT8_T,    // 2
    INT,        // 3
    INT64_T,    // 4
    SIZE_T,     // 5
    RATIONAL    // 6
};
