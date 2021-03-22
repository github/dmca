.section        __TEXT,__text,regular,pure_instructions
.ios_version_min 11, 0
.globl  _$fcntl1                 ; -- Begin function $fcntl1
.p2align        2
_$fcntl1:                                ; @"$fcntl1"
.cfi_startproc
; %bb.0:
cmp     w0, #98                 ; =98
b.ne    LBB1_2
mov     w0, #0
ret
LBB1_2:
cmp     w0, #97                 ; =97
b.ne    LBB2_2
mov     w0, #-1
ret
LBB2_2:
adrp    x8, __fcntl1@GOTPAGE
ldr     x8, [x8, __fcntl1@GOTPAGEOFF]
ldr     x8, [x8]
br      x8
.cfi_endproc
; -- End function
.subsections_via_symbols
