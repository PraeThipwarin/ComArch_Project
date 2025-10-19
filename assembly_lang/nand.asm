        lw      0       1       a       ; $1 = 5
        lw      0       2       b       ; $2 = 12
        nand    1       2       3       ; $3 = ~($1 & $2)  => ควรได้ -5
        halt
a       .fill   5
b       .fill   12