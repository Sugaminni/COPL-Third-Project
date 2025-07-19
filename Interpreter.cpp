#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Structure for type and value
struct Token {
    string type;
    string value;
};

class Interpreter {
private:
    vector<Token> tokens;
    unordered_map<string, int> memory;
    size_t index = 0;

public:
    // Loads tokens from the JSON file
    Interpreter(const string& jsonPath) {
        ifstream file(jsonPath);
        if (!file.is_open()) {
            cerr << "Error: Could not open file: " << jsonPath << endl;
            exit(1);
        }

        if (file.peek() == ifstream::traits_type::eof()) {
            cerr << "Error: JSON file is empty." << endl;
            exit(1);
        }

        json j;
        file >> j;
        for (const auto& item : j) {
            tokens.push_back({ item["type"], item["value"] });
        }
    }

    // Executes the list of tokens as SCL statements
    void run() {
        while (index < tokens.size()) {
            if (tokens[index].value == "set") {
                parseSet();           // Handles set statements
            } else if (tokens[index].value == "display") {
                parseDisplay();       // Handles display statements
            } else if (tokens[index].value == "input") {
                parseInput();         // Handles input statements
            } else if (tokens[index].value == "if") {
                parseIf();            // Handles if statements
            } else if (tokens[index].value == "exit") {
                cout << "Exiting program." << endl;
                break;                // Kills the interpreter
            } else {
                cout << "Unexpected token: " << tokens[index].value << endl;
                ++index;              // Skips unrecognized token
            }
        }
    }

private:
    // Parses a value
    int parseValue() {
        string val = tokens[index++].value;
        if (isdigit(val[0]) || (val[0] == '-' && isdigit(val[1]))) {
            return stoi(val);
        } else if (memory.count(val)) {
            return memory[val];
        } else {
            cout << "Undefined variable: " << val << endl;
            return 0;
        }
    }

    // Parses and evaluates an arithmetic expression
    int parseExpression() {
        int result = parseValue();
        while (index < tokens.size() &&
              (tokens[index].value == "+" || tokens[index].value == "-" ||
               tokens[index].value == "*" || tokens[index].value == "/")) {
            string op = tokens[index++].value;
            int right = parseValue();
            if (op == "+") result += right;
            else if (op == "-") result -= right;
            else if (op == "*") result *= right;
            else if (op == "/") result /= right;
        }
        return result;
    }

    // Parses and executes a set statement (ex. set x = 10 + 5)
    void parseSet() {
        index++; // skips 'set'
        string varName = tokens[index++].value;
        index++; // skips '='
        int value = parseExpression();
        memory[varName] = value;
        cout << "Set " << varName << " = " << value << endl;
    }

    // Parses and executes a display statement (ex. display x)
    void parseDisplay() {
        index++; // skips 'display'
        string varName = tokens[index++].value;
        if (memory.count(varName)) {
            cout << varName << " = " << memory[varName] << endl;
        } else {
            cout << "Variable " << varName << " isn't defined" << endl;
        }
    }

    // Parses and executes an input statement (ex. input x)
    void parseInput() {
        index++; // skips 'input'
        string varName = tokens[index++].value;
        int userInput;
        cout << "Enter value for " << varName << ": ";
        cin >> userInput;
        memory[varName] = userInput;
    }

    // Parses and executes an if statement (ex. if x > 5)
    void parseIf() {
        index++; // skips 'if'
        int left = parseValue();
        string op = tokens[index++].value;
        int right = parseValue();

        bool condition = false;
        if (op == "==") condition = (left == right);
        else if (op == "!=") condition = (left != right);
        else if (op == "<")  condition = (left < right);
        else if (op == ">")  condition = (left > right);
        else if (op == "<=") condition = (left <= right);
        else if (op == ">=") condition = (left >= right);

        if (tokens[index].value == "then") index++; // skips 'then'

        if (condition) {
            cout << "[IF true] running nested statement" << endl;
            run(); // recursively runs the next statements
        } else {
            cout << "[IF false] Skipping nested statement" << endl;
            skipStatement(); // skips over one statement
        }
    }

    // Skips one statement if inside a false if condition
    void skipStatement() {
        if (index >= tokens.size()) return;
        string keyword = tokens[index++].value;
        if (keyword == "set") index += 3;  // set x = val
        else if (keyword == "display") index++;
        else if (keyword == "input") index++;
        else if (keyword == "exit") {}
        else if (keyword == "if") {
            index += 5;
            skipStatement(); // skip nested
        }
    }
};

// Entry point that creates and runs the interpreter
int main() {
    Interpreter interpreter("tokens.json");
    interpreter.run();
    return 0;
}
