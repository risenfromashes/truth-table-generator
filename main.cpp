#include "Boolean/Formula.h"

#include "UI/TruthTableGenerator.h"

#include <QApplication>
#include <qwindowdefs.h>

int main(int argc, char **argsv) {
  //Boolean::Formula<int> X(argsv[1]);
  //X.printTruthTable();
  QApplication app(argc, argsv);
  
  TruthTableGenerator truthtable_generator;

  truthtable_generator.show();

  app.exec();

}
