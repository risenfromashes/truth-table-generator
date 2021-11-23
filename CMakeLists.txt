cmake_minimum_required(VERSION 3.10)

project(truthtablegen)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

add_executable(main 
  Boolean/Expression.h
  Boolean/Variable.h
  Boolean/Constant.h
  Boolean/Operator.h
  Boolean/ExpressionParser.h
  Boolean/Formula.h
  UI/TruthTableGenerator.h
  UI/TruthTableGenerator.cpp
  main.cpp)

target_link_libraries(main PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
