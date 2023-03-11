# CP/RV32

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

## Specifying a Program to Run

When running, CP/RV32 will present a READY prompt, similar to but not the same
as, BASIC.  It is asking for something to do.

    **** CP/RV32 V1 ****

    READY; U8
    _

To run a program, simply type its name as it appears in the disk directory.
For example, to run the HELLO application, which does little more than print
out HELLO WORLD to the screen, type in HELLO and press enter.

    **** CP/RV32 V1 ****

    READY; U8
    HELLO
    HELLO WORLD!

    READY; U8
    _

For this to work, HELLO must reside on the disk mounted in device 8.  The U8
after the READY prompt is a reminder to the user that disk device 8 is what
will be searched when looking for commands to run.

If you want to run a program which is located on another disk device, you'll
need tell CP/RV32 on what device to look for the command.  To do this, you'll
want to use the UNIT built-in command.

    READY; U8
    UNIT 12

    READY; U12
    _

Those with CMD hard drives will be pleased to know that you can also specify a
partition number in the filename as well.  Note that this also works as
expected for those using Commodore dual-drive floppy devices (e.g., the CBM
8050) as well.

    READY; U12
    4:HELLO
    HELLO WORLD!

    READY; U12
    _

Similarly, a pathname through several subdirectories can also be specified by
placing it after the partition number.  Note that CMD's HD DOS uses slashes to
separate path name components, not colons; this is to retain compatibility with
the older CBM DOS filename syntax rules.

    READY; U12
    2//some/subdirectory/names/here/:HELLO
    HELLO WORLD!

    READY; U12
    _

My understanding of SD2IEC-based storage devices suggests that it is compatible
with the above path naming notations.

NOTE: If, for some reason, you desire to run a command literally named UNIT,
you can only do so by prefixing it with the partition number, like so:

    READY; U12
    UNIT 8

    READY; U8
    0:UNIT
    ...

If there is a space anywhere in the path name or file name, you must wrap the
whole command name in quotation marks, like so:

    READY; U12
    "//ROOTED/PATH/TO/SOME DIR W SPACE/:A FILE NAME HERE"
    ...

## Specifying Program Parameters

The entire command input buffer will be passed as-is to the program you invoke.
In this way, you can specify parameters to the program, including the name used
to invoke the command itself.

    READY; U12
    ECHO HELLO WORLD!
    HELLO WORLD!

    READY; U12
    _

It is entirely up to the invoked program to interpret any command line options
provided.  General command syntax is not specified here.
