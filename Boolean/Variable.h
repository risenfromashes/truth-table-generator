#pragma once

#include <concepts>

#include "Expression.h"

namespace Boolean {

template <std::integral T> struct VariableProperty {
  const std::string name;
  std::shared_ptr<T> value;

  VariableProperty(std::string_view name)
      : name(name), value(std::make_shared<T>(T{})) {}
  VariableProperty(std::string_view name, const std::shared_ptr<T> &value)
      : name(name), value(value) {}
  VariableProperty(const VariableProperty &other)
      : name(other.name), value(other.value) {}

  T getValue() const { return *value; }
  void setValue(T val) { *value = val; }
  VariableProperty &operator=(T val) { *value = val; }
  operator T() { return *value; }
};

template <std::integral T> class Variable : public Expression<T> {

  VariableProperty<T> m_property;

  unsigned precedence() const override { return 0; }

  unsigned operandCount() const override { return 0; }

public:
  Variable(std::string_view name) : m_property(name) {}

  Variable(const VariableProperty<T> &other)
      : m_property(other.name, other.value) {}

  std::string name() const { return m_property.name; }

  T value() const { return *m_property.value; }

  void setValue(T val) { *m_property.value = val; }

  VariableProperty<T> property() const { return m_property; }

  T eval() const override { return *m_property.value; }

  void print() const override { std::cout << m_property.name; }
};

} // namespace Boolean
