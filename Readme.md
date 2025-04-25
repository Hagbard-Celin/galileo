Galileo File-Manager and Workbench replacement.
This is a GPL fork of the 2012 APL release of the legendary Amiga file manager
and Workbench replacement by GPSoftware.

Re-licensing to GPL by permission of Dr Greg Perry, Managing Director of
GPSoftware.

        ---WARNING--- 25 Apr 2025 Commit: 25c6019  ---WARNING---
         Any compiles done before this commit includes the file:
                 "Storage/Modules/recyclebin.gfmmodule".
   Including it in the compile was a mistake, as it is far from complete.
  It only works as an AppIcon that simply deletes files, and is thus quite
 dangerous if someone should mistake it for a real recycle-bin with restore
       functionality. I recommend deleting the file to avoid this.

First, a word of warning: This fork exists mainly because I chose this code-base
to as my training-ground for my latest attempt at finally learning Amiga
programming. Even-though I will make an effort to keep them confined to my
local copy of the source, stupid errors will be made. Use at you own peril.

Secondly, another warning: The version numbers are set to 0 for a reason.
There will be backward-compatibility breaking changes until the versions are
bumped to 1.


The current version has gone through substantially more testing than was the
case with v0.1. But there are still parts that has not passed more than the
the "it compiles" test. There might be dragons lurking in them dark corners.


For a almost complete list of changes since version 5.82 see "ChangeLog.txt".
Some of the highlights are:

v0.2a
- Fixed the build-system and build instructions again. Previous version still
  did not work as advertised. Thanks to riker77 on EAB for testing and
  reporting.
- No code changes. All resulting binaries are still v0.2, and should be
  identical except the the pool-functions for use on Kickstart 2.x systems
  will be taken from "amiga.lib" as opposed to the depreciated "pool.lib".

v0.2:
- Fixed build-system that actually works.
- All known memory leaks removed.
- Merged DiskInfo into IconInfo.
- Redesigned IconInfo with multiple fixes/enhancements.
- Fixed the "Discourage NewIcons" setting.
- Fixed ScreenMode Workbench:Clone setting on WB2.x, and Workbench:Clone with
  different screen-depth than WB, that I broke with my earlier fix for
  Workbench:Clone sometimes failing to open a screen.

v0.1:
- Included XADopus renamed to XADbrowser. With lots of bugfixes and 
  enhancements. Now requires xadmaster.library version 13.0.
- Plugged multiple memory leaks.
- Fixed popup menus over inactive windows in WinUAE with "Install virtual mouse
  driver" enabled.
- Fixed multiple issues with "Preform all actions on icons" setting.
- Fixed bug with screen sometimes failing to open when set to "Workbench:Clone"
- Fixed issue with main library open-count making it sometimes impossible to
  flush it from memory.
- Changed "Copy", "CopyAs", "Encrypt", "MakeLink" and "MakeLinkAs" to
  single-destination commands. And added new commands with prefix "Multi" that
  preserves the original behavior.
- P96 is now the only supported RTG-system.


Build instructions:
(For v0.2a and later only, earlier versions had multiple errors in the
build-system.)

For compiling, SAS/C 6.58 with y2kfix is required. AmigaOS NDK 3.2r4 (or
later) needs to be correctly installed/assigned. And the catcomp command from
"Tools/Catcomp" dir in NDK needs to be copied to a directory that is in the
path.

The build system is set for fastest possible compile time, and thus maximum
memory usage. If you do not have large amounts of ram, you might want to go
trough all "scoptions" files and change "MEMORYSIZE=HUGE" to
"MEMORYSIZE=LARGE", "MEMORYSIZE=MEDIUM" or "MEMORYSIZE=SMALL" depending on
the amount of available memory.

With this in place, these are the steps to compile and install:

1.  Unpack the Galileo_Source_<ver>.<rev>.lha to your preferred directory.

1b. (Optional) Copy move the directory "galileo/Build" to another location.
    And change the variable "$builddir" in the script "galileo/buildme"
    accordingly.

2.  Open a shell and cd to the "galileo" directory at the location where you
    extracted archive.

3.  Run the script "buildme".

4.  Unpack "galileo/Basedir.lha" to where you want the program installed.

5.  Copy the contents of "Build/Binary/" into the "Galileo" directory at install
    location.

5b. (Optional) Copy "<install_location>/Galileo/C/LoadGB" to "c:LoadGB".
    Add "Assign Galileo: <install_location>/Galileo/" to "s:User-Startup".
    Replace "LoadWB" with "LoadGB" at end of "s:startup-sequence".

For building the SDK only replace the script in step 3 with
"buildme Only SDK". This will fill/update the directories "SDK/source" and
"SDK/lib" from the current source.

After installation you will have in your "Galilo/C" directory the command
"IFFtoGalileo". This can be used to convert filetypes and button banks to/from
a previous install of the original version 5.82. It takes wildcards, but will
not enter sub-directories.
WARNING: It will convert files in-place, overwriting the original files.


For "Play" command to work with mod files, you will have to acquire
inovamusic.library from elsewhere and place it in "Libs:" or
"<install_location>/Galileo/Libs/".


Special thanks to Roman Kargin <kas1e@yandex.ru>, Szilard Biro and Mathias Heyer
for letting me use their code in my GPL-licensed fork.
As a thanks, special permission is hereby given to the corresponding projects
for using any of my changes to this code-base in their APL-licensed versions.

Also a special thanks to all the nice people at eab.abime.net for answering all
my stupid programming questions.


-Hagbard Celine

---

Opus® and Directory Opus® and associated images are registered trademarks of
GPSoftware.

DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by
GPSoftware.

Opus Magellan (TM) is trademarked 1996 GPSoftware.

All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

                 http://www.gpsoft.com.au
