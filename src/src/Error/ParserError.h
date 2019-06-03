#pragma once

#include <string>
#include <stdexcept>

namespace ska {
    class ParserError : public std::runtime_error {
    public:
        ParserError(std::string message, std::size_t line) : 
            std::runtime_error( "(l." + std::to_string(line) + ") " + std::move(message)) {}
        
        virtual ~ParserError() = default;
    };
}