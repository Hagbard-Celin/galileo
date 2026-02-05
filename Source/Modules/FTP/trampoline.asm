              SECTION      text,CODE

	XREF _MyLibBase

TRAMPOLINE MACRO

        XREF _\1
        XDEF _\1Tr


_\1Tr:
        move.l a6,-(sp)
        move.l _MyLibBase,a6
        jsr _\1
        move.l (sp)+,a6
        rts

	ENDM

        TRAMPOLINE addressbook
        TRAMPOLINE format_code
        TRAMPOLINE lister
        TRAMPOLINE galileo_ftp
        TRAMPOLINE addressbook_init
        TRAMPOLINE ftplister_init

        END
