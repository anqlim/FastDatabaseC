#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"

c++ -std=c++17 -Wall -Wextra -o lab2 \
  main.cpp parser.cpp database.cpp avl_tree.cpp record.cpp exception.cpp

echo "built: $(pwd)/lab2"

./lab2
