#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string.h>

#include "InspectXml.h"
#include "CreateDOM.h"

// Disable all of these warnings that 'stb.h' still generates
#pragma warning(disable:4996 4244 4100 4267 4189 4456 4311 4302 4312 4838 4505 4701 4703)
#define STB_DEFINE
#include "stb.h"

char *lastSlash(char *scan)
{
	char *ret = NULL;
	while (*scan)
	{
		if (*scan == '\\' || *scan == '/')
		{
			ret = scan;
		}
		scan++;
	}
	return ret;
}

const char *getDirName(const char *fname)
{
	const char *ret = NULL;

	static char scratch[512];
	strncpy(scratch, fname, 512);

	char *slash = lastSlash(scratch);
	if (slash)
	{
		*slash = 0;
		ret = scratch;
	}

	return ret;
}

__int64 pathDir(const char *spec,
				std::vector< std::string > &flist,
				std::vector< std::string > &dlist)
{
	__int64 fileSize = 0;

	printf("Registering all files in the current directory and sub-directories that matches '%s'.\n", spec);
	char **files = stb_readdir_recursive((char *)"./", (char *)spec);
	if (files)
	{
		int count = stb_arr_len(files);
		printf("Found %d  files.\n", count);
		for (int i = 0; i < count; i++)
		{
			char *fname = files[i];
			FILE *fph = fopen(fname, "rb");
			if (fph)
			{
				std::string str = fname;
				flist.push_back(str);
				fseek(fph, 0L, SEEK_END);
				__int64 s = ftell(fph);
				fileSize += s;
				fclose(fph);
			}
		}
		stb_readdir_free(files);
	}

	for (size_t i = 0; i < flist.size(); i++)
	{
		const char *fname = flist[i].c_str();
		const char *dirName = getDirName(fname);
		if (dirName)
		{
			std::string str(dirName);
			bool found = false;
			for (size_t i = 0; i < dlist.size(); i++)
			{
				if (str == dlist[i])
				{
					found = true;
					break;
				}
			}
			if (!found)
			{
				dlist.push_back(str);
			}
		}
	}

	return fileSize;
}

#define CREATE_DOM_FILE 0

int main(int argc,const char **argv)
{
	if ( argc != 2 )
	{
		printf("Usage: InpsectXML <fname*.xml>\r\n");
		printf("\r\n");
		printf("Will scan this directory and all sub-directories for files matching the spec provided.\r\n");
		printf("This will then parse each XML file matching and collate a list of all unique element and attribute names.\r\n");
		printf("The results will be saved to a file call 'xml.cpp'\r\n");
	}
#if CREATE_DOM_FILE
	else
	{
		const char *spec = argv[1];
		printf("Searching for files match: %s\r\n", spec);
		std::vector< std::string > flist;
		std::vector< std::string > dlist;
		pathDir(spec, flist, dlist);
		printf("Found: %d matching files in %d sub-directories.\r\n", uint32_t(flist.size()), uint32_t(dlist.size()));
		CREATE_DOM::CreateDOM *ixml = CREATE_DOM::CreateDOM::create();
		if (ixml)
		{
			for (auto &i : flist)
			{
				printf("Inspecting: %s\r\n", i.c_str());
				ixml->inspectXml(i.c_str());
			}
			printf("Saving 'DOM.csv'\r\n");
			ixml->saveDOM("DOM.csv");
		}
	}
#else
	else
	{
		const char *spec = argv[1];
		printf("Searching for files match: %s\r\n", spec);
		std::vector< std::string > flist;
		std::vector< std::string > dlist;
		pathDir(spec, flist, dlist);
		printf("Found: %d matching files in %d sub-directories.\r\n", uint32_t(flist.size()), uint32_t(dlist.size()));
		INSPECT_XML::InspectXml *ixml = INSPECT_XML::InspectXml::create();
		if (ixml)
		{
			for (auto &i : flist)
			{
				printf("Inspecting: %s\r\n", i.c_str());
				ixml->inspectXml(i.c_str());
			}
			printf("Saving 'xml.cpp'\r\n");
			ixml->saveSourceCode("xml.cpp");
		}
	}
#endif
}
