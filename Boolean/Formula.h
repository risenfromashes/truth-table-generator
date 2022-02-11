#pragma once

#include <map>

#include "ExpressionParser.h"
#include "Variable.h"

namespace Boolean {
template <std::integral T> class Formula {

  std::map<std::string, VariableProperty<T>> m_var_table;
  std::vector<VariableProperty<T>> m_var_list;

  std::shared_ptr<Expression<T>> m_expression_root;

public:
  const std::vector<VariableProperty<T>> &variables() const {
    return m_var_list;
  }

  void zeroVariables() {
    for (auto &var : m_var_list) {
      var.setValue(0);
    }
  }

  bool increment() {
    for (auto itr = m_var_list.rbegin(); itr != m_var_list.rend(); ++itr) {
      auto &var = *itr;
      if (var.getValue() == 0) {
        var.setValue(1);
        return true;
      } else {
        var.setValue(0);
      }
    }
    return false;
  }

  Formula(std::string_view str) {
    ExpressionParser<T> parser(str, m_var_table);
    m_expression_root = parser.parse();
    for (auto [c, var] : m_var_table) {
      m_var_list.push_back(var);
    }
  }

  VariableProperty<T> getVariable(std::string name) {
    if (!m_var_table.contains(name)) {
      throw std::runtime_error("Variable named '" + std::string(name) +
                               "' not found");
    }
    return m_var_table.at(name);
  }

  bool hasVariable(std::string name) {
    return m_var_table.contains(name);
  }

  T eval(std::initializer_list<std::pair<std::string, T>> vals = {}) {
    for (auto [name, val] : vals) {
      getVariable(name).setValue(val);
    }
    return m_expression_root->eval();
  }

  void printTruthTable() {
    int cols = 0;
    for (auto &var : m_var_list) {
      std::cout << "| " << var.name << " ";
      cols += 3 + var.name.size();
    }
    std::cout << " | = |" << std::endl;
    cols += 6;
    zeroVariables();
    for (int i = 0; i < cols; i++) {
      std::cout << '-';
    }
    std::cout << std::endl;
    do {
      for (const auto &var : m_var_list) {
        std::cout << "| " << var.getValue() << " ";
      }
      std::cout << " | " << eval() << " |" << std::endl;
    } while (increment());
  }

  void print() { m_expression_root->print(); }

  /* Value indicates precedence */
};
} // namespace Boolean
