    .intel_syntax noprefix
    .section .text
    .global isr_stub_table

    // Generate 16 IRQ entries (vectors 32–47) for example:
    .macro ISR_STUB vec
    .global isr\vec
    isr\vec:
        push    \vec            // push vector number
        jmp     isr_common_stub
    .endm

    // CPU exceptions (0–31) could use the same macro if you want to handle them in C,
    // e.g. ISR_STUB 0, ISR_STUB 1, ..., ISR_STUB 31

    // Remapped IRQs 32 through 47
    ISR_STUB 32
    ISR_STUB 33
    ISR_STUB 34
    ISR_STUB 35
    ISR_STUB 36
    ISR_STUB 37
    ISR_STUB 38
    ISR_STUB 39
    ISR_STUB 40
    ISR_STUB 41
    ISR_STUB 42
    ISR_STUB 43
    ISR_STUB 44
    ISR_STUB 45
    ISR_STUB 46
    ISR_STUB 47

    .global isr_common_stub
isr_common_stub:
    cli
    // Save general‐purpose registers
    push    eax
    push    ebx
    push    ecx
    push    edx
    push    esi
    push    edi
    push    ebp

    mov     ebp, esp
    call    isr_handler_c      // C handler (you can pop vector there)

    mov     esp, ebp
    // Restore registers
    pop     ebp
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     eax

    //
    // Pop the vector number into EAX, so we know which IRQ fired:
    //
    pop     eax                // EAX = vector pushed by stub
    // If vector ≥ 32, it’s an IRQ from the PIC:
    cmp     eax, 32
    jl      .no_eoi
    // Send EOI to master PIC
    mov     al, 0x20           // EOI command
    out     0x20, al           // outb AL to port 0x20
    // If vector ≥ 40, also send EOI to slave PIC
    cmp     eax, 40
    jl      .no_eoi
    out     0xA0, al           // outb AL to port 0xA0

isr_stub_table:
    /* .long isr0
    .long isr1
    .long isr2
    .long isr3
    .long isr4
    .long isr5
    .long isr6
    .long isr7
    .long isr8
    .long isr9
    .long isr10
    .long isr11
    .long isr12
    .long isr13
    .long isr14
    .long isr15
    .long isr16
    .long isr17
    .long isr18
    .long isr19
    .long isr20
    .long isr21
    .long isr22
    .long isr23
    .long isr24
    .long isr25
    .long isr26
    .long isr27
    .long isr28
    .long isr29
    .long isr30
    .long isr31 */
    .long isr32
    .long isr33
    .long isr34
    .long isr35
    .long isr36
    .long isr37
    .long isr38
    .long isr39
    .long isr40
    .long isr41
    .long isr42
    .long isr43
    .long isr44
    .long isr45
    .long isr46
    .long isr47

.no_eoi:
    sti
    iret
