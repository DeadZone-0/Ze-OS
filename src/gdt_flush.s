; gdt_flush.s - Load GDTR and reload segment registers

global gdt_flush

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    ; Reload data segments with kernel data selector (0x10)
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Far jump to reload CS with kernel code selector (0x08)
    jmp 0x08:.flush
.flush:
    ret
