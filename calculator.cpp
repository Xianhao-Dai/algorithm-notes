#include<string>
#include<vector>
#include<map>
#include<stack>

#include<regex>

#define CALCULATOR_DEBUG

#define EPS 1e-9
#define PAI 3.1415

#ifdef CALCULATOR_DEBUG
#include<iostream>
#endif

struct Token {
    enum TokenType {
        NONE = 0,
        PI,
        NUMBER,
        PLUS,
        MINUS,
        OPPOSITE,
        MULTIPLY,
        DIVIDE,
        POWER,
        PERCENTAGE,
        LBRACKET,
        RBRACKET,
        COS,
        SIN,
        TAN,
        COT
    } type;
    std::string origin_text;
    explicit Token(TokenType type, std::string origin_text) : type(type), origin_text(origin_text) {
    }
};

class Lexer {
public:
    static std::vector<Token> tokenize(std::string expr) {
        std::vector<Token> tokens;
        static const std::vector<std::pair<Token::TokenType, std::regex>> rules = {
            {Token::NONE, std::regex(R"(\s+)")},
            {Token::PI, std::regex(R"(π)")},
            {Token::NUMBER, std::regex(R"(\d+(?:\.\d+)?)")},
            {Token::PLUS, std::regex(R"(\+)")},
            {Token::MINUS, std::regex(R"(-)")},
            {Token::MULTIPLY, std::regex(R"(\*)")},
            {Token::DIVIDE, std::regex(R"(/)")},
            {Token::POWER, std::regex(R"(\^)")},
            {Token::PERCENTAGE, std::regex(R"(%)")},
            {Token::LBRACKET, std::regex(R"(\()")},
            {Token::RBRACKET, std::regex(R"(\))")},
            {Token::COS, std::regex(R"(cos)")},
            {Token::SIN, std::regex(R"(sin)")},
            {Token::TAN, std::regex(R"(tan)")},
            {Token::COT, std::regex(R"(cot)")},
        };
        std::smatch m;
        auto front = expr.cbegin();
        auto rear = expr.cend();
        std::size_t offset = 0;
        while (front + offset < rear) {
            bool is_success = false;
            for (auto rule : rules) {
                if (std::regex_search(front + offset, rear, m, rule.second, std::regex_constants::match_continuous)) {
                    #ifdef CALCULATOR_DEBUG
                    std::cout << "REGEX MATCHED, TYPE:" << rule.first << " ORIGIN TEXT:" << m[0].str() << std::endl;
                    #endif
                    offset += m[0].str().length();
                    if (rule.first != Token::NONE) {
                        Token token(rule.first, m[0].str());
                        tokens.push_back(token);
                    }
                    is_success = true;
                    break;
                }
            }
            if (!is_success) {
                throw std::logic_error("[LOGIC ERROR] INVALID EXPR: NO RULE MATCHED");
            }
        }
        for (int i = 0;i < tokens.size();i++) {
            if (tokens[i].type == Token::MINUS) {
                if (i == 0 || tokens[i - 1].type == Token::LBRACKET) {
                    tokens[i].type = Token::OPPOSITE;
                }
            } else if (tokens[i].type == Token::PI) {
                if (i > 0 && tokens[i - 1].type == Token::NUMBER) {
                    Token mult(Token::MULTIPLY, "*");
                    tokens.insert(tokens.begin() + i, mult);
                }
            }
        }
        return tokens;
    }
};

class Calculator {
public:
    static double calculate(std::string expr) {
        tokens = Lexer::tokenize(expr);
        return calculate(0, tokens.size() - 1);
    }
private:
    inline static std::vector<Token> tokens;

    inline static const std::map<Token::TokenType, int> priority_map = {
        {Token::PLUS, 0},
        {Token::MINUS, 0},
        {Token::OPPOSITE, INT_MAX},
        {Token::PERCENTAGE, INT_MAX},
        {Token::MULTIPLY, 1},
        {Token::DIVIDE, 1},
        {Token::POWER, 2},
        {Token::COS, INT_MAX},
        {Token::SIN, INT_MAX},
        {Token::TAN, INT_MAX},
        {Token::COT, INT_MAX},
    };

