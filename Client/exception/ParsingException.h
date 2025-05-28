#pragma once

#include <stdexcept>
#include <string>
#include <QString>

// Простой класс исключения для ошибок парсинга
class ParsingException : public std::runtime_error {
public:
    // Конструктор из QString
    explicit ParsingException(const QString& message)
        : std::runtime_error(message.toStdString()) {}

    // Конструктор из const char*
    explicit ParsingException(const char* message)
        : std::runtime_error(message) {}

    // Конструктор из std::string
    explicit ParsingException(const std::string& message)
        : std::runtime_error(message) {}
};

