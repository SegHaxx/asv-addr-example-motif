/*
 *	Project: GEM/ATX convertion example
 *	OS:	 TOS
 *	
 *	Module: addrdb.c
 *
 *	Date:	06/21/1990
 *	
 *	Description:
 *		This module contains the database functions of the address
 *		program. The database is implemented as a b-tree. See N. Wirth
 *		'Algorithms and Data Structures' for more information.
 *
 *	Copyright 1990 by Atari Corp.
 *
 *	Written in 1989/90 by Hans-Martin Kroeber
 */

#include <stdio.h>
#include <string.h>
#include "defs.h"
#include "db.h"
#include "addrdb.h"

/*
 *	:create_db()
 *
 *	Description:
 *		Create a new database.
 *	
 *	Parameters:
 *		DB	*create_db(fname)
 *		char	*fname; 	Name of the dadabase
 *
 *	Returns:
 *		Pointer to the newly created datbase.
 */

DB	*create_db(fname)
char	*fname;
{
	FILE	*ifp, *xfp, *dfp, *lfp;
	char	ifname[FNAMELEN];		/* file names, see below */
	char	xfname[FNAMELEN];
	char	dfname[FNAMELEN];
	char	lfname[FNAMELEN];
	DB	*db;			/* database buffer */
	
	/*
	 *	Each database consists of four files:
	 *	'dbname.dat'
	 *		this file contains the data (all the addresses)
	 *	'dbname.ind'
	 *		this file contains the b-tree information (index)
	 *	'dbname.xnd'
	 *		this file contains the extended index. It's used
	 *		for multiple entries under the same key in the
	 *		b-tree. 
	 *	'dbname.flt'
	 *		this file contains a list of free slots in 
	 *		the data file (dbname.dat).
	 *
	 *	Create all four filenames. Concat the suffixes to the file
	 *	name of the database.
	 */
	strcpy(ifname, fname);
	strcat(ifname, INDSUF);
	strcpy(xfname, fname);
	strcat(xfname, XNDSUF);
	strcpy(dfname, fname);
	strcat(dfname, DATSUF);
	strcpy(lfname, fname);
	strcat(lfname, FLTSUF);

	/*	
	if (db_exist(ifname) == TRUE)
		return NULL;
	*/
		
	/*
	 *	Allocate memory for the database buffer
	 */
	if ((db = (DB *) malloc(sizeof(DB))) == NULL) {
		alert("Out of memory error!");
		free(db);
		return NULL;
	}
	
	/*
	 *	Create the four files of the database
	 */		
	if ((ifp = fopen(ifname, "w+b")) == NULL) {
		alert("File open error");
		free(db);
		return NULL;
	}
	if ((xfp = fopen(xfname, "w+b")) == NULL) {
		alert("File open error");
		free(db);
		return NULL;
	}
	if ((dfp = fopen(dfname, "w+b")) == NULL) {
		alert("File open error");
		free(db);
		return NULL;
	}
	if ((lfp = fopen(lfname, "w+b")) == NULL) {
		alert("File open error");
		free(db);
		return NULL;
	}
	db->ifp = ifp;
	db->xfp = xfp;
	db->dfp = dfp;
	db->lfp = lfp;
	/*
	 *	No data has been entered yet. So the root of the 
	 *	b-tree is NULL.
	 */ 
	db->number = 0;
	db->root = NULL;
	
	/*
	 *	Store file names in database buffer
	 */
	strcpy(db->ifname, ifname);
	strcpy(db->xfname, xfname);
	strcpy(db->dfname, dfname);
	strcpy(db->lfname, lfname);
	
	return db;
}

/*
 *	:open_db()
 *
 *	Description:
 *		Open an existing database.
 *	
 *	Parameters:
 *		DB	*open_db(fname)
 *		char	*fname; 	Name of the dadabase
 *
 *	Returns:
 *		Pointer to the datbase.
 */
 
