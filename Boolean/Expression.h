#pragma once

#include <algorithm>
#include <cassert>
#include <concepts>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>

namespace Boolean {

template <std::integral T> class Operation;
template <std::integral T> class ExpressionParser;
template <std::integral T> class Formula;

template <std::integral T> class Expression {
  friend class ExpressionParser<T>;

protected:
  std::shared_ptr<Expression<T>> m_parent;
  std::vector<std::shared_ptr<Expression<T>>> m_operands;

  virtual unsigned precedence() const = 0;
  virtual unsigned operandCount() const = 0;

  /* Set precedence to zero, so that it acts like a variable/leaf */
  virtual void makeParenthesised(){};

  inline std::shared_ptr<Expression<T>> parent() const { return m_parent; }

  inline void setParent(const std::shared_ptr<Expression<T>> &parent) {
    m_parent = parent;
  }

  void addOperand(const std::shared_ptr<Expression<T>> &expr) {
    m_operands.push_back(expr);
  }

  void removeOperand(const std::shared_ptr<Expression<T>> &expr) {
    std::erase(m_operands, expr);
  }

public:
  virtual T eval() const = 0;
  virtual void print() const = 0;
};
} // namespace Boolean
