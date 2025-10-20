// วิธีคอมไพล์: g++ -std=c++17 -O2 simulator.cpp -o simulator // บอกคำสั่งสร้างโปรแกรม

// ------------------------------------------------------------------------- //
#include <cstdio> // ใช้ฟังก์ชัน printf
#include <cstring> // ใช้ฟังก์ชันจัดการอักขระพื้นฐาน
#include <cstdlib> 
#include <string> // เตรียมไว้หากต้องจัดการสตริงแบบ C++
#include <iostream> 
// ------------------------------------------------------------------------- // 
#define NUMMEMORY 65536   // กำหนดจำนวนช่องหน่วยความจำสูงสุดที่เครื่องรองรับ
#define NUMREGS   8       // กำหนดจำนวนรีจิสเตอร์ในเครื่องตัวอย่างนี้
#define MAXLINELENGTH 1000 // กำหนดความยาวบรรทัดสูงสุดเวลารับข้อมูลจากไฟล์
// ------------------------------------------------------------------------- // 

// ------------------------------------------------------------------------- // 
typedef struct stateStruct { // สร้างโครงสร้างไว้เก็บสถานะเครื่องจำลองทั้งหมด
    int pc;                       // เก็บตำแหน่งคำสั่งปัจจุบันในหน่วยความจำ
    int mem[NUMMEMORY];           // เก็บค่าหน่วยความจำแต่ละช่องของโปรแกรม
    int reg[NUMREGS];             // เก็บค่าของรีจิสเตอร์ทั้งแปดตัวของเครื่อง
    int numMemory;                // เก็บจำนวนช่องหน่วยความจำที่มีข้อมูลจริง
} stateType;
// ------------------------------------------------------------------------- // 

/* ---- ห้ามแก้รูปแบบการพิมพ์ของ printState ตามสเปกงาน ---- */
void printState(stateType *statePtr) // ฟังก์ชัน printState
{
    int i; // ตัวนับสำหรับลูป
    printf("\n@@@\nstate:\n"); // print state
    printf("\tpc %d\n", statePtr->pc); // / print pc ปัจจุบัน
    printf("\tmemory:\n"); // print memory
    for (i = 0; i < statePtr->numMemory; i++) { // วนแสดงค่าหน่วยความจำเท่าที่มีจริง
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]); // / print ตำแหน่งและค่าของแต่ละช่อง
    }
    printf("\tregisters:\n"); // print registers
    for (i = 0; i < NUMREGS; i++) { // วนแสดงค่าของรีจิสเตอร์ทั้งแปดตัว
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]); // พิมพ์หมายเลขรีจิสเตอร์และค่าปัจจุบัน
    }
    printf("end state\n"); // แจ้งว่าจบการพิมพ์สถานะแล้ว
} 
/* --------------------------------------------------------------- */ 

// ------------------------------------------------------------------------- // 
static inline int convertNum(int num) { // ฟังก์ชันแปลงจำนวน 16 บิตให้เป็น 32 บิตพร้อมเครื่องหมาย
    if (num & (1 << 15)) { // ถ้าบิตซ้ายสุดของค่าทันทีเป็นหนึ่ง แปลว่าเป็นค่าลบ
        num -= (1 << 16); // ปรับค่าด้วยการลบ 65,536 เพื่อให้กลายเป็นจำนวนลบที่ถูกต้อง
    }
    return num; // ส่งค่าที่แปลงแล้วกลับไปใช้งานต่อ
} 
// ------------------------------------------------------------------------- //

static inline void assertMemInRange(int addr) { // ฟังก์ชันเช็กว่าที่อยู่หน่วยความจำอยู่ในช่วงที่อนุญาต
    if (addr < 0 || addr >= NUMMEMORY) { // ถ้าที่อยู่น้อยกว่า 0 หรือมากกว่าขอบเขตสูงสุดถือว่าผิด
        std::fprintf(stderr, "error: memory address %d out of range [0..%d]\n", addr, NUMMEMORY-1); // แจ้ง error
        std::exit(1); // หยุดการทำงานทันที
    }
}
// ------------------------------------------------------------------------- // 

