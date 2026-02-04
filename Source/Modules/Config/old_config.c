/*

Galileo Amiga File-Manager and Workbench Replacement
Copyright 1993-2012 Jonathan Potter & GP Software

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

This program is based on the source code of Directory Opus Magellan II,
released by GPSoftware under the APL license in 2012. Re-licensed under GPL by
permission of Dr Greg Perry, Managing Director of GPSoftware.

Opus® and Directory Opus® and associated images are registered trademarks of GPSoftware.
DOpus(TM), DirOpus(TM) and Opus File Manager(TM) are trademarked 1991 by GPSoftware.
Opus Magellan (TM) is trademarked 1996 GPSoftware.
All other trademarks are the property of their respective owners.

The release of Directory Opus 5 under the GPL in NO WAY affects
the existing commercial status of Directory Opus for Windows.

For more information on Directory Opus for Windows please see:

                 http://www.gpsoft.com.au

*/

#include <graphics/gfx.h>
#include <intuition/intuition.h>
#include <gfm/config_return.h>
#include "config_lib.h"
#include "old_config.h"

readline(char *,int,char *,int);
void DoAssignDrive(ConfigStuff *,int,char *,char *);
void DoAssignMenu(ConfigStuff *cstuff,int men,char *name,char *func);
void DoAssignGadget(ConfigStuff *cstuff,int bk,int gad,char *name,char *func);
void DoGetDevices(ConfigStuff *cstuff);
void linkinnewfiletype(ConfigStuff *,struct galileofiletype *);
char *getstringcopy(char *str,APTR memory);

ReadOldConfig(char *name,ConfigStuff *cstuff)
{
	int a,pos,b,bk,gad;
	ULONG size;
	UWORD ver,mag;
	char *cbuf,*buf,*tbuf;
	struct galileofiletype *newtype;
	struct galileohotkey *curhotkey=0;
	GadgetBank *bank=0,*temp;
	struct Config *config;
	BPTR in;
	BPTR lock;
	struct FileInfoBlock __aligned fib;

	if (!(config=cstuff->config)) return(-1);

	if (!(lock=Lock(name,ACCESS_READ))) return(ERROR_OBJECT_NOT_FOUND);
	Examine(lock,&fib);
	UnLock(lock);
	size=fib.fib_Size-sizeof(struct Config);
	if (!(in=Open(name,MODE_OLDFILE))) return(IoErr());

	if ((Read(in,(char *)&ver,2))<2 || (Read(in,(char *)&mag,2))<2 ||
		mag!=CONFIG_MAGIC || ver==0xde) {
		Close(in);
		return(ERROR_NOT_CONFIG);
	}

	Seek(in,0,OFFSET_BEGINNING);
	if ((Read(in,(char *)config,sizeof(struct Config)))<sizeof(struct Config)) {
		Close(in);
		return(IoErr());
	}

	if (!(cstuff->memory=NewMemHandle(0,0,MEMF_CLEAR)))
	{
		Close(in);
		return ERROR_NO_FREE_STORE;
	}

	if (!(cbuf=(char *)AllocMemH(cstuff->memory,size)) ||
		!(buf=(char *)AllocMemH(cstuff->memory,4096)) ||
		!(tbuf=(char *)AllocMemH(cstuff->memory,256))) {
		Close(in);
		FreeMemHandle(cstuff->memory);
		return(ERROR_NO_FREE_STORE);
	}
	Read(in,cbuf,size);
	Close(in);

	pos=0;
	for (a=0;a<MENUCOUNT;a++) {
		pos=readline(cbuf,pos,buf,size);
		if (buf[0]) DoAssignMenu(cstuff,a,(char *)-1,buf);
		if (pos==-1) goto endthis;
	}
	for (a=0;a<MENUCOUNT;a++) {
		pos=readline(cbuf,pos,buf,size);
		if (buf[0]) DoAssignMenu(cstuff,a,buf,(char *)-1);
		if (pos==-1) goto endthis;
	}

	for (a=0;a<DRIVECOUNT;a++) {
		pos=readline(cbuf,pos,buf,size);
		if (buf[0]) DoAssignDrive(cstuff,a,(char *)-1,buf);
		if (pos==-1) goto endthis;
	}

	bk=-1;
	FOREVER {
		if (pos>=size || pos==-1) break;
		if (cbuf[pos]==5 && cbuf[pos+1]=='H') {
			struct galileohotkey *hotkey;
			pos+=2;
			if ((pos+sizeof(struct galileohotkey))>=size) break;
			if ((hotkey=AllocMemH(cstuff->memory,sizeof(struct galileohotkey)))) {
				CopyMem((char *)&cbuf[pos],(char *)hotkey,sizeof(struct galileohotkey));
				hotkey->func.function=NULL;
				hotkey->next=NULL;
			}
			pos+=sizeof(struct galileohotkey);
			if ((pos=readline(cbuf,pos,buf,size))==-1) break;
			if (hotkey) {
				hotkey->func.function=getstringcopy(buf,cstuff->memory);
				if (curhotkey) curhotkey->next=hotkey;
				else cstuff->firsthotkey=hotkey;
				curhotkey=hotkey;
			}
		}
		else if (cbuf[pos]==6 && cbuf[pos+1]=='F') {
			pos+=2;
			if ((pos+sizeof(struct wr_galileofiletype))>=size) break;
			if ((newtype=(Filetype *)AllocMemH(cstuff->memory,sizeof(Filetype)))) {
				CopyMem((char *)&cbuf[pos],(char *)newtype,sizeof(struct wr_galileofiletype));
				pos+=sizeof(struct wr_galileofiletype);
				newtype->recognition=NULL;
				for (a=0;a<FILETYPE_FUNCNUM;a++) newtype->function[a]=NULL;
				if ((pos=readline(cbuf,pos,buf,size))==-1) break;
				newtype->recognition=getstringcopy(buf,cstuff->memory);
				for (a=0;a<FILETYPE_FUNCNUM;a++) {
					if ((pos=readline(cbuf,pos,buf,size))==-1) break;
					newtype->function[a]=getstringcopy(buf,cstuff->memory);
				}
				newtype->iconpath=NULL;
				pos=readline(cbuf,pos,buf,size);
				if (buf[0]) newtype->iconpath=getstringcopy(buf,cstuff->memory);
				linkinnewfiletype(cstuff,newtype);
			}
		}
		else if (cbuf[pos]==3 && cbuf[pos+1]=='G') {
			b=sizeof(struct newgalileofmfunction)*GADCOUNT;
			pos+=2;
			if ((pos+b)>=size) break;
			++bk;
			if (!(temp=AllocMemH(cstuff->memory,sizeof(GadgetBank)))) goto endthis;
			if (!cstuff->firstbank) cstuff->firstbank=temp;
			else {
				bank->next=temp;
				temp->prev=bank;
			}
			bank=temp;

			CopyMem((char *)&cbuf[pos],(char *)bank->gadgets,sizeof(struct newgalileofmfunction)*GADCOUNT);
			pos+=(sizeof(struct newgalileofmfunction)*GADCOUNT);

			for (gad=0;gad<GADCOUNT;gad++) {
				bank->gadgets[gad].function=NULL;
				buf[0]=0;
				if (pos>-1) {
					pos=readline(cbuf,pos,buf,size);
					DoAssignGadget(cstuff,bk,gad,(char *)-1,buf);
				}
			}

			for (gad=0;gad<GADCOUNT;gad++) {
				bank->gadgets[gad].name=NULL;
				buf[0]=0;
				if (pos>-1) {
					pos=readline(cbuf,pos,buf,size);
					DoAssignGadget(cstuff,bk,gad,buf,(char *)-1);
				}
			}
			if (pos==-1) break;
		}
	}
endthis:
	return(1);
}

