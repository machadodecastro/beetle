#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

enum class TokenType {
    // Keywords
    KW_INT,
    KW_STRING,
    KW_BOOL,
    KW_PRINT,
    KW_IF,
    KW_ELSE,
    KW_WHILE,
    KW_FN,
    KW_RETURN,
    KW_TRUE,
    KW_FALSE,

    // Identifiers / literals
    IDENT,
    NUMBER,
    STRING_LITERAL,

    // Operators
    ASSIGN,       // =
    EQ,           // ==
    NEQ,          // !=
    LT,           // <
    LTE,          // <=
    GT,           // >
    GTE,          // >=
    PLUS,         // +
    MINUS,        // -
    STAR,         // *
    SLASH,        // /
    PERCENT,      // %
    CARET,        // ^
    AND,          // &&
    OR,           // ||
    NOT,          // !

    // Punctuation
    SEMICOLON,
    COMMA,
    LBRACE,
    RBRACE,
    LBRACKET,
    RBRACKET,
    LPAREN,
    RPAREN,

    END
};

struct Token {
    TokenType type;
    std::string value;
};

class Lexer {
private:
    std::string src;
    size_t pos = 0;

    char peek(size_t offset = 0) const {
        size_t p = pos + offset;
        if (p >= src.size()) return '\0';
        return src[p];
    }

    char get() {
        if (pos >= src.size()) return '\0';
        return src[pos++];
    }

    void skipWhitespaceAndComments() {
        while (true) {
            while (std::isspace(static_cast<unsigned char>(peek()))) {
                get();
            }

            // Single-line comment: //
            if (peek() == '/' && peek(1) == '/') {
                get();
                get();
                while (peek() != '\n' && peek() != '\0') {
                    get();
                }
                continue;
            }

            // Multi-line comment: /* ... */
            if (peek() == '/' && peek(1) == '*') {
                get();
                get();
                while (!(peek() == '*' && peek(1) == '/')) {
                    if (peek() == '\0') {
                        throw std::runtime_error("Unclosed block comment.");
                    }
                    get();
                }
                get();
                get();
                continue;
            }

            break;
        }
    }

    Token identifier() {
        std::string value;
        while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
            value += get();
        }

        if (value == "int") return {TokenType::KW_INT, value};
        if (value == "string") return {TokenType::KW_STRING, value};
        if (value == "bool") return {TokenType::KW_BOOL, value};
        if (value == "print") return {TokenType::KW_PRINT, value};
        if (value == "if") return {TokenType::KW_IF, value};
        if (value == "else") return {TokenType::KW_ELSE, value};
        if (value == "while") return {TokenType::KW_WHILE, value};
        if (value == "fn") return {TokenType::KW_FN, value};
        if (value == "return") return {TokenType::KW_RETURN, value};
        if (value == "true") return {TokenType::KW_TRUE, value};
        if (value == "false") return {TokenType::KW_FALSE, value};

        return {TokenType::IDENT, value};
    }

    Token number() {
        std::string value;
        while (std::isdigit(static_cast<unsigned char>(peek()))) {
            value += get();
        }
        return {TokenType::NUMBER, value};
    }

    Token stringLiteral() {
        std::string value;
        get(); // opening "

        while (peek() != '"' && peek() != '\0') {
            if (peek() == '\\') {
                value += get();
                if (peek() != '\0') value += get();
            } else {
                value += get();
            }
        }

        if (peek() != '"') {
            throw std::runtime_error("Unclosed string.");
        }

        get(); // closing "
        return {TokenType::STRING_LITERAL, value};
    }

