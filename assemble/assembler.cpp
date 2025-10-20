#include <iostream> //ใช้สำหรับการรับ–ส่งข้อมูลผ่านconsole
#include <fstream>  //ใช้สำหรับเปิดอ่านไฟล์
#include <sstream>  //ใช้สำหรับจัดการ string ที่ต้องการอ่าน/เขียน
#include <vector>   //ใช้สำหรับเก็บข้อมูลหลายค่า
#include <string>   //ใช้สำหรับการใช้งาน stringstream
#include <cstdlib>  //รวมฟังก์ชันของภาษา C++ เช่น strtol(), exit()
#include <cctype>   //ใช้สำหรับฟังก์ชันตรวจสอบ/แปลงตัวอักษร
#include <set>      //ใช้สำหรับโครงสร้างข้อมูลแบบ set (ไม่เก็บค่าซ้ำ)
#include <iomanip>  //ใช้สำหรับจัดรูปแบบการแสดงผล เช่น setw(), setfill(), hex, dec
using namespace std;

// สร้างโครงสร้างข้อมูลชื่อ label ที่จะเก็บชื่อ และที่อยู่ของ labelนั้นๆ
struct Label
{
    string name;
    int address;
};

// ใช้สำหรับprint ข้อความ error โดยชี้ไปยังไฟล์ที่เปิดอ่านอยู่(font) และเขียนข้อความ error(msg)
[[noreturn]] void printError(ofstream &fout, const string &msg) // noreturn ฟังก์ชันนี้จะไม่กลับมาเรียกซ้ำอีก คือจะจบโปรแกรมหรือ exit ไปเลย
{
    cerr << msg;
    fout << msg;
    fout.close();
    exit(1);
}

// ใช้สำหรับเช็คว่าstringที่รับมาเป็นตัวเลขหรือไม่
// ก่อนอื่นจะเช็คค่าที่รับมาว่า ว่างหรือไม่ ถ้าว่าจะถือว่าไม่เป็นตัวเลข จากนั้นจะทำการแปลง string ให้เป็น long int
// ถ้าเป็นตัวเลขทั้งหมด endptr จะชี้ไปยัง \0 แต่ถ้าเจอที่ไม่ใช่ตัวเลข จะหยุด และ endptr จะชี้ไปตรงนั้น
bool isNumber(const string &s)
{
    if (s.empty())
        return false;
    char *endptr;
    strtol(s.c_str(), &endptr, 10); // 10ตัวสุดท้ายคือเลข ฐาน10
    return (*endptr == '\0');       // เป็น null
}

// ใช้สำหรับตรงสอบความถูกต้องของ label ตามเงื่อนไขที่โจทย์กำหนด
bool isValidLabelName(const string &s)
{
    if (s.empty() || s.size() > 6) // มีความยาวน้อยกว่า 6 ตัวอักษร และจะต้องไม่เว้นว่าง
        return false;
    if (!isalpha(static_cast<unsigned char>(s[0]))) // จะต้องขึ้นต้นด้วยตัวหนังสือเท่านั้น
        return false;
    for (char c : s) // สุดท้ายจะต้องไม่มีอักษรพิเศษอยู่
        if (!isalnum(static_cast<unsigned char>(c)))
            return false;
    return true;
}

// ใช้สำหรับหาที่อยู่ของ label
// โดยจะค้นหา label ที่ชื่อ name จากรายชื่อ labels แล้วคืนค่า address ของมัน
int findLabelAddress(ofstream &fout, const vector<Label> &labels, const string &name)
{
    for (auto &l : labels)
        if (l.name == name)
            return l.address;
    printError(fout, "error: undefined label \"" + name + "\"\n");
    return -1;
}

// ใช้ตรวจสอบว่าหมายเลข register อยู่ในช่วง 0–7 หรือไม่
// ถ้าถูกต้องจะทำงานต่อปกติ แต่หากไม่ถูกต้อง จะเสดง error และหยุดการทำงานของโปรแกรม
void checkRegRange(ofstream &fout, int r, const string &which, int line)
{
    if (r < 0 || r > 7)
        printError(fout, "error: register out of range (" + which + "=" + to_string(r) +
                             ") at line " + to_string(line) + "\n");
}

