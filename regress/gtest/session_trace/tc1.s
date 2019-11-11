.global main
main:
    ldi r16, 0x40
    mov r17, r16
    inc r17
    mov r18, r17
    ldi r19, 0x10
    add r18, r19
    ldi r20, 0x00

endless:
    inc r20
    rjmp  endless

; EOF