#pragma once

#include <stdexcept>
#include <string>
#include <QString>

class ParsingException : public std::runtime_error {
public:
    explicit ParsingException(const QString& message)
        : std::runtime_error(message.toStdString()) {}

    explicit ParsingException(const char* message)
        : std::runtime_error(message) {}

    explicit ParsingException(const std::string& message)
        : std::runtime_error(message) {}
};