DB	*open_db(fname)
char	*fname;
{
	FILE	*ifp, *xfp, *dfp, *lfp;
	char	ifname[FNAMELEN];	/* file names, see also create_db() */
	char	xfname[FNAMELEN];
	char	dfname[FNAMELEN];
	char	lfname[FNAMELEN];
	DB	*db;			/* database buffer */
	char	*s;			/* general purpose */
	PAGE	*read_pages();

	/*
	 *	Create file names
	 */
	if (strcmp(s = &fname[strlen(fname) - strlen(INDSUF)], INDSUF) == 0) {	
		strcpy(ifname, fname);
		*s = '\0';
		strcpy(xfname, fname);
		strcat(xfname, XNDSUF);
		strcpy(dfname, fname);
		strcat(dfname, DATSUF);
		strcpy(lfname, fname);
		strcat(lfname, FLTSUF);
	} else 
	 if (strcmp(s = &fname[strlen(fname) - strlen(INDSUF)], DATSUF) == 0) {
		strcpy(dfname, fname);
		*s = '\0';
		strcpy(xfname, fname);
		strcat(xfname, XNDSUF);
		strcpy(ifname, fname);
		strcat(ifname, INDSUF);
		strcpy(lfname, fname);
		strcat(lfname, FLTSUF);
	} else {
		alert("Not a db file!");
		return NULL;
	}
	/*
	 *	Allocate memory for database buffer
	 */
	if ((db = (DB *) malloc(sizeof(DB))) == NULL) {
		alert("Out of memory error!");
		return NULL;
	}
	/*
	 *	Open database
	 */
	if ((ifp = fopen(ifname, "r+b")) == NULL) {
		alert("File open error 1");
		free(db);
		return NULL;
	}
	if ((xfp = fopen(xfname, "r+b")) == NULL) {
		alert("File open error 2");
		free(db);
		return NULL;
	}
	if ((dfp = fopen(dfname, "r+b")) == NULL) {
		alert("File open error 3");
		free(db);
		return NULL;
	}
	if ((lfp = fopen(lfname, "r+b")) == NULL) {
		alert("File open error 4");
		free(db);
		return NULL;
	}
	db->ifp = ifp;
	db->xfp = xfp;
	db->dfp = dfp;
	db->lfp = lfp;
	/*
	 *	Store file names in database buffer
	 */
	strcpy(db->ifname, ifname);
	strcpy(db->xfname, xfname);
	strcpy(db->dfname, dfname);
	strcpy(db->lfname, lfname);
	/*
	 *	Read file with list of free slots in data file
	 */
	if (read_flst(db) != 0) {
		free(db);
		return NULL;
	}
	/* 
	 *	Read index file and create the b-tree.
	 */
	if ((db->root = read_pages(db->ifp, db->xfp)) == NULL)	{
		free(db);
		return NULL;
	}

	return db;
}

/*
 *	:close_db()
 *
 *	Description:
 *		Open an existing database.
 *	
 *	Parameters:
 *		int	close_db(db)
 *		DB	*db;		pointer to database buffer
 *
 *	Returns:
 *		Nothing.
 */

int	close_db(db)
DB	*db;
{
	/*
	 *	Write the b-tree onto disk
	 */
	fseek(db->ifp, 0L, 0);
	fseek(db->xfp, 0L, 0);
	write_pages(db->ifp, db->xfp, db->root);
	/*
	 *	Write the list of free slots onto disk
	 */
	write_flst(db);
	
	/*
	 *	Close the database files
	 */
	fclose(db->ifp);
	fclose(db->xfp);
	fclose(db->dfp);
	fclose(db->lfp);
	
	free(db);
	db = NULL;
}

/*
 *	:write_addr()
 *
 *	Description:
 *		Write an item of data onto disk.
 *	
 *	Parameters:
 *		int	write_addr(db, index, addr)
 *		DB	*db;		pointer to database buffer
 *		int	index;		where to put the data in the data file
 *		ADDRESS *addr;		data to write onto disk
 *
 *	Returns:
 *		 0, if data was written successfully
 *		-1, if an error occured
 *		
 */
 
