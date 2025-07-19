#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include "json.hpp"

using json = nlohmann::json;
using namespace std;

// Represents a token with type and value
struct Token {
    string type;
    string value;
};

class Interpreter {
private:
    vector<Token> tokens;
    unordered_map<string, string> memory;
    size_t index = 0;

public:
    // Loads tokens from the JSON file
    Interpreter(const string& jsonPath) {
        ifstream file(jsonPath);
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
    // Parses and executes a set statement (ex. set x = 10)
    void parseSet() {
        index++; // skips 'set'
        string varName = tokens[index++].value;
        index++; // skips '='
        string value = tokens[index++].value;
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
            cout << "Variable " << varName << " not defined!" << endl;
        }
    }

    // Parses and executes an input statement (ex. input x)
    void parseInput() {
        index++; // skips 'input'
        string varName = tokens[index++].value;
        string userInput;
        cout << "Enter value for " << varName << ": ";
        cin >> userInput;
        memory[varName] = userInput;
    }
};

// Entry point that creates and runs the interpreter
int main() {
    Interpreter interpreter("tokens.json");
    interpreter.run();
    return 0;
}
