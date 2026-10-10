#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

GTEST_PREFIX="$(brew --prefix googletest)"

c++ -std=c++17 -Wall -Wextra -I. -Itests \
  tests/*_test.cpp \
  parser.cpp database.cpp avl_tree.cpp record.cpp exception.cpp \
  -I"${GTEST_PREFIX}/include" \
  -L"${GTEST_PREFIX}/lib" \
  -lgtest_main -lgtest -pthread \
  -o lab2_test

./lab2_test
