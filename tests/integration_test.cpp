#include <fstream>
#include <sstream>
#include <string>

#include <gtest/gtest.h>

#include "database.h"
#include "parser.h"

namespace {

void runScenario(const std::string& inputPath, const std::string& expectedPath) {
    std::ifstream input(inputPath);
    ASSERT_TRUE(input.is_open()) << inputPath;

    std::ifstream expectedIn(expectedPath);
    ASSERT_TRUE(expectedIn.is_open()) << expectedPath;
    std::ostringstream expectedBuffer;
    expectedBuffer << expectedIn.rdbuf();

    std::ostringstream output;
    Database db;
    Parser parser(output, db);
    parser.parse(input);

    EXPECT_EQ(output.str(), expectedBuffer.str()) << inputPath;
}

}  // namespace

TEST(Lab2Integration, Input1) {
    runScenario("tests/input1.txt", "tests/output1.txt");
}

TEST(Lab2Integration, Input2) {
    runScenario("tests/input2.txt", "tests/output2.txt");
}

TEST(Lab2Integration, Input3) {
    runScenario("tests/input3.txt", "tests/output3.txt");
}

TEST(Lab2Integration, Input4) {
    runScenario("tests/input4.txt", "tests/output4.txt");
}

TEST(Lab2Integration, Input5) {
    runScenario("tests/input5.txt", "tests/output5.txt");
}

TEST(Lab2Integration, Input6) {
    runScenario("tests/input6.txt", "tests/output6.txt");
}

TEST(Lab2Integration, InputFull) {
    runScenario("tests/input.txt", "tests/output.txt");
}
