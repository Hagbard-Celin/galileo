#include 	<proto/dos.h>
#include    <proto/exec.h>

#include <gfm/version.h>

UBYTE *vers = "\0$VER: IFFtoGalileo "__VERSTR__" "__AMIGADATE__" "__SUBTITLE__;

#define TEMPLATE "FILES/A/M,QUIET/S,REVERSE/S"
#define OPT_FILES  	0
#define OPT_QUIET  	1
#define OPT_REVERSE	2
#define OPT_COUNT  	3

extern struct ExecBase *SysBase;

LONG result[OPT_COUNT];
struct RDArgs *rda;

void convertFiles(char *pattern, BOOL quiet, BOOL reverse);
void convertFile(char *file, BOOL quiet, BOOL reverse);

int main(void)
{
    UWORD x;
    char **files;
    BOOL  quiet=0, reverse=0;

    /* Need to ask DOS for a RDArgs structure */
    if (rda = ReadArgs(TEMPLATE, result, NULL))
    {
        if (result[OPT_QUIET])
            quiet=1;

        if (result[OPT_REVERSE])
            reverse=1;

        files = (char **)result[OPT_FILES];
        if (!(quiet)) VPrintf("Files specified:\n", NULL);
        for (x=0; files[x]; x++)
            convertFiles(files[x], quiet, reverse);

        FreeArgs(rda);

        return 0;
    }

    return 1;
}

void convertFiles(char *pattern, BOOL quiet, BOOL reverse)
{
   	struct AnchorPath *anchor;
   	BPTR origDir;

   	/*Allocate a structure required by MatchFirst()*/
   	anchor=(struct AnchorPath *)AllocMem(sizeof(struct AnchorPath),MEMF_CLEAR);
   	if(anchor==NULL)
      	return;

   	/*Get first file*/
   	if(MatchFirst(pattern,anchor)==0)
   	{
      	/*Convert*/
      	origDir=CurrentDir(anchor->ap_Current->an_Lock);
      	convertFile(anchor->ap_Info.fib_FileName, quiet, reverse);
      	CurrentDir(origDir);

      	/*Convert the rest of the files*/
      	while(MatchNext(anchor)==0)
      	{
	 		origDir=CurrentDir(anchor->ap_Current->an_Lock);
	 		convertFile(anchor->ap_Info.fib_FileName, quiet, reverse);
	 		CurrentDir(origDir);
      	}
   	}
    else if (!(quiet)) VPrintf("File not found: %s\n", &pattern);

 	/*All done*/
    MatchEnd(anchor);
   	FreeMem(anchor,sizeof(struct AnchorPath));

	return;
}

void convertFile(char *file, BOOL quiet, BOOL reverse)
{
	BPTR fh=0;

    if (!(quiet)) VPrintf("\t%s\n", &file);
    if (fh=Open(file, MODE_OLDFILE))
    {
    	ULONG formbuffer[1]={0};

        Read(fh, formbuffer, 4);
        if (formbuffer[0]==0x464F524D)
        {
            ULONG idbuffer[1]={0};
        	
            Seek(fh, 4, OFFSET_CURRENT);
            Read(fh, idbuffer, 4);
            if (idbuffer[0]==0x4F505553 && !reverse)
            {
            	ULONG newidbuffer[1]={0x47494C4F};

				Seek(fh, -4, OFFSET_CURRENT);
				Write(fh, newidbuffer, 4);
	        }
            else
            if (idbuffer[0]==0x47494C4F && reverse)
            {
            	ULONG newidbuffer[1]={0x4F505553};

				Seek(fh, -4, OFFSET_CURRENT);
				Write(fh, newidbuffer, 4);
	        }
		    else if (!(quiet)) VPrintf("Wrong FORM file: %s\n", &file);
        }
        else if (!(quiet)) VPrintf("Not an IFF file: %s\n", &file);

        Close(fh);

    }
	else if (!(quiet)) VPrintf("Could not open %s (Maybe a directory?)\n", &file);

}