public:
    explicit Lexer(const std::string& input) : src(input) {}

    std::vector<Token> tokenize() {
        std::vector<Token> tokens;

        while (true) {
            skipWhitespaceAndComments();
            char c = peek();

            if (c == '\0') {
                tokens.push_back({TokenType::END, ""});
                break;
            }

            if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
                tokens.push_back(identifier());
                continue;
            }

            if (std::isdigit(static_cast<unsigned char>(c))) {
                tokens.push_back(number());
                continue;
            }

            if (c == '"') {
                tokens.push_back(stringLiteral());
                continue;
            }

            // Two-character operators
            if (c == '=' && peek(1) == '=') { get(); get(); tokens.push_back({TokenType::EQ, "=="}); continue; }
            if (c == '!' && peek(1) == '=') { get(); get(); tokens.push_back({TokenType::NEQ, "!="}); continue; }
            if (c == '<' && peek(1) == '=') { get(); get(); tokens.push_back({TokenType::LTE, "<="}); continue; }
            if (c == '>' && peek(1) == '=') { get(); get(); tokens.push_back({TokenType::GTE, ">="}); continue; }
            if (c == '&' && peek(1) == '&') { get(); get(); tokens.push_back({TokenType::AND, "&&"}); continue; }
            if (c == '|' && peek(1) == '|') { get(); get(); tokens.push_back({TokenType::OR, "||"}); continue; }

            switch (c) {
                case '=': get(); tokens.push_back({TokenType::ASSIGN, "="}); break;
                case '<': get(); tokens.push_back({TokenType::LT, "<"}); break;
                case '>': get(); tokens.push_back({TokenType::GT, ">"}); break;
                case '+': get(); tokens.push_back({TokenType::PLUS, "+"}); break;
                case '-': get(); tokens.push_back({TokenType::MINUS, "-"}); break;
                case '*': get(); tokens.push_back({TokenType::STAR, "*"}); break;
                case '/': get(); tokens.push_back({TokenType::SLASH, "/"}); break;
                case '%': get(); tokens.push_back({TokenType::PERCENT, "%"}); break;
                case '^': get(); tokens.push_back({TokenType::CARET, "^"}); break;
                case '!': get(); tokens.push_back({TokenType::NOT, "!"}); break;
                case ';': get(); tokens.push_back({TokenType::SEMICOLON, ";"}); break;
                case ',': get(); tokens.push_back({TokenType::COMMA, ","}); break;
                case '{': get(); tokens.push_back({TokenType::LBRACE, "{"}); break;
                case '}': get(); tokens.push_back({TokenType::RBRACE, "}"}); break;
                case '[': get(); tokens.push_back({TokenType::LBRACKET, "["}); break;
                case ']': get(); tokens.push_back({TokenType::RBRACKET, "]"}); break;
                case '(': get(); tokens.push_back({TokenType::LPAREN, "("}); break;
                case ')': get(); tokens.push_back({TokenType::RPAREN, ")"}); break;
                default:
                    throw std::runtime_error(std::string("Invalid character: ") + c);
            }
        }

        return tokens;
    }
};

class Parser {
private:
    std::vector<Token> tokens;
    size_t pos = 0;

    std::ostringstream functionOut;
    std::ostringstream mainOut;
    std::ostringstream* out = &mainOut;

    Token& current() {
        return tokens[pos];
    }

    Token& previous() {
        return tokens[pos - 1];
    }

    bool match(TokenType type) {
        if (current().type == type) {
            pos++;
            return true;
        }
        return false;
    }

    bool check(TokenType type) const {
        return tokens[pos].type == type;
    }

    bool checkNext(TokenType type) const {
        if (pos + 1 >= tokens.size()) return false;
        return tokens[pos + 1].type == type;
    }

    void expect(TokenType type, const std::string& msg) {
        if (!match(type)) {
            throw std::runtime_error(msg);
        }
    }

    std::string indent() const {
        return "    ";
    }

    bool isTypeToken(TokenType t) const {
        return t == TokenType::KW_INT || t == TokenType::KW_STRING || t == TokenType::KW_BOOL;
    }

    std::string parseType() {
        if (match(TokenType::KW_INT)) return "int";
        if (match(TokenType::KW_STRING)) return "std::string";
        if (match(TokenType::KW_BOOL)) return "bool";
        throw std::runtime_error("Expected type.");
    }

    // Expressions
    std::string parseExpression() {
        return parseLogicalOr();
    }

    std::string parseLogicalOr() {
        std::string left = parseLogicalAnd();
        while (match(TokenType::OR)) {
            std::string right = parseLogicalAnd();
            left = "(" + left + " || " + right + ")";
        }
        return left;
    }

    std::string parseLogicalAnd() {
        std::string left = parseEquality();
        while (match(TokenType::AND)) {
            std::string right = parseEquality();
            left = "(" + left + " && " + right + ")";
        }
        return left;
    }

    std::string parseEquality() {
        std::string left = parseComparison();
        while (check(TokenType::EQ) || check(TokenType::NEQ)) {
            std::string op = current().value;
            pos++;
            std::string right = parseComparison();
            left = "(" + left + " " + op + " " + right + ")";
        }
        return left;
    }

