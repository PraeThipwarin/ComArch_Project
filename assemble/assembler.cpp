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

/**
 * @struct Label
 * @brief โครงสร้างที่ใช้เก็บข้อมูล label และ address ที่เกี่ยวข้อง
 *
 * ใช้ในขั้นตอนการแปลง assembly เป็น machine code
 * โดยจะเก็บชื่อ label และตำแหน่งบรรทัดที่ label นั้นปรากฏ
 */
struct Label
{
    string name;    // ชื่อของ label
    int address;    // ตำแหน่งบรรทัดของ label ในโปรแกรม
};

/**
 * @brief แสดงข้อความ error และหยุดการทำงานของโปรแกรม
 * 
 * @param fout ไฟล์ output ที่ใช้บันทึกข้อความ error
 * @param msg ข้อความ error ที่ต้องการแสดง
 * @throws none (ใช้ exit() ออกจากโปรแกรม)
 */
[[noreturn]] void printError(ofstream &fout, const string &msg) //noreturn ฟังก์ชันนี้จะไม่กลับมาเรียกซ้ำอีก คือจะจบโปรแกรมหรือ exit ไปเลย
{
    cerr << msg;
    fout << msg;
    fout.close();
    exit(1);
}

/**
 * @brief ตรวจสอบว่า string ที่รับเข้ามาเป็นตัวเลขล้วนหรือไม่
 * 
 * ฟังก์ชันนี้จะตรวจสอบว่า string ที่รับมามีค่าเป็นตัวเลขทั้งหมดหรือไม่ โดยมีขั้นตอนดังนี้:
 * 1. ถ้า string ว่างเปล่า (empty) จะถือว่าไม่เป็นตัวเลข
 * 2. ใช้ฟังก์ชัน `strtol()` เพื่อแปลง string เป็นค่าจำนวนเต็มแบบ long
 * 3. หากทุกตัวอักษรใน string เป็นตัวเลขทั้งหมด ตัวชี้ `endptr` จะชี้ไปที่ '\0'
 *    ซึ่งหมายความว่า string เป็นตัวเลขล้วน
 * 4. ถ้ามีอักขระที่ไม่ใช่ตัวเลข `endptr` จะหยุดก่อนถึง '\0'
 * 
 * @param s ข้อความ (string) ที่ต้องการตรวจสอบ
 * @return true ถ้า string เป็นตัวเลขล้วน
 * @return false ถ้า string ว่างเปล่าหรือมีอักขระที่ไม่ใช่ตัวเลข
 */
bool isNumber(const string &s)
{
    if (s.empty())
        return false;
    char *endptr;
    strtol(s.c_str(), &endptr, 10); //10ตัวสุดท้ายคือเลข ฐาน10
    return (*endptr == '\0'); // เป็น null
}

/**
 * @brief ตรวจสอบความถูกต้องของชื่อ label ตามเงื่อนไขที่กำหนด
 *
 * ฟังก์ชันนี้ใช้สำหรับตรวจสอบว่า string ที่รับเข้ามาเป็นชื่อ label ที่ถูกต้องหรือไม่
 * โดยจะตรวจสอบตามกฎที่โจทย์กำหนดดังนี้:
 * 
 * 1. ชื่อ label ต้องไม่ว่างเปล่า และมีความยาวไม่เกิน 6 ตัวอักษร  
 * 2. ตัวอักษรตัวแรกต้องเป็นตัวอักษร (A–Z, a–z) เท่านั้น  
 * 3. ทุกตัวอักษรที่เหลือต้องเป็นตัวอักษรหรือตัวเลข (ห้ามมีอักขระพิเศษ)
 * 
 * ตัวอย่างที่ถูกต้อง: `LOOP1`, `start`, `X`  
 * ตัวอย่างที่ไม่ถูกต้อง: `1ABC`, `AB@C`, `TOO_LONG_LABEL`
 *
 * @param s string ที่ต้องการตรวจสอบว่าเป็น label ถูกต้องหรือไม่
 * @return true ถ้า label ถูกต้องตามกฎ
 * @return false ถ้า label ผิดเงื่อนไขข้อใดข้อหนึ่ง
 */
bool isValidLabelName(const std::string &s)
{
    if (s.empty() || s.size() > 6)                      // ต้องไม่ว่างและมีความยาวไม่เกิน 6 ตัวอักษร
        return false;
    if (!isalpha(static_cast<unsigned char>(s[0])))     // ตัวแรกต้องเป็นตัวอักษรเท่านั้น
        return false;
    for (char c : s)                                    // ห้ามมีอักษรพิเศษ
        if (!isalnum(static_cast<unsigned char>(c)))
            return false;
    return true;
}

