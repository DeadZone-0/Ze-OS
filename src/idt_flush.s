; idt_flush.s - Load IDTR

global idt_flush

idt_flush:
    mov eax, [esp + 4]
    lidt [eax]
    ret
