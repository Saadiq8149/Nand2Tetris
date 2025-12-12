#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <math.h>
#include <map>

using namespace std;

vector<string> minify(vector<string> program);
void create_symbol_table(vector<string> program);
vector<string> remove_pseudo_instructions(vector<string> program);
vector<string> parse(string instruction);
string translate(vector<string> fields);

string trim(const std::string & source);
string decimalToBinary(string decimal);
bool isDigit(string s);

map<string, string> COMP_INSTRUCTION_TO_BINARY {
    // a=0
    {"0",   "0101010"},
    {"1",   "0111111"},
    {"-1",  "0111010"},
    {"D",   "0001100"},
    {"A",   "0110000"},
    {"!D",  "0001101"},
    {"!A",  "0110001"},
    {"-D",  "0001111"},
    {"-A",  "0110011"},
    {"D+1", "0011111"},
    {"A+1", "0110111"},
    {"D-1", "0001110"},
    {"A-1", "0110010"},
    {"D+A", "0000010"},
    {"D-A", "0010011"},
    {"A-D", "0000111"},
    {"D&A", "0000000"},
    {"D|A", "0010101"},
    // a=1
    {"M",   "1110000"},
    {"!M",  "1110001"},
    {"-M",  "1110011"},
    {"M+1", "1110111"},
    {"M-1", "1110010"},
    {"D+M", "1000010"},
    {"D-M", "1010011"},
    {"M-D", "1000111"},
    {"D&M", "1000000"},
    {"D|M", "1010101"}
};
map<string, string> DEST_TO_BINARY {
    {"null","000"},   
    {"M",   "001"},
    {"D",   "010"},
    {"MD",  "011"},
    {"A",   "100"},
    {"AM",  "101"},
    {"AD",  "110"},
    {"AMD", "111"}
};
map<string, string> JUMP_TO_BINARY {
    {"null","000"},  
    {"JGT", "001"},
    {"JEQ", "010"},
    {"JGE", "011"},
    {"JLT", "100"},
    {"JNE", "101"},
    {"JLE", "110"},
    {"JMP", "111"}
};
map<string, string> SYMBOL_TABLE {
    {"R0",     "000000000000000"},
    {"R1",     "000000000000001"},
    {"R2",     "000000000000010"},
    {"R3",     "000000000000011"},
    {"R4",     "000000000000100"},
    {"R5",     "000000000000101"},
    {"R6",     "000000000000110"},
    {"R7",     "000000000000111"},
    {"R8",     "000000000001000"},
    {"R9",     "000000000001001"},
    {"R10",    "000000000001010"},
    {"R11",    "000000000001011"},
    {"R12",    "000000000001100"},
    {"R13",    "000000000001101"},
    {"R14",    "000000000001110"},
    {"R15",    "000000000001111"},

    {"SCREEN", "100000000000000"},
    {"KBD",    "110000000000000"},

    {"SP",     "000000000000000"},
    {"LCL",    "000000000000001"},
    {"ARG",    "000000000000010"},
    {"THIS",   "000000000000011"},
    {"THAT",   "000000000000100"},
};

int main(int argc, char* argv[])
{
    vector<string> assembledProgram;

    if (argc != 2) {
        cout << "Usage: " << argv[0] << " file.asm" << endl;
        return 1;
    }

    vector<string> program;

    // Read the .asm file into a vector of strings
    string line;
    ifstream file(argv[1]);
    while (getline(file, line)) {
        program.push_back(line);
    }
    file.close();

    // Minify the program by removing comments and whitespace
    program = minify(program);

    // Construct the symbol table for user defined lables and variables
    create_symbol_table(program);

    for (auto it = SYMBOL_TABLE.begin(); it != SYMBOL_TABLE.end(); it++) {
        cout << it->first << " " << it->second << endl;
    }

    // Remove pseudo instructions (LABELS)
    program = remove_pseudo_instructions(program);
    
    // Parse the program into a vector of tokens
    for (int i = 0; i < program.size(); i++) {
        vector<string> fields = parse(program[i]);
        string binary = translate(fields);
        assembledProgram.push_back(binary);
    }

    // Write the assembled program to a file
    string outFileName = argv[1];
    outFileName.replace(outFileName.end() - 4, outFileName.end(), ".hack");
    ofstream outFile(outFileName);
    for (int i = 0; i < assembledProgram.size(); i++) {
        outFile << assembledProgram[i];
        if (i != assembledProgram.size() - 1) {
            outFile << endl;
        }
    }
    outFile.close();

    return 0;
}

