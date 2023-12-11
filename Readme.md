Galileo File-Manager and Workbench replacement.
This is a GPL fork of the 2012 APL release of the legendary Amiga file manager
and Workbench replacement by GPSoftware.

Re-licensing to GPL by permission of Dr Greg Perry, Managing Director of
GPSoftware.


First, a word of warning: This fork exists mainly because I chose this code-base
to as my training-ground for my latest attempt at finally learning Amiga
programming. Even-though I will make an effort to keep them confined to my
local copy of the source, stupid errors will be made. Use at you own peril.

Secondly, another warning: The version numbers are set to 0 for a reason.
There will be backward-compatibility breaking changes until the versions are
bumped to 1.

Also, I would have wanted to do more testing before this upload. But one of my
backup disks died, so the upload was expedited in fear further data-loss. Thus
not all parts of this fork have been tested further than that it compiles.
This goes for the FTP and recyclebin module, among other parts.


For a (almost) complete list of changes since version 5.82 see "ChangeLog.txt".
Some of the highlights are:
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
- Picasso96 is now the only supported RTG-system.


Build instructions:

For compiling, SAS/C 6.58 with y2kfix is required. Also, AmigaOS NDK 3.2 needs
to be correctly installed/assigned.

The build system is set for fastest possible compile time, and thus maximum
memory usage. If you do not have large amounts of ram, you might want to go
trough all "scoptions" files and change "MEMORYSIZE=HUGE" to
"MEMORYSIZE=LARGE", "MEMORYSIZE=MEDIUM" or "MEMORYSIZE=SMALL" depending on
the amount of available memory.

With this in place, these are the steps to compile and install:

1.  Unpack the Galileo_Source_<ver>.<rev>.lha to your preferred directory.

1b. (Optional) Copy move the directory GalileoDist/Build to another location.
    And change the variable "$builddir" in the script GalileoDist/buildme
    accordingly.

2.  Open a shell and cd to the GalileoDist directory at the location where you
    extracted archive.

3.  Run the script "buildme".

4.  Unpack "GalileoDist/Basedir.lha" to where you want the program installed.

5.  Copy the contents of "Build/Binary/" into the Galileo directory at install
    location.

5b. (Optional) Rename c:LoadWB to c:LoadWB_Old.
    Copy <install_location>/Galileo/LoadGB to c:LoadWB.
    Add "Assign Galileo: <install_location>/Galileo/" to s:User-Startup.


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