    inline static const std::map<Token::TokenType, int> arity_map = {
        {Token::PERCENTAGE, -1},
        {Token::OPPOSITE, 1},
        {Token::COS, 1},
        {Token::SIN, 1},
        {Token::TAN, 1},
        {Token::COT, 1},
        {Token::PLUS, 2},
        {Token::MINUS, 2},
        {Token::MULTIPLY, 2},
        {Token::DIVIDE, 2},
        {Token::POWER, 2},
    };

    static double calculate(int p, int q) {
        if (p > q) {
            throw std::logic_error("[LOGIC ERROR] INVALID PARSE: INVALID BOUNDARY");
        } else if (p == q) {
            if (tokens[p].type == Token::NUMBER) {
                return stod(tokens[p].origin_text);
            } else if (tokens[p].type == Token::PI) {
                return PAI;
            } else {
                throw std::logic_error("[LOGIC ERROR] INVALID PARSE: SINGLE TOKEN NOT BE A NUMBER");
            }
        } else {
            bool is_legal = true;
            bool has_parentheses = check_parentheses(p, q, is_legal);
            if (!is_legal) {
                throw std::logic_error("[LOGIC ERROR] INVALID PARSE: PARENTHESES NOT PAIRED");
            }
            if (has_parentheses) {
                return calculate(p + 1, q - 1);
            }
            int op_index = find_main_op(p, q);
            Token token = tokens[op_index];
            if (arity_map.at(token.type) == -1) {
                double val = calculate(p, op_index - 1);
                return calculate(token, val);
            } else if (arity_map.at(token.type) == 1) {
                double val = calculate(op_index + 1, q);
                return calculate(token, val);
            } else if (arity_map.at(token.type) == 2) {
                double val_1 = calculate(p, op_index - 1);
                double val_2 = calculate(op_index + 1, q);
                return calculate(token, val_1, val_2);
            }
            throw std::logic_error("[LOGIC ERROR] INVALID PARSE: INVALID OPERATOR");
        }
    }

    static int find_main_op(int p, int q) {
        int min_priority = INT_MAX;
        int op_index = -1;
        int brackets = 0;
        for (int index = p;index <= q;index++) {
            Token::TokenType type = tokens[index].type;
            if (priority_map.find(type) != priority_map.end() && brackets == 0) {
                int priority = priority_map.at(type);
                if (priority <= min_priority) {
                    min_priority = priority;
                    op_index = index;
                }
            } else if (type == Token::LBRACKET) {
                brackets++;
            } else if (type == Token::RBRACKET) {
                brackets--;
            }
        }
        return op_index;
    }

    static bool check_parentheses(int p, int q, bool& is_legal) {
        is_legal = true;
        int brackets = 0;
        for (int i = p;i <= q;i++) {
            if (tokens[i].type == Token::LBRACKET) {
                brackets++;
            } else if (tokens[i].type == Token::RBRACKET) {
                brackets--;
                if (brackets < 0) {
                    is_legal = false;
                }
            }
        }
        if (brackets != 0) {
            is_legal = false;
        }
        return is_legal && tokens[p].type == Token::LBRACKET && tokens[q].type == Token::RBRACKET;
    }

    static double calculate(Token op, double val_1, double val_2 = 0) {
        switch (op.type)
        {
        case Token::OPPOSITE:
            return -val_1;
        case Token::PLUS:
            return val_1 + val_2;
        case Token::MINUS:
            return val_1 - val_2;
        case Token::MULTIPLY:
            return val_1 * val_2;
        case Token::DIVIDE:
            if (fabs(val_2) < EPS) {
                throw std::logic_error("[LOGIC ERROR] INVALID EXPR: DIVIDED BY ZERO");
            }
            return val_1 / val_2;
        case Token::POWER:
            return pow(val_1, val_2);
        case Token::PERCENTAGE:
            return val_1 / 100.0;
        case Token::COS:
            return cos(val_1);
        case Token::SIN:
            return sin(val_1);
        case Token::TAN:
            return tan(val_1);
        case Token::COT:
            return 1 / tan(val_1);
        default:
            throw std::logic_error("[LOGIC ERROR] INVALID EXPR: INVALID OPERATOR");
        }
        return 0;
    }
};

int main() {
    #ifdef CALCULATOR_DEBUG
    std::cout << Calculator::calculate("cos(2π)");
    #endif
    return 0;
}