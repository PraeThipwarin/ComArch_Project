        lw      0       1       sub1p      load $1 = address of sub1
        lw      0       2       ret1p      $2 -> ret1 slot
        lw      0       3       mark1p     $3 -> mark1 slot
        lw      0       4       one        $4 = 1 constant
        lw      0       5       sub2p      $5 = address of sub2
        lw      0       6       samep      $6 -> same slot (for later)
        jalr    1       7                  call sub1, expect return address in $7
        sw      2       7       0          save return address from first jalr
        lw      0       2       ret2p      $2 -> ret2 slot
        lw      0       3       mark2p     prepare mark pointer for sub2
        jalr    5       6                  call sub2, expect return address in $6
        sw      2       6       0          save return address from second jalr
        lw      0       3       errPtr     $3 -> error flag slot
        lw      0       6       samep      restore pointer to same slot
        lw      0       1       skipA      load target label for same-register jalr test
        jalr    1       1                  registers equal: should fall through and overwrite $1 with PC+1
        sw      6       1       0          record value written into $1 (should be PC+1)
        beq     0       0       done       skip over failure handler
skip    sw      3       4       0          would only run if jalr incorrectly jumped to skip
done    halt

sub1    sw      3       4       0          mark sub1 execution
        jalr    7       0                  return via $7 (keeps $0 zero)

sub2    sw      3       4       0          mark sub2 execution
        jalr    6       0                  return via $6

sub1p   .fill   sub1
sub2p   .fill   sub2
ret1p   .fill   ret1
ret2p   .fill   ret2
mark1p  .fill   mark1
mark2p  .fill   mark2
samep   .fill   same
errPtr  .fill   err
skipA   .fill   skip
one     .fill   1
ret1    .fill   0
ret2    .fill   0
mark1   .fill   0
mark2   .fill   0
same    .fill   0
err     .fill   0