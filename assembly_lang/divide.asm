	lw      0 4 pos1        //load reg[4] = +1
        lw      0 1 dvdnd       //load reg[1] = dividend
        lw      0 2 dvsor       //load reg[2] = divisor
        lw      0 6 sgnmsk      //load reg[6] = signmask
        lw      0 5 dvadr       //load reg[5] = address ของ div
        jalr    5 7             // ให้โดดไปที่บรรทัดที่ div อยู่ แล้วเก็บค่า addr ขากลับไว้ที่ reg[7]
        halt                    //จบการทำงาน

div     sw      0 7 rtadr       //เอา reg[07] -> mem rtadr
        add     0 0 3           // กำหนดค่าเริ่มต้นให้ผลหาร
        nand    2 2 5           [// เอาตัวหารมาทำเป็นลบโดยใช้ 2's complement
        add     5 4 5           ]

loop    add     1 5 1           // เอาตัวตั้ง - ตัวหาร
        add     3 4 3           // quotient = quotient + 1
        nand    1 6 7           // 
        nand    7 7 7
        beq     7 0 cont        // เอาไว้ดักเช็คว่า dvdnd - dvsor ตัว dvdnd มันมีค่าเป็น +,- เผื่อดูว่ามันน้อยกว่ายังหรือมากกว่ายัง
        add     1 2 1           // คืนค่าตัวตั้งกลับคืนมา (dvdnd = dvdnd + dvsor)
        nand    0 0 7           // reg[7] = -1
        add     3 7 3           // ลบค่า quotient ที่เกินมา 1 
        beq     0 0 done        // กระโดดไป label จบ

cont    beq     0 0 loop        // ทำ loop ต่อ

done    lw      0 7 rtadr       // load ค่า return addr ที่เก็บใน memory กลับมา
        jalr    7 0             //return กลับไปที่คำสั่ง halt

dvdnd   .fill   13
dvsor   .fill   4
pos1    .fill   1
dvadr   .fill   div
sgnmsk  .fill   -2147483648
rtadr   .fill   0
