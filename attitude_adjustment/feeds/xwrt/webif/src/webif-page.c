/*
 * Webif page translator
 *
 * Copyright (C) 2005 Felix Fietkau <nbd@openwrt.org>
 * Copyright (c) 2007 Jeremy Collake <jeremy@bitsum.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>
#ifdef NVRAM
#include <bcmnvram.h>
#endif

#define HASH_MAX	1536
#define LINE_BUF	2048	/* max. buffer allocated for one line */
#define MAX_TR		24	/* max. translations done on one line */
#define TR_START	"@TR<<"
#define TR_END		">>"

struct lstr {
	char *name;
	char *value;
	struct lstr *next;
};
typedef struct lstr lstr;

static lstr *ltable[HASH_MAX];
static char buf[LINE_BUF], buf2[LINE_BUF];

/* lang parsing code block begin - doesn't really fully parse UCI
   (c) Jeremy Collake, released under GPL as rest of source
  **************************************************************** */

#define MAX_TOKEN_SIZE 256  /* maximum UCI token size (for extract_lang) */
#define LANG_TYPE_MAX 32    /* maximum language name size (for main) */

/* strip_next_token -
 *  specialized tokenization - handles quoted strings,
 *  whitespace as delimiter (hard coded, todo: change) */
char *strip_next_token(char *pszSource, char *pszToken, int nTokenLen)
{
	char cClosingQuote=0; /* set to ' or " when quoted string encountered */
	int nTokenIndex=0;
	int bInLeadingWhitespace=1;
	int nI;
	for(nI=0;nI<nTokenLen && pszSource[nI];nI++)
	{
		if(!cClosingQuote)	/* if not in quoted string */
		{
			switch(pszSource[nI])
			{
				/* if opening quote */
				case '\'':
				case '\"':
					bInLeadingWhitespace=0;
					cClosingQuote=pszSource[nI];
					continue;
				/* if whitespace */
				case ' ':
				case '\t':
					if(bInLeadingWhitespace)
					{
						continue;
					}
					/* fall-through */
				case '\n':
					pszToken[nTokenIndex]=0; /* terminate token */
					return pszSource+nI;
					break;
				default:
					bInLeadingWhitespace=0;
					break;
			}
		}
		else if(pszSource[nI]==cClosingQuote) /* if in quoted string, and ending quote */
		{
			pszToken[nTokenIndex]=0; /* terminate token */
			nI++; /* advance to the next source char */
			break;
		}
		pszToken[nTokenIndex]=pszSource[nI];
		nTokenIndex++;
	}
	pszToken[nTokenIndex]=0; /* terminate token */
	return pszSource+nI;
}

char *extract_lang(char *pszLine, char *pszBuffer, int nBufLen)
{
	char szTokenBuffer[MAX_TOKEN_SIZE];
	char *pszToken=szTokenBuffer;
	int nCount;
	pszLine=strip_next_token(pszLine, szTokenBuffer, MAX_TOKEN_SIZE);
	for(nCount=0;pszToken[0];)
	{
		switch(nCount)
		{
			case 0:
				/* if first argument is 'option' */
				if(!strcasecmp(pszToken, "option"))
				{
					nCount++;
				}
				/* if first argument not 'option' */
				else
				{
					return NULL;
				}
				break;
			case 1:
				/* if first non-empty argument is 'lang' */
				if(!strcasecmp(pszToken, "lang"))
				{
					nCount++;
				}
				/* if first argument not 'lang' */
				else
				{
					return NULL;
				}
				break;
			case 2:
				if(strlen(pszToken)>nBufLen)
				{
					/* truncate string if can't fit in buffer */
					pszToken[nBufLen-1]=0;
				}
				strcpy(pszBuffer,pszToken);
				return pszBuffer;
		}
		pszLine=strip_next_token(pszLine, szTokenBuffer, MAX_TOKEN_SIZE);
	}
	return NULL;
}
/* lang parsing code block end
   **************************************************************** */

/* djb2 hash function */
static inline unsigned long hash(char *str)
{
	unsigned long hash = 5381;
	int c;

	while ((c = *str++))
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
}

static inline char *translate_lookup(char *str)
{
	char *name, *def, *p, *res = NULL;
	lstr *i;
	int h;

	def = name = str;
	if (((p = strchr(str, '|')) != NULL)
		|| ((p = strchr(str, '#')) != NULL)) {
		def = p + 1;
		*p = 0;
	}

	h = hash(name) % HASH_MAX;
	i = ltable[h];
	while ((res == NULL) && (i != NULL)) {
		if (strcmp(name, i->name) == 0)
			res = i->value;
		i = i->next;
	}

	if (res == NULL)
		res = def;

	return res;
}

static inline void add_line(char *name, char *value)
{
	int h = hash(name) % HASH_MAX;
	lstr *s = malloc(sizeof(lstr));
	lstr *p;

	s->name = strdup(name);
	s->value = strdup(value);
	s->next = NULL;

	if (ltable[h] == NULL)
		ltable[h] = s;
	else {
		for(p = ltable[h]; p->next != NULL; p = p->next);
		p->next = s;
	}
}