int	write_addr(db, index, addr)
DB	*db;
int	index;
ADDRESS *addr;
{
	int d;
	/*
	 *	position file pointer and write the data
	 */
	fseek(db->dfp, (long) (index * sizeof(ADDRESS)), 0);
	
	if ((d = fwrite(addr, sizeof(ADDRESS), 1, db->dfp)) != 1) {
		alert("Error: Can't write data!");
		return -1;
	}
	return 0;
}

/*
 *	:read_addr()
 *
 *	Description:
 *		Read an item of data from disk.
 *	
 *	Parameters:
 *		int	read_addr(db, index, addr)
 *		DB	*db;		pointer to database buffer
 *		int	index;		where to get the data in the data file
 *		ADDRESS *addr;		buffer into which data will be read
 *
 *	Returns:
 *		 0, if reading was successful
 *		-1, if an error occured
 *		
 */

int	read_addr(db, index, addr)
DB	*db;
int	index;
ADDRESS *addr;
{
	/*
	 *	position file pointer and read the data
	 */
	fseek(db->dfp, (long) (index * sizeof(ADDRESS)), 0);
	
	if (fread(addr, sizeof(ADDRESS), 1, db->dfp) != 1) {
		alert("Error: Can't read data!");
		return -1;
	}
	return 0;
}

/*
 *	:load_addr_list()
 *
 *	Description:
 *		Walk recursivly through the b-tree to list a number
 *		of items. This can be used to ouput a list of items on the
 *		screen or printer.
 *		To do the actual work, this function calls load_tree().
 *	Parameters:
 *		long	*load_addr_list(db, bufp, buf_size, from_nr)
 *		DB	*db;		pointer to database buffer
 *		ADDRESS *bufp;
 *		long	bufsize;
 *		int	from_nr;
 *		
 *		The read in items will be placed into 'bufp'. load_addr_list()
 *		will read as many items as fit into bufp. The size of the buffer
 *		is specified by 'bufsize'. load_add_list() will start the list
 *		from the index 'from_nr'.
 *		
 *	Returns:
 *		The number of bytes actually read.
 */

long	load_addr_list(db, bufp, bufsize, from_nr)
DB	*db;		/* ptr to database */
ADDRESS *bufp;		/* ptr to bufer which will contain the addresses */
long	bufsize;	/* size of the buffer */
int	from_nr;	/* start index */
{
	long		size;		
	int		how_many;	/* how many items will be read */
	
	if (from_nr > db->number)
		return 0;	
		
	/*
	 *	calc # of to be read items
	 */
	if (bufsize > (long) (db->number * sizeof(ADDRESS)))
		size = (long) db->number * (long) sizeof(ADDRESS);
	else
		size = bufsize;
	
	how_many = (int) (size / (long) sizeof(ADDRESS));	
	
	/*
	 *	walk through the b-tree and build the list
	 */
	load_tree(db->dfp, &bufp, db->root, &from_nr, &how_many);
	
	return size;
}

/*
 *	:load_tree()
 *
 *	Description:
 *		Walk recursivly through the b-tree. Start at a given index
 *		and read a specified number of items into a buffer. 
 *		This function is called by load_addr_list().
 *	Parameters:
 *		long	*load_tree(fp, bufp, p, from_nr, how_many)
 *		FILE	*fp		data file descriptor
 *		ADDRESS **bufp; 	buffer for the read in data
 *		PAGE	*p		current b-tree page
 *		int	from_nr;	start index
 *		int	how_may;	number of items to be read
 *		
 *	Returns:
 *		-1, if an error occured
 *		 0, else
 */

