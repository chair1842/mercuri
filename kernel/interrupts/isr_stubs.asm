extern exception_handler

; ——————————————————————————————————————————————————————————————
; ISRs without a hardware-pushed error code
; we push a dummy error code (0), then the vector number
%macro isr_no_err_stub 1
global isr_stub_%1
isr_stub_%1:
    cli
    push dword %1         ; <-- Push the vector number
    call exception_handler
    add esp, 4            ; Clean up the pushed number
    sti
    iretd
%endmacro

; ——————————————————————————————————————————————————————————————
; ISRs with a hardware-pushed error code
; CPU has already pushed the error code for these,
; so we only push the vector number
%macro isr_err_stub 1
global isr_stub_%1
isr_stub_%1:
    push dword %1      ; vector number
    call exception_handler
    add  esp, 4        ; clean up just the vector
    iret
%endmacro

extern irq_handler

%macro irq_stub 1
global irq_stub_%1
irq_stub_%1:
    cli
    push dword %1        ; IRQ number
    call irq_handler
    add  esp, 4          ; clean up
    sti
    iretd
%endmacro

; ——————————————————————————————————————————————————————————————
; Generate all 32 stubs
isr_no_err_stub 0
isr_no_err_stub 1
isr_no_err_stub 2
isr_no_err_stub 3
isr_no_err_stub 4
isr_no_err_stub 5
isr_no_err_stub 6
isr_no_err_stub 7
isr_err_stub    8
isr_no_err_stub 9
isr_err_stub    10
isr_err_stub    11
isr_err_stub    12
isr_err_stub    13
isr_err_stub    14
isr_no_err_stub 15
isr_no_err_stub 16
isr_err_stub    17
isr_no_err_stub 18
isr_no_err_stub 19
isr_no_err_stub 20
isr_no_err_stub 21
isr_no_err_stub 22
isr_no_err_stub 23
isr_no_err_stub 24
isr_no_err_stub 25
isr_no_err_stub 26
isr_no_err_stub 27
isr_no_err_stub 28
isr_no_err_stub 29
isr_err_stub    30
isr_no_err_stub 31
; Generate for IRQ0..IRQ1 (add more as needed)
irq_stub 32   ; IRQ0 → vector 32
irq_stub 33   ; IRQ1 → vector 33

; ——————————————————————————————————————————————————————————————
; Build the table of stub entry points
global isr_stub_table
section .data
isr_stub_table:
%assign i 0
%rep 32
    dd isr_stub_%+i
%assign i i+1
%endrep
%assign i 32
%rep 2
    dd irq_stub_%+i
%assign i i+1
%endrep