/**
 * @brief ค้นหา address ของ label จากรายชื่อทั้งหมด
 * 
 * @param fout ใช้สำหรับบันทึก error (ถ้ามี)
 * @param labels รายชื่อ label ที่เก็บไว้ใน Pass 1
 * @param name ชื่อ label ที่ต้องการค้นหา
 * @return address ของ label ถ้าพบ
 * @throws program exit ถ้า label ไม่ถูกนิยาม
 */
int findLabelAddress(ofstream &fout, const vector<Label> &labels, const string &name)
{
    for (auto &l : labels)
        if (l.name == name)
            return l.address;
    printError(fout, "error: undefined label \"" + name + "\"\n");
    return -1;
}

/**
 * @brief ตรวจสอบว่าหมายเลข register อยู่ในช่วง 0–7 หรือไม่
 * 
 * @param fout ไฟล์สำหรับบันทึก error
 * @param r หมายเลข register ที่ต้องการตรวจสอบ
 * @param which ชื่อ register ที่ใช้ในข้อความ error
 * @param line หมายเลขบรรทัดที่พบข้อผิดพลาด
 */
void checkRegRange(ofstream &fout, int r, const string &which, int line)
{
    if (r < 0 || r > 7)
        printError(fout, "error: register out of range (" + which + "=" + to_string(r) +
                             ") at line " + to_string(line) + "\n");
}

/**
 * @brief ฟังก์ชันหลักของโปรแกรม Assembler
 *
 * ทำหน้าที่เป็นจุดเริ่มต้นของโปรแกรม โดยรับพารามิเตอร์จาก command line
 * เพื่อระบุไฟล์ต้นฉบับ (input file) และไฟล์ผลลัพธ์ (output file)
 * จากนั้นจะเรียกใช้ฟังก์ชันภายในเพื่อทำการแปลง assembly code 
 * ให้กลายเป็น machine code ที่สามารถรันได้จริง
 *
 * @param argc จำนวน argument ที่ส่งเข้ามา (รวมชื่อโปรแกรมด้วย)
 * @param argv อาร์เรย์ของ argument ที่ส่งเข้ามา:
 *             - argv[1]: ชื่อไฟล์ input (assembly file)
 *             - argv[2]: ชื่อไฟล์ output (machine code file)
 * @return 0 เมื่อโปรแกรมทำงานสำเร็จ  
 * @return 1 หากเกิดข้อผิดพลาด (เช่น argument ไม่ครบ หรือไม่สามารถเปิดไฟล์ได้)
 */
