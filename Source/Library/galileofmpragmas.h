/* "galileofm.library"                                             */
/*                                                             */
/* Support routines for Galileo and associated programs */
/* (c) Copyright 1995 Jonathan Potter                          */
/*                                                             */
#pragma libcall GalileoFMBase Random 1e 001
/**/
#pragma libcall GalileoFMBase Atoh 24 0802
#pragma libcall GalileoFMBase BtoCStr 2a 09803
#pragma libcall GalileoFMBase DivideU 30 981004
#pragma libcall GalileoFMBase Itoa 36 18003
#pragma libcall GalileoFMBase ItoaU 3c 18003
/**/
#pragma libcall GalileoFMBase BytesToString 42 218004
#pragma libcall GalileoFMBase DivideToString 48 3210805
/**/
#pragma libcall GalileoFMBase SetBusyPointer 4e 801
#pragma libcall GalileoFMBase BuildKeyString 54 8321005
#pragma libcall GalileoFMBase ActivateStrGad 5a 9802
/**/
#pragma libcall GalileoFMBase AllocTimer 60 8002
#pragma libcall GalileoFMBase FreeTimer 66 801
#pragma libcall GalileoFMBase StartTimer 6c 10803
#pragma libcall GalileoFMBase CheckTimer 72 801
#pragma libcall GalileoFMBase StopTimer 78 801
/**/
#pragma libcall GalileoFMBase GetDosPathList 7e 801
#pragma libcall GalileoFMBase FreeDosPathList 84 801
/**/
#pragma libcall GalileoFMBase DoSimpleRequest 8a 9802
#pragma libcall GalileoFMBase SimpleRequest 90 10DCBA9808
#pragma libcall GalileoFMBase SelectionList 96 DC43210BA980B
/*                                                             */
#pragma libcall GalileoFMBase WriteIcon 9c 9802
#pragma libcall GalileoFMBase WriteFileIcon a2 9802
/**/
#pragma libcall GalileoFMBase GetDragInfo a8 2109805
#pragma libcall GalileoFMBase FreeDragInfo ae 801
#pragma libcall GalileoFMBase GetDragImage b4 10803
#pragma libcall GalileoFMBase ShowDragImage ba 10803
#pragma libcall GalileoFMBase HideDragImage c0 801
#pragma libcall GalileoFMBase StampDragImage c6 10803
#pragma libcall GalileoFMBase GetDragMask cc 801
#pragma libcall GalileoFMBase CheckDragDeadlock d2 801
#pragma libcall GalileoFMBase AddDragImage d8 801
#pragma libcall GalileoFMBase RemDragImage de 801
/**/
#pragma libcall GalileoFMBase OpenConfigWindow e4 801
#pragma libcall GalileoFMBase CloseConfigWindow ea 801
/**/
#pragma libcall GalileoFMBase GetWindowMsg f0 801
#pragma libcall GalileoFMBase ReplyWindowMsg f6 801
#pragma libcall GalileoFMBase StripIntuiMessages fc 801
#pragma libcall GalileoFMBase CloseWindowSafely 102 801
/**/
#pragma libcall GalileoFMBase CalcObjectDims 108 210CBA9808
#pragma libcall GalileoFMBase CalcWindowDims 10e 0BA9805
/**/
#pragma libcall GalileoFMBase AddObjectList 114 9802
#pragma libcall GalileoFMBase FreeObject 11a 9802
#pragma libcall GalileoFMBase FreeObjectList 120 801
#pragma libcall GalileoFMBase RefreshObjectList 126 9802
/**/
#pragma libcall GalileoFMBase GetObject 12c 0802
/**/
#pragma libcall GalileoFMBase StoreGadgetValue 132 9802
#pragma libcall GalileoFMBase SetGadgetValue 138 10803
#pragma libcall GalileoFMBase GetGadgetValue 13e 9802
#pragma libcall GalileoFMBase SetObjectKind 144 10803
#pragma libcall GalileoFMBase SetGadgetChoices 14a 90803
/**/
#pragma libcall GalileoFMBase CheckObjectArea 150 10803
#pragma libcall GalileoFMBase GetObjectRect 156 90803
#pragma libcall GalileoFMBase DisplayObject 15c A109805
#pragma libcall GalileoFMBase DisableObject 162 10803
#pragma libcall GalileoFMBase BoundsCheckGadget 168 210804
/**/
#pragma libcall GalileoFMBase AddWindowMenus 16e 9802
#pragma libcall GalileoFMBase FreeWindowMenus 174 801
/**/
#pragma libcall GalileoFMBase SetWindowBusy 17a 801
#pragma libcall GalileoFMBase ClearWindowBusy 180 801
/**/
#pragma libcall GalileoFMBase GetString 186 0802
/**/
#pragma libcall GalileoFMBase FindKeyEquivalent 18c 09803
#pragma libcall GalileoFMBase ShowProgressBar 192 109804
/**/
#pragma libcall GalileoFMBase SetWindowID 198 A09804
#pragma libcall GalileoFMBase GetWindowID 19e 801
#pragma libcall GalileoFMBase GetWindowAppPort 1a4 801
/**/
#pragma libcall GalileoFMBase Att_NewList 1aa 001
#pragma libcall GalileoFMBase Att_NewNode 1b0 109804
#pragma libcall GalileoFMBase Att_RemNode 1b6 801
#pragma libcall GalileoFMBase Att_PosNode 1bc A9803
#pragma libcall GalileoFMBase Att_RemList 1c2 0802
#pragma libcall GalileoFMBase Att_FindNode 1c8 0802
#pragma libcall GalileoFMBase Att_NodeNumber 1ce 9802
#pragma libcall GalileoFMBase Att_FindNodeData 1d4 0802
#pragma libcall GalileoFMBase Att_NodeDataNumber 1da 0802
#pragma libcall GalileoFMBase Att_NodeName 1e0 0802
#pragma libcall GalileoFMBase Att_NodeCount 1e6 801
#pragma libcall GalileoFMBase Att_ChangeNodeName 1ec 9802
#pragma libcall GalileoFMBase Att_FindNodeNumber 1f2 9802
#pragma libcall GalileoFMBase AddSorted 1f8 9802
/**/
#pragma libcall GalileoFMBase BuildMenuStrip 1fe 9802
#pragma libcall GalileoFMBase FindMenuItem 204 0802
#pragma libcall GalileoFMBase DoPopUpMenu 20a 0A9804
#pragma libcall GalileoFMBase GetPopUpItem 210 0802
/*                                                             */
#pragma libcall GalileoFMBase IPC_Startup 216 A9803
#pragma libcall GalileoFMBase IPC_Command 21c BA910806
#pragma libcall GalileoFMBase IPC_Reply 222 801
#pragma libcall GalileoFMBase IPC_Free 228 801
#pragma libcall GalileoFMBase IPC_FindProc 22e 109804
#pragma libcall GalileoFMBase IPC_Quit 234 10803
#pragma libcall GalileoFMBase IPC_Hello 23a 9802
#pragma libcall GalileoFMBase IPC_Goodbye 240 09803
#pragma libcall GalileoFMBase IPC_GetGoodbye 246 801
#pragma libcall GalileoFMBase IPC_ListQuit 24c 109804
#pragma libcall GalileoFMBase IPC_Flush 252 801
#pragma libcall GalileoFMBase IPC_ListCommand 258 3210805
#pragma libcall GalileoFMBase IPC_ProcStartup 25e 9802
#pragma libcall GalileoFMBase IPC_Launch 264 B210A9807
/**/
#pragma libcall GalileoFMBase OpenImage 26a 9802
#pragma libcall GalileoFMBase CloseImage 270 801
#pragma libcall GalileoFMBase CopyImage 276 801
#pragma libcall GalileoFMBase FlushImages 27c 0
#pragma libcall GalileoFMBase RenderImage 282 A109805
#pragma libcall GalileoFMBase GetImageAttrs 288 9802
/**/
#pragma libcall GalileoFMBase NewMemHandle 28e 21003
#pragma libcall GalileoFMBase FreeMemHandle 294 801
#pragma libcall GalileoFMBase ClearMemHandle 29a 801
#pragma libcall GalileoFMBase AllocMemH 2a0 0802
#pragma libcall GalileoFMBase FreeMemH 2a6 801
/**/
#pragma libcall GalileoFMBase DrawBox 2ac 0A9804
#pragma libcall GalileoFMBase DrawFieldBox 2b2 A9803
/**/
#pragma libcall GalileoFMBase NewLister 2b8 801
#pragma libcall GalileoFMBase NewButtonBank 2be 1002
#pragma libcall GalileoFMBase NewButton 2c4 801
#pragma libcall GalileoFMBase NewFunction 2ca 0802
#pragma libcall GalileoFMBase NewInstruction 2d0 90803
#pragma libcall GalileoFMBase ReadSettings 2d6 9802
#pragma libcall GalileoFMBase ReadListerDef 2dc 0802
#pragma libcall GalileoFMBase OpenButtonBank 2e2 801
#pragma libcall GalileoFMBase DefaultSettings 2e8 801
#pragma libcall GalileoFMBase DefaultEnvironment 2ee 801
#pragma libcall GalileoFMBase DefaultButtonBank 2f4 0
#pragma libcall GalileoFMBase SaveSettings 2fa 9802
#pragma libcall GalileoFMBase SaveListerDef 300 9802
#pragma libcall GalileoFMBase SaveButtonBank 306 9802
#pragma libcall GalileoFMBase CloseButtonBank 30c 801
#pragma libcall GalileoFMBase FreeListerDef 312 801
#pragma libcall GalileoFMBase FreeButtonList 318 801
#pragma libcall GalileoFMBase FreeButtonImages 31e 801
#pragma libcall GalileoFMBase FreeButton 324 801
#pragma libcall GalileoFMBase FreeFunction 32a 801
#pragma libcall GalileoFMBase FreeInstruction 330 801
#pragma libcall GalileoFMBase FreeInstructionList 336 801
#pragma libcall GalileoFMBase CopyButtonBank 33c 801
#pragma libcall GalileoFMBase CopyButton 342 09803
#pragma libcall GalileoFMBase CopyFunction 348 A9803
#pragma libcall GalileoFMBase NewFiletype 34e 801
#pragma libcall GalileoFMBase ReadFiletypes 354 9802
#pragma libcall GalileoFMBase SaveFiletypeList 35a 9802
#pragma libcall GalileoFMBase FreeFiletypeList 360 801
#pragma libcall GalileoFMBase FreeFiletype 366 801
#pragma libcall GalileoFMBase CopyFiletype 36c 9802
#pragma libcall GalileoFMBase FindFunctionType 372 0802
#pragma libcall GalileoFMBase SaveButton 378 9802
#pragma libcall GalileoFMBase ReadButton 37e 9802
#pragma libcall GalileoFMBase ReadFunction 384 BA9804
/**/
#pragma libcall GalileoFMBase OpenStatusWindow 38a 01A9805
#pragma libcall GalileoFMBase SetStatusText 390 9802
#pragma libcall GalileoFMBase UpdateStatusGraph 396 109804
/**/
#pragma libcall GalileoFMBase ReadILBM 39c 0802
#pragma libcall GalileoFMBase FreeILBM 3a2 801
#pragma libcall GalileoFMBase DecodeILBM 3a8 439210807
#pragma libcall GalileoFMBase DecodeRLE 3ae 801
/**/
#pragma libcall GalileoFMBase LoadPalette32 3b4 9802
#pragma libcall GalileoFMBase GetPalette32 3ba 109804
/**/
#pragma libcall GalileoFMBase OpenBuf 3c0 10803
#pragma libcall GalileoFMBase CloseBuf 3c6 801
#pragma libcall GalileoFMBase ReadBuf 3cc 09803
#pragma libcall GalileoFMBase WriteBuf 3d2 09803
#pragma libcall GalileoFMBase FlushBuf 3d8 801
#pragma libcall GalileoFMBase SeekBuf 3de 10803
#pragma libcall GalileoFMBase ExamineBuf 3e4 9802
/**/
#pragma libcall GalileoFMBase OpenDisk 3ea 9802
#pragma libcall GalileoFMBase CloseDisk 3f0 801
/**/
#pragma libcall GalileoFMBase AddScrollBars 3f6 0A9804
#pragma libcall GalileoFMBase FindBOOPSIGadget 3fc 0802
#pragma libcall GalileoFMBase BOOPSIFree 402 801
/**/
#pragma libcall GalileoFMBase WB_Install_Patch 408 0
#pragma libcall GalileoFMBase WB_Remove_Patch 40e 0
#pragma libcall GalileoFMBase WB_AddAppWindow 414 A981005
#pragma libcall GalileoFMBase WB_RemoveAppWindow 41a 801
#pragma libcall GalileoFMBase WB_FindAppWindow 420 801
#pragma libcall GalileoFMBase WB_AppWindowData 426 A9803
#pragma libcall GalileoFMBase WB_AppWindowLocal 42c 801
#pragma libcall GalileoFMBase LockAppList 432 0
#pragma libcall GalileoFMBase NextAppEntry 438 0802
#pragma libcall GalileoFMBase UnlockAppList 43e 0
/**/
#pragma libcall GalileoFMBase AddNotifyRequest 444 81003
#pragma libcall GalileoFMBase RemoveNotifyRequest 44a 801
#pragma libcall GalileoFMBase SendNotifyMsg 450 98321006
/**/
#pragma libcall GalileoFMBase StrCombine 456 0A9804
#pragma libcall GalileoFMBase StrConcat 45c 09803
/**/
#pragma libcall GalileoFMBase WB_Launch 462 09803
/**/
#pragma libcall GalileoFMBase CopyLocalEnv 468 801
/**/
#pragma libcall GalileoFMBase CLI_Launch 46e 432109807
/**/
#pragma libcall GalileoFMBase ReplyFreeMsg 474 801
/**/
#pragma libcall GalileoFMBase TimerActive 47a 801
/**/
#pragma libcall GalileoFMBase NewButtonFunction 480 0802
/**/
#pragma libcall GalileoFMBase IFFOpen 486 10803
#pragma libcall GalileoFMBase IFFClose 48c 801
#pragma libcall GalileoFMBase IFFPushChunk 492 0802
#pragma libcall GalileoFMBase IFFWriteChunkBytes 498 09803
#pragma libcall GalileoFMBase IFFPopChunk 49e 801
#pragma libcall GalileoFMBase IFFWriteChunk 4a4 109804
/**/
#pragma libcall GalileoFMBase FindNameI 4aa 9802
/**/
#pragma libcall GalileoFMBase AnimDecodeRIFFXor 4b0 109804
#pragma libcall GalileoFMBase AnimDecodeRIFFSet 4b6 109804
/**/
#pragma libcall GalileoFMBase ConvertRawKey 4bc 81003
/**/
#pragma libcall GalileoFMBase OpenClipBoard 4c2 001
#pragma libcall GalileoFMBase CloseClipBoard 4c8 801
#pragma libcall GalileoFMBase WriteClipString 4ce 09803
#pragma libcall GalileoFMBase ReadClipString 4d4 09803
/**/
#pragma libcall GalileoFMBase LockAttList 4da 0802
#pragma libcall GalileoFMBase UnlockAttList 4e0 801
/**/
#pragma libcall GalileoFMBase GetSemaphore 4e6 90803
#pragma libcall GalileoFMBase FreeSemaphore 4ec 801
#pragma libcall GalileoFMBase ShowSemaphore 4f2 801
/**/
#pragma libcall GalileoFMBase SaveFunction 4f8 9802
/**/
#pragma libcall GalileoFMBase IFFNextChunk 4fe 0802
#pragma libcall GalileoFMBase IFFChunkSize 504 801
#pragma libcall GalileoFMBase IFFReadChunkBytes 50a 09803
#pragma libcall GalileoFMBase IFFFileHandle 510 801
#pragma libcall GalileoFMBase IFFChunkRemain 516 801
#pragma libcall GalileoFMBase IFFChunkID 51c 801
#pragma libcall GalileoFMBase IFFGetFORM 522 801
/**/
#pragma libcall GalileoFMBase ScreenInfo 528 801
/**/
#pragma libcall GalileoFMBase GetEditHook 52e 81003
#pragma libcall GalileoFMBase FreeEditHook 534 801
/**/
#pragma libcall GalileoFMBase InitWindowDims 53a 9802
#pragma libcall GalileoFMBase StoreWindowDims 540 9802
#pragma libcall GalileoFMBase CheckWindowDims 546 9802
/**/
#pragma libcall GalileoFMBase InitListLock 54c 9802
/**/
#pragma libcall GalileoFMBase IPC_QuitName 552 09803
/**/
#pragma libcall GalileoFMBase QualValid 558 001
/**/
#pragma libcall GalileoFMBase FHFromBuf 55e 801
/**/
#pragma libcall GalileoFMBase WB_AppIconFlags 564 801
/**/
#pragma libcall GalileoFMBase GetWBArgPath 56a 09803
/**/
#pragma libcall GalileoFMBase DeviceFromLock 570 9802
#pragma libcall GalileoFMBase DeviceFromHandler 576 9802
#pragma libcall GalileoFMBase DevNameFromLock 57c 32103
/**/
#pragma libcall GalileoFMBase GetIconFlags 582 801
#pragma libcall GalileoFMBase SetIconFlags 588 0802
#pragma libcall GalileoFMBase GetIconPosition 58e A9803
#pragma libcall GalileoFMBase SetIconPosition 594 10803
/**/
#pragma libcall GalileoFMBase BuildTransDragMask 59a 32109806
/**/
#pragma libcall GalileoFMBase GetImagePalette 5a0 801
#pragma libcall GalileoFMBase FreeImageRemap 5a6 801
/**/
#pragma libcall GalileoFMBase SwapListNodes 5ac A9803
/**/
#pragma libcall GalileoFMBase Seed 5b2 001
/**/
#pragma libcall GalileoFMBase CopyDiskObject 5b8 0802
#pragma libcall GalileoFMBase FreeDiskObjectCopy 5be 801
/**/
#pragma libcall GalileoFMBase IFFFailure 5c4 801
/**/
#pragma libcall GalileoFMBase GetCachedDefDiskObject 5ca 001
#pragma libcall GalileoFMBase FreeCachedDiskObject 5d0 801
#pragma libcall GalileoFMBase GetCachedDiskObject 5d6 0802
#pragma libcall GalileoFMBase GetCachedDiskObjectNew 5dc 0802
#pragma libcall GalileoFMBase IconCheckSum 5e2 0802
/**/
#pragma libcall GalileoFMBase OpenProgressWindow 5e8 801
#pragma libcall GalileoFMBase CloseProgressWindow 5ee 801
#pragma libcall GalileoFMBase HideProgressWindow 5f4 801
#pragma libcall GalileoFMBase ShowProgressWindow 5fa A9803
#pragma libcall GalileoFMBase SetProgressWindow 600 9802
#pragma libcall GalileoFMBase GetProgressWindow 606 9802
/**/
#pragma libcall GalileoFMBase SetNotifyRequest 60c 10803
/**/
#pragma libcall GalileoFMBase ChangeAppIcon 612 0BA9805
/**/
#pragma libcall GalileoFMBase CheckProgressAbort 618 801
/**/
#pragma libcall GalileoFMBase GetSecureString 61e 801
/**/
#pragma libcall GalileoFMBase NewButtonWithFunc 624 09803
/**/
#pragma libcall GalileoFMBase FreeButtonFunction 62a 801
#pragma libcall GalileoFMBase CopyButtonFunction 630 A9803
/**/
#pragma libcall GalileoFMBase FindPubScreen 636 0802
/**/
#pragma libcall GalileoFMBase SetAppIconMenuState 63c 10803
/**/
#pragma libcall GalileoFMBase SearchFile 642 1A09805
#pragma libcall GalileoFMBase ParseDateStrings 648 BA9804
#pragma libcall GalileoFMBase DateFromStrings 64e A9803
/**/
#pragma libcall GalileoFMBase GetMatchHandle 654 801
#pragma libcall GalileoFMBase FreeMatchHandle 65a 801
#pragma libcall GalileoFMBase MatchFiletype 660 9802
/**/
#pragma libcall GalileoFMBase LayoutResize 666 801
/**/
#pragma libcall GalileoFMBase GetFileVersion 66c A910805
/**/
#pragma libcall GalileoFMBase AsyncRequest 672 1BA90806
#pragma libcall GalileoFMBase CheckRefreshMsg 678 0802
/**/
#pragma libcall GalileoFMBase RemapImage 67e A9803
#pragma libcall GalileoFMBase FreeRemapImage 684 9802
/**/
#pragma libcall GalileoFMBase FreeAppMessage 68a 801
#pragma libcall GalileoFMBase ReplyAppMessage 690 801
/**/
#pragma libcall GalileoFMBase SetLibraryFlags 696 1002
/**/
#pragma libcall GalileoFMBase StartRefreshConfigWindow 69c 0802
#pragma libcall GalileoFMBase EndRefreshConfigWindow 6a2 801
/**/
#pragma libcall GalileoFMBase CompareListFormat 6a8 9802
/**/
#pragma libcall GalileoFMBase UpdateGadgetValue 6ae 09803
#pragma libcall GalileoFMBase UpdateGadgetList 6b4 801
/**/
#pragma libcall GalileoFMBase NewBitMap 6ba 8321005
#pragma libcall GalileoFMBase DisposeBitMap 6c0 801
/**/
#pragma libcall GalileoFMBase ParseArgs 6c6 9802
#pragma libcall GalileoFMBase DisposeArgs 6cc 801
/**/
#pragma libcall GalileoFMBase SetConfigWindowLimits 6d2 A9803
/**/
#pragma libcall GalileoFMBase SetEnv 6d8 09803
#pragma libcall GalileoFMBase IsListLockEmpty 6de 801
/**/
#pragma libcall GalileoFMBase AllocAppMessage 6e4 09803
#pragma libcall GalileoFMBase CheckAppMessage 6ea 801
#pragma libcall GalileoFMBase CopyAppMessage 6f0 9802
#pragma libcall GalileoFMBase SetWBArg 6f6 A910805
/**/
#pragma libcall GalileoFMBase OriginalCreateDir 6fc 101
#pragma libcall GalileoFMBase OriginalDeleteFile 702 101
#pragma libcall GalileoFMBase OriginalSetFileDate 708 2102
#pragma libcall GalileoFMBase OriginalSetComment 70e 2102
#pragma libcall GalileoFMBase OriginalSetProtection 714 2102
#pragma libcall GalileoFMBase OriginalRename 71a 2102
#pragma libcall GalileoFMBase OriginalOpen 720 2102
#pragma libcall GalileoFMBase OriginalClose 726 101
#pragma libcall GalileoFMBase OriginalWrite 72c 32103
/**/
#pragma libcall GalileoFMBase CreateTitleGadget 732 32109806
#pragma libcall GalileoFMBase FindGadgetType 738 0802
#pragma libcall GalileoFMBase FixTitleGadgets 73e 801
/**/
#pragma libcall GalileoFMBase OriginalRelabel 744 2102
/**/
#pragma libcall GalileoFMBase FakeILBM 74a 32109806
/**/
#pragma libcall GalileoFMBase IPC_SafeCommand 750 CBA910807
/**/
#pragma libcall GalileoFMBase ClearFiletypeCache 756 0
/**/
#pragma libcall GalileoFMBase GetTimerBase 75c 0
/**/
#pragma libcall GalileoFMBase InitDragDBuf 762 801
/**/
#pragma libcall GalileoFMBase FreeRexxMsgEx 768 801
#pragma libcall GalileoFMBase CreateRexxMsgEx 76e 09803
#pragma libcall GalileoFMBase SetRexxVarEx 774 109804
#pragma libcall GalileoFMBase GetRexxVarEx 77a A9803
#pragma libcall GalileoFMBase BuildRexxMsgEx 780 A09804
/**/
#pragma libcall GalileoFMBase NotifyDiskChange 786 0
#pragma libcall GalileoFMBase GetDosListCopy 78c 9802
#pragma libcall GalileoFMBase FreeDosListCopy 792 801
/**/
#pragma libcall GalileoFMBase DateFromStringsNew 798 0A9804
/**/
#pragma libcall GalileoFMBase RemapIcon 79e 09803
#pragma libcall GalileoFMBase GetOriginalIcon 7a4 801
/**/
#pragma libcall GalileoFMBase CalcPercent 7aa 81003
/**/
#pragma libcall GalileoFMBase IsDiskDevice 7b0 801
/**/
#pragma libcall GalileoFMBase DrawDragList 7b6 09803
#pragma libcall GalileoFMBase RemoveDragImage 7bc 801
/**/
#pragma libcall GalileoFMBase SetNewIconsFlags 7c2 1002
/**/
#pragma libcall GalileoFMBase ReadBufLine 7c8 09803
/**/
#pragma libcall GalileoFMBase GetLibraryFlags 7ce 0
/**/
#pragma libcall GalileoFMBase GetIconType 7d4 801
/**/
#pragma libcall GalileoFMBase SetReqBackFill 7da 9802
#pragma libcall GalileoFMBase LockReqBackFill 7e0 801
#pragma libcall GalileoFMBase UnlockReqBackFill 7e6 0
/**/
#pragma libcall GalileoFMBase DragCustomOk 7ec 801
/**/
#pragma libcall GalileoFMBase WB_LaunchNew 7f2 A109805
/**/
#pragma libcall GalileoFMBase UpdatePathList 7f8 0
#pragma libcall GalileoFMBase UpdateMyPaths 7fe 0
/**/
#pragma libcall GalileoFMBase GetPopUpImageSize 804 BA9804
/**/
#pragma libcall GalileoFMBase GetDeviceUnit 80a A9803
/**/
#pragma libcall GalileoFMBase StripWindowMessages 810 9802
/**/
#pragma libcall GalileoFMBase DeleteIcon 816 801
/**/
#pragma libcall GalileoFMBase MUFSLogin 81c A9803
/**/
/**/
#pragma libcall GalileoFMBase UpdateEnvironment 822 801
/**/
#pragma libcall GalileoFMBase ConvertStartMenu 828 801
/**/
#pragma libcall GalileoFMBase GetGalileoPathList 82e 0
/**/
#pragma libcall GalileoFMBase GetStatistics 834 001
/**/
#pragma libcall GalileoFMBase SetPopUpDelay 83a 001
/**/
#pragma libcall GalileoFMBase WB_LaunchNotify 840 2CBA109808
/**/
/**/
#pragma libcall GalileoFMBase WB_AppWindowWindow 846 801
/**/
#pragma libcall GalileoFMBase OpenEnvironment 84c 9802
/**/
/**/
/**/
#pragma libcall GalileoFMBase PopUpNewHandle 852 98003
#pragma libcall GalileoFMBase PopUpFreeHandle 858 801
#pragma libcall GalileoFMBase PopUpNewItem 85e 210804
#pragma libcall GalileoFMBase PopUpSeparator 864 801
#pragma libcall GalileoFMBase PopUpItemSub 86a 9802
#pragma libcall GalileoFMBase PopUpEndSub 870 801
#pragma libcall GalileoFMBase PopUpSetFlags 876 210804
/**/
/**/
#pragma libcall GalileoFMBase LoadPos 87c 09803
#pragma libcall GalileoFMBase SavePos 882 09803
/**/
/**/
/**/
/**/
/* end "galileofm.library" pragmas */
