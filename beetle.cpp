#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cctype>
#include <stdexcept>

enum class TokenType {
    KW_INT,
    KW_STRING,
    KW_PRINT,
    IDENT,
    NUMBER,
    STRING_LITERAL,
    ASSIGN,
    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,
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

    char peek() const {
        if (pos >= src.size()) return '\0';
        return src[pos];
    }

    char get() {
        if (pos >= src.size()) return '\0';
        return src[pos++];
    }

    void skipWhitespace() {
        while (std::isspace(static_cast<unsigned char>(peek()))) {
            get();
        }
    }

    Token identifier() {
        std::string value;
        while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') {
            value += get();
        }

        if (value == "int") return {TokenType::KW_INT, value};
        if (value == "string") return {TokenType::KW_STRING, value};
        if (value == "print") return {TokenType::KW_PRINT, value};

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
            value += get();
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
            skipWhitespace();
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

            switch (c) {
                case '=': get(); tokens.push_back({TokenType::ASSIGN, "="}); break;
                case '+': get(); tokens.push_back({TokenType::PLUS, "+"}); break;
                case '-': get(); tokens.push_back({TokenType::MINUS, "-"}); break;
                case '*': get(); tokens.push_back({TokenType::STAR, "*"}); break;
                case '/': get(); tokens.push_back({TokenType::SLASH, "/"}); break;
                case '%': get(); tokens.push_back({TokenType::PERCENT, "%"}); break;
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
    std::ostringstream out;

    Token& current() {
        return tokens[pos];
    }

    bool match(TokenType type) {
        if (current().type == type) {
            pos++;
            return true;
        }
        return false;
    }

    void expect(TokenType type, const std::string& msg) {
        if (!match(type)) {
            throw std::runtime_error(msg);
        }
    }

    std::string parseExpression() {
        return parseAddSub();
    }

    std::string parseAddSub() {
        std::string left = parseMulDiv();
        while (current().type == TokenType::PLUS || current().type == TokenType::MINUS) {
            std::string op = current().value;
            pos++;
            std::string right = parseMulDiv();
            left = left + " " + op + " " + right;
        }
        return left;
    }

    std::string parseMulDiv() {
        std::string left = parsePrimary();
        while (current().type == TokenType::STAR ||
               current().type == TokenType::SLASH ||
               current().type == TokenType::PERCENT) {
            std::string op = current().value;
            pos++;
            std::string right = parsePrimary();
            left = left + " " + op + " " + right;
        }
        return left;
    }

    std::string parsePrimary() {
        if (current().type == TokenType::NUMBER) {
            std::string v = current().value;
            pos++;
            return v;
        }

        if (current().type == TokenType::STRING_LITERAL) {
            std::string v = "\"" + current().value + "\"";
            pos++;
            return v;
        }

        if (current().type == TokenType::IDENT) {
            std::string v = current().value;
            pos++;
            return v;
        }

        if (match(TokenType::LPAREN)) {
            std::string expr = parseExpression();
            expect(TokenType::RPAREN, "Expected ')'");
            return "(" + expr + ")";
        }

        throw std::runtime_error("Invalid expression.");
    }

    void parseDeclaration() {
        if (match(TokenType::KW_INT)) {
            expect(TokenType::IDENT, "Expected identifier after 'int'");
            std::string name = tokens[pos - 1].value;

            expect(TokenType::ASSIGN, "Expected '='");
            std::string expr = parseExpression();
            expect(TokenType::SEMICOLON, "Expected ';'");

            out << "    int " << name << " = " << expr << ";\n";
            return;
        }

        if (match(TokenType::KW_STRING)) {
            expect(TokenType::IDENT, "Expected identifier after 'string'");
            std::string name = tokens[pos - 1].value;

            expect(TokenType::ASSIGN, "Expected '='");
            std::string expr = parseExpression();
            expect(TokenType::SEMICOLON, "Expected ';'");

            out << "    std::string " << name << " = " << expr << ";\n";
            return;
        }

        throw std::runtime_error("Invalid declaration.");
    }

    void parseAssignment() {
        expect(TokenType::IDENT, "Expected identifier");
        std::string name = tokens[pos - 1].value;

        expect(TokenType::ASSIGN, "Expected '='");
        std::string expr = parseExpression();
        expect(TokenType::SEMICOLON, "Expected ';'");

        out << "    " << name << " = " << expr << ";\n";
    }

    void parsePrint() {
        expect(TokenType::KW_PRINT, "Expected 'print'");
        expect(TokenType::LPAREN, "Expected '(' after print");
        std::string expr = parseExpression();
        expect(TokenType::RPAREN, "Expected ')'");
        expect(TokenType::SEMICOLON, "Expected ';'");

        out << "    std::cout << " << expr << " << std::endl;\n";
    }

    void parseBlock() {
        expect(TokenType::LBRACE, "Expected '{'");
        out << "    {\n";

        while (current().type != TokenType::RBRACE && current().type != TokenType::END) {
            parseStatement();
        }

        expect(TokenType::RBRACE, "Expected '}'");
        out << "    }\n";
    }

    void parseStatement() {
        if (current().type == TokenType::KW_INT || current().type == TokenType::KW_STRING) {
            parseDeclaration();
            return;
        }

        if (current().type == TokenType::KW_PRINT) {
            parsePrint();
            return;
        }

        if (current().type == TokenType::LBRACE) {
            parseBlock();
            return;
        }

        if (current().type == TokenType::IDENT) {
            parseAssignment();
            return;
        }

        throw std::runtime_error("Invalid command next to: " + current().value);
    }

public:
    explicit Parser(const std::vector<Token>& toks) : tokens(toks) {}

    std::string transpile() {
        out << "#include <iostream>\n";
        out << "#include <string>\n\n";
        out << "int main() {\n";

        while (current().type != TokenType::END) {
            parseStatement();
        }

        out << "    return 0;\n";
        out << "}\n";

        return out.str();
    }
};

std::string readFile(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("The file could not be opened.: " + filename);

    std::stringstream buffer;
    buffer << in.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& filename, const std::string& content) {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("It was not possible to write to the file.: " + filename);
    out << content;
}

std::string outputCppName(const std::string& input) {
    size_t dot = input.find_last_of('.');
    if (dot == std::string::npos) return input + ".cpp";
    return input.substr(0, dot) + ".cpp";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Use: beetle <file.bt>\n";
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