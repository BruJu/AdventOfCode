#include "../advent_of_code.hpp"

#include <vector>
#include <variant>
#include <memory>

// https://adventofcode.com/2020/day/18

using Value = long long int;

enum class Operator { Plus, Times };

class Operation;

class Expression {
private:
    std::variant<Value, std::unique_ptr<Operation>> m_content;
public:
    [[nodiscard]] Value evaluate() const;

    Expression(Value v) : m_content(v) {}
    Expression(Operation c);
};

class Operation {
private:
    Expression m_lhs;
    Operator m_operator;
    Expression m_rhs;
public:
    Operation(Expression left, Operator operator_, Expression right)
    : m_lhs(std::move(left)), m_operator(operator_), m_rhs(std::move(right)) {}

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

Expression::Expression(Operation c) : m_content(std::make_unique<Operation>(std::move(c))) {}

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

struct ExpressionComputer {
    std::vector<Expression> expressions;
    std::vector<Operator>   operators;
    std::vector<size_t>     stack_pointers;
    
    template<typename ExpressionListResolver>
    Expression terminate() {
        // assert(stack_pointers.empty());
        ExpressionListResolver()(*this, 0);
        return std::move(expressions[0]);
    }

    void open_parenthesis() {
        stack_pointers.push_back(expressions.size());
    }

    template<typename ExpressionListResolver>
    void close_parenthesis() {
        ExpressionListResolver()(*this, stack_pointers.back());
        stack_pointers.pop_back();
    }

    [[nodiscard]] size_t size() const noexcept { return expressions.size(); }

    template <typename Predicate>
    void make_operation_from_if(size_t sp, Predicate predicate) {
        while (size() != sp + 1) {
            if (predicate(operators[sp])) {
                make_operation(sp);
            } else {
                ++sp;
            }
        }
    }
    
    void make_operation(size_t position) {
        expressions[position] = Operation(
            std::move(expressions[position]),
            operators[position],
            std::move(expressions[position + 1])
        );

        operators  .erase(operators  .begin() + position);
        expressions.erase(expressions.begin() + position + 1);
    }
};

struct NoOperatorPriority {
    void operator()(ExpressionComputer & computer, size_t sp) const {
        computer.make_operation_from_if(sp, [](Operator) { return true; });
    }
};

struct PlusPriority {
    void operator()(ExpressionComputer & computer, size_t sp) const {
        computer.make_operation_from_if(sp, [](Operator op) { return op == Operator::Plus; });
        computer.make_operation_from_if(sp, [](Operator)    { return true; });
    }
};


template<typename ExpressionListResolver>
Expression read_line(std::string_view line) {
    ExpressionComputer s;
    size_t i = 0;

    while (true) {
        if (i == line.size()) break;

        char c = line[i];
        ++i;

               if (c == ')') { s.close_parenthesis<ExpressionListResolver>();
        } else if (c == '(') { s.open_parenthesis();
        } else if (c == '+') { s.operators.push_back(Operator::Plus);
        } else if (c == '*') { s.operators.push_back(Operator::Times);
        } else {
            // a number
            --i;
            Value acc = 0;
            while (i < line.size() && line[i] >= '0' && line[i] <= '9') {
                acc += (line[i] - '0');
                ++i;
            }
            s.expressions.push_back(Expression { acc });
        }
    }

    return s.terminate<ExpressionListResolver>();
}

template<typename ExpressionListResolver>
Expression mapper(const std::string & line) {
    std::string cleaned_line;

    for (char c : line) {
        if (c != ' ') {
            cleaned_line += c;
        }
    }

    return read_line<ExpressionListResolver>(cleaned_line);
}


// ======================================================

template <typename ExpressionListResolver>
Value compute_sum_of_results(const std::vector<std::string> & lines) {
    Value sum = 0;
    for (const Expression & e : lines_transform::map<Expression>(lines, mapper<ExpressionListResolver>)) {
        sum += e.evaluate();
    }
    return sum;
}

Output day_2020_18(const std::vector<std::string> & lines, const DayExtraInfo &) {
    return Output(
        compute_sum_of_results<NoOperatorPriority>(lines),
        compute_sum_of_results<PlusPriority>(lines)
    );
}