int main(int argc, char *argv[])
{
        /**
     * @brief ตรวจสอบจำนวน argument ที่รับจาก command line
     *
     * ฟังก์ชัน main จะต้องได้รับ argument ครบ 3 ตัว ได้แก่:
     * - argv[0] : ชื่อโปรแกรม
     * - argv[1] : ชื่อไฟล์ input (ไฟล์ assembly)
     * - argv[2] : ชื่อไฟล์ output (ไฟล์ machine code)
     *
     * หากผู้ใช้ป้อนไม่ครบ จะทำการแสดงข้อความ error ทาง standard error (cerr)
     * และคืนค่า 1 เพื่อบ่งบอกว่าเกิดข้อผิดพลาดในการเริ่มต้นโปรแกรม
     */
    if (argc != 3)
    {
        cerr << "usage: assembler <inputfile> <outputfile>\n";
        return 1;
    }

    /**
     * @brief ตรวจสอบการเปิดไฟล์ input และ output
     *
     * ฟังก์ชันจะพยายามเปิดไฟล์ตามชื่อที่รับมาจาก argument:
     * - argv[1] : ไฟล์ input (ไฟล์ assembly ที่ต้องการอ่าน)
     * - argv[2] : ไฟล์ output (ไฟล์ machine code ที่ต้องการเขียนผลลัพธ์)
     *
     * หากไม่สามารถเปิดไฟล์ใดไฟล์หนึ่งได้ (เช่น ไฟล์ไม่พบ หรือไม่มีสิทธิ์เข้าถึง)
     * โปรแกรมจะแสดงข้อความ error ผ่าน `cerr`
     * และคืนค่า 1 เพื่อยุติการทำงาน
     */
    ifstream fin(argv[1]);
    ofstream fout(argv[2]);
    if (!fin || !fout)
    {
        cerr << "error: cannot open file\n";
        return 1;
    }

    /**
     * @brief ฟังก์ชันสำหรับอ่านไฟล์และลบแท็บ/เว้นวรรคหน้า และบรรทัดว่าง
     *
     * ฟังก์ชันจะอ่านไฟล์ทีละบรรทัด:
     * - ลบช่องว่าง, แท็บ, และ carriage return/line feed ที่อยู่ด้านหน้า
     * - ข้ามบรรทัดว่าง
     * - เก็บบรรทัดที่เหลือไว้ใน vector
     *
     * @param fin ifstream& Reference ของ input stream ที่เปิดไฟล์ไว้แล้ว
     * @return vector<string> รายการบรรทัดที่ถูกตัดช่องว่างหน้าแล้ว
     */
    vector<string> lines;   // เก็บบรรทัดที่ถูกตัดแท็บ/เว้นวรรคหน้าแล้ว
    string line;            // ตัวแปรชั่วคราวสำหรับเก็บบรรทัดที่อ่านจากไฟล์
    while (getline(fin, line))  // อ่านไฟล์ทีละบรรทัด
    {
        if (!line.empty())
        {
            // ลบ whitespace/แท็บ/CRLF ด้านหน้า
            line.erase(0, line.find_first_not_of(" \t\r\n"));   

            // ถ้าบรรทัดยังไม่ว่าง ให้เพิ่มเข้า vector
            if (!line.empty())
                lines.push_back(line);
        }
    }

    // ---------- Pass 1: เก็บตำแหน่งของ Label ----------

    /**
     * @brief Pass 1 ของ assembler
     *
     * ทำหน้าที่สแกนทุกบรรทัดของไฟล์ assembly และเก็บตำแหน่ง label ทั้งหมด
     * เพื่อใช้ใน pass 2 สำหรับแทนที่ label ด้วย address
     *
     * @param lines vector<string> ข้อมูลบรรทัดของไฟล์ assembly
     * @param fout ofstream& output stream สำหรับแสดง error
     * @return vector<Label> รายการ label ทั้งหมดพร้อม address
     */
    vector<Label> labels;   //เก็บ label ทั้งหมดพร้อม address
    for (int i = 0; i < (int)lines.size(); i++) // วนลูปทุกบรรทัดของ assembly
    {
        stringstream ss(lines[i]); //ใช้ stringstream เพื่อ “ตัดคำ” ในบรรทัดนั้นออกเป็น tokens(คำย่อยๆ)
        vector<string> tokens;
        string tok;

        // แยกคำในบรรทัดเป็น tokens
        while (ss >> tok)
            tokens.push_back(tok);

        // ข้ามบรรทัดว่าง
        if (tokens.empty())
            continue;

        string first = tokens[0];  // token แรกของบรรทัด

        // ถ้า token แรกไม่ใช่ opcode แสดงว่าเป็น label
        if (first != "add" && first != "nand" && first != "lw" && first != "sw" &&
            first != "beq" && first != "jalr" && first != "halt" &&
            first != "noop" && first != ".fill")
        {

            // ตรวจสอบชื่อ label ถูกต้องหรือไม่
            if (!isValidLabelName(first))   
                printError(fout, "error: invalid label name \"" + first + "\"\n");

            // ตรวจสอบ label ซ้ำ
            for (auto &l : labels)          
                if (l.name == first)
                    printError(fout, "error: duplicate label \"" + first + "\"\n");

            // เพิ่ม label ลงใน vector พร้อม address
            labels.push_back({first, i});   
        }
    }

    // ---------- Pass 2: แปลง Assembly เป็น Machine Code ----------

    /**
     * @brief Pass 2 ของ Assembler
     *
     * ทำหน้าที่แปลง assembly code ที่เก็บใน vector<string> lines
     * เป็นเลข machine code ตาม opcode และ argument ของแต่ละคำสั่ง
     * จากนั้นเขียนผลลัพธ์ลงไฟล์ output
     *
     * @param lines vector<string> รายการบรรทัด assembly
     * @param labels vector<Label> รายการ label พร้อม address จาก pass 1
     * @param fout ofstream& ไฟล์ output สำหรับเขียน machine code
     */
    for (int i = 0; i < (int)lines.size(); i++)  // วนลูปทุกบรรทัดของ assembly
    {
        stringstream ss(lines[i]);  // ใช้ stringstream เพื่อตัดคำในบรรทัด
        vector<string> tokens;      // เก็บ tokens ของบรรทัด
        string tok;                 // ตัวแปรชั่วคราวสำหรับเก็บคำแต่ละคำ

        // อ่านคำจาก stringstream ทีละคำ
        while (ss >> tok)
            tokens.push_back(tok);

        // ข้ามบรรทัดว่าง
        if (tokens.empty())
            continue;

        string opcode, arg0, arg1, arg2;

        /**
         * @brief Lambda function สำหรับเช็คว่า token เป็น opcode หรือไม่
         *
         * เนื่องจากบางบรรทัดมี label อยู่ด้านหน้า token แรก
         */
        auto isOpcodeWord = [&](const string &w)            
        {
            return (w == "add" || w == "nand" || w == "lw" || w == "sw" ||                      
                    w == "beq" || w == "jalr" || w == "halt" || w == "noop" || w == ".fill");
        };

        /**
         * @brief แยก opcode และ operand จาก tokens ของแต่ละบรรทัด
         *
         * ฟังก์ชันนี้ตรวจสอบว่าบรรทัดเริ่มด้วย label หรือ opcode โดยใช้ lambda `isOpcodeWord`
         * - ถ้าคำที่สองเป็น opcode แสดงว่าคำแรกคือ label
         * - มิฉะนั้น คำแรกคือ opcode
         * จากนั้นกำหนดค่า arg0, arg1, arg2 ตามจำนวน tokens
         *
         * @param tokens vector<string> คำย่อยของบรรทัด assembly
         * @param opcode string& ตัวแปรที่จะเก็บ opcode ของคำสั่ง
         * @param arg0 string& ตัวแปรเก็บ operand แรก (ถ้ามี)
         * @param arg1 string& ตัวแปรเก็บ operand ที่สอง (ถ้ามี)
         * @param arg2 string& ตัวแปรเก็บ operand ที่สาม (ถ้ามี)
         * @param isOpcodeWord lambda function ตรวจสอบว่า token เป็น opcode หรือไม่
         */
        if (tokens.size() >= 2 && isOpcodeWord(tokens[1]))
        {
            opcode = tokens[1];     //คำที่ 2 เป็น opcode
            if (tokens.size() > 2)
                arg0 = tokens[2];
            if (tokens.size() > 3)
                arg1 = tokens[3];
            if (tokens.size() > 4)
                arg2 = tokens[4];
        }
        else
        {
            opcode = tokens[0];     //คำที่แรก เป็น opcode เลย
            if (tokens.size() > 1)
                arg0 = tokens[1];
            if (tokens.size() > 2)
                arg1 = tokens[2];
            if (tokens.size() > 3)
                arg2 = tokens[3];
        }

        /**
         * @brief แปลงชื่อ opcode เป็นตัวเลขสำหรับ machine code
         *
         * opcode แต่ละตัวถูกแมปเป็นรหัสตัวเลข:
         * - R-type: add = 0, nand = 1
         * - I-type: lw = 2, sw = 3, beq = 4
         * - J-type: jalr = 5
         * - O-type: halt = 6, noop = 7
         * - .fill ใช้เก็บค่าคงที่หรือ address ของ label, กำหนด op = -2
         *
         * หากไม่ใช่ opcode ที่รู้จัก จะเรียก printError เพื่อแสดง error และหยุดโปรแกรม
         *
         * @param opcode string ชื่อ opcode ที่ต้องการแปลง
         * @param fout ofstream& ไฟล์ output สำหรับเขียน error
         * @param i int ตำแหน่งบรรทัดในไฟล์ assembly
         * @return int รหัสตัวเลขของ opcode (-2 สำหรับ .fill)
         */
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

        /**
         * @brief ประมวลผลคำสั่ง .fill`
         *
         * .fill ใช้เก็บค่าคงที่ (constant) หรือ address ของ label
         * ฟังก์ชันนี้จะตรวจสอบว่า arg0 เป็นตัวเลขหรือ label
         * - ถ้าเป็นตัวเลข จะแปลงเป็น int
         * - ถ้าเป็น label จะค้นหา address จาก vector<Label> ที่ได้จาก pass 1
         * จากนั้นแสดงผล machine code ทั้งฐาน 10 และฐาน 16
         * และเขียนลงไฟล์ output
         *
         * @param arg0 string ค่า operand ของ .fill
         * @param i int ตำแหน่งบรรทัด (address)
         * @param fout ofstream& ไฟล์ output สำหรับเขียน machine code
         * @param labels vector<Label> รายการ label และ address จาก pass 1
         */
        if (op == -2)   // ตรวจสอบว่าเป็น .fill
        { 
            int value;  // ตัวแปรเก็บค่าที่จะแปลงเป็น machine code

            // ถ้า arg0 เป็นตัวเลข
            if (isNumber(arg0))       
                value = stoi(arg0);     // แปลง string เป็น int
            else
                value = findLabelAddress(fout, labels, arg0);   // ถ้าเป็น label หา address จาก labels

            // แสดงผล machine code ทั้งฐาน 10 และฐาน 16
            cout << "(address " << i << "): " << value
                 << " (hex 0x" << hex << uppercase << value << dec << ")\n";

            // เขียน machine code ลงไฟล์ output
            fout << value << "\n";
            continue;   // ข้ามไปยังบรรทัดถัดไป
        }

        /**
         * @brief ประมวลผล operands และสร้าง machine code ตามประเภทคำสั่ง
         *
         * ฟังก์ชันนี้รองรับทุกประเภท instruction:
         * - R-type (add, nand) ใช้ regA, regB, dest
         * - I-type (lw, sw, beq) ใช้ regA, regB, offset
         * - J-type (jalr) ใช้ regA, regB
         * - O-type (halt, noop) ใช้ opcode เพียงอย่างเดียว
         *
         * @param op int รหัสตัวเลขของ opcode
         * @param arg0, arg1, arg2 string operands ของคำสั่ง
         * @param i int ตำแหน่งบรรทัด (address)
         * @param fout ofstream& ไฟล์ output สำหรับเขียน machine code หรือ error
         * @param labels vector<Label> รายการ label จาก pass 1 (ใช้สำหรับ I-type offset)
         * @return int machineCode ตัวเลข machine code ของคำสั่ง
         */
        int regA = 0, regB = 0, dest = 0, offset = 0;
        try
        {
            /**
             * @brief ประมวลผล R-type instruction (add, nand)
             *
             * R-type instruction มีรูปแบบ:
             *   opcode regA regB dest
             *
             * Bit layout ของ machine code:
             * - 22–24: opcode
             * - 19–21: regA
             * - 16–18: regB
             * - 0–2  : dest
             *
             * ฟังก์ชันนี้ทำงานดังนี้:
             * 1. แปลง operands จาก string เป็น int (regA, regB, dest)
             * 2. ตรวจสอบว่า register อยู่ในช่วงที่ถูกต้อง (0–7)
             * 3. รวม opcode และ register เป็น machine code
             *
             * @param arg0 string operand แรก (regA)
             * @param arg1 string operand ที่สอง (regB)
             * @param arg2 string operand ที่สาม (dest)
             * @param op int รหัส opcode
             * @param fout ofstream& ไฟล์ output สำหรับเขียน error
             * @param i int ตำแหน่งบรรทัดใน assembly
             * @return int machineCode ตัวเลข machine code ของคำสั่ง R-type
             */
            if (op <= 1)  
            { 
                regA = stoi(arg0);
                regB = stoi(arg1);
                dest = stoi(arg2);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                checkRegRange(fout, dest, "destReg", i);
                machineCode = (op << 22) | (regA << 19) | (regB << 16) | dest;  
            }

            /**
             * @brief ประมวลผล I-type instruction (lw, sw, beq)
             *
             * I-type instruction มีรูปแบบ:
             *   opcode regA regB offset
             *
             * Bit layout ของ machine code:
             * - 22–24: opcode
             * - 19–21: regA
             * - 16–18: regB
             * - 0–15 : offset (16-bit signed)
             *
             * ฟังก์ชันนี้ทำงานดังนี้:
             * 1. แปลง operands regA และ regB จาก string เป็น int
             * 2. ตรวจสอบขอบเขตของ register (0–7)
             * 3. แปลง arg2 เป็น offset:
             *    - ถ้าเป็นตัวเลข ให้ใช้ตรงๆ
             *    - ถ้าเป็น label:
             *        - beq ใช้ relative address (labelAddr - (i+1))
             *        - lw/sw ใช้ absolute address
             * 4. ตรวจสอบว่า offset อยู่ในช่วง -32768 ถึง 32767
             * 5. รวม opcode, regA, regB, offset เป็น machine code
             *
             * @param arg0 string operand แรก (regA)
             * @param arg1 string operand ที่สอง (regB)
             * @param arg2 string operand ที่สาม (offset หรือ label)
             * @param op int รหัส opcode
             * @param i int ตำแหน่งบรรทัดใน assembly
             * @param fout ofstream& ไฟล์ output สำหรับเขียน error
             * @param labels vector<Label> รายการ label จาก pass 1
             * @return int machineCode ตัวเลข machine code ของคำสั่ง I-type
             */
            else if (op >= 2 && op <= 4)   
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
                    if (op == 4)    //beq
                        offset = labelAddr - (i + 1);
                    else
                        offset = labelAddr;
                }
                if (offset < -32768 || offset > 32767)  //เช็คขอบเขต offset
                    printError(fout, "error: offsetField out of range at line " + to_string(i) + "\n");
                offset &= 0xFFFF;
                machineCode = (op << 22) | (regA << 19) | (regB << 16) | offset;
            }

            /**
             * @brief ประมวลผล J-type instruction (jalr)
             *
             * J-type instruction มีรูปแบบ:
             *   jalr regA regB
             *
             * Bit layout ของ machine code:
             * - 22–24: opcode
             * - 19–21: regA
             * - 16–18: regB
             * - 0–15 : เป็นศูนย์ทั้งหมด
             *
             * ฟังก์ชันนี้ทำงานดังนี้:
             * 1. แปลง operands regA และ regB จาก string เป็น int
             * 2. ตรวจสอบขอบเขตของ register (0–7)
             * 3. รวม opcode, regA, regB เป็น machine code
             *
             * @param arg0 string operand แรก (regA)
             * @param arg1 string operand ที่สอง (regB)
             * @param op int รหัส opcode
             * @param i int ตำแหน่งบรรทัดใน assembly
             * @param fout ofstream& ไฟล์ output สำหรับเขียน error
             * @return int machineCode ตัวเลข machine code ของคำสั่ง J-type
             */
            else if (op == 5)  
            { 
                regA = stoi(arg0);
                regB = stoi(arg1);
                checkRegRange(fout, regA, "regA", i);
                checkRegRange(fout, regB, "regB", i);
                machineCode = (op << 22) | (regA << 19) | (regB << 16);
            }

            /**
             * @brief ประมวลผล O-type instruction (halt, noop)
             *
             * O-type instruction ไม่มี operands ใช้เพียง opcode
             *
             * Bit layout ของ machine code:
             * - 22–24: opcode
             * - 0–21 : เป็นศูนย์ทั้งหมด
             *
             * ฟังก์ชันนี้ทำงานดังนี้:
             * 1. ใช้รหัส opcode (op) วางที่บิต 22–24
             * 2. ส่วนที่เหลือเป็น 0
             *
             * @param op int รหัส opcode
             * @return int machineCode ตัวเลข machine code ของคำสั่ง O-type
             */
            else if (op >= 6)  
            { 
                // สร้าง machine code โดยใช้ opcode 3 บิตบนสุด ส่วนที่เหลือเป็น 0
                machineCode = (op << 22); 
            }
        }

        /**
         * @brief จับข้อผิดพลาดของ operand และแสดง/บันทึก machine code
         *
         * ฟังก์ชันนี้ทำงานหลังจากสร้าง machine code แล้ว:
         * 1. ใช้ try-catch เพื่อตรวจจับข้อผิดพลาดที่เกิดจากการแปลง string → int
         *    เช่น operand ไม่ใช่ตัวเลข, register หรือ offset ผิดพลาด
         * 2. หากเกิดข้อผิดพลาด จะเรียก printError เพื่อแสดงข้อความและหยุดโปรแกรม
         * 3. แสดง machine code ทั้งเลขฐาน 10 และฐาน 16 บน console
         * 4. เขียน machine code ลงไฟล์ output
         *
         * @param machineCode int ตัวเลข machine code ที่สร้างขึ้น
         * @param i int ตำแหน่งบรรทัด (address) ใน assembly
         * @param fout ofstream& ไฟล์ output สำหรับเขียน machine code หรือ error
         */
        catch (...)
        {
            // จับ exception ทุกชนิดและแสดง error
            printError(fout, "error: invalid operand in line " + to_string(i) + "\n");  
        }

        // แสดง machine code บน console ทั้งฐาน 10 และฐาน 16
        cout << "(address " << i << "): " << machineCode                           
             << " (hex 0x" << hex << uppercase << machineCode << dec << ")\n";     

        // เขียน machine code ลงไฟล์ output 
        fout << machineCode << "\n";                                            
    }

    // ปิดไฟล์หลังจบการประมวลผล
    fout.close();
    return 0;
}
