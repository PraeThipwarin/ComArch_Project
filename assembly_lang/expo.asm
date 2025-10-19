	lw      0 1 base        //load ค่า reg1 = base ฐาน
        lw      0 2 exp         //load ค่า reg2 = exp เลขชี้กำลัง
        beq     2 0 isExp0      //basecase ไว้เช็คว่า base^0 ไหมถ้าใช่ให้โดดไปเข้าเงื่อนไข isExp0
        
        add     1 0 3           // result = base + 0
        lw      0 7 negOne      // reg7 = -1
        add     2 7 2           // exp_counter = exp - 1
        beq     2 0 done        // check ว่า exp_counter = 0 (exp = 1) ให้จับการทำงาน

pwL     sw      0 1 tmpBse      //พักค่าฐานที่ต้องใช้ซ้ำ
        sw      0 2 tmpExp      //พักค่าตัวนับลูปนอก
        sw      0 3 tmpRes      //พักค่า sum 

        lw      0 2 tmpRes      //โหลดตัวตั้งคูณ (Current Result)
        lw      0 3 tmpBse      //โหลดตัวคูณ (Original Base)
        
        add     0 0 1           //กำหนดค่า result ของการคูณ = 0
        lw      0 4 bmask       //โหลด Bit Mask
        lw      0 5 bcount      //โหลดตัวนับบิต
Lcheck  nand    3 4 6           //เริ่มทำ bit check เหมือนตอนที่ทำการคูณ
        nand    6 6 6           
        beq     0 6 bitis0      //check Bit Value
        add     1 2 1           //result = result + mcand
bitis0  add     2 2 2           //ถ้า bit = 0 ก็ left shift mcand
        add     4 4 4           //left shift bitmask
        add     7 5 5           // ลด bitcount ลง 1
        beq     5 0 mltDn       // if bcount == 0
        beq     0 0 Lcheck      // วนซ้ำลูปที่ทำการคูณ
        
mltDn   add     1 0 3           //ถ้าคูณจบแล้วให้ result = ผลลัพธฺ์ของการคูณ

        lw      0 1 tmpBse      // load ค่าฐานเดิม
        lw      0 2 tmpExp      // load ค่าเลขชี้กำลังเดิม

        add     2 7 2           //ลดตัวนับลูปนอก (exp_counter)
        beq     2 0 done        //If exp_counter == 0 ให้จบการทำงาน
        beq     0 0 pwL         //ถ้ายังก็ให้ไปวนลูปทำการยกกำลังต่อ

isExp0   lw      0 3 one        //ถ้าเลขยกกำลังเป็น 0 ให้ return 1
done    halt                    //จบการทำงาน

base        .fill   4
exp         .fill   6
one         .fill   1
negOne      .fill   -1
bmask       .fill   1
bcount      .fill   15
tmpRes      .fill   0   // เอาไว้เก็บผลลัพธ์ชั่วคราว
tmpExp      .fill   0   // เอาไว้เก็บเลขชี้กำลัชั่วคราว
tmpBse      .fill   0   // เอาไว้เก็บเลขฐานชั่วคราว
