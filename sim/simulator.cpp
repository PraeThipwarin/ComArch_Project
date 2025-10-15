// simulator.cpp
// SMC Instruction-level Simulator (C++) — ตามสเปกในโจทย์
// คอมไพล์: g++ -std=c++17 -O2 simulator.cpp -o simulator

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iostream>

#define NUMMEMORY 65536   /* maximum number of words in memory */
#define NUMREGS   8       /* number of machine registers */
#define MAXLINELENGTH 1000

typedef struct stateStruct {
    int pc;
    int mem[NUMMEMORY];
    int reg[NUMREGS];
    int numMemory;    // = จำนวนบรรทัดในไฟล์ machine code
} stateType;

/* ---- ห้ามแก้ลายเซ็น/รูปแบบการพิมพ์ของ printState ตามกติกา ---- */
void printState(stateType *statePtr)
{
    int i;
    printf("\n@@@\nstate:\n");
    printf("\tpc %d\n", statePtr->pc);
    printf("\tmemory:\n");
    for (i = 0; i < statePtr->numMemory; i++) {
        printf("\t\tmem[ %d ] %d\n", i, statePtr->mem[i]);
    }
    printf("\tregisters:\n");
    for (i = 0; i < NUMREGS; i++) {
        printf("\t\treg[ %d ] %d\n", i, statePtr->reg[i]);
    }
    printf("end state\n");
}
/* --------------------------------------------------------------- */

static inline int convertNum(int num) {
    /* convert a 16-bit number into a 32-bit signed integer (sign-extend) */
    if (num & (1 << 15)) {
        num -= (1 << 16);
    }
    return num;
}

static inline void assertMemInRange(int addr) {
    if (addr < 0 || addr >= NUMMEMORY) {
        std::fprintf(stderr, "error: memory address %d out of range [0..%d]\n", addr, NUMMEMORY-1);
        std::exit(1);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::fprintf(stderr, "error: usage: %s <machine-code file>\n", argv[0]);
        return 1;
    }

    const char* filename = argv[1];
    FILE* filePtr = std::fopen(filename, "r");
    if (!filePtr) {
        std::perror("error opening machine-code file");
        return 1;
    }

    stateType state;
    char line[MAXLINELENGTH];

    // 1) โหลดไฟล์ machine code ทั้งหมดเข้าสู่ memory
    state.numMemory = 0;
    while (std::fgets(line, MAXLINELENGTH, filePtr) != nullptr) {
        // ข้ามบรรทัดว่าง
        int tmp;
        if (std::sscanf(line, "%d", &tmp) != 1) {
            std::fprintf(stderr, "error in reading address %d\n", state.numMemory);
            return 1;
        }
        if (state.numMemory >= NUMMEMORY) {
            std::fprintf(stderr, "error: program too large (>%d words)\n", NUMMEMORY);
            return 1;
        }
        state.mem[state.numMemory] = tmp;
        // echo แบบตัวอย่างสตาร์ทเตอร์ (ถ้าไม่ต้องการ สามารถคอมเมนต์ออกได้)
        std::printf("memory[%d]=%d\n", state.numMemory, state.mem[state.numMemory]);
        state.numMemory++;
    }
    std::fclose(filePtr);

    // 2) Initialize registers และ PC
    state.pc = 0;
    for (int i = 0; i < NUMREGS; ++i) state.reg[i] = 0;

    // 3) วนลูป fetch-decode-execute
    long long instrCount = 0;  // สำหรับสรุป (ไม่บังคับ แต่มีประโยชน์)

    while (true) {
        // เรียก printState ก่อน execute ทุกคำสั่ง
        printState(&state);

        // Fetch
        assertMemInRange(state.pc);
        int instr = state.mem[state.pc];

        // Decode
        int opcode = (instr >> 22) & 0x7;
        int rs     = (instr >> 19) & 0x7;
        int rt     = (instr >> 16) & 0x7;
        int rd     =  instr        & 0x7;       // R-type เท่านั้น
        int imm16  =  instr        & 0xFFFF;    // I-type จะ convert ทีหลัง

        // Execute
        switch (opcode) {
            case 0: { // add (R-type)
                // reg[rd] = reg[rs] + reg[rt]
                int result = state.reg[rs] + state.reg[rt];
                state.reg[rd] = result;
                state.pc += 1;
                break;
            }
            case 1: { // nand (R-type)
                // reg[rd] = ~(reg[rs] & reg[rt])
                int result = ~(state.reg[rs] & state.reg[rt]);
                state.reg[rd] = result;
                state.pc += 1;
                break;
            }
            case 2: { // lw (I-type)
                // reg[rt] = mem[ reg[rs] + imm ]
                int imm = convertNum(imm16);
                int addr = state.reg[rs] + imm;
                assertMemInRange(addr);
                state.reg[rt] = state.mem[addr];
                state.pc += 1;
                break;
            }
            case 3: { // sw (I-type)
                // mem[ reg[rs] + imm ] = reg[rt]
                int imm = convertNum(imm16);
                int addr = state.reg[rs] + imm;
                assertMemInRange(addr);
                state.mem[addr] = state.reg[rt];
                state.pc += 1;
                break;
            }
            case 4: { // beq (I-type)
                // if (reg[rs]==reg[rt]) pc = pc + 1 + imm; else pc++
                int imm = convertNum(imm16);
                if (state.reg[rs] == state.reg[rt]) {
                    state.pc = state.pc + 1 + imm;
                } else {
                    state.pc += 1;
                }
                break;
            }
            case 5: { // jalr (J-type)
                // เก็บ pc+1 ลง reg[rt] (ต้องเก็บก่อนแม้ rs==rt) แล้ว pc = reg[rs]
                int nextPC = state.pc + 1;
                int target = state.reg[rs];
                state.reg[rt] = nextPC;
                state.pc = target;
                break;
            }
            case 6: { // halt (O-type)
                // ทำเหมือนคำสั่งอื่น (pc++) แล้ว "printState อีกครั้งก่อน exit" ตามกติกา
                state.pc += 1;
                // รักษา $0 ก่อนพิมพ์สถานะสุดท้าย
                state.reg[0] = 0;
                printState(&state);      // ก่อนออก
                std::printf("machine halted\n");
                std::printf("total of %lld instructions executed\n", instrCount);
                std::printf("final state of machine:\n");
                // ตาม rule (4) เราได้ printState ก่อนออกแล้วบรรทัดบน
                // บางสภาพแวดล้อมตัวอย่างจะพิมพ์ state อีกครั้งหลังข้อความ;
                // หากเข้มงวดตาม rule ให้คอมเมนต์บรรทัดต่อไปนี้ออก
                // printState(&state);
                return 0;
            }
            case 7: { // noop (O-type)
                state.pc += 1;
                break;
            }
            default: {
                std::fprintf(stderr, "error: bad opcode %d at pc=%d\n", opcode, state.pc);
                return 1;
            }
        }

        // เงื่อนไขสำคัญ: reg0 ต้องคงค่า 0 เสมอ
        state.reg[0] = 0;

        // นับจำนวนคำสั่งที่ execute เสร็จแล้ว (ไม่รวม halt หลังพิมพ์สถานะสุดท้าย)
        instrCount++;
    }

    // ไม่ควรมาถึงตรงนี้
    return 0;
}
