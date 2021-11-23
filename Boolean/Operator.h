#pragma once

#include "Expression.h"

namespace Boolean {

enum class Operator { NOT = 1, AND = 2, XOR = 3, OR = 4 };

template <std::integral T> class Operation : public Expression<int> {
  friend class ExpressionParser<T>;

  Operator m_operator;

  unsigned m_precedence;
  unsigned m_operand_count;

public:
  Operation(Operator op) : m_operator(op), m_precedence((unsigned)op) {
    m_operand_count = std::numeric_limits<unsigned>::infinity();
    if (m_operator == Operator::NOT) {
      m_operand_count = 1;
    }
  }

  inline Operator operatorType() const { return m_operator; }

  void makeParenthesised() override { m_precedence = 0; };

  unsigned precedence() const override { return m_precedence; }

  unsigned operandCount() const override { return m_operand_count; }

  T eval() const override {
    T ret;
    switch (m_operator) {
    case Operator::NOT:
      assert(this->m_operands.size() == 1);
      ret = ~(this->m_operands.front()->eval());
      return ret;
    case Operator::AND:
      ret = 1;
      for (const auto &expr : this->m_operands) {
        ret &= expr->eval();
      }
      return ret;
    case Operator::OR:
      ret = 0;
      for (const auto &expr : this->m_operands) {
        ret |= expr->eval();
      }
      return ret;
    case Operator::XOR:
      ret = 0;
      for (const auto &expr : this->m_operands) {
        ret ^= expr->eval();
      }
      return ret;
    }
  }

  void print() const override {
    char sym;
    switch (m_operator) {
    case Operator::NOT:
      sym = '~';
      break;
    case Operator::AND:
      sym = '.';
      break;
    case Operator::OR:
      sym = '+';
      break;
    case Operator::XOR:
      sym = '^';
      break;
    }
    std::cout << sym;
    std::cout << "(";
    for (const auto &child : m_operands) {
      child->print();
      std::cout << ',';
    }
    std::cout << "\b)";
  }
};

} // namespace Boolean
