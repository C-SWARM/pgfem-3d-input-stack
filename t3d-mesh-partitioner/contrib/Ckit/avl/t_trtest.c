/* t_trtest - test the tree functions
 * vix 24jul87 [documented, added savestr for net distribution]
 * vix 22jan93 [revisited; uses RCS, ANSI, POSIX; has bug fixes]
 */

#ifndef LINT
static char RCSid[] = "$Id:";
#endif

#define MAIN

#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include "vixie.h"
#include "tree.h"


#ifdef __STDC__
#define __P(x) x
#else
#define	__P(x) ()
#endif

static void	trtest		__P( (tree **, char *, int) );
static void	tree_trav1	__P( (tree *, int) );
static int	compar		__P( (char *, char *) );
static void	duar		__P( (char *) );
static char	*savestr	__P( (char *) );

#undef __P


int
main()
{
	tree	*t;
	char	line[100];

	tree_init(&t);
	while (printf("key (or .):  "), gets(line), line[0] != '.')
	{
		if (strncmp(line, "~r ", 3)) {
			trtest(&t, line, 1);
		}
		else {
			FILE *f;

			if (!(f = fopen(&line[3], "r")))
				perror(&line[3]);
			else {
				while (fgets(line, 100, f)) {
					line[strlen(line)-1] = '\0';
					printf("(%s)\n", line);
					trtest(&t, line, 0);
				}
				fclose(f);
			}
		}
	}
	return 0;
}

static void
trtest(tt, line, inter)
	tree	**tt;
	char	*line;
	int	inter;
{
	char	opts[100], *pc, *n;
	int	opt, status;

	pc = tree_srch(tt, compar, line);
	printf("tree_srch=%x\n", (unsigned)pc);
	if (pc)
	{
		printf("     <%s>\n", pc);

		if (inter) {
			printf("delete? "); gets(opts); opt = (opts[0]=='y');
		}
		else
			opt = 1;

		if (opt) {
			status = tree_delete(tt, compar, line, duar);
			printf("delete=%d\n", status);
		}
	}
	else
	{
		if (inter) {
			printf("add? "); gets(opts); opt = (opts[0]=='y');
		}
		else
			opt = 1;

		if (opt) {
			char	*savestr();

			n = savestr(line);
			tree_add(tt, compar, n, duar);
		}
	}
	tree_trav1(*tt, 0);
}

static void
tree_trav1(t, l)
	tree	*t;
	int	l;
{
	int	i;

	if (!t) return;
	tree_trav1(t->tree_l, l+1);
	for (i=0;  i<l;  i++) printf("  ");
	printf("%08lx (%s)\n", (unsigned)t->tree_p, (char*)t->tree_p);
	tree_trav1(t->tree_r, l+1);
}	
	
static void
duar(pc)
	char *pc;
{
	printf("duar called, pc=%08X: <%s>\n", (unsigned)pc, pc?pc:"");
	free(pc);
}

static int
compar(l, r)
	char *l, *r;
{
	printf("compar(%s,%s)=%d\n", l, r, strcmp(l, r));
	return strcmp(l, r);
}

static char *
savestr(str)
	char	*str;
{
	char	*save;

	save = malloc(strlen(str) + 1);
	strcpy(save, str);
	return save;
}
