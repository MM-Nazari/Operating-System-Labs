MACRO
solveW $varW, $varX, $varY, $varZ
ADD $varW, $varW, $varX, lsl #1
ADD $varW, $varY, $varY, lsl #2
SUB $varW, $varW, $varZ, lsl #2
SUB $varW, $varW, #6
MEND

