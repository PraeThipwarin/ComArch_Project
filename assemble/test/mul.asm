add 0 0 1
lw 0 2 mcand
lw 0 3 mplier
lw 0 4 bitmas
lw 0 5 bitc
lw 0 6 neg1
loop nand 3 4 7
nand 7 7 7
beq 0 7 zero
add 1 2 1
zero add 2 2 2
add 4 4 4
add 6 5 5
beq 5 0 end
beq 0 0 loop
noop
end halt
mcand .fill 32766
mplier .fill 10383
neg1 .fill -1
bitc .fill 15
bitmas .fill 1
