        lw      0       1       n
        lw      0       2       r
        lw      0       6       pos1
        lw      0       4       addrC
        lw      0       5       Stack
        jalr    4       7
        halt
comb    sw      5       7       0
        add     5       6       5
        sw      5       1       0 
        add     5       6       5
        sw      5       2       0
        add     5       6       5
        beq     2       0       base
        beq     1       2       base     
        nand    0       0       3   
        add     1       3       1         
        lw      0       4       addrC
        jalr    4       7                  
        sw      5       3       0
        add     5       6       5
        nand    0       0       3
        add     2       3       2         
        lw      0       4       addrC
        jalr    4       7                 
        nand    0       0       4
        add     5       4       5
        lw      5       4       0
        add     3       4       3         
        beq     0       0       done     
base    lw      0       3       pos1     
done    nand    0       0       4 
        add     5       4       5  
        lw      5       2       0   
        add     5       4       5       
        lw      5       1       0       
        add     5       4       5       
        lw      5       7       0          
        jalr    7       0                                   
n       .fill   7
r       .fill   3
pos1    .fill   1
addrC   .fill   comb
Stack   .fill   stack
stack   .fill   0