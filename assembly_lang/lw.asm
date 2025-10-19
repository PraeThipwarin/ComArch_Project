        lw      0       1       a
        lw      0       2       b
        lw      0       3       arrAdr
        lw      3       4       0
        lw      3       5       2
        lw      0       6       midAdr
        lw      6       7       -1
        halt
a       .fill   42
b       .fill   -7
arrAdr  .fill   arr
midAdr  .fill   arr1
arr     .fill   10
arr1    .fill   20
        .fill   30