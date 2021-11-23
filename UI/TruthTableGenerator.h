#pragma once

#include <QLineEdit>
#include <QTableWidget>
#include <QWidget>

class TruthTableGenerator : public QWidget {
  Q_OBJECT

  QLineEdit *m_input;
  QTableWidget *m_table;

public:
  TruthTableGenerator(QWidget *parent = nullptr);

private slots:
  void clearButtonClicked(bool clicked);
  void inputChanged();
};