int	load_tree(fp, bufp, p, from_nr, how_many)
FILE	*fp;
ADDRESS **bufp;
PAGE	*p;
int	*from_nr;
int	*how_many;
{
	register int i, j;
	
	if ((*how_many <= 0) || (p == NULL)) {
		/*
		 *	All items read in or 
		 *	empty page, go back one level.
		 */	 
		return 0;		
	} else {
		/*
		 *	Still items to be read in.
		 *	Walk recursivly through tree; look one level (page)
		 *	below (p->p0).
		 */
		load_tree(fp, bufp, p->p0, from_nr, how_many);
		
		if ((*from_nr - p->m) > 0) {
			/*
			 *	we haven't reached our start index yet
			 */
			*from_nr -= p->m;
		} else {
			i = *from_nr;
			*from_nr = 0;
			
			for (; (i < p->m) && (*how_many > 0); i++) { 
				/*
				 *	seek to position 'index' and 
				 *	read in the data
				 */
				fseek(fp, 
				(long) (p->elm[i].index * sizeof(ADDRESS)), 0);
		
				if (fread(*bufp, sizeof(ADDRESS),1, fp) != 1) {
					alert("Read Error!");
					return -1;
				}
				(*bufp)++;
				(*how_many)--;
				if (*how_many <= 0)
					break;
					
				if (p->elm[i].count > 0) {
					NINDEX	*ip = p->elm[i].ip;
						
					for (; ip != NULL; ip = ip->nip) {
						for (j=0; j < ip->count; j++) {
							fseek(fp, 
							(long) (ip->index[j] * 
							sizeof(ADDRESS)), 0);
							
							if (fread(*bufp, 
							sizeof(ADDRESS), 1, fp) 
							!= 1) {
								alert(
								"Read Error!");
								return -1;
							}
							(*bufp)++;
							(*how_many)--;
							if (*how_many <= 0)
								break;
						}
					}
				}
							
				if (p->elm[i].p != NULL)
					load_tree(fp, bufp, 
						p->elm[i].p, from_nr, how_many);
			}
		}
	}
	return 0;
}

/*
 *	:insert_addr()
 *	insert an address into the db
 *	see "Algorithms & Data Structures" from N. Wirth (b-tree)
 */
int	insert_addr(db, addr)
DB	*db;		/* database */
ADDRESS *addr;		/* to be inserted address */
{
	PAGE	*root, *q;
	ITEM	u;
	int	h;
	int	index;
	
	root = db->root;
	index = find_free_index(db);
	if (isearch(addr->name, index, root, &h, &u) == -1) {
		db->number--;	/* HACK!, see find_free_index() */
		return -1;
	}
	
	if (h) {	/* we have to insert a new root page */
		q = root;
		root = (PAGE *) calloc(1, sizeof(PAGE));
		root->m = 1;
		root->p0 = q;
		root->elm[0] = u;
		db->root = root;
	}
	write_addr(db, index, addr);
	return 0;
}

/*
 *	:isearch()
 *	function which does the actual work for insert_addr()
 *	see "Algorithms & Data Structures" from N. Wirth (b-tree)
 */
 
