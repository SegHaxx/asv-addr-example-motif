/*
 *	db.h
 */


#define INDSUF	".ind"
#define XNDSUF	".xnd"
#define DATSUF	".dat"
#define FLTSUF	".flt"

#define FNAMELEN	512


#define NOSLOT	"-1"

#define NN	8
#define N	4


typedef struct item ITEM;
typedef struct page PAGE;
typedef struct nindex NINDEX;

struct item {
	char	name[48];	/* name of person's address */
	int	count;
	int	index;		/* index into the file */
	NINDEX	*ip;		/* next index */
	PAGE	*p;		/* next page */
};

struct page {
	int		m;		/* nr of used slots in this page */
	PAGE		*p0;		/* ptr to page 0 */
	ITEM		elm[NN];	/* elements of this page */
};

struct nindex {
	int		count;
	int		index[NN];	/* index into file */
	NINDEX		*nip;		/* next index */
}; 

/*
 *	data structures
 */

#define NAMESIZE	48

typedef struct address {
	char	name[NAMESIZE];
	char	street[48];
	char	city[48];
	char	state[4];
	char	zip[12];
	char	country[48];
	char	phone[48];	
	char	fax[48];
} ADDRESS;

typedef struct header {
	char	date[10];	/* not used yet */
	int	number;		/* numer of records in db */
	char	resvd[256];
} HEADER;

typedef struct db {
	char	ifname[FNAMELEN];	/* index file name */
	char	xfname[FNAMELEN];	/* extended index file name */
	char	dfname[FNAMELEN];	/* data file name  */
	char	lfname[FNAMELEN];	/* free list file name  */
	FILE	*ifp;		/* index file ptr */
	FILE	*xfp;		/* extended index file ptr */
	FILE	*dfp;		/* data file ptr */
	FILE	*lfp;		/* free list file ptr */
	int	number;		/* number of address */
	PAGE	*root;		/* root of index tree */
} DB;

/*
 *	db functions
 */
 
DB	*create_db();
DB	*open_db();

