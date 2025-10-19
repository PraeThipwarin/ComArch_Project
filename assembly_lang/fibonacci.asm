        lw      0 1 n
        add     0 0 3
        add     0 0 6
        beq     1 0 done
        lw      0 3 one
        add     6 3 6
        lw      0 4 neg1
        add     1 4 1
        beq     1 0 done
        add     0 0 2
loop    add     2 3 5
        add     6 5 6
        add     3 0 2
        add     5 0 3
        add     1 4 1
        beq     1 0 done
        beq     0 0 loop
done    halt
n         .fill   20
one       .fill   1
neg1      .fill   -1