int	isearch(name, index, a, h, v)
char	*name;
int	index;
PAGE	*a;
int	*h;
ITEM	*v;
{
	int	k, l, r;
	PAGE	*q;
	ITEM	u;
	int	strcmp();
	
	/*
	 *	search key (addr->name) on page a; NOT h 
	 */
	 
	if (a == NULL) {
		*h = TRUE;
		strcpy(v->name, name);
		v->count = 0;
		v->index = index;
		v->p = NULL;
		v->ip = NULL;
	} else {
		l = 0; r = a->m - 1;
		do {	/* binary search in array */
			k = (l + r) / 2;
			if (strcmp(name, a->elm[k].name) <= 0)
				r = k - 1;
			if (strcmp(name, a->elm[k].name) >= 0)
				l = k + 1;
		} while (r >= l);
		if ((l - r) > 1) {	/* found */
			ITEM		*v = &a->elm[k];
			register NINDEX *ip;
			
			v->count++;
			/*
			 *	find next index page 
			 */
			if ((ip = v->ip) == NULL) {
				ip = v->ip = (NINDEX*)calloc(1, sizeof(NINDEX));
				ip->count = 0;
				ip->nip = NULL;
			} else {
				for (; ip->nip != NULL; ip = ip->nip)
					if (ip->count < NN)
						break;
			}
			if (ip->count == NN) {
				ip->nip = (NINDEX *) calloc(1, sizeof(NINDEX));
				ip = ip->nip;
				ip->count = 1;
				ip->nip = NULL;
			} else
				ip->count++;
				
			ip->index[ip->count - 1] = index;
			
			*h = FALSE;
			
		} else {	/* not in this page */
			if (r == -1) 
				q = a->p0;
			else
				q = a->elm[r].p;
				
			isearch(name, index, q, h, &u);
				
			if (*h) {	/* insert */
				int	i;
				PAGE	*b;
				
				if (a->m < NN) {
					a->m++; *h = FALSE;
					for (i = a->m - 1; i >= (r + 2); i--)
						a->elm[i] = a->elm[i - 1];
					a->elm[r + 1] = u;
				} else {	/* page is full, create new */
					b = (PAGE *) calloc(1, sizeof(PAGE));
					if (r <= (N - 1)) {
						if (r == (N - 1)) 
							*v = u;
						else {
							*v = a->elm[N - 1];
							for (i = N-1; 
							i >= (r+2); i--)
								a->elm[i] =
								  a->elm[i - 1];
							a->elm[r + 1] = u;
						} 
						for (i = 0; i < N; i++)
							b->elm[i] = 
								a->elm[i+N];
					} else {
						r = r - N; *v = a->elm[N];
						for (i = 0; i <= (r - 1); i ++)
							b->elm[i] = 
								a->elm[i+N+1];
						b->elm[r] = u;
						for (i = r+1; i < N; i++)
							b->elm[i] = a->elm[i+N];
					}
					a->m = N; b->m = N; 
					b->p0 = v->p; v->p = b;
				}
			}
									
					
		}
	}
	return 0;
}

/*
 *	:search_addr()
 *	search a name (key) in the database and an index into the data file
 *	where the complete item can be found
 */
 
ITEM	*search_addr(db, name)
DB	*db;
char	*name;
{
	ITEM	*ssearch();
	int	index;
	
	return ssearch(name, db->root);
	
}

/*
 *	:ssearch()
 *	search name and return index into data file.
 *	this function does the actual work for search_addr().
 *	see "Algorithms & Data Structures" from N. Wirth (b-tree)
 */
 
ITEM	*ssearch(name, a)
char	*name;
PAGE	*a;
{
	int	k, l, r;
	PAGE	*q;
	int	strcmp();
	
	/*
	 *	search key (addr->name) on page a; NOT h 
	 */
	 
	if (a == NULL) {
		return NULL;
	} else {
		l = 0; r = a->m - 1;
		do {	/* binary search in array */
			k = (l + r) / 2;
			if (strcmp(name, a->elm[k].name) <= 0)
				r = k - 1;
			if (strcmp(name, a->elm[k].name) >= 0)
				l = k + 1;
		} while (r >= l);
		if ((l - r) > 1) {	/* found */
			return &a->elm[k];
		} else {	/* not in this page */
			if (r == -1) 
				q = a->p0;
			else
				q = a->elm[r].p;
				
			return ssearch(name, q);
		}
	}
}

/*
 *	:delete_addr()
 *	this function is not working correctly yet
 */

int	delete_addr(db, name)
DB	*db;
char	*name;
{
	PAGE	*root, *q;
	ITEM	u;
	int	h;
	
	root = db->root;
	
	delete(name, root, &h); 
	
	if (h) {
		if (root->m == 0) {
			q = root;
			root = q->p0;
			free(q);
		}
	}	
       
}

/*
 *	:delete()
 *	this function is not working correctly yet
 */
 
