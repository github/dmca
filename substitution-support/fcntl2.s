.section        __TEXT,__text,regular,pure_instructions
.ios_version_min 11, 0
.globl  _$fcntl2                 ; -- Begin function $fcntl2
.p2align        2
_$fcntl2:                                ; @"$fcntl2"
.cfi_startproc
; %bb.0:
cmp     w0, #98                 ; =98
b.ne    LBB1_2
bl ___error
mov x8, x0
movz w0, #0x0
str wzr, [x8]
mov     w0, #0
ret
LBB1_2:
adrp    x8, __fcntl2@GOTPAGE
ldr     x8, [x8, __fcntl2@GOTPAGEOFF]
ldr     x8, [x8]
br      x8
.cfi_endproc
; -- End function
.subsections_via_symbols
