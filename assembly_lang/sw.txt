        lw      0       1       value      load value to store
        lw      0       2       base       reg2 = address of store0
        lw      0       6       step       reg6 = +1 for pointer updates
        sw      0       1       direct     direct label store using zero register base
        sw      2       1       0          store at base + 0 (writes store0)
        sw      2       1       1          store at base + 1 (writes store1)
        add     2       6       2          advance base pointer to store1
        sw      2       1       -1         store at (store1 - 1) to exercise negative offset
        halt
value   .fill   12345
base    .fill   store0
step    .fill   1
direct  .fill   0
store0  .fill   0
store1  .fill   0
store2  .fill   0