static
int	delete(name, a, h)
char	*name;
PAGE	*a;
int	*h;
{
	int	i, k, l, r;
	PAGE	*q;
	
	if (a == NULL) {
		*h = FALSE;
		return FALSE;
	}
	l = 0; r = a->m - 1;
	do {	/* binary search in array */
		k = (l + r) / 2;
		if (strcmp(name, a->elm[k].name) <= 0)
			r = k - 1;
		if (strcmp(name, a->elm[k].name) >= 0)
			l = k + 1;
	} while (r >= l);
	if (r == -1)
		q = a->p0;
	else
		q = a->elm[r].p;
	if ((l - r) > 1) {	/* found */
		if (q == NULL) {	/* a is an end page */
			a->m--; 
			*h = (a->m < N);
			for (i = k; i <= a->m; i++) {
				a->elm[i] = a->elm[i+1];
			}
		} else {
			del(a, k, q, h);
			if (*h)
				underflow(a, q, r, h);
		}
	} else {
		delete(name, q, h);	
		if (*h) 
			underflow(a, q, r, h);
	}
	return TRUE;
}
int	del(a, k, p, h)
PAGE	*a;
int	k;
PAGE	*p;
int	*h;
{
	PAGE	*q;
	
	q = p->elm[p->m].p;
	if (q != NULL) {
		del(a, k, q, h);
		if (*h)
			underflow(p, q, p->m, h);
	} else {
		p->elm[p->m].p = a->elm[k].p;
		a->elm[k] = p->elm[p->m];
		p->m--;
		*h = (p->m < N);
	}
}

/*
 *	:underflow()
 *	not working yet. belongs to delete()
 */
static
int	underflow(c, a, s, h)
PAGE	*c;
PAGE	*a;
int	s;
int	*h;
{
	PAGE	*b;
	int	i, k, mb, mc;
	
	mc = c->m;
	
	if (s < mc) {
		s++;
		b = c->elm[s].p;
		mb = b->m;
		k = (mb - N + 1) / 2;
		a->elm[N] = c->elm[s];
		a->elm[N].p = b->p0;
		if (k > 0) {
			for (i = 0; i < k; i++) {	/* !!!!!!! */
				a->elm[i + N] = b->elm[i];
			}
			c->elm[s] = b->elm[k];
			c->elm[s].p = b;
			b->p0 = b->elm[k].p;
			mb = mb - k;
			for (i = 0; i < mb; i++) {	/* !!!!!!! */
				b->elm[i] = b->elm[i + k];
			}
			b->m = mb; 
			a->m = N - 1 + k;
			*h = FALSE;
		} else {
			for (i = 0; i < N; i++) 	/* !!!!!!! */
				a->elm[i + N] = b->elm[i];
			for (i = s; i < mc; i++)	/* !!!!!!! */
				c->elm[i] = c->elm[i + 1];
			a->m = NN;
			c->m = mc - 1;
			*h = (mc <= N); 		/* !!!!!!! */
		}
	} else {
		if (s == 0)				/* !!!!!!! */
			b = c->p0;
		else
			b = c->elm[s - 1].p;
		mb = b->m + 1;
		k = (mb - N) / 2;
		if (k > 0) {
			for (i = N - 1; i >= 0; i--) {	/* !!!!!!! */
				a->elm[i + k] = a->elm[i];
			}
			a->elm[k] = c->elm[s];
			a->elm[k].p = a->p0;
			mb = mb - k;
			for (i = k - 1; i >= 0; i--) {	/* !!!!!!! */
				a->elm[i] = b->elm[i + mb];
			}
			a->p0 = b->elm[mb].p;
			c->elm[s] = b->elm[mb];
			c->elm[s].p = a;
			b->m = mb - 1;
			a->m = N - 1 + k;
			*h = FALSE;
		} else {
			b->elm[mb] = c->elm[s];
			b->elm[mb].p = a->p0;
			for (i = 0; i < (N - 1); i++) { /* !!!!!!!! */
				b->elm[i + mb] = a->elm[i];
			}
			b->m = NN;
			c->m = mc - 1;
			*h = (mc <= N);
		}
	}
}

/*
 *	:find_free_index()
 */	
int	find_free_index(db)
DB	*db;
{
	return db->number++;
}

/* 
 *	:write_pages()
 *	write the b-tree onto the disk
 */
