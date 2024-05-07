#ifndef _VERSION_H
#define _VERSION_H

/* Git */
#define GIT_COMMITID		"88888888"
#define GIT_COMMITDATE		"2024.4.26 15:30"

/* Version */
#define VER_MAJOR			1
#define VER_MINOR			0
#define VER_PATCH			1

/* Author */
#define AUTHOR_NAME			"Winter_Dong@ymtc.com"
#define COPYRIGHT			"YMTC @2016~2024"

#define VER_MAJOR_SHIFT		16
#define VER_MINOR_SHIFT		8
#define VER_PATCH_SHIFT		0

uint32_t GetVersion(void)
{
	return (VER_MAJOR << VER_MAJOR_SHIFT) |
			(VER_MINOR << VER_MINOR_SHIFT)|
			(VER_PATCH << VER_PATCH_SHIFT);
};

const char* GetGitCommitID(void)
{
	return GIT_COMMITID;
};

const char* GetGitCommitDate(void)
{
	return GIT_COMMITDATE;
};

const char* GetAuthor(void)
{
	return AUTHOR_NAME;
};

const char* GetCopyRight(void)
{
	return COPYRIGHT;
}

void prtToolInfo(void)
{
	printf("\nSSD Testing Utilities Set");
	printf("\n");
	printf("\nVersion\t\t: %x",GetVersion());
	printf("\nCommit ID\t: %s",GetGitCommitID());
	printf("\nCommit Date\t: %s",GetGitCommitDate());
	printf("\nAuthor\t\t: %s",GetAuthor());
	printf("\nCopyRight\t: %s",GetCopyRight());
	printf("\n");
};

#define WRAPPER_F_PRINTINFO		def("Info",prtToolInfo);
#define WRAPPER_F_VERSION		def("Version",GetVersion);
#define WRAPPER_F_COMMIDIT		def("CommitID",GetGitCommitID);
#define WRAPPER_F_COMMIDDATE	def("CommitDate",GetGitCommitDate);
#define WRAPPER_F_AUTHOR		def("Author",GetAuthor);
#define WRAPPER_F_COPYRIGHT		def("CopyRight",GetCopyRight);

#define WRAPPER_F_LIST			if(1){\
	WRAPPER_F_PRINTINFO\
	WRAPPER_F_VERSION\
	WRAPPER_F_COMMIDIT\
	WRAPPER_F_COMMIDDATE\
	WRAPPER_F_AUTHOR\
	WRAPPER_F_COPYRIGHT\
	}\

#endif
