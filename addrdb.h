/*
 *	addrdb.h
 */
 
#ifdef ANSI
DB	*create_db(char *fname);
DB	*open_db(char *fname);
int	close_db(DB *db);
int	write_addr(DB *db, int index, ADDRESS *addr);
int	read_addr(DB *db, int index, ADDRESS *addr);
long	load_addr_list(DB *db, ADDRESS *bufp, long bufsize, int from_nr);
int	load_tree(FILE *fp, ADDRESS **bufp, PAGE *p, int *from_nr, int *how_many);
int	insert_addr(DB *db, ADDRESS *addr);
int	isearch(char *name, int index, PAGE *a, int *h, ITEM *v);
ITEM	*search_addr(DB *db, char *name);
ITEM	*ssearch(char *name, PAGE *a);
int	find_free_index(DB *db);
int	write_pages(FILE *fp, FILE *ixfp, PAGE *p);
PAGE	*read_pages(FILE *fp, FILE *ixfp);
int	write_flst(DB *db);
int	read_flst(DB *db);
#else
DB	*create_db();
DB	*open_db();
int	close_db();
int	write_addr();
int	read_addr();
long	load_addr_list();
int	load_tree();
int	insert_addr();
int	isearch();
ITEM	*search_addr();
ITEM	*ssearch();
int	find_free_index();
int	write_pages();
PAGE	*read_pages();
int	write_flst();
int	read_flst();

char	*malloc();
char	*calloc();
#endif



