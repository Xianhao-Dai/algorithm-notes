#include<string>
#include<vector>
#include<map>
#include<stack>

#include<regex>

#define CALCULATOR_DEBUG

#ifdef CALCULATOR_DEBUG
#include<iostream>
#endif

struct Token {
    enum TokenType {
        NONE = 0,
        NUMBER,
        PLUS,
        MINUS,
        MULTIPLY,
        DIVIDE,
        POWER,
        LBRACKET,
        RBRACKET,
    } type;
    std::string originText;

    explicit Token(TokenType type, std::string originText) : type(type), originText(originText){
    }
};

class Lexer {
public:
    static std::vector<Token> tokenize(std::string expr) {
        std::vector<Token> tokens;
        static const std::vector<std::pair<Token::TokenType, std::regex>> rules = {
            {Token::NUMBER, std::regex(R"(\d+(?:\.\d+)?)")},
            {Token::PLUS,     std::regex(R"(\+)")},
            {Token::MINUS,     std::regex(R"(-)")},
            {Token::MULTIPLY,     std::regex(R"(\*)")},
            {Token::DIVIDE,     std::regex(R"(/)")},
            {Token::POWER,     std::regex(R"(\^)")},
            {Token::LBRACKET, std::regex(R"(\()")},
            {Token::RBRACKET, std::regex(R"(\))")}
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
                    Token token(rule.first, m[0].str());
                    tokens.push_back(token);
                    is_success = true;
                    break;
                }
            }
            if (!is_success) {
                throw std::invalid_argument("INVALID EXPR - NO RULE MATCHED");
            }
        }
        return tokens;
    }
};

class Calculator {
public:
    static double calculate(std::string expr) {
        std::vector<Token> tokens = Lexer::tokenize(expr);
        std::stack<Token> operands;
        std::stack<Token> operators;
        Token eof(Token::NONE, "");
        operators.push(eof);
        for (auto token : tokens) {
            if (token.type == Token::NUMBER) {
                operands.push(token);
            } else {
                Token op = operators.top();
                if (isp_map.at(op.type) > icp_map.at(token.type)) {
                    while (isp_map.at(op.type) > icp_map.at(token.type) && !operators.empty()) {
                        Token operand_1 = operands.top();
                        operands.pop();
                        Token operand_2 = operands.top();
                        operands.pop();
                        operators.pop();
                        Token res = calculate(operand_2, operand_1, op);
                        operands.push(res);
                        #ifdef CALCULATOR_DEBUG
                        std::cout << "OPERANDS CALCULATED, OPERAND_1:" << operand_1.originText
                        << " OPERAND_2:" << operand_2.originText
                        << " OP:" << op.originText
                        << " RES:" << res.originText 
                        << std::endl;
                        #endif
                        op = operators.top();
                    }
                    if (isp_map.at(op.type) == icp_map.at(token.type)) {
                        operators.pop();
                    } else {
                        operators.push(token);
                    }
                } else if (isp_map.at(op.type) == icp_map.at(token.type)) {
                    operators.pop();
                } else {
                    operators.push(token);
                }
            }
        }
        while (operators.top().type != Token::NONE) {
            Token operand_1 = operands.top();
            operands.pop();
            Token operand_2 = operands.top();
            operands.pop();
            Token op = operators.top();
            operators.pop();
            Token res = calculate(operand_2, operand_1, op);
            operands.push(res);
            #ifdef CALCULATOR_DEBUG
            std::cout << "OPERANDS CALCULATED, OPERAND_1:" << operand_1.originText
            << " OPERAND_2:" << operand_2.originText
            << " OP:" << op.originText
            << " RES:" << res.originText 
            << std::endl;
            #endif
        }
        return stod(operands.top().originText);
    }
private:
    static Token calculate(Token operand_1, Token operand_2, Token op) {
        double a = stod(operand_1.originText);
        double b = stod(operand_2.originText);
        double res = 0;
        switch (op.type)
        {
        case Token::PLUS:
            res = a + b;
            break;
        case Token::MINUS:
            res = a - b;
            break;
        case Token::MULTIPLY:
            res = a * b;
            break;
        case Token::DIVIDE:
            res = a / b;
            break;
        case Token::POWER:
            res = pow(a, b);
            break;
        default:
            break;
        }
        return Token(Token::NUMBER, std::to_string(res));
    }

    enum PRIORITY {
        PRIORITY_MIN = INT_MIN,
        PRIORITY_0 = 0,
        PRIORITY_1 = 1,
        PRIORITY_2 = 2,
        PRIORITY_3 = 3,
        PRIORITY_4 = 4,
        PRIORITY_5 = 5,
        PRIORITY_6 = 6,
        PRIORITY_7 = 7,
        PRIORITY_MAX = INT_MAX,
    } type;

    inline static const std::map<Token::TokenType, PRIORITY> icp_map = {
        {Token::NONE, PRIORITY_MIN},
        {Token::PLUS, PRIORITY_1},
        {Token::MINUS, PRIORITY_1},
        {Token::MULTIPLY, PRIORITY_3},
        {Token::DIVIDE, PRIORITY_3},
        {Token::POWER, PRIORITY_5},
        {Token::LBRACKET, PRIORITY_MAX},
        {Token::RBRACKET, PRIORITY_0},
    };

    inline static const std::map<Token::TokenType, PRIORITY> isp_map = {
        {Token::NONE, PRIORITY_MIN},
        {Token::PLUS, PRIORITY_2},
        {Token::MINUS, PRIORITY_2},
        {Token::MULTIPLY, PRIORITY_4},
        {Token::DIVIDE, PRIORITY_4},
        {Token::POWER, PRIORITY_6},
        {Token::LBRACKET, PRIORITY_0},
        {Token::RBRACKET, PRIORITY_MAX},
    };
};

int main() {
    #ifdef CALCULATOR_DEBUG
    std::cout << Calculator::calculate("(1/0.32+2.5*8.3)^2");
    #endif
    return 0;
}