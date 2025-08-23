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

        TRAMPOLINE L_WB_AddAppWindow
        TRAMPOLINE L_WB_RemoveAppWindow
        TRAMPOLINE L_WB_AddAppIcon
        TRAMPOLINE L_WB_RemoveAppIcon
        TRAMPOLINE L_WB_AddAppMenuItem
        TRAMPOLINE L_WB_RemoveAppMenuItem
        TRAMPOLINE L_WB_CloseWorkBench
        TRAMPOLINE L_WB_OpenWorkBench
        TRAMPOLINE L_WB_PutDiskObject
        TRAMPOLINE L_WB_DeleteDiskObject
        TRAMPOLINE L_WB_AddPort
        TRAMPOLINE L_WB_CloseWindow
        TRAMPOLINE L_PatchedCreateDir
        TRAMPOLINE L_PatchedDeleteFile
        TRAMPOLINE L_PatchedSetFileDate
        TRAMPOLINE L_PatchedSetComment
        TRAMPOLINE L_PatchedSetProtection
        TRAMPOLINE L_PatchedRename
        TRAMPOLINE L_PatchedOpen
        TRAMPOLINE L_PatchedClose
        TRAMPOLINE L_PatchedWrite
        TRAMPOLINE L_PatchedRelabel
        TRAMPOLINE L_PatchedWBInfo
        TRAMPOLINE L_PatchedAddTask
        TRAMPOLINE L_PatchedRemTask
        TRAMPOLINE L_PatchedFindTask
        TRAMPOLINE L_PatchedOpenWindowTags
        TRAMPOLINE button_dispatch
        TRAMPOLINE image_dispatch
        TRAMPOLINE listview_dispatch
        TRAMPOLINE palette_dispatch
        TRAMPOLINE pathgadget_dispatch
        TRAMPOLINE propgadget_dispatch
        TRAMPOLINE launcher_proc
        TRAMPOLINE launch_exit_code
        TRAMPOLINE progress_task
        TRAMPOLINE requester_proc
        TRAMPOLINE string_edit_hook
        TRAMPOLINE low_mem_handler

        IFD FAKEWB
        TRAMPOLINE fake_workbench
        ENDC

        IFD DEBUG
        TRAMPOLINE debug_proc
        ENDC

        END
