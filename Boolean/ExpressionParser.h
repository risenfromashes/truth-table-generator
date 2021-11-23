#pragma once

#include <map>

#include "Expression.h"
#include "Variable.h"
#include "Operator.h"
#include "Constant.h"

namespace Boolean {

template <std::integral T> class Formula;

template <std::integral T> class ExpressionParser {

  friend class Formula<T>;

  std::string_view &m_str;
  std::map<std::string, VariableProperty<T>> &m_var_table;

  std::shared_ptr<Expression<T>> m_root;
  std::shared_ptr<Expression<T>> m_curr;

  ExpressionParser(std::string_view &str,
                   std::map<std::string, VariableProperty<T>> &var_table)
      : m_str(str), m_var_table(var_table) {}

  /* variable name or apostrophe (') or parenthesis followed by another variable
   * name or parenthesis is implicitly AND */
  bool m_is_implicit_and = false;

  std::shared_ptr<Expression<T>> root() const { return m_root; }

  void consumeSpace() {
    size_t offset = 0;
    while (offset < m_str.length() &&
           (m_str[offset] == ' ' || m_str[offset] == '\t' ||
            m_str[offset] == '\n')) {
      offset++;
    }
    m_str.remove_prefix(offset);
  }

  void consumeConstant() {
    size_t offset = 0;
    while (offset < m_str.length() && std::isdigit(m_str[offset])) {
      offset++;
    }

    T val = std::stoull(std::string(m_str.substr(0, offset)));
    auto cnst = std::make_shared<Constant<T>>(val);

    consumeOperand(cnst);
    m_str.remove_prefix(offset);
  }

  void consumeVariable() {
    size_t offset = 1;
    // consume possible number subscript
    while (offset < m_str.length() && std::isdigit(m_str[offset])) {
      offset++;
    }
    // a variable name
    auto name = std::string(m_str.substr(0, offset));
    std::shared_ptr<Variable<T>> new_var;

    // std::cout << "name: '" << name << "'" << std::endl
    //<< "Contains 1: " << m_var_table.contains(name) << std::endl;

    if (!m_var_table.contains(name)) {
      new_var = std::make_shared<Variable<T>>(name);
      auto [itr, inserted] = m_var_table.emplace(
          std::string_view(new_var->property().name), new_var->property());
      // if (!inserted) {
      // std::cout << "name: '" << name << "'" << std::endl
      //<< "property.name: '" << new_var->property().name << "'"
      //<< std::endl
      //<< "Contains 1: " << m_var_table.contains(name) << std::endl
      //<< "Contains 2: "
      //<< m_var_table.contains(new_var->property().name)
      //<< std::endl;
      //}
      assert(inserted);
    } else {
      new_var = std::make_shared<Variable<T>>(m_var_table.at(name));
    }

    consumeOperand(new_var);
    // one character will be consumed later
    m_str.remove_prefix(offset - 1);
  }

  void consumeOperation(Operator op_type) {
    unsigned precedence = static_cast<unsigned>(op_type);
    // non-var must have > 0 precedence
    assert(precedence > 0);
    if (m_curr) {
      // If operation matches then next operand can just be added to it
      if (precedence != m_curr->precedence()) {
        // if operator doesn't match then add it as the parent (of parent of ..)
        // or child of the current operator according precedence
        if (precedence < m_curr->precedence()) {
          // less precedence; add as child
          auto new_op = std::make_shared<Operation<T>>(op_type);
          m_curr->addOperand(new_op);
          new_op->setParent(m_curr);
          m_curr = new_op;
        } else {
          // higher precedence; ascend until greater precedence parent or root
          // is found
          while (m_curr->parent() &&
                 precedence >= m_curr->parent()->precedence()) {
            m_curr = m_curr->parent();
          }
          // if precendce is same with this parent then we have just set that as
          // the current operator, nothing to do
          if (precedence != m_curr->precedence()) {
            // we now need to create new node
            auto new_op = std::make_shared<Operation<T>>(op_type);
            if (m_curr->parent()) {
              // precedence between parent and current node
              // insert new node in between
              assert(precedence < m_curr->parent()->precedence() &&
                     precedence > m_curr->precedence());
              m_curr->parent()->removeOperand(m_curr);
              new_op->addOperand(m_curr);
              m_curr->parent()->addOperand(new_op);
              new_op->setParent(m_curr->parent());
              m_curr->setParent(new_op);
              m_curr = new_op;
            } else {
              // this is the new root
              assert(precedence > m_curr->precedence());
              m_curr->setParent(new_op);
              new_op->addOperand(m_curr);
              m_curr = new_op;
              m_root = new_op;
            }
          }
        }
      }
    } else {
      // first node
      m_curr = std::make_shared<Operation<T>>(op_type);
      m_root = m_curr;
    }
  }

  void consumeOperand(const std::shared_ptr<Expression<T>> &ex) {
    if (m_curr) {
      // cannot have two consecutive variables
      assert(m_curr->precedence() > 0);
      m_curr->addOperand(ex);
      ex->setParent(m_curr);
      m_curr = ex;
    } else {
      // first node
      assert(!m_root);
      m_curr = ex;
      m_root = ex;
    }
  }

  void consumeCharacter() {
    char c = m_str.front();

    if (std::isalpha(c)) {
      if (m_is_implicit_and) {
        consumeOperation(Operator::AND);
      }
      consumeVariable();
      m_is_implicit_and = true;
    } else if (std::isdigit(c)) {
      consumeConstant();
      m_is_implicit_and = true;
    } else if (c == '\'') {
      consumeOperation(Operator::NOT);
      m_is_implicit_and = true;
    } else if (c == '+') {
      consumeOperation(Operator::OR);
      m_is_implicit_and = false;
    } else if (c == '.') {
      consumeOperation(Operator::AND);
      m_is_implicit_and = false;
    } else if (c == '^') {
      consumeOperation(Operator::XOR);
      m_is_implicit_and = false;
    } else if (c == '(') {
      if (m_is_implicit_and) {
        consumeOperation(Operator::AND);
      }
      // create new parser to parse the content of parenthesis
      m_str.remove_prefix(1);
      ExpressionParser<T> new_parser(m_str, m_var_table);
      auto ex = new_parser.parse();
      ex->makeParenthesised();
      consumeOperand(ex);
      m_is_implicit_and = true;
    }
    m_str.remove_prefix(1);
  }

  std::shared_ptr<Expression<T>> parse() {
    while (!m_str.empty() && m_str.front() != ')') {
      consumeSpace();
      consumeCharacter();
      consumeSpace();
    }
    return root();
  }
};
} // namespace Boolean