static char *translate_line(char *line)
{
	static char *tok[MAX_TR * 3];
	char *l, *p, *p2, *res;
	int len = 0, pos = 0, i;

	l = line;
	while (l != NULL) {
		if ((p = strstr(l, TR_START)) == NULL) {
			len += strlen((tok[pos++] = l));
			break;
		}

		p2 = strstr(p, TR_END);
		if (p2 == NULL)
			break;

		*p = 0;
		*p2 = 0;
		len += strlen((tok[pos++] = l));
		len += strlen((tok[pos++] = translate_lookup(p + strlen(TR_START))));

		l = p2;
		l += strlen(TR_END);
	}
	len++;

	if (len > LINE_BUF)
		p = malloc(len);
	else
		p = buf2;

	p[0] = 0;
	res = p;
	for (i = 0; i < pos; i++) {
		strcat(p, tok[i]);
		p += strlen(tok[i]);
	}

	return res;
}

/* load and parse language file */
static void load_lang(char *file)
{
	FILE *f;
	char *b, *name, *value;

	if ((f = fopen(file, "r")) == NULL)
		return;

	while (!feof(f) && (fgets(buf, LINE_BUF - 1, f) != NULL)) {
		b = buf;
		if (*b == '#')
			continue; /* skip comments */

		while (isspace(*b))
			b++; /* skip leading spaces */
		if (!*b)
			continue;

		name = b;
		if ((b = strstr(name, "=>")) == NULL)
			continue; /* separator not found */

		value = b + 2;
		if (!*value)
			continue;

		*b = 0;
		for (b--; isspace(*b); b--)
			*b = 0; /* remove trailing spaces */

		while (isspace(*value))
			value++; /* skip leading spaces */

		for (b = value + strlen(value) - 1; isspace(*b); b--)
			*b = 0; /* remove trailing spaces */

		if (!*value)
			continue;

		add_line(name, value);
	}

	fclose(f);
}

int
#ifdef _METAPACK
webifpage_main
#else
main
#endif
(int argc, char **argv)
{
	FILE *f;
	int i, done;
	char line[LINE_BUF], *tmp, *arg;
	char szLangBuffer[LANG_TYPE_MAX];
	char *lang = NULL;
	char *proc = "/usr/bin/haserl";
	const char *langfmt = "/usr/lib/webif/lang/%s/%s.txt";

	memset(ltable, 0, HASH_MAX * sizeof(lstr *));
	if ((f = fopen("/etc/config/webif", "r")) != NULL) {

		while (!feof(f) && (lang == NULL)) {
			fgets(line, LINE_BUF - 1, f);
			lang=extract_lang(line, szLangBuffer, LANG_TYPE_MAX);
		}
		fclose(f);

		if (lang != NULL) {
			sprintf(buf, langfmt, lang, "common");
			load_lang(buf);
		}
	}

	/*
	 * command line options for this parser are stored in argv[1] only.
	 * filename to be processed is in argv[2]
	 */
	done = 0;
	i = 1;
	while (!done) {
		if (argv[1] == NULL) {
			done = 1;
		} else if (strncmp(argv[1], "-e", 2) == 0) {
			argv[1] = strchr(argv[1], ' ');
			argv[1]++;
			if (argv[1] != NULL) {
				arg = argv[1];
				if ((tmp = strchr(argv[1], ' ')) != NULL) {
					*tmp = 0;
					argv[1] = &tmp[1];
				} else {
					argv[1] = NULL;
					i++;
				}
				system(arg);
			}
		} else if (strncmp(argv[1], "-p", 2) == 0) {
			argv[1] = strchr(argv[1], ' ');
			argv[1]++;
			if (argv[1] != NULL) {
				arg = argv[1];
				if ((tmp = strchr(argv[1], ' ')) != NULL) {
					*tmp = 0;
					argv[1] = &tmp[1];
				} else {
					argv[1] = NULL;
					i++;
				}
				proc = strdup(arg);
			}
		} else {
			done = 1;
		}
	}

	strcpy(line, proc);
	while (argv[i]) {
		sprintf(line + strlen(line), " %s", argv[i++]);
	}

	/*
	 * Load standalone translation file
	 */
	if (lang != NULL) {
		if ((arg = strdup(line)) != NULL) {
			if ((tmp = strrchr(arg, '.')) != NULL)
				*tmp = 0;

			if ((tmp = strrchr(arg, '/')) != NULL)
				tmp++;
			else {
				if ((tmp = strrchr(arg, ' ')) != NULL)
					tmp++;
				else
					tmp = arg;
			}

			sprintf(buf2, langfmt, lang, tmp);
			load_lang(buf2);
			free(arg);
		}
	}

	f = popen(line, "r");

	while (!feof(f) && (fgets(buf, LINE_BUF - 1, f)) != NULL) {
		fprintf(stdout, "%s", translate_line(buf));
		fflush(stdout);
	}

	return 0;
}
