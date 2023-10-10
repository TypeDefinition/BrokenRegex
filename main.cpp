#include <iostream>
#include <regex>
#include <vector>
#include <utility>
#include <string>
#include <cstdint>
#include <optional>

// None & All
static constexpr uint64_t kNone = 0;
static constexpr uint64_t kAll = 0xFFFFFFFFFFFFFFFF;
// Ignored
static constexpr uint64_t kIgnore = 1;
// Keywords
static constexpr uint64_t kProcedure = 1 << 1;
static constexpr uint64_t kRead = 1 << 2;
static constexpr uint64_t kPrint = 1 << 3;
static constexpr uint64_t kCall = 1 << 4;
static constexpr uint64_t kWhile = 1 << 5;
static constexpr uint64_t kIf = 1 << 6;
static constexpr uint64_t kThen = 1 << 7;
static constexpr uint64_t kElse = 1 << 8;
// Boundaries
static constexpr uint64_t kSemicolon = 1 << 9;
static constexpr uint64_t kLBrace = 1 << 10;
static constexpr uint64_t kRBrace = 1 << 11;
static constexpr uint64_t kLParen = 1 << 12;
static constexpr uint64_t kRParen = 1 << 13;
// Equality Operators
static constexpr uint64_t kEqual = 1 << 14;
static constexpr uint64_t kNotEqual = 1 << 15;
static constexpr uint64_t kEqualityOps = kEqual | kNotEqual;
// Relational Operators
static constexpr uint64_t kLessThanEqual = 1 << 16;
static constexpr uint64_t kGreaterThanEqual = 1 << 17;
static constexpr uint64_t kLessThan = 1 << 18;
static constexpr uint64_t kGreaterThan = 1 << 19;
static constexpr uint64_t kRelationalOps = kLessThanEqual | kGreaterThanEqual | kLessThan | kGreaterThan;
// Multiplication Operators
static constexpr uint64_t kModulo = 1 << 20;
static constexpr uint64_t kMultiply = 1 << 21;
static constexpr uint64_t kDivide = 1 << 22;
static constexpr uint64_t kMultiplicationOps = kModulo | kMultiply | kDivide;
// Addition Operators
static constexpr uint64_t kAdd = 1 << 23;
static constexpr uint64_t kSubtract = 1 << 24;
static constexpr uint64_t kAdditionOps = kAdd | kSubtract;
// Logical Operators
static constexpr uint64_t kLogicalAnd = 1 << 25;
static constexpr uint64_t kLogicalOr = 1 << 26;
static constexpr uint64_t kLogicalNot = 1 << 27;
static constexpr uint64_t kLogicalOps = kLogicalAnd | kLogicalOr | kLogicalNot;
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
        {std::regex(R"(^\bread\b)"), kRead | kIdentifier},
        {std::regex(R"(^\bprint\b)"), kPrint | kIdentifier},
        {std::regex(R"(^\bcall\b)"), kCall | kIdentifier},
        {std::regex(R"(^\bwhile\b)"), kWhile | kIdentifier},
        {std::regex(R"(^\bif\b)"), kIf | kIdentifier},
        {std::regex(R"(^\bthen\b)"), kThen | kIdentifier},
        {std::regex(R"(^\belse\b)"), kElse | kIdentifier},
        // Boundaries
        {std::regex(R"(^;)"), kSemicolon},
        {std::regex(R"(^\{)"), kLBrace},
        {std::regex(R"(^\})"), kRBrace},
        {std::regex(R"(^\()"), kLParen},
        {std::regex(R"(^\))"), kRParen},
        // Equality Operators
        {std::regex(R"(^==)"), kEqual},
        {std::regex(R"(^!=)"), kNotEqual},
        // Relational Operators
        {std::regex(R"(^<=)"), kLessThanEqual},
        {std::regex(R"(^>=)"), kGreaterThanEqual},
        {std::regex(R"(^<)"), kLessThan},
        {std::regex(R"(^>)"), kGreaterThan},
        // Multiplication Operators
        {std::regex(R"(^\%)"), kModulo},
        {std::regex(R"(^\*)"), kMultiply},
        {std::regex(R"(^\/)"), kDivide},
        // Addition Operators
        {std::regex(R"(^\+)"), kAdd},
        {std::regex(R"(^\-)"), kSubtract},
        // Logical Operators
        {std::regex(R"(^&&)"), kLogicalAnd},
        {std::regex(R"(^\|\|)"), kLogicalOr},
        {std::regex(R"(^!)"), kLogicalNot},
        // Assignment Operator// Assignment Operator
        {std::regex(R"(^=)"), kAssign},
        // Constants & Variables
        {std::regex(R"(^[1-9]\d*)"), kInteger},
        {std::regex(R"(^0)"), kInteger},
        {std::regex(R"(^[a-zA-Z][a-zA-Z0-9]*)"), kIdentifier},
};

std::string source;

std::optional<Token> getNextToken() {
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
            return getNextToken();
        }

        return Token{types, value};
    }

    return std::nullopt;
}

int main() {
    source = "procedure p1 {\n"
             "    x = a + (b - c) * 4;\n"
             "}";

    uint32_t counter = 0;
    while (true) {
        auto token = getNextToken();
        if (!token.has_value()) {
            std::cout << "No more tokens." << std::endl;
            break;
        }

        std::cout << "Token " << counter++ << ": " << token->value << std::endl;
    }

    return 0;
}
