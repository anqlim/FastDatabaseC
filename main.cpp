#include <fstream>

#include "database.h"
#include "parser.h"

int main() {
    std::ifstream input("input.txt");
    std::ofstream output("output.txt");
    if (!input.is_open() || !output.is_open()) {
        return 1;
    }

    Database db;
    Parser parser(output, db);
    parser.parse(input);

    return 0;
}
