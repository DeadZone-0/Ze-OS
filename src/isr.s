; isr.s - Interrupt Service Routine Stubs

; This macro creates a stub for an ISR which does NOT pass its own error code
; (We pass a dummy error code of 0 so the stack frame is identical for all ISRs)
%macro ISR_NOERRCODE 1
    global isr%1
    isr%1:
        cli
        push byte 0      ; Dummy error code
        push byte %1     ; Interrupt number
        jmp isr_common_stub
%endmacro

; This macro creates a stub for an ISR which DOES pass its own error code
; (Exceptions 8, 10, 11, 12, 13, 14, 17, 21 push an error code)
%macro ISR_ERRCODE 1
    global isr%1
    isr%1:
        cli
        ; The CPU pushed the error code automatically
        push byte %1     ; Interrupt number
        jmp isr_common_stub
%endmacro

; Define the 32 CPU exception ISRs
ISR_NOERRCODE  0 ; Divide By Zero Exception
ISR_NOERRCODE  1 ; Debug Exception
ISR_NOERRCODE  2 ; Non Maskable Interrupt Exception
ISR_NOERRCODE  3 ; Int 3 Exception
ISR_NOERRCODE  4 ; INTO Exception
ISR_NOERRCODE  5 ; Out of Bounds Exception
ISR_NOERRCODE  6 ; Invalid Opcode Exception
ISR_NOERRCODE  7 ; Coprocessor Not Available Exception
ISR_ERRCODE    8 ; Double Fault Exception
ISR_NOERRCODE  9 ; Coprocessor Segment Overrun Exception
ISR_ERRCODE   10 ; Bad TSS Exception
ISR_ERRCODE   11 ; Segment Not Present Exception
ISR_ERRCODE   12 ; Stack Fault Exception
ISR_ERRCODE   13 ; General Protection Fault Exception
ISR_ERRCODE   14 ; Page Fault Exception
ISR_NOERRCODE 15 ; Reserved Exception
ISR_NOERRCODE 16 ; Floating Point Exception
ISR_ERRCODE   17 ; Alignment Check Exception
ISR_NOERRCODE 18 ; Machine Check Exception
ISR_NOERRCODE 19 ; Reserved
ISR_NOERRCODE 20 ; Reserved
ISR_ERRCODE   21 ; Reserved
ISR_NOERRCODE 22 ; Reserved
ISR_NOERRCODE 23 ; Reserved
ISR_NOERRCODE 24 ; Reserved
ISR_NOERRCODE 25 ; Reserved
ISR_NOERRCODE 26 ; Reserved
ISR_NOERRCODE 27 ; Reserved
ISR_NOERRCODE 28 ; Reserved
ISR_NOERRCODE 29 ; Reserved
ISR_NOERRCODE 30 ; Reserved
ISR_NOERRCODE 31 ; Reserved

; Common ISR stub — saves registers, calls C handler, restores registers
extern isr_handler
extern irq_handler

isr_common_stub:
    pusha           ; Pushes edi,esi,ebp,esp,ebx,edx,ecx,eax

    mov ax, ds      ; Lower 16 bits of eax = ds.
    push eax        ; Save the data segment descriptor

    mov ax, 0x10    ; Load the kernel data segment descriptor
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp        ; Push pointer to the registers struct (passed to isr_handler)
    call isr_handler
    add esp, 4      ; Clean up pushed esp

    pop ebx         ; Reload the original data segment descriptor
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa            ; Restores edi,esi,ebp,esp,ebx,edx,ecx,eax
    add esp, 8      ; Cleans up the pushed error code and pushed ISR number
    sti             ; Re-enable interrupts
    iret            ; Pops cs, eip, eflags, ss, and esp

; This macro creates a stub for an IRQ
%macro IRQ 2
    global irq%1
    irq%1:
        cli
        push byte 0      ; Dummy error code
        push byte %2     ; Interrupt number
        jmp irq_common_stub
%endmacro

IRQ   0,    32
IRQ   1,    33
IRQ   2,    34
IRQ   3,    35
IRQ   4,    36
IRQ   5,    37
IRQ   6,    38
IRQ   7,    39
IRQ   8,    40
IRQ   9,    41
IRQ  10,    42
IRQ  11,    43
IRQ  12,    44
IRQ  13,    45
IRQ  14,    46
IRQ  15,    47

irq_common_stub:
    pusha

    mov ax, ds
    push eax

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    push esp
    call irq_handler
    add esp, 4

    pop ebx
    mov ds, bx
    mov es, bx
    mov fs, bx
    mov gs, bx

    popa
    add esp, 8
    sti
    iret

