        lw      0       1       A       ; $1 = 7
        lw      0       2       B       ; $2 = -5

        add     1       2       3       ; TC1: $3 = $1 + $2 = 7 + (-5) = 2
        add     3       1       4       ; TC2: $4 = $3 + $1 = 2 + 7 = 9
        add     0       2       5       ; TC3: $5 = $0 + $2 = copy $2  (= -5)
        add     2       0       6       ; TC4: $6 = $2 + $0 = copy $2  (= -5)
        add     0       0       7       ; TC5: $7 = 0 + 0 = 0

        halt

A       .fill   7
B       .fill   -5