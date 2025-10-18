#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cctype>
#include <set>
#include <iomanip>
using namespace std;

struct Label
{
    string name;
    int address;
};

[[noreturn]] void printError(ofstream &fout, const string &msg)
{
    cerr << msg;
    fout << msg;
    fout.close();
    exit(1);
}

bool isNumber(const string &s)
{
    if (s.empty())
        return false;
    char *endptr;
    strtol(s.c_str(), &endptr, 10);
    return (*endptr == '\0'); // เป็น null
}

bool isValidLabelName(const string &s)
{
    if (s.empty() || s.size() > 6)
        return false;
    if (!isalpha(static_cast<unsigned char>(s[0])))
        return false;
    for (char c : s)
        if (!isalnum(static_cast<unsigned char>(c)))
            return false;
    return true;
}

int findLabelAddress(ofstream &fout, const vector<Label> &labels, const string &name)
{
    for (auto &l : labels)
        if (l.name == name)
            return l.address;
    printError(fout, "error: undefined label \"" + name + "\"\n");
    return -1;
}

void checkRegRange(ofstream &fout, int r, const string &which, int line)
{
    if (r < 0 || r > 7)
        printError(fout, "error: register out of range (" + which + "=" + to_string(r) +
                             ") at line " + to_string(line) + "\n");
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        cerr << "usage: assembler <inputfile> <outputfile>\n";
        return 1;
    }

    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    if (!fin || !fout)
    {
        cerr << "error: cannot open file\n";
        return 1;
    }

    vector<string> lines;
    string line;
    while (getline(fin, line))
    {
        if (!line.empty())
        {
            line.erase(0, line.find_first_not_of(" \t\r\n"));
            if (!line.empty())
                lines.push_back(line);
        }
    }

    vector<Label> labels;
    for (int i = 0; i < (int)lines.size(); i++)
    {
        stringstream ss(lines[i]);
        vector<string> tokens;
        string tok;
        while (ss >> tok)
            tokens.push_back(tok);
        if (tokens.empty())
            continue;

        string first = tokens[0];
        if (first != "add" && first != "nand" && first != "lw" && first != "sw" &&
            first != "beq" && first != "jalr" && first != "halt" &&
            first != "noop" && first != ".fill")
        {

            if (!isValidLabelName(first))
                printError(fout, "error: invalid label name \"" + first + "\"\n");
            for (auto &l : labels)
                if (l.name == first)
                    printError(fout, "error: duplicate label \"" + first + "\"\n");
            labels.push_back({first, i});
        }
    }

    // pass 2
    for (int i = 0; i < (int)lines.size(); i++)
    {
        stringstream ss(lines[i]);
        vector<string> tokens;
        string tok;
        while (ss >> tok)
            tokens.push_back(tok);
        if (tokens.empty())
            continue;

        string opcode, arg0, arg1, arg2;
        auto isOpcodeWord = [&](const string &w)
        {
            return (w == "add" || w == "nand" || w == "lw" || w == "sw" ||
                    w == "beq" || w == "jalr" || w == "halt" || w == "noop" || w == ".fill");
        };

        // label
        if (tokens.size() >= 2 && isOpcodeWord(tokens[1]))
        {
            opcode = tokens[1];
            if (tokens.size() > 2)
                arg0 = tokens[2];
            if (tokens.size() > 3)
                arg1 = tokens[3];
            if (tokens.size() > 4)
                arg2 = tokens[4];
        }
        else
        {
            opcode = tokens[0];
            if (tokens.size() > 1)
                arg0 = tokens[1];
            if (tokens.size() > 2)
                arg1 = tokens[2];
            if (tokens.size() > 3)
                arg2 = tokens[3];
        }

        int machineCode = 0, op = -1;
        if (opcode == "add")
            op = 0;
        else if (opcode == "nand")
            op = 1;
        else if (opcode == "lw")
            op = 2;
        else if (opcode == "sw")
            op = 3;
        else if (opcode == "beq")
            op = 4;
        else if (opcode == "jalr")
            op = 5;
        else if (opcode == "halt")
            op = 6;
        else if (opcode == "noop")
            op = 7;
        else if (opcode == ".fill")
            op = -2;
        else
            printError(fout, "error: invalid opcode \"" + opcode + "\" at line " + to_string(i) + "\n");

        if (op == -2)
        { // .fill
            int value;
            if (isNumber(arg0))
                value = stoi(arg0);
            else
                value = findLabelAddress(fout, labels, arg0);
            cout << "(address " << i << "): " << value
                 << " (hex 0x" << hex << uppercase << value << dec << ")\n";
            fout << value << "\n";
            continue;
        }

        int regA = 0, regB = 0, dest = 0, offset = 0;
        try
        {
            if (op <= 1)
            { // R-type
                regA = stoi(arg0);
                regB = stoi(arg1);
                dest = stoi(arg2);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                checkRegRange(fout, dest, "destReg", i);
                machineCode = (op << 22) | (regA << 19) | (regB << 16) | dest;
            }
            else if (op >= 2 && op <= 4)
            { // I-type
                regA = stoi(arg0);
                regB = stoi(arg1);
                if (isNumber(arg2))
                    offset = stoi(arg2);
                else
                {
                    int labelAddr = findLabelAddress(fout, labels, arg2);
                    if (op == 4)
                        offset = labelAddr - (i + 1);
                    else
                        offset = labelAddr;
                }
                if (offset < -32768 || offset > 32767)
                    printError(fout, "error: offsetField out of range at line " + to_string(i) + "\n");
                offset &= 0xFFFF;
                machineCode = (op << 22) | (regA << 19) | (regB << 16) | offset;
            }
            else if (op == 5)
            { // J-type
                regA = stoi(arg0);
                regB = stoi(arg1);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                machineCode = (op << 22) | (regA << 19) | (regB << 16);
            }
            else if (op >= 6)
            { // O-type
                machineCode = (op << 22);
            }
        }
        catch (...)
        {
            printError(fout, "error: invalid operand in line " + to_string(i) + "\n");
        }
        cout << "(address " << i << "): " << machineCode
             << " (hex 0x" << hex << uppercase << machineCode << dec << ")\n";
        fout << machineCode << "\n";
    }

    fout.close();
    return 0;
}