readline(buf,pos,buf1,size)
char *buf;
int pos;
char *buf1;
int size;
{
	int a;

	for (a=0;a<4096;a++) {
		if (size==pos || buf[pos]==0) {
			buf1[a]=0;
			if (size==pos) return(-1);
			return(pos+1);
		}
		buf1[a]=buf[pos];
		++pos;
	}
	buf1[pos]=0;
	return(pos);
}

void DoAssignGadget(ConfigStuff *cstuff,int bk,int gad,char *name,char *func)
{
	GadgetBank *bank,*temp;
	int a;

	bank=cstuff->firstbank;
	for (a=0;a<bk;a++)
	{
		if (!bank || !bank->next) break;
		bank=bank->next;
	}
	if (a<bk || !bank)
	{
		if (!(temp=AllocMemH(cstuff->memory,sizeof(GadgetBank)))) return;
		if (bank) bank->next=temp;
		else cstuff->firstbank=temp;
		bank=temp;
	}

	if (name!=(char *)-1)
	{
		if (name && name[0]) bank->gadgets[gad].name=getstringcopy(name,cstuff->memory);
	}
	if (func!=(char *)-1)
	{
		if (func && func[0]) bank->gadgets[gad].function=getstringcopy(func,cstuff->memory);
	}
}

void DoAssignMenu(ConfigStuff *cstuff,int men,char *name,char *func)
{
	struct Config *config;

	if (!(config=cstuff->config)) return;
	if (name!=(char *)-1)
	{
		if (name && name[0]) config->menu[men].name=getstringcopy(name,cstuff->memory);
	}
	if (func!=(char *)-1)
	{
		if (func && func[0]) config->menu[men].function=getstringcopy(func,cstuff->memory);
	}
}

void DoAssignDrive(cstuff,drv,name,path)
ConfigStuff *cstuff;
int drv;
char *name,*path;
{
	struct Config *config;

	if (!(config=cstuff->config)) return;
	if (name!=(char *)-1)
	{
		if (!name) config->drive[drv].name[0]=0;
		else strcpy(config->drive[drv].name,name);
	}
	if (path!=(char *)-1)
	{
		if (path && path[0]) config->drive[drv].function=getstringcopy(path,cstuff->memory);
	}
}

void linkinnewfiletype(cstuff,temp)
ConfigStuff *cstuff;
struct galileofiletype *temp;
{
	struct galileofiletype *pos;

	temp->next=NULL;
	if (!(pos=cstuff->firsttype)) cstuff->firsttype=temp;
	else {
		while (pos->next) {
			if (strcmp(pos->next->type,"Default")==0) {
				temp->next=pos->next;
				break;
			}
			pos=pos->next;
		}
		pos->next=temp;
	}
}

void FreeOldConfig(ConfigStuff *cstuff)
{
	if (cstuff) FreeMemHandle(cstuff->memory);
}

char *getstringcopy(char *str,APTR memory)
{
	char *copy;

	if (str && (copy=AllocMemH(memory,strlen(str)+1)))
	{
		strcpy(copy,str);
		return copy;
	}
	return 0;
}