// main program
int main(int argc, char *argv[])
{
    // ใช้ตรวจสอบว่ามีการป้อน argument ครบหรือยัง ตอนรันโปรแกรมจาก command line หากป้อนไม่ครบ 3 ตัว จะแสดง error
    if (argc != 3)
    {
        cerr << "usage: assembler <inputfile> <outputfile>\n";
        return 1;
    }

    // ใช้ตรวจสอบไฟล์ที่อ่าน และเขียนว่ามีปัญหาหรือไม่ หากมีจะแสดง error
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    if (!fin || !fout)
    {
        cerr << "error: cannot open file\n";
        return 1;
    }

    // ใช้สำหรับตัดแท็บข้างหน้า เว้นวรรค และบรรทัดว่างทิ้ง
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

    // pass 1
    // ทำหน้าที่เก็บตำแหน่งของ Label ทั้งหมด เพื่อให้ในรอบถัดไป(pass 2) โปรแกรมสามารถรู้ว่า label แต่ละตัวอยู่บรรทัดไหน แล้วแทนเป็น address ได้
    vector<Label> labels;
    for (int i = 0; i < (int)lines.size(); i++) // i แทนตำแหน่ง address
    {
        stringstream ss(lines[i]); // ใช้ stringstream เพื่อ “ตัดคำ” ในบรรทัดนั้นออกเป็น tokens(คำย่อยๆ)
        vector<string> tokens;
        string tok;
        while (ss >> tok)
            tokens.push_back(tok);
        if (tokens.empty())
            continue;

        string first = tokens[0]; // tokens[o] จะต้องไม่เป็น opcode
        if (first != "add" && first != "nand" && first != "lw" && first != "sw" &&
            first != "beq" && first != "jalr" && first != "halt" &&
            first != "noop" && first != ".fill")
        {

            if (!isValidLabelName(first)) // เช็กว่าชื่อ label ถูกต้องไหม
                printError(fout, "error: invalid label name \"" + first + "\"\n");
            for (auto &l : labels) // เช็คว่ามีการใช้ label ซ้ำมั้ย
                if (l.name == first)
                    printError(fout, "error: duplicate label \"" + first + "\"\n");
            labels.push_back({first, i}); // ถ้าถูกต้องทั้งหมด ให้บันทึกลงใน vector
        }
    }

    // pass 2
    // ทำหน้าที่ในการแปลง assembly ที่เก็บไว้ใน lines ให้กลายเป็น เลข machine code แล้วเขียนผลลัพธ์ออกไฟล์ .txt
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

        // เป็น lambda function (ฟังก์ชันที่ไม่มีชื่อ) ที่เช็คว่าคำเป็น opcode หรือไม่ เพราะบางบรรทัดมี label อยู่ก่อน opcode
        auto isOpcodeWord = [&](const string &w)
        {
            return (w == "add" || w == "nand" || w == "lw" || w == "sw" ||
                    w == "beq" || w == "jalr" || w == "halt" || w == "noop" || w == ".fill");
        };

        // label
        if (tokens.size() >= 2 && isOpcodeWord(tokens[1])) // ถ้าคำที่สองเป็น opcode แสดงว่าคำแรกคือ label
        {
            opcode = tokens[1]; // คำที่ 2 เป็น opcode
            if (tokens.size() > 2)
                arg0 = tokens[2];
            if (tokens.size() > 3)
                arg1 = tokens[3];
            if (tokens.size() > 4)
                arg2 = tokens[4];
        }
        else
        {
            opcode = tokens[0]; // คำที่แรก เป็น opcode เลย
            if (tokens.size() > 1)
                arg0 = tokens[1];
            if (tokens.size() > 2)
                arg1 = tokens[2];
            if (tokens.size() > 3)
                arg2 = tokens[3];
        }

        // แปลงชื่อ opcode เป็นตัวเลข
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

        if (op == -2) //.fill ใช้เก็บค่าคงที่ (constant) หรือ address ของ label
        {
            int value;
            if (isNumber(arg0)) // ถ้าเป็นตัวเลข จะแปลง string ให้เป็น int
                value = stoi(arg0);
            else
                value = findLabelAddress(fout, labels, arg0); // ถ้าเป็น label จะหา address จาก labels
            cout << "(address " << i << "): " << value
                 << " (hex 0x" << hex << uppercase << value << dec << ")\n";
            fout << value << "\n";
            continue;
        }

        int regA = 0, regB = 0, dest = 0, offset = 0;
        try
        {
            if (op <= 1) // R-type (add, nand)
            {
                regA = stoi(arg0);
                regB = stoi(arg1);
                dest = stoi(arg2);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                checkRegRange(fout, dest, "destReg", i);
                machineCode = (op << 22) | (regA << 19) | (regB << 16) | dest; // ใช้บิต 22–24 เป็น opcode, 19–21 เป็น regA, 16–18 เป็น regB, 0–2 เป็น dest
            }
            else if (op >= 2 && op <= 4) // I-type (lw, sw, beq)
            {
                regA = stoi(arg0);
                regB = stoi(arg1);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                if (isNumber(arg2))
                    offset = stoi(arg2);
                else
                {
                    int labelAddr = findLabelAddress(fout, labels, arg2);
                    if (op == 4) // beq
                        offset = labelAddr - (i + 1);
                    else
                        offset = labelAddr;
                }
                if (offset < -32768 || offset > 32767) // เช็คขอบเขต offset
                    printError(fout, "error: offsetField out of range at line " + to_string(i) + "\n");
                offset &= 0xFFFF;
                machineCode = (op << 22) | (regA << 19) | (regB << 16) | offset;
            }
            else if (op == 5) // J-type (jalr)
            {
                regA = stoi(arg0);
                regB = stoi(arg1);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                machineCode = (op << 22) | (regA << 19) | (regB << 16); // ใช้เพียง regA, regB ส่วนที่เหลือเป็นศูนย์
            }
            else if (op >= 6) // O-type (halt, noop)
            {
                machineCode = (op << 22); // ใช้แค่ opcode 3 บิตบนสุด ไม่มี register เลย
            }
        }
        catch (...)
        {
            printError(fout, "error: invalid operand in line " + to_string(i) + "\n");
        }
        cout << "(address " << i << "): " << machineCode // แสดง machine code ทั้งเลขฐาน 10 และฐาน 16
             << " (hex 0x" << hex << uppercase << machineCode << dec << ")\n";
        fout << machineCode << "\n"; // เขียนค่า machine code ลงไฟล์ output
    }

    fout.close();
    return 0;
}
