#pragma once 

#include "Expression.h"

namespace Boolean {
template <std::integral T> class Constant : public Expression<T> {

  unsigned precedence() const override { return 0; }

  unsigned operandCount() const override { return 0; }

  T m_value;

public:
  Constant(T value) : m_value(value) {}

  T value() const { return m_value; }

  T eval() const override { return m_value; }

  void print() const override { std::cout << m_value; }
};
} // namespace Boolean