vector<string> minify(vector<string> program) { 
    for (int i = 0; i < program.size(); i++) {
        program[i] = trim(program[i]);
        if (program[i].empty() || (program[i][0] == '/' && program[i][1] == '/')) {
            program.erase(program.begin() + i);
            i--;
            continue;
        }
    }
    return program;
}

vector<string> parse(string instruction) {
    vector<string> fields;
    string token;
    for (int i = 0; i < instruction.size(); i++) {
        if (instruction[i] == '@') {
            fields.push_back("@");
            token = "";
            continue;
        } else if (instruction[i] == '=') {
            fields.push_back(token);
            fields.push_back("=");
            token = "";
            continue;
        } else if (instruction[i] == ';') {
            fields.push_back(token);
            fields.push_back(";");
            token = "";
            continue;
        }
        token += instruction[i];
    }
    fields.push_back(token);
    return fields;
}

string translate(vector<string> fields) {
    string binary = "";
    if (fields[0] == "@") {
        // A instruction
        binary += "0";

        // Replaces the symbol with its binary representation (address)
        if (SYMBOL_TABLE.find(fields[1]) != SYMBOL_TABLE.end()) {
            binary += SYMBOL_TABLE[fields[1]];
        } else {
            binary += decimalToBinary(fields[1]);
        }

    } else {
        // C instruction
        binary += "111";
        string dest = "null", comp = "null", jump = "null";
        if (fields[1] == "=") {
            dest = fields[0];
            comp = fields[2];
        }
        if (fields[1] == ";") {
            comp = fields[0];
            jump = fields[2];
        } else if (fields[3] == ";") {
            comp = fields[2];
            jump = fields[4];
        }

        comp = COMP_INSTRUCTION_TO_BINARY[comp];
        dest = DEST_TO_BINARY[dest];
        jump = JUMP_TO_BINARY[jump];

        binary += (comp + dest + jump);
    }
    return binary;
}

string trim(const std::string & source) {
    std::string s(source);
    s.erase(0,s.find_first_not_of(" \n\r\t"));
    s.erase(s.find_last_not_of(" \n\r\t")+1);
    return s;
}

string decimalToBinary(string decimal) {
    string binary = "000000000000000";
    int num = stoi(decimal);
    for (int i = 14; i >= 0; i--) {
        binary[14 - i] = (num >> i & 1) ? '1' : '0';
    }
    return binary;
}

bool isDigit(string s) {
    for (int i = 0; i < s.size(); i++) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

void create_symbol_table(vector<string> program) {
    int instrNum = 0; 
    int address = 16;
    
    // Labels
    for (int i = 0; i < program.size(); i++) {
        if (program[i][0] == '(') {
            string label = program[i].substr(1, program[i].size() - 2);
            SYMBOL_TABLE[label] = decimalToBinary(to_string(instrNum));
        } else {
            instrNum++;
        }
    }

    // Variables
    for (int i = 0; i < program.size(); i++) {
        if (program[i][0] == '@') {
            string variable = program[i].substr(1, program[i].size() - 1);
            if (SYMBOL_TABLE.find(variable) == SYMBOL_TABLE.end() && !isDigit(variable)) {
                SYMBOL_TABLE[variable] = decimalToBinary(to_string(address));
                address ++;
            }
        } 
    }
}

vector<string> remove_pseudo_instructions(vector<string> program) {
    vector<string> newProgram;
    for (int i = 0; i < program.size(); i++) {
        if (program[i][0] != '(') {
            newProgram.push_back(program[i]);
        }
    }
    return newProgram;
}