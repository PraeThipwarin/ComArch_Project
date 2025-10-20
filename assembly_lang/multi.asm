        add     0   0   1       //กำหนดค่าเริ่มต้นของ result = 0         
        lw      0   2   mcand   //load ค่าจาก mem -> reg[2] = mcand     
        lw      0   3   mplier  //load ค่าจาก mem -> reg[3] = mplier    
        lw      0   4   bitmas  //load ค่าจาก mem -> reg[4] = bitmask    
        lw      0   5   bitc    //load ค่าจาก mem -> reg[4] = bitcount   
        lw      0   6   neg1    //load ค่าจาก mem -> reg[6] = -1   

loop    nand    3   4   7        // สองบรรทัดนี้ใช้ดูว่าbitตัวแรกเป็น 1 หรือ 0 
        nand    7   7   7        
        beq     0   7   zero     // ถ้า current bit = 0 ให้โดดไปที่บรรทัด zero 
        add     1   2   1        // แต่ถ้าไม่ให้ทำการ(ซึ่งแปลว่ามันคือ 1) บวกตัวตั้ง(mcand)เข้ากับผลลัพธ์ result=result+mcand 

zero    add     2   2   2        // mcand left shift 1 bit  
        add     4   4   4        // bitmask left shift 1 bit
        add     6   5   5        // เอาลบหนึ่งลดจำนวน loop ลง 
        beq     5   0   end      // ถ้าจำนวน loop=0 จบการทำงาน     
        beq     0   0   loop      //ถ้าไม่ให้ไปทำ loop อีกรอบ  
        noop                    

end     halt                        

mcand   .fill   32766              
mplier  .fill   10383 
neg1    .fill   -1    
bitc    .fill   15 
bitmas  .fill   1                  
