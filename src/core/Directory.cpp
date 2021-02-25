#include "common.h"

#include "General.h"
#include "FileMgr.h"
#include "Directory.h"

CDirectory::CDirectory(int32 maxEntries)
 : numEntries(0), maxEntries(maxEntries)
{
	entries = new DirectoryInfo[maxEntries];
}

CDirectory::~CDirectory(void)
{
	delete[] entries;
}

void
CDirectory::ReadDirFile(const char *filename)
{
	int fd;
	DirectoryInfo dirinfo;

	fd = CFileMgr::OpenFile(filename, "rb");
	while(CFileMgr::Read(fd, (char*)&dirinfo, sizeof(dirinfo)))
		AddItem(dirinfo);
	CFileMgr::CloseFile(fd);
}

bool
CDirectory::WriteDirFile(const char *filename)
{
	int fd;
	size_t n;
	fd = CFileMgr::OpenFileForWriting(filename);
	n = CFileMgr::Write(fd, (char*)entries, numEntries*sizeof(DirectoryInfo));
	CFileMgr::CloseFile(fd);
	return n == numEntries*sizeof(DirectoryInfo);
}

void
CDirectory::AddItem(const DirectoryInfo &dirinfo)
{
	assert(numEntries < maxEntries);
#ifdef FIX_BUGS
	// don't add if already exists
	uint32 offset, size;
	if(FindItem(dirinfo.name, offset, size))
		return;
#endif
	entries[numEntries++] = dirinfo;
}

void
CDirectory::AddItem(const DirectoryInfo &dirinfo, int32 imgId)
{
	DirectoryInfo di = dirinfo;
	di.offset |= imgId<<24;
	AddItem(di);
}

bool
CDirectory::FindItem(const char *name, uint32 &offset, uint32 &size)
{
	int i;

	for(i = 0; i < numEntries; i++)
		if(!CGeneral::faststricmp(entries[i].name, name)){
			offset = entries[i].offset;
			size = entries[i].size;
			return true;
		}
	return false;
}
