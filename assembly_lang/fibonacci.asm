        lw      0 1 n    // load reg[1] = n (20)
        add     0 0 3    // กำหนดค่าเริ่มต้นของ F(n-1) หรือค่าปัจจุบัน result = 0
        add     0 0 6    //reg[6\ = 0 (กำหนดค่าเริ่มต้นของ sum (Fibo(n)))
        beq     1 0 done //basecase n==0? ถ้า fibo(0) ให้จบการทำงาน 
        lw      0 3 one  //F(1) = 1
        add     6 3 6    // sum = sum + F(1)
        lw      0 4 neg1 //reg[4] = -1
        add     1 4 1    //ลดตัวนับลง 1 เนื่องจาก F(1) ถูกนับไปแล้ว
        beq     1 0 done //basecase check ถ้าตัวนับเป็น 0 (n=1)
        add     0 0 2   //กำหนด F(0) = 0, ซึ่งเป็นค่า 'ก่อนหน้า' หรือ prev
loop    add     2 3 5   //คำนวณค่าถัดไป: F(n) = F(n-2) + F(n-1)
        add     6 5 6   //สะสมผลรวม: Sum += F(n)
        add     3 0 2   //อัปเดตลำดับ: prev = current
        add     5 0 3   //อัปเดตลำดับ: current = next
        add     1 4 1   //reg[1] = reg[1] - 1 (ลดค่าตัวนับลูป)
        beq     1 0 done //check termination ถ้าตัวนับ == 0 ให้จับการทำงาน
        beq     0 0 loop //เอาไว้ทำ loop
done    halt
n         .fill   20
one       .fill   1
neg1      .fill   -1
