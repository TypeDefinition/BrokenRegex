#include <iostream>
#include <regex>
#include <vector>
#include <utility>
#include <string>
#include <fstream>
#include <sstream>
#include <streambuf>
#include <cstdint>
#include <optional>

// Ignored
static constexpr uint64_t kIgnore = 1;
// Keywords
static constexpr uint64_t kProcedure = 1 << 1;
// Boundaries
static constexpr uint64_t kSemicolon = 1 << 9;
static constexpr uint64_t kLBrace = 1 << 10;
static constexpr uint64_t kRBrace = 1 << 11;
static constexpr uint64_t kLParen = 1 << 12;
static constexpr uint64_t kRParen = 1 << 13;
// Multiplication Operators
static constexpr uint64_t kModulo = 1 << 20;
static constexpr uint64_t kMultiply = 1 << 21;
static constexpr uint64_t kDivide = 1 << 22;
// Addition Operators
static constexpr uint64_t kAdd = 1 << 23;
static constexpr uint64_t kSubtract = 1 << 24;
// Assignment Operators
static constexpr uint64_t kAssign = 1 << 28;
// Constants & Variables
static constexpr uint64_t kInteger = 1 << 29;
static constexpr uint64_t kIdentifier = 1 << 30;

struct Token {
    const uint64_t types;
    const std::string value;
};

const std::vector<std::pair<std::regex, uint64_t>> kSpecs = {
        // Ignored Tokens
        {std::regex(R"(^\s)"), kIgnore}, // Whitespace
        {std::regex(R"(^\/\/.*)"), kIgnore}, // Single-Line Comments
        {std::regex(R"(^\/\*[\s\S]*?\*\/)"), kIgnore}, // Multi-Line Comments
        // Keywords
        {std::regex(R"(^\bprocedure\b)"), kProcedure | kIdentifier},
        // Boundaries
        {std::regex(R"(^;)"), kSemicolon},
        {std::regex(R"(^\{)"), kLBrace},
        {std::regex(R"(^\})"), kRBrace},
        {std::regex(R"(^\()"), kLParen},
        {std::regex(R"(^\))"), kRParen},
        // Multiplication Operators
        {std::regex(R"(^\%)"), kModulo},
        {std::regex(R"(^\*)"), kMultiply},
        {std::regex(R"(^\/)"), kDivide},
        // Addition Operators
        {std::regex(R"(^\+)"), kAdd},
        {std::regex(R"(^\-)"), kSubtract},
        // Assignment Operator
        {std::regex(R"(^=)"), kAssign},
        // Constants & Variables
        {std::regex(R"(^[1-9]\d*)"), kInteger},
        {std::regex(R"(^0)"), kInteger},
        {std::regex(R"(^[a-zA-Z][a-zA-Z0-9]*)"), kIdentifier},
};

std::optional<Token> getNextToken(std::string& source) {
    for (const auto &spec : kSpecs) {
        // If we cannot match this rule, try the next rule.
        std::smatch matches;
        if (!std::regex_search(source, matches, spec.first)) {
            continue;
        }

        // Retrieve the type and value.
        auto types = spec.second;
        auto value = matches.str();
        source = matches.suffix().str();

        // Should ignore token, e.g. whitespace.
        if ((types & kIgnore) == kIgnore) {
            return getNextToken(source);
        }

        return Token{types, value};
    }

    return std::nullopt;
}

int main() {
    std::string source = "procedure p1 {\n"
                         "    x = a + (b - c) * 4;\n"
                         "}";

    uint32_t counter = 0;
    while (true) {
        auto token = getNextToken(source);
        if (!token.has_value()) {
            std::cout << "No more tokens." << std::endl;
            break;
        }

        std::cout << "Token " << counter++ << ": " << token->value << std::endl;
    }

    return 0;
}