int main(int argc, char *argv[]) // จุดเริ่มต้นของโปรแกรม
{
    if (argc != 2) { // เช็คว่าส่งตัวแปรมาถูกจำนวนไหม
        std::fprintf(stderr, "error: usage: %s <machine-code file>\n", argv[0]); // ขึ้น error และ บอกวิธีใช้งานที่ถูกต้อง
        return 1; // จบโปรแกรม error เพราะ input ไม่ถูกต้อง
    }
// ------------------------------------------------------------------------- //

    const char* filename = argv[1]; // เก็บชื่อไฟล์ machine code จากตัวแปรตำแหน่งที่ 1
    FILE* filePtr = std::fopen(filename, "r"); // เปิดไฟล์ในโหมดอ่าน
    if (!filePtr) { // ถ้าเปิดไฟล์ไม่สำเร็จเพราะไฟล์ไม่มีหรือไม่มีสิทธิ์
        std::perror("error opening machine-code file"); // error สาเหตุของความผิดพลาด
        return 1; // จบโปรแกรมเนื่องจากไม่สามารถอ่านไฟล์ได้
    }
// ------------------------------------------------------------------------- //

    stateType state; // ประกาศตัวแปรโครงสร้างเพื่อเก็บสถานะทั้งหมดของเครื่องจำลอง
    char line[MAXLINELENGTH]; // เตรียมบัฟเฟอร์ไว้รับข้อมูลหนึ่งบรรทัดจากไฟล์
// ------------------------------------------------------------------------- //

    // 1) โหลดไฟล์ machine code ทั้งหมดเข้าสู่ memory
    state.numMemory = 0; // เริ่มต้นจำนวนช่อง memory ที่ใช้จริงให้ = 0
    for (int i = 0; i < NUMMEMORY; ++i) state.mem[i] = 0; // ล้างค่า memory ทั้งหมดให้เป็นศูนย์ก่อนใช้
    while (std::fgets(line, MAXLINELENGTH, filePtr) != nullptr) { // อ่านไฟล์ทีละบรรทัดจนกว่าจะถึงท้ายไฟล์

        int tmp; // เตรียมตัวแปรไว้เก็บค่าที่อ่านออกมาจากบรรทัดปัจจุบัน
        if (std::sscanf(line, "%d", &tmp) != 1) { // ถ้าแปลงบรรทัดเป็นตัวเลขไม่ได้
            std::fprintf(stderr, "error in reading address %d\n", state.numMemory); // แจ้งว่าอ่านข้อมูลไม่ได้ตรงตำแหน่งไหน
            return 1; // หยุดการทำงานเพราะข้อมูลไม่ถูกต้อง
        }
        if (state.numMemory >= NUMMEMORY) { // ถ้าข้อมูลเยอะเกินกว่าหน่วยความจำของเครื่องจะรับได้
            std::fprintf(stderr, "error: program too large (>%d words)\n", NUMMEMORY); // แจ้งว่าขนาดโปรแกรมใหญ่เกินไป
            return 1; // ออกจากโปรแกรม
        }
        state.mem[state.numMemory] = tmp; // เก็บค่าที่อ่านได้ลงในหน่วยความจำตำแหน่งปัจจุบัน
        std::printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]); // print ค่าที่โหลดเข้ามาเพื่อให้ผู้ใช้เห็นความคืบหน้า
        state.numMemory++; // เพิ่มตัวนับจำนวนช่องหน่วยความจำที่มีข้อมูลจริง
    }
    std::fclose(filePtr); // ปิดไฟล์หลังจากอ่านข้อมูลเสร็จสิ้น

// ------------------------------------------------------------------------- //
    // 2) Initialize registers และ PC 
    state.pc = 0; // ให้ program counter เริ่มที่คำสั่งช่องแรก
    for (int i = 0; i < NUMREGS; ++i) state.reg[i] = 0; // ตั้งค่าทุกรีจิสเตอร์ให้เป็นศูนย์ก่อนเริ่มการจำลอง