    std::string parseComparison() {
        std::string left = parseAddSub();
        while (check(TokenType::LT) || check(TokenType::LTE) ||
               check(TokenType::GT) || check(TokenType::GTE)) {
            std::string op = current().value;
            pos++;
            std::string right = parseAddSub();
            left = "(" + left + " " + op + " " + right + ")";
        }
        return left;
    }

    std::string parseAddSub() {
        std::string left = parseMulDiv();
        while (check(TokenType::PLUS) || check(TokenType::MINUS)) {
            std::string op = current().value;
            pos++;
            std::string right = parseMulDiv();
            left = "(" + left + " " + op + " " + right + ")";
        }
        return left;
    }

    std::string parseMulDiv() {
        std::string left = parsePower();
        while (check(TokenType::STAR) || check(TokenType::SLASH) || check(TokenType::PERCENT)) {
            std::string op = current().value;
            pos++;
            std::string right = parsePower();
            left = "(" + left + " " + op + " " + right + ")";
        }
        return left;
    }

    std::string parsePower() {
        std::string left = parseUnary();

        if (match(TokenType::CARET)) {
            std::string right = parsePower(); // right associative
            left = "beetle_pow(" + left + ", " + right + ")";
        }

        return left;
    }

    std::string parseUnary() {
        if (match(TokenType::NOT)) {
            return "(!" + parseUnary() + ")";
        }

        if (match(TokenType::MINUS)) {
            return "(-" + parseUnary() + ")";
        }

        return parsePrimary();
    }

    std::vector<std::string> parseArguments() {
        std::vector<std::string> args;

        expect(TokenType::LPAREN, "Expected '('");

        if (!check(TokenType::RPAREN)) {
            do {
                args.push_back(parseExpression());
            } while (match(TokenType::COMMA));
        }

        expect(TokenType::RPAREN, "Expected ')' after arguments");
        return args;
    }

    std::string parsePrimary() {
        if (match(TokenType::NUMBER)) {
            return previous().value;
        }

        if (match(TokenType::STRING_LITERAL)) {
            return "\"" + previous().value + "\"";
        }

        if (match(TokenType::KW_TRUE)) {
            return "true";
        }

        if (match(TokenType::KW_FALSE)) {
            return "false";
        }

        if (match(TokenType::IDENT)) {
            std::string name = previous().value;

            if (check(TokenType::LPAREN)) {
                auto args = parseArguments();
                std::ostringstream call;
                call << name << "(";
                for (size_t i = 0; i < args.size(); ++i) {
                    if (i > 0) call << ", ";
                    call << args[i];
                }
                call << ")";
                return call.str();
            }

            return name;
        }

        if (match(TokenType::LPAREN)) {
            std::string expr = parseExpression();
            expect(TokenType::RPAREN, "Expected ')'");
            return "(" + expr + ")";
        }

        throw std::runtime_error("Invalid expression.");
    }

    // Statements
    void parseDeclaration() {
        std::string cppType = parseType();

        expect(TokenType::IDENT, "Expected identifier after type");
        std::string name = previous().value;

        expect(TokenType::ASSIGN, "Expected '=' in declaration");
        std::string expr = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';'");

        *out << indent() << cppType << " " << name << " = " << expr << ";\n";
    }

    void parseAssignment() {
        expect(TokenType::IDENT, "Expected identifier");
        std::string name = previous().value;

        expect(TokenType::ASSIGN, "Expected '='");
        std::string expr = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';'");

        *out << indent() << name << " = " << expr << ";\n";
    }

    void parsePrint() {
        expect(TokenType::KW_PRINT, "Expected 'print'");
        expect(TokenType::LPAREN, "Expected '(' after print");
        std::string expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        expect(TokenType::SEMICOLON, "Expected ';'");

        *out << indent() << "std::cout << " << expr << " << std::endl;\n";
    }

    void parseReturn() {
        expect(TokenType::KW_RETURN, "Expected 'return'");
        std::string expr = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';' after return");
        *out << indent() << "return " << expr << ";\n";
    }

    void parseBlockBody() {
        expect(TokenType::LBRACE, "Expected '{'");

        while (!check(TokenType::RBRACE) && !check(TokenType::END)) {
            parseStatement();
        }

        expect(TokenType::RBRACE, "Expected '}'");
    }

    void parseIf() {
        expect(TokenType::KW_IF, "Expected 'if'");
        expect(TokenType::LPAREN, "Expected '(' after if");
        std::string condition = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after if condition");

        *out << indent() << "if (" << condition << ") {\n";
        parseBlockBody();
        *out << indent() << "}\n";

        if (match(TokenType::KW_ELSE)) {
            *out << indent() << "else {\n";
            parseBlockBody();
            *out << indent() << "}\n";
        }
    }

