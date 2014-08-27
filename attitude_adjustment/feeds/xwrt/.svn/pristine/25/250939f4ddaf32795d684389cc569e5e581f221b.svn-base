/* silly stupid meta binary stub
 * (c)2006 Jeremy Collake, released under GPL license. */

#include <stdio.h>
#include <string.h>

typedef int (*PFN_MAIN)(int argc, char **argv);
int wepkeygen_main(int argc, char **argv);
int int2human_main(int argc, char **argv);
int bstrip_main(int argc, char **argv);
int webifpage_main(int argc, char **argv);

/* parallel arrays representing entry points of
 *  applets and corresponding applet names */
char *pszAppletNames[] =
{
	"webif-page",
	"bstrip",
	"int2human",
	"wepkeygen",
	NULL
};

PFN_MAIN ppEntryPoints[] =
{
	&webifpage_main,
	&bstrip_main,
	&int2human_main,
	&wepkeygen_main
};

int main(int argc, char **argv)
{
	int nI;
	for(nI=0; pszAppletNames[nI]; nI++)
	{
		if(strstr(argv[0], pszAppletNames[nI]))
		{
			return ppEntryPoints[nI](argc, argv);
		}
	}

	printf(" ERROR: Must symlink to a supported applet.\n");
	printf(" Applets supported are:\n");
	for(nI=0; pszAppletNames[nI]; nI++)
	{
		printf("  %s\n", pszAppletNames[nI]);
	}
	return 1;
}
