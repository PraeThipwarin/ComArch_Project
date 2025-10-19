	lw      0 1 base
        lw      0 2 exp
        beq     2 0 isExp0
        
        add     1 0 3
        lw      0 7 negOne
        add     2 7 2
        beq     2 0 done

pwL     sw      0 1 tmpBse
        sw      0 2 tmpExp
        sw      0 3 tmpRes

        lw      0 2 tmpRes
        lw      0 3 tmpBse
        
        add     0 0 1
        lw      0 4 bmask
        lw      0 5 bcount
Lcheck  nand    3 4 6
        nand    6 6 6
        beq     0 6 bitis0
        add     1 2 1
bitis0  add     2 2 2
        add     4 4 4
        add     7 5 5
        beq     5 0 mltDn
        beq     0 0 Lcheck
        
mltDn   add     1 0 3

        lw      0 1 tmpBse
        lw      0 2 tmpExp

        add     2 7 2
        beq     2 0 done
        beq     0 0 pwL

isExp0   lw      0 3 one
done    halt

base        .fill   4
exp         .fill   6
one         .fill   1
negOne      .fill   -1
bmask       .fill   1
bcount      .fill   15
tmpRes      .fill   0
tmpExp      .fill   0
tmpBse      .fill   0
