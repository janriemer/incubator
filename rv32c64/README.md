# RV32C64

This is an experimental program intended to execute code written in (no-std)
Rust on a Commodore 64 equipped with a 16MB SuperCPU accelerator, or its
emulated equivalent.

Rather than going through the difficulties of writing an LLVM back-end for the
65816 processor, it was deemed easier to implement a software emulator for an
RV32I instruction set compatible virtual machine.

As a first-order approximation, the virtual machine's memory map appears below:

    $01000000   +--------------------------------+
                |                                |
                |                                |
                | RISC-V RAM                     |
                |                                |
                |                                |
    $00020000   +--------------------------------+
                |                                |
                | reserved; currently unused     |
                |                                |
    $00010000   +--------------------------------+
                | KERNAL ROM, Emulator Nucleus   |
    $0000E000   +--------------------------------+
                | I/O Resources, Em. Nucleus     |
    $0000D000   +--------------------------------+
                | Emulator Nucleus               |
    $0000C000   +--------------------------------+
                | BASIC ROM, video bitmap buffer |
    $0000A000   +--------------------------------+
                | reserved for sprites, etc.     |
    $00008000   +--------------------------------+
                | reserved; currently unused     |
    $00000B00   +--------------------------------+
                | Emulator Stack                 |
    $00000A00   +--------------------------------+
                | Emulator Direct-Page           |
    $00000900   +--------------------------------+
                | Emulator Bootstrap             |
    $00000801   +--------------------------------+
                |                                |
                | BASIC, KERNAL RAM              |
                |                                |
    $00000200   +--------------------------------+
                | 6510 Stack                     |
    $00000100   +--------------------------------+
                | 6510 Zero-Page                 |
    $00000000   +--------------------------------+

The 65816 has global access to memory.  It must react to pending interrupts
exactly as a normal 6510 processor should.  Therefore, part of the
responsibilities of the nucleus will be delegating all processing of IRQs and
NMIs generated by the Commodore 64 hardware back to the 8-bit KERNAL.

The virtual RISC-V processor only has access to the range $00020000-$00FFFFFF.
Access to memory outside of this range may cause the emulator to abort running
the RISC-V program unless documented otherwise.  Unsupported or illegal
instruction encodings will also cause program termination.

Unless documented otherwise, CSRs are **not** supported.  All reads from CSRs
in user-space will return 0.  All writes to CSRs in user-space will be ignored.
Any access to CSRs in non-user space will cause program termination.

Since the RISC-V code generally cannot access memory outside of its assigned
region in RAM, it will not be able to affect physical I/O resources.  This
includes VIC-II frame buffer or color maps, CIA registers, SID settings, etc.
Gaining access to these resources will require invoking higher-level services
of the emulator.  **How this is done remains to be determined.**