int	write_pages(fp, ixfp, p)
FILE	*fp;
FILE	*ixfp;
PAGE	*p;
{
	register int i, m;
	register ITEM *elmp;
	
	if (fwrite((char *) p, sizeof(PAGE), 1, fp) != 1) {
		alert("Write Error: write_pages()");
		return -1;
	}
	/*
	 *	write extended index
	 */
	m = p->m;
	elmp = p->elm;
	for (i = 0; i < m; i++, elmp++) {
		if (elmp->count > 0) {
			register NINDEX *ip;
			
			for (ip = elmp->ip; ip != NULL; ip = ip->nip)
				fwrite((char *) ip, sizeof(NINDEX), 1, ixfp);
		}
	}
	if (p->p0 != NULL)
		write_pages(fp, ixfp, p->p0);
	for (i = 0; i < p->m; i++) {
		if (p->elm[i].p != NULL)
			write_pages(fp, ixfp, p->elm[i].p);
	}
	return 0;
}

/*
 *	:read_pages()
 *	read the b-tree from the disk
 */
 
PAGE	*read_pages(fp, ixfp)
FILE	*fp;
FILE	*ixfp;
{
	register int i, m;
	register ITEM *elmp;
	PAGE	*p;
	
	p = (PAGE *) malloc(sizeof(PAGE));
	
	if (fread((char *) p, sizeof(PAGE), 1, fp) != 1) {
		alert("Read Error: read_pages().");
		return NULL;
	}
	/*
	 *	read extended index
	 */
	m = p->m;
	elmp = p->elm;
	for (i = 0; i <m; i++, elmp++) {
		if (elmp->count != 0) {
			register NINDEX *ip;
			
			elmp->ip = ip = (NINDEX *) calloc(1, sizeof(NINDEX));
			
			for (ip = elmp->ip; ip != NULL; ) {
				if (fread((char *) ip, sizeof(NINDEX), 1, 
				ixfp) != 1) {
					alert("Read Error: X read_pages().");
					return NULL;
				}
				if (ip->nip != NULL) {
					ip->nip = (NINDEX *) 
						calloc(1, sizeof(NINDEX));
					ip = ip->nip;
					ip->nip = NULL;
				}
				else
					break;
			}
		}
	}
	if (p->p0 != NULL) {
		p->p0 = read_pages(fp, ixfp);
	}
	for (i = 0; i < p->m; i++) {
		if (p->elm[i].p != NULL) {
			p->elm[i].p = read_pages(fp, ixfp);
		}
	}
	return p;
}
	
/*
 *	:write_flst()
 *	write free list
 *	not needed yet, because delete is not working yet
 */
 
int	write_flst(db)
DB	*db;
{
	HEADER	h;
	
	h.number = db->number;
	
	fseek(db->lfp, 0L, 0);
	if (fwrite((char *) &h, sizeof(HEADER), 1, db->lfp) != 1)
		alert("Write Error: write_flst().");
	
	/*
	 *	...
	 */

}

/*
 *	:read_flst()
 *	read free list
 *	not needed yet, because delete is not working yet
 */
 
int	read_flst(db)
DB	*db;
{
	HEADER	h;
	
	if (fread((char *) &h, sizeof(HEADER), 1, db->lfp) != 1) {
		alert("Read error: read_flst().");
		return -1;
	}
	db->number = h.number;
	
	/*
	 *	...
	 */
	return 0;
}

/*
 *	:find_pos_in_tree()
 *	return the position of an item in the tree
 */
int	find_pos_in_tree(db, name)
DB	*db;
char	*name;
{
	int pos = 0;

	if (_find_pos_in_tree(db->root, name, &pos) == TRUE)
		return pos;
	else
		return 0;
}

/*
 *	:_find_pos_in_tree()
 */
int	_find_pos_in_tree(p, name, pos)
PAGE	*p;
char	*name;
int	*pos;
{
	int i;

	if (p->p0 != NULL)
		if (_find_pos_in_tree(p->p0, name, pos) == TRUE)
			return TRUE;
	for (i = 0; i < p->m; i++) {
		if (strcmp(name, p->elm[i].name) == 0)
			return TRUE;
		else {
			*pos = *pos + p->elm[i].count + 1;;
			if (_find_pos_in_tree(p->elm[i].p, name, pos) == TRUE)
				return TRUE;
		}
	}
	return FALSE;
}

