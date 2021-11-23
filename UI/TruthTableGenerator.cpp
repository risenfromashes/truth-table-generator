#include "TruthTableGenerator.h"

#include <iostream>

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

  vlayout->addLayout(input_bar);
  vlayout->addWidget(m_table);
}

void TruthTableGenerator::inputChanged() {
  auto text = m_input->text();
  Boolean::Formula<int> formula(text.toStdString());
  const auto &vars = formula.variables();

  if (vars.size() >= 31) {
    throw std::runtime_error("Too many variables");
  }

  int cols = vars.size() + 1;
  int rows = 1 << vars.size();

  m_table->setColumnCount(cols);
  m_table->setRowCount(rows);

  QStringList hlabels;
  QStringList vlabels;

  for (const auto &v : vars) {
    hlabels.push_back(QString::fromStdString(v.name));
  }
  hlabels.push_back(text);

  for (int i = 0; i < rows; i++) {
    vlabels.push_back(QString::number(i));
  }

  m_table->setHorizontalHeaderLabels(hlabels);
  m_table->setVerticalHeaderLabels(vlabels);

  formula.zeroVariables();
  for (int r = 0; r < rows; r++) {
    for (int c = 0; c < cols - 1; c++) {
      auto *item = new QTableWidgetItem(QString::number(vars[c].getValue()));
      m_table->setItem(r, c, item);
    }
    auto *item = new QTableWidgetItem(QString::number(formula.eval()));
    m_table->setItem(r, cols - 1, item);
    formula.increment();
  }
}
