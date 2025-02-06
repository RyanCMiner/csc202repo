.global my_asm_16bitset;
.global my_asm_16bitclr;
.global my_asm_16bitcheck;

my_asm_16bitset:
    ORRS R0, R1
    BX LR

my_asm_16bitclr:
    MVNS R1,R1
    ANDS R0, R1
    BX LR

my_asm_16bitcheck:
    ANDS R0,R1
    CMP R0,R1
    BEQ my_asm_16bitset
    MOVS R0,#0
    BX LR
.end
