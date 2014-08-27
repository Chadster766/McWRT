/* stupid stand-alone stub - Jeremy Collake <jeremy@bitsum.com> */
/* This code GPL, as if anyone cares. */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "human_readable.h"

void int2human_usage()
{
	printf("Usage: int2human integer1 integer2 integer3 ...\n\n"
		" Where integerX is an integer to convert to human readable form. If multiple\n"
		" integers are supplied, the output is delimited by a space. Currently this\n"
		" program is limited to integers the width of the system's unsigned long. If no\n"
		" command line arguments are supplied, stdin is used.\n\n");
}

#define LAME_STATIC_SIZE 4096 	/* be very lazy and limit stdin input to 4KB */

int
#ifdef _METAPACK
int2human_main
#else
main
#endif
(int argc, char **argv)
{
	char *pszInputText;
	int nI;
	if(argc<2)
	{
		pszInputText=(char *)malloc(LAME_STATIC_SIZE+1);
		if(!fgets(pszInputText,LAME_STATIC_SIZE,stdin))
		{
			int2human_usage();
			exit(1);
		}
	}
	else
	{
		/* calculate required buffer size and allocate */
		int nReqLen=1;  /* require at least null terminator */
		for(nI=1; nI<argc;nI++)
		{
			if(!strcmp(argv[nI], "--?") || !strcmp(argv[nI], "--help"))
			{
				int2human_usage();
				exit(1);
			}
			nReqLen+=strlen(argv[nI])+1; /* extra for delimeter */
		}
		pszInputText=(char *)malloc(nReqLen);
		pszInputText[0]=0;
		for(nI=1; nI<argc;nI++)
		{
			strcat(pszInputText,argv[nI]);
			strcat(pszInputText," ");
		}
	}
	char *p;
	for(p=pszInputText; *p; )
	{
		char *pS=p;
		/* throw a null at end of field */
		for(;*p!=0x20 && *p; p++);
		*p++=0;
		printf("%s ", make_human_readable_str(strtoul(pS, NULL, 10), 1,  0));
	}
	free(pszInputText);
	printf("\n");
	exit(0);
}