    void parseWhile() {
        expect(TokenType::KW_WHILE, "Expected 'while'");
        expect(TokenType::LPAREN, "Expected '(' after while");
        std::string condition = parseExpression();
        expect(TokenType::RPAREN, "Expected ')' after while condition");

        *out << indent() << "while (" << condition << ") {\n";
        parseBlockBody();
        *out << indent() << "}\n";
    }

    void parseExpressionStatement() {
        std::string expr = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';'");
        *out << indent() << expr << ";\n";
    }

    void parseFunction() {
        expect(TokenType::KW_FN, "Expected 'fn'");

        std::string returnType = parseType();

        expect(TokenType::IDENT, "Expected function name");
        std::string funcName = previous().value;

        expect(TokenType::LPAREN, "Expected '(' after function name");

        std::vector<std::pair<std::string, std::string>> params;
        if (!check(TokenType::RPAREN)) {
            do {
                std::string paramType = parseType();
                expect(TokenType::IDENT, "Expected parameter name");
                std::string paramName = previous().value;
                params.push_back({paramType, paramName});
            } while (match(TokenType::COMMA));
        }

        expect(TokenType::RPAREN, "Expected ')' after parameters");

        std::ostringstream localFn;
        localFn << returnType << " " << funcName << "(";
        for (size_t i = 0; i < params.size(); ++i) {
            if (i > 0) localFn << ", ";
            localFn << params[i].first << " " << params[i].second;
        }
        localFn << ") {\n";

        std::ostringstream* previousOut = out;
        out = &localFn;

        parseBlockBody();

        out = previousOut;
        localFn << "}\n\n";

        functionOut << localFn.str();
    }

    void parseStatement() {
        if (check(TokenType::KW_FN)) {
            parseFunction();
            return;
        }

        if (isTypeToken(current().type)) {
            parseDeclaration();
            return;
        }

        if (check(TokenType::KW_PRINT)) {
            parsePrint();
            return;
        }

        if (check(TokenType::KW_IF)) {
            parseIf();
            return;
        }

        if (check(TokenType::KW_WHILE)) {
            parseWhile();
            return;
        }

        if (check(TokenType::KW_RETURN)) {
            parseReturn();
            return;
        }

        if (check(TokenType::IDENT) && checkNext(TokenType::ASSIGN)) {
            parseAssignment();
            return;
        }

        if (check(TokenType::IDENT) && checkNext(TokenType::LPAREN)) {
            parseExpressionStatement();
            return;
        }

        throw std::runtime_error("Invalid command next to: " + current().value);
    }

public:
    explicit Parser(const std::vector<Token>& toks) : tokens(toks) {}

    std::string transpile() {
        std::ostringstream finalOut;

        finalOut << "#include <iostream>\n";
        finalOut << "#include <string>\n\n";

        finalOut << "int beetle_pow(int base, int exp) {\n";
        finalOut << "    if (exp < 0) return 0;\n";
        finalOut << "    int result = 1;\n";
        finalOut << "    while (exp > 0) {\n";
        finalOut << "        result *= base;\n";
        finalOut << "        exp--;\n";
        finalOut << "    }\n";
        finalOut << "    return result;\n";
        finalOut << "}\n\n";

        while (current().type != TokenType::END) {
            if (check(TokenType::KW_FN)) {
                parseFunction();
            } else {
                parseStatement();
            }
        }

        finalOut << functionOut.str();
        finalOut << "int main() {\n";
        finalOut << mainOut.str();
        finalOut << "    return 0;\n";
        finalOut << "}\n";

        return finalOut.str();
    }
};

std::string readFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("The file could not be opened: " + filename);

    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("It was not possible to write to the file: " + filename);
    out << content;
}

std::string outputCppName(const std::string& input) {
    size_t dot = input.find_last_of('.');
    if (dot == std::string::npos) return input + ".cpp";
    return input.substr(0, dot) + ".cpp";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: beetle <file.bt>\n";
        return 1;
    }

    try {
        std::string source = readFile(argv[1]);

        Lexer lexer(source);
        auto tokens = lexer.tokenize();

        Parser parser(tokens);
        std::string cppCode = parser.transpile();

        std::string outFile = outputCppName(argv[1]);
        writeFile(outFile, cppCode);

        std::cout << "C++ generated in: " << outFile << "\n";
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}