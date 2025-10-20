        lw      0       1       n     // load reg[1] = n
        lw      0       2       r     // load reg[2] = r
        lw      0       6       pos1   // load reg[6] = +1
        lw      0       4       addrC // reg[4] = addr fn comb
        lw      0       5       Stack  //reg[5] = Stack
        jalr    4       7              // jump to reg[4]'s addr กับ save return addr to reg[7]
        halt                           //จบการทำงาน
comb    sw      5       7       0      //push return addr -> stack
        add     5       6       5      //stack++
        sw      5       1       0      //push n -> stack
        add     5       6       5      //stack++
        sw      5       2       0      //push r -> stack
        add     5       6       5      //stack++
        beq     2       0       base   [// basecase เอาไว้เช็คว่า n==r, r==0 ? เป็น basecase ไว้ออก loop
        beq     1       2       base    ] 
        nand    0       0       3      // reg[3] = -1
        add     1       3       1      // made n = n-1 comb(n-1, r)   
        lw      0       4       addrC  // load reg[4] == addr ของ comb
        jalr    4       7              // jump to comb(n-1,r)  
        sw      5       3       0      //store result in reg[03] into stack
        add     5       6       5      //stack++
        nand    0       0       3      // ทำ -1
        add     2       3       2      // r-1   
        lw      0       4       addrC  // load address comb กลับเข้า reg[4]
        jalr    4       7              // jump ไปทำ comb(n-1,r-1)
        nand    0       0       4      
        add     5       4       5       // pop stack เพื่อให้มันชี้ไปที่ result1
        lw      5       4       0       // load Result 1 เข้า reg[4]
        add     3       4       3       // result = result1 + result2
        beq     0       0       done    // jump to done 
base    lw      0       3       pos1    // basecase implement
done    nand    0       0       4       // reg[4] = -1 (ใช้สำหรับลด SP)
        add     5       4       5       // pop ลด SP 1 ครั้ง (เคลียร์ Result 1 ที่เคย push ไว้)
        lw      5       2       0       // load original r (reg[2])
        add     5       4       5       // pop
        lw      5       1       0       // load original n (reg[1])
        add     5       4       5       // pop
        lw      5       7       0       // pop return addr -> reg[07]
        jalr    7       0               // โดดกลับ return addr

n       .fill   7
r       .fill   3
pos1    .fill   1
addrC   .fill   comb
Stack   .fill   stack
stack   .fill   0