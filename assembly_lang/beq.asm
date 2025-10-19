        lw      0       1       eqVal          load $1 = test value
        lw      0       2       eqVal          load $2 = same value (for taken branch)
        lw      0       3       diff           load $3 = different value (for not-taken branch)
        lw      0       4       fwdPtr         $4 -> resultForward
        lw      0       5       bkPtr          $5 -> resultBackward
        lw      0       6       const1         $6 = 1 (write marker)
        lw      0       7       notPtr         $7 -> resNTk marker
        beq     1       2       tkFwd        taken branch with positive offset
        sw      4       6       0            would run only if beq failed (expect skip)
tkFwd   sw      4       6       0              mark forward branch taken
        beq     1       3       skpNT        unequal, so branch not taken and store executes
        sw      7       6       0              mark fall-through path (branch not taken)
skpNT   beq     0       0       skpBI        unconditional skip so backStore runs only via backward branch
backS   sw      5       6       0              mark backward branch taken
        add     2       6       2              break equality so loop ends after backward hit
        beq     0       0       done          jump past backward test body
skpBI   lw      0       2       eqVal          restore $2 so $1==$2
        beq     1       2       backS         negative offset branch (taken exactly once)
done    halt
eqVal          .fill   5
diff           .fill   -3
fwdPtr         .fill   resFwd
bkPtr          .fill   resBkw
const1         .fill   1
notPtr         .fill   resNTk
resFwd         .fill   0
resBkw         .fill   0
resNTk         .fill   0
