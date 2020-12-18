#include "libs.hpp"

#include <algorithm>
#include <unordered_map>
#include <map>
#include <unordered_set>
#include <set>
#include <vector>
#include <variant>
#include <memory>

// https://adventofcode.com/2020/day/18

// TODO : improve the code with std::move / rvalue references

using Value = long long int;

enum class Operator { Plus, Times };

class Operation;

class Expression {
private:
    std::variant<Value, std::unique_ptr<Operation>> m_content;
public:
    Value evaluate() const;

    Expression(Value v) : m_content(v) {}
    Expression(const Operation & c);

    Expression(const Expression & other) : m_content(0) { *this = other; }
    Expression & operator=(const Expression & other);

    Expression(Expression && e) : m_content(std::move(e.m_content)) {}
    Expression & operator=(Expression && e) { if (this != &e) { m_content = std::move(e.m_content); } return *this; } 
    ~Expression() = default;
};

class Operation {
private:
    Expression m_lhs;
    Operator m_operator;
    Expression m_rhs;
public:
    Operation(const Expression & left, Operator operator_, const Expression & right)
    : m_lhs(left), m_operator(operator_), m_rhs(right) {}

    [[nodiscard]] Value evaluate() const {
        Value val_lhs = m_lhs.evaluate();
        Value val_rhs = m_rhs.evaluate();

        switch (m_operator) {
            case Operator::Plus : return val_lhs + val_rhs;
            case Operator::Times: return val_lhs * val_rhs;
            default: return 0;
        }
    }
};

// -- Expression missing implementations (depends on Operation)

Expression::Expression(const Operation & c) : m_content(std::make_unique<Operation>(c)) {}

Expression & Expression::operator=(const Expression & other) {
    if (this == &other) return *this;

    struct Copist {
        Expression * e;

        void operator()(Value v) {
            e->m_content = v;
        }

        void operator()(const std::unique_ptr<Operation> & c) {
            e->m_content = std::make_unique<Operation>(*c);
        }
    };

    std::visit(Copist{ this }, other.m_content);

    return *this;
}

Value Expression::evaluate() const {
    struct Evaluator {
        [[nodiscard]] Value operator()(Value v) const { return v; }
        [[nodiscard]] Value operator()(const std::unique_ptr<Operation> & operation) {
            return operation->evaluate();
        }
    };

    return std::visit(Evaluator{}, m_content);
}


// ======================================================

struct NoOperatorPriority {
    [[nodiscard]] Expression operator()(std::vector<Expression> expressions, std::vector<Operator> operators) const {
        Expression expr = expressions[0];

        for (size_t i = 0 ; i != operators.size() ; ++i) {
            expr = Operation(expr, operators[i], expressions[i + 1]);
        }

        return expr;
    }
};

struct PlusPriority {
    [[nodiscard]] Expression operator()(std::vector<Expression> expressions, std::vector<Operator> operators) const {
        for (size_t i = 0 ; i != operators.size() ; ++i) {
            if (operators[i] == Operator::Plus) {
                Operation operation = Operation(expressions[i], operators[i], expressions[i + 1] );
                
                expressions[i] = std::move(operation);

                expressions.erase(expressions.begin() + i + 1);
                operators.erase(operators.begin() + i);

                --i;
            }
        }

        Expression expr = expressions[0];

        for (size_t i = 0 ; i != operators.size() ; ++i) {
            expr = Operation(expr, operators[i], expressions[i + 1]);
        }

        return expr;
    }
};


template<typename ExpressionListResolver>
Expression read_line(const std::string & line, size_t & i) {
    std::vector<Expression> expressions;
    std::vector<Operator> operators;

    while (true) {
        if (i == line.size()) break;

        char c = line[i];
        ++i;

               if (c == ')') { break;
        } else if (c == '(') { expressions.push_back(read_line<ExpressionListResolver>(line, i));
        } else if (c == '+') { operators.push_back(Operator::Plus);
        } else if (c == '*') { operators.push_back(Operator::Times);
        } else {
            // a number
            --i;
            Value acc = 0;
            while (i < line.size() && line[i] >= '0' && line[i] <= '9') {
                acc += (line[i] - '0');
                ++i;
            }
            expressions.push_back(Expression { acc });
        }
    }

    return ExpressionListResolver()(expressions, operators);
}

template<typename ExpressionListResolver>
Expression mapper(const std::string & line) {
    std::string cleaned_line;

    for (char c : line) {
        if (c != ' ') {
            cleaned_line += c;
        }
    }
    size_t i = 0;
    return read_line<ExpressionListResolver>(cleaned_line, i);
}


// ======================================================

template <typename ExpressionListResolver>
test::Value compute_sum_of_results(const std::vector<std::string> & lines) {
    test::Value sum = 0;
    for (const Expression & e : lines_transform::map<Expression>(lines, mapper<ExpressionListResolver>)) {
        sum += e.evaluate();
    }
    return sum;
}

Output day18(const std::vector<std::string> & lines, const DayExtraInfo &) {
    return Output(
        compute_sum_of_results<NoOperatorPriority>(lines),
        compute_sum_of_results<PlusPriority>(lines)
    );
}
