#include "TruthTableGenerator.h"

#include <iostream>
#include <set>

#include <QBoxLayout>
#include <QFont>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>

#include "../Boolean/Formula.h"

TruthTableGenerator::TruthTableGenerator(QWidget *parent) : QWidget(parent) {
  auto vlayout = new QVBoxLayout(this);

  m_input = new QLineEdit();
  m_table = new QTableWidget(0, 0);

  auto input_bar = new QHBoxLayout();
  auto button = new QPushButton("AC");
  button->setMaximumWidth(50);
  input_bar->setSizeConstraint(QLayout::SetMinAndMaxSize);
  m_input->setAlignment(Qt::AlignLeft);
  input_bar->addWidget(m_input, 1);
  input_bar->addWidget(button, 0, Qt::AlignmentFlag::AlignRight);
  auto font = m_input->font();
  font.setPointSize(font.pointSize() + 4);
  m_input->setFont(font);
  button->setFont(font);

  connect(m_input, &QLineEdit::textChanged, this,
          &TruthTableGenerator::inputChanged);

  connect(button, &QPushButton::clicked, this,
          &TruthTableGenerator::clearButtonClicked);

  vlayout->addLayout(input_bar);
  vlayout->addWidget(m_table);
}

std::string trim(std::string str) {
  size_t l = 0, r = str.length();
  while (str[l] == ' ' && l < r)
    l++;
  while (str[r - 1] == ' ' && l < r)
    r--;
  auto ret = str.substr(l, r - l);
  return ret;
}

std::vector<std::string> split(std::string str, char delim) {
  std::vector<std::string> ret;
  while (!str.empty()) {
    size_t next = str.find(delim);
    auto nstr = trim(str.substr(0, next));
    if (!nstr.empty()) {
      ret.push_back(nstr);
    }
    if (next == std::string::npos) {
      break;
    } else {
      str = str.substr(next + 1);
    }
  }
  return ret;
}

void increment(std::vector<bool> &v) {
  for (auto i = v.size() - 1; i >= 0; i--) {
    if (v[i]) {
      v[i] = false;
      return;
    } else {
      v[i] = true;
    }
  }
}

void zero_out(std::vector<bool> &v) {
  for (size_t i = 0; i < v.size(); i++) {
    v[i] = false;
  }
}

void TruthTableGenerator::inputChanged() {
  auto text = m_input->text();

  if (text.isEmpty()) {
    return;
  }

  std::vector<Boolean::Formula<bool>> formulas;
  std::set<std::string> var_set;
  auto exps = split(text.toStdString(), ',');
  for (auto &str : exps) {
    std::cout << str << std::endl;
    formulas.push_back(Boolean::Formula<bool>(str));
    for (auto &var : formulas.back().variables()) {
      var_set.insert(var.name);
    }
  }

  if (formulas.empty())
    return;

  std::vector<std::string> vars(var_set.begin(), var_set.end());

  if (vars.size() >= 31) {
    throw std::runtime_error("Too many variables");
  }

  int n_vars = vars.size();
  int n_formulas = formulas.size();

  int cols = n_vars + n_formulas;
  int rows = 1 << vars.size();

  m_table->setColumnCount(cols);
  m_table->setRowCount(rows);

  QStringList hlabels;
  QStringList vlabels;

  for (const auto &v : vars) {
    hlabels.push_back(QString::fromStdString(v));
  }

  for (const auto &e : exps) {
    hlabels.push_back(QString::fromStdString(e));
  }

  for (int i = 0; i < rows; i++) {
    vlabels.push_back(QString::number(i));
  }

  m_table->setHorizontalHeaderLabels(hlabels);
  m_table->setVerticalHeaderLabels(vlabels);

  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols; c++) {
      bool val;
      if (c < n_vars) {
        val = r & (1 << (n_vars - c - 1));
        for (auto &formula : formulas) {
          if (formula.hasVariable(vars[c])) {
            formula.getVariable(vars[c]).setValue(val);
          }
        }
      } else {
        assert(c - n_vars < n_formulas);
        val = formulas[c - n_vars].eval();
      }

      QTableWidgetItem *item = new QTableWidgetItem(QString::number(val));
      item->setTextAlignment(Qt::AlignHCenter);
      m_table->setItem(r, c, item);
      item->setSelected(val);
    }
  }

  m_table->resizeColumnsToContents();
}

void TruthTableGenerator::clearButtonClicked(bool clicked) { m_input->clear(); }