// ------------------------------------------------------------------------- //
    // 3) วนลูป fetch-decode-execute 
    long long instrCount = 0; //ตัวแปรนับจำนวนคำสั่งที่ได้ทำงานแล้ว
// ------------------------------------------------------------------------- //

    while (true) { // ลูปหลักที่ทำงานจนกว่าจะเจอคำสั่ง halt
        printState(&state); // เรียกฟังก์ชันเพื่อพิมพ์สถานะปัจจุบันของเครื่อง
        assertMemInRange(state.pc); // ตรวจว่าค่า pc อยู่ในช่วงที่อนุญาตก่อนอ่าน
        int instr = state.mem[state.pc]; // ดึงคำสั่งจากหน่วยความจำตำแหน่ง pc มาใช้

        // Decode: แยก field ต่าง ๆ ตามรูปแบบ opcode 3 bits, register 3 bits, immediate 16 bits 
        int opcode = (instr >> 22) & 0x7; // ตัดบิต 24..22 มาเป็น opcode
        int rs     = (instr >> 19) & 0x7; // ตัดบิต 21..19 เลขรีจิสเตอร์ rs
        int rt     = (instr >> 16) & 0x7; // ตัดบิต 18..16 เลขรีจิสเตอร์ rt
        int rd     =  instr        & 0x7;       // R-type เท่านั้น
        int imm16  =  instr        & 0xFFFF;    // I-type จะ convert ทีหลัง 
// ------------------------------------------------------------------------- // 

        // Execute: ตัดสินเส้นทางตาม opcode แล้วอัปเดต state เครื่อง SMC 
        switch (opcode) { // ตรวจค่า opcode
            case 0: { // opcode=0 หมายถึงคำสั่ง add แบบ R-type
                // reg[rd] = reg[rs] + reg[rt] // สูตรการบวกค่าจากรีจิสเตอร์สองตัว
                int result = state.reg[rs] + state.reg[rt]; // นำค่าจากรีจิสเตอร์ทั้งสองมาบวกกัน
                state.reg[rd] = result; // เก็บผลลัพธ์ลงรีจิสเตอร์ปลายทาง
                state.pc += 1; // ขยับไปคำสั่งถัดไปตามปกติ
                break; // จบการทำงานของคำสั่งนี้
            } 
            case 1: { // opcode=1 หมายถึง nand แบบ R-type
                // reg[rd] = ~(reg[rs] & reg[rt]) // สูตรในการคำนวณ NAND
                int result = ~(state.reg[rs] & state.reg[rt]); // ทำ AND แล้วกลับบิตทุกตัวเพื่อให้เป็น NAND
                state.reg[rd] = result; // เก็บผลลัพธ์ลงรีจิสเตอร์ปลายทาง
                state.pc += 1; // ไปยังคำสั่งถัดไป
                break; // จบการทำงานของคำสั่งนี้
            } 
            case 2: { // opcode=2 หมายถึง lw (load word แบบ I-type (โหลดข้อมูลจากหน่วยความจำมาไว้ในรีจิสเตอร์))
                // reg[rt] = mem[ reg[rs] + imm ] // สูตรการอ่านข้อมูลจากหน่วยความจำ
                int imm = convertNum(imm16); // แปลง imm16 ให้เป็น signed 32 บิต (รองรับค่าลบ)
                int addr = state.reg[rs] + imm; // คำนวณ address ปลายทาง
                assertMemInRange(addr); // ตรวจว่าตำแหน่งเป้าหมายนั้นอยู่ในช่วงที่อนุญาต
                state.reg[rt] = state.mem[addr]; // โหลดค่าจาก memory มาเก็บในรีจิสเตอร์ rt
                state.pc += 1; // เดินไปยังคำสั่งถัดไป
                break; // จบคำสั่งโหลดข้อมูล
            }
            case 3: { // opcode=3 หมายถึง sw (store word แบบ I-type (บันทึกค่าจากรีจิสเตอร์ลงหน่วยความจำ))
                // mem[ reg[rs] + imm ] = reg[rt] // สูตรการเขียนค่าลงหน่วยความจำ
                int imm = convertNum(imm16); // แปลง imm16 เป็น signed 32 บิต
                int addr = state.reg[rs] + imm; // คำนวณ address ปลายทาง
                assertMemInRange(addr); // ตรวจช่วงความถูกต้องของ address
                state.mem[addr] = state.reg[rt]; // เขียนค่าจากรีจิสเตอร์ rt ลง memory[addr]
                state.pc += 1; // ขยับไปคำสั่งถัดไป
                break; // จบคำสั่งบันทึกข้อมูล
            } 
            case 4: { // opcode=4 หมายถึง beq (branch if equal กระโดดหากค่าตรงกัน)
                // if (reg[rs]==reg[rt]) pc = pc + 1 + imm; else pc++ // สูตรการตัดสินใจกระโดด
                int imm = convertNum(imm16); // แปลงค่า offset ให้ใช้ได้
                if (state.reg[rs] == state.reg[rt]) { // ถ้ารีจิสเตอร์ทั้งสองมีค่าเท่ากัน
                    state.pc = state.pc + 1 + imm; // กระโดดไปยังตำแหน่งใหม่ที่คำนวณไว้
                } else { // ถ้าไม่เท่ากัน
                    state.pc += 1; // แค่เดินไปคำสั่งถัดไปตามปกติ
                }
                break; // จบคำสั่ง
            } 
            case 5: {  // opcode=5 หมายถึง jalr (jump and link register)
                // เก็บ pc+1 ลง reg[rt] (ต้องเก็บก่อนแม้ rs==rt) แล้ว pc = reg[rs]
                int nextPC = state.pc + 1; // ค่าที่จะใช้เป็น return address (pc ถัดไป)
                int target = state.reg[rs]; // อ่านตำแหน่งเป้าหมายจากรีจิสเตอร์ต้นทาง
                state.reg[rt] = nextPC; // บันทึก return address ลง rt ก่อนเปลี่ยน pc
                state.pc = target; // กระโดดไปยังตำแหน่งใหม่ที่ต้องการทำงานต่อ
                break; // จบคำสั่ง
            } 
            case 6: { // opcode=6 หมายถึง halt (หยุดเครื่อง)
                state.pc += 1; // เพิ่มค่า pc ก่อนหยุด
                state.reg[0] = 0; // บังคับให้ reg0 เป็น 0 (บังคับ)
                printState(&state); // พิมพ์สถานะสุดท้ายของเครื่อง
                std::printf("machine halted\n"); // แจ้งว่าหยุดเครื่องแล้ว
                std::printf("total of %lld instructions executed\n", instrCount); // บอกจำนวนคำสั่งที่ทำงานทั้งหมดจนถึงตอนนี้
                std::printf("final state of machine:\n"); // พิมพ์หัวข้อสำหรับสถานะสุดท้าย
                return 0; // จบโปรแกรมคำสั่ง halt
            } 
            case 7: { // opcode=7 หมายถึง noop (ไม่ทำอะไร)
                state.pc += 1; // เพิ่มค่า pc เพื่อข้ามไปคำสั่งถัดไปทันที
                break; // จบคำสั่ง noop
            }
            default: { // กรณี opcode ไม่ตรงกับคำสั่งที่รองรับ
                std::fprintf(stderr, "error: bad opcode %d at pc=%d\n", opcode, state.pc); // แจ้งว่าพบคำสั่งที่ไม่รู้จักพร้อมตำแหน่ง
                return 1; // หยุดโปรแกรม
            } 
        }
// ------------------------------------------------------------------------- // 
        state.reg[0] = 0; // บังคับให้ reg[0] กลับเป็น 0 ทุกครั้งหลังทำงานหนึ่งคำสั่ง
// ------------------------------------------------------------------------- //
        instrCount++; // เพิ่มค่าตัวนับคำสั่งที่ทำเสร็จแล้วหนึ่งครั้ง
    } // จบลูป while หลักที่ทำงานไม่รู้จบถ้าไม่เจอ halt
// ------------------------------------------------------------------------- //
    return 0; // คืนค่า 0 เพื่อความถูกต้อง ถึงแม้จะคาดว่าไม่ถูกเรียกใช้งาน
} 
