/*
 *	Project: 	Example of how to convert GEM programs to Motif
 *	Program:	Small Address Application
 *	Version:	1.1
 *	OS:		Atari UNIX System V Rel 4.0
 *	Author:		Hans-Martin Kroeber
 *	Starting Date:	4/3/1991
 *	Current Date:	5/29/1991
 *
 *	Description:
 *		
 *		Addr is a small address application. The user is able to
 *	insert addresses into a database, search for addresses and to list
 *	all addresses in a window. The main purpose of this program is to
 *	show developers of how to convert TOS/GEM programs to ASV/Motif.
 *	The program was created with the help of FaceMaker, the interface
 *	builder porgram userd by Atari.
 *
 *	Outstanding issues: 
 *		Modifying and deleteing is not implemented yet. 
 *		Help is not implemented yet.
 */

#include <stdio.h>
#include <Fm.h>
#include <Xm/List.h>
#include <Xm/Xm.h>
#include <Atari/libgls.h>
#include "db.h"

/*
 *	Filename of the FM file
 */
#define FmFILE		"/usr/lib/AtariExample/Fm/addr.fm"

/*
 *      NLSPATH and XBMLANGPATH environment variable defines
 */
#define NLSPATH                 "NLSPATH"
#define NLSPATH_DELIMITER       ':'
#define XBMLANGPATH             "XBMLANGPATH"
#define XBMLANGPATH_DELIMITER   ':'

static char *nls_path[] = {
        "/usr/lib/AtariExample/gls/%c/%t/%l/%N.cat",
        "/usr/lib/AtariExample/gls/%t/%l/%N.cat",
        "/usr/lib/AtariExample/gls/%l/%N.cat",
        "/usr/lib/AtariExample/gls/%N.cat",
        NULL
};
static char *xbm_lang_path[] = {
        "/usr/lib/AtariExample/bitmaps/%B",
        "/usr/include/X11/bitmaps/%B",
        NULL
};

/*
 *	length of a line in the list window
 */
#define ADDRLISTLINE	320

#define LINEBUFSIZE	1024

#ifndef SUCCESS
#define SUCCESS	0
#define FAILURE -1
#endif


/*
 *	functions declarations
 */

void	QuitAddr();	/* Quit Address application */
void	AddButton();	/* Add Button was selected */
void	SearchButton(); /* Search Button ... */
void	ClearButton();	/* Clear text fields */
void	CreateDB();	/* Create Menu Button ... */
void	OpenDB();	/* Open Menu Button */
void	CloseDB();	/* Close Menu Button */
void	DoAddrListWindow();	/* Open the list window */
void	BuildAddrList();	/* copy all addresses into the list window */
void	DestroyList();		/* destroy the address list */
void	ListCloseButton();	/* close list window */
void	UpdateList();		/* add an entry to the list */
void	ListSelect();	/* An item in the list was selected */
void	AV_to_Addr();	/* copy Active Values in ADDRESS struct */
void	Addr_to_AV();	/* vice versa */
void	clear_textfields();	/* clear all text inut fields */
void	clear_addrFoundLine();	/* clear message line */
char	*strNcpy();		/* cop y max. of N chars */

/*
 *	toplevel shell widget
 */

extern Widget	toplevel;


char	*shortname;	/* short version of the name of the application */

/*
 *	Active Values
 */

String	NameAV;				/* Address text input fields */
char	*DNameAV = "NameAV";		/* (name of AV in FaceMaker script) */
String	StreetAV;
char	*DStreetAV = "StreetAV";
String	CityAV;
char	*DCityAV = "CityAV";
String	StateAV;
char	*DStateAV = "StateAV";
String	ZIPAV;
char	*DZIPAV = "ZIPAV";
String	CountryAV;
char	*DCountryAV = "CountryAV";
String	PhoneAV;
char	*DPhoneAV = "PhoneAV";
String	FAXAV;
char	*DFAXAV = "FAXAV";
String	NewDBFnameAV;			/* Create new db text field (db name) */
char	*DNewDBFnameAV = "NewDBFnameAV";
XmString FileDirSpecAV;		/* Selected file in file selection box */
char  	*DFileDirSpecAV = "FileDirSpecAV";
XmString CurrentDBAV;		/* Name of current db (displayed in window)*/
char  	*DCurrentDBAV = "CurrentDBAV";
XmString AddrFoundAV;		/* Number of found addresses (msg line) */
char  	*DAddrFoundAV = "AddrFoundAV";
int 	AutomaticClearAV = 1;	/* Toggle button for text field clearing */
char  	*DAutomaticClearAV = "AutomaticClearAV";
int 	ListButtonAV = 0;	/* Toggle button for the list window */
char  	*DListButtonAV = "ListButtonAV";
Widget	ListWidgetAV;		/* XmList Widget Address */
char  	*DListWidgetAV = "ListWidgetAV";
Widget	ListShellAV;		/* XmToplevel Shell Widget Addr. of list wind.*/
char  	*DListShellAV = "ListShellAV";
int	ListItemCountAV;	/* number of items in the address list */
char	*DListItemCountAV = "ListItemCountAV";
XmStringTable	ListItemAV;	/* ptr to table of items */
char	*DListItemAV = "ListItemAV";
Widget	MainWindowAV;		/* XmMainWindow Widget of the Addr window */
char	*DMainWindowAV = "MainWindowAV";

/*
 *	some strings
 */

char	*NoneDBstr;		/* Points to "None" (internationalized) */
char	*AddrFoundFmtstr;	/* Format string for nr of found addr. */
char	*OKButtonMsg;		/* Message of the OK button (AtariLib) */

/*
 *	database global variabls
 */

int	newflag = TRUE;		/* TRUE=no db opened, FALSE=db was opned */
DB	*db;			/* ptr to currend db */
ADDRESS	addr;			/* ADDRESS struct for passing data to db fcts */

int	BuildListFlag = False;	/* TRUE = Address list was built */
/*
 *	:main()
 *	the never ending story
 */

int	main(argc, argv)
int	argc;
char	**argv;
{
	Widget	FmInitialize();

	/*
	 *	init FaceMaker & AtariLib (needed for alert boxes etc.)
	 */
	LogOpen("addr");	/* log to logfile (AtariLib) */

	/*
	 *	Add directories to the environment variables,
	 *	so that the files are found (.fm and .cat)
	 */
	AddPathV(XBMLANGPATH,xbm_lang_path,XBMLANGPATH_DELIMITER,TRUE);
        AddPathV(NLSPATH,nls_path,NLSPATH_DELIMITER,TRUE);


	AtariLibInit(FmInitialize("addr", "Addr", NULL, 0, &argc, argv));

	/*
	 *	attach the fcts called in the FM script to the real C-fcts
	 */
	FmAttachFunction("QuitAddr", QuitAddr, "None", 1, "Widget");
	FmAttachFunction("AddButton", AddButton, "None", 1, "Widget");
	FmAttachFunction("SearchButton", SearchButton, "None", 1, "Widget");
	FmAttachFunction("DoAddrListWindow", DoAddrListWindow, "None", 0);
	FmAttachFunction("ListCloseButton", ListCloseButton, "None", 0);
	FmAttachFunction("ListSelect", ListSelect, "None", 1, "XmStringTable");
	FmAttachFunction("ClearButton", ClearButton, "None", 0);
	FmAttachFunction("CreateDB", CreateDB, "None", 0);
	FmAttachFunction("OpenDB", OpenDB, "None", 0);
	FmAttachFunction("CloseDB", CloseDB, "None", 0);

	FmLoad(FmFILE);		/* load the FM file */

	/*
	 *	init AtariLib alert fcts
	 */
	shortname = strdup(FmCatGetS(100, 1, "Addr"));
	if (shortname == NULL)  {
		LogWrite ("Addr main: Cannot allocate short name.\n");
		exit (1);
	}

	if (AlertInit (shortname) == -1)  {
		LogWrite ("Addr main: Cannot set up alert.\n");
		exit (1);
	}


	/*
	 *	attach Active Values
	 */
	FmAttachAv(DNameAV, &NameAV);
	FmAttachAv(DStreetAV, &StreetAV);
	FmAttachAv(DCityAV, &CityAV);
	FmAttachAv(DStateAV, &StateAV);
	FmAttachAv(DZIPAV, &ZIPAV);
	FmAttachAv(DCountryAV, &CountryAV);
	FmAttachAv(DPhoneAV, &PhoneAV);
	FmAttachAv(DFAXAV, &FAXAV);
	FmAttachAv(DNewDBFnameAV, &NewDBFnameAV);
	FmAttachAv(DFileDirSpecAV, &FileDirSpecAV);
	FmAttachAv(DCurrentDBAV, &CurrentDBAV);
	FmAttachAv(DAddrFoundAV, &AddrFoundAV);
	FmAttachAv(DAutomaticClearAV, &AutomaticClearAV);
	FmAttachAv(DListButtonAV, &ListButtonAV);
	FmAttachAv(DListWidgetAV, &ListWidgetAV);
	FmAttachAv(DListShellAV, &ListShellAV);
	FmAttachAv(DListItemAV, &ListItemAV);
	FmAttachAv(DListItemCountAV, &ListItemCountAV);
	FmAttachAv(DMainWindowAV, &MainWindowAV);

	FmUpdateAv(DMainWindowAV);	
	FmUpdateAv(DListWidgetAV);	
	FmUpdateAv(DListShellAV);	
	FmGetAv(DMainWindowAV);	/* get the address of the XmMainWindow Widget */
	FmGetAv(DListWidgetAV);	/* get the address of the XmList Widget */
	FmGetAv(DCurrentDBAV);	/* get string "None" from FM file */

	NoneDBstr = strdup(CurrentDBAV); /* because string in FM is intern. */

	/*
	 *	get message of the OK button of the AtariLib
	 */
	OKButtonMsg = strdup(FmCatGetS(BUTTON_SET, OK_BUTTON_MSG, "OK"));
	/*
	 *	create format string for line of found addresses
	 */
	AddrFoundFmtstr = strdup(FmCatGetS(101, 1, "Found %d address(es)."));

	if ((NoneDBstr == NULL) || (AddrFoundFmtstr == NULL) ||
	(OKButtonMsg == NULL)) {
		LogWrite("Addr main: Cannot allocate strings.");
		exit(1);
	}

	/*
	 *	start the baby...
	 */
	FmLoop();
}



/*
 *	:QuitAddr()
 *	quit the address application
 */

void	QuitAddr(w)
Widget	w;	/* quit button widget */
{
	char	*button;

	/*
	 *	ask if user really wants to quit
	 */
	button = (char *) PopupQuestionAndWait(w, 201);
			/*
			 * PopupQuestionAndWait(widget, msgnr)
			 * 	AtariLib fct to pop up a Question Box.
			 *	Msgnr is defined in "addrapp.msg", 
			 *	$set 253, where the AtariLib looks
			 *	for messages for alert boxes.
			 */	

	if (strcmp(button, OKButtonMsg) == 0) { 	/* OK button selected */
		if (!newflag && (db != NULL))
			close_db(db);			/* close open db */

		exit(0);
	}
}

/*
 *	:AddButton()
 *	add an address to the db
 */

void	AddButton(w)
Widget	w;	/* add button widget */
{
	int pos;	/* position of the new address in the b-tree */

	if ((newflag) || (db == NULL)) {
		alertbox(w, 101);	/* No database loaded */
		return;
	}

	if (*NameAV == '\0') {		/* no name was specified */
		alertbox(w, 102);
		return;
	}

	AV_to_Addr(&addr);		/* get data from text fields */

	/*
	 *	add address to db
	 */
	if (insert_addr(db, &addr) == -1) 
		alertbox(w, 103);  /* this should never happen, */
						/* because multiple entries */
						/* are possible */
	/*
	 *	see whether the list window is open, and if so, add
	 *	the mew address to the list
	 */
	FmGetAv(DListButtonAV);
	if (ListButtonAV) {			/* list window open */
		pos = find_pos_in_tree(db, (char *) NameAV) + 1;
		UpdateList(ListWidgetAV, &addr, pos); /* add new addr to list */
	}
	/*
	 *	clear text fields, if toggle button in options menu is turned on
	 */
	FmUpdateAv(DAutomaticClearAV);
	FmGetAv(DAutomaticClearAV);

	if (AutomaticClearAV == True) 
		clear_textfields();
	
	clear_addrFoundLine();		/* clear line with nr of found addr. */

}

/*
 *	:SearchButton()
 *	search an address in the db
 */

void	SearchButton(w)
Widget	w;	/* search button widget */
{
	ITEM		*addrlst;	/* ptr to found item (address) */
	ITEM		*search_addr(); /* fct to search an addr in db */
	char		tmpstr[256];
	static NINDEX	*ip;		/* ptr to current index in db */
	static int	ic; 		/* nr of current index (index count) */
	static int	last = FALSE; 	/* TRUE=last entry found */
	static char	tmpname[512];	/* copy of Name text field (seach key)*/

	FmGetAv(DNameAV);		/* name is the search key,  */
					/* so get the name from text field */


	if ((newflag) || (db == NULL)) {	/* no database loaded */
		alertbox(w, 101);
		return;
	}
	if (strcmp(NameAV, tmpname) != 0) {
		addrlst = search_addr(db, NameAV);	/* first search */
		if (addrlst == NULL) {			/* Address not found */
			alertbox(w, 104);
			return;
		}
		/*
		 *	build msg line with nr of found addr.
		 */
		sprintf(tmpstr, AddrFoundFmtstr, addrlst->count + 1);
		AddrFoundAV = XmStringCreateLtoR(tmpstr, 
			XmSTRING_DEFAULT_CHARSET);
		FmSetAv(DAddrFoundAV);
		FmUpdateAv(DAddrFoundAV);
		
		/*
		 *	read data
		 */
		if (read_addr(db, addrlst->index, &addr) == FAILURE) {
			alertbox(w, 107);
			return;
		}
		/*
		 *	copy read in data into text fields
		 */
		Addr_to_AV(&addr);

		if (addrlst->count > 0) {	/* multiple entries */
			ip = addrlst->ip;
			ic = 0;
			strcpy(tmpname, NameAV);
			last = FALSE;
		} else {			/* only one entry found */
			ip = NULL;
			tmpname[0] = EOF;
			tmpname[1] = '\0';
			last = TRUE;
		}
	} else if (!last) {				/* search again */
		if (ic < ip->count) {
			if (read_addr(db, ip->index[ic], &addr) == FAILURE) {
				alertbox(w, 107);
				return;
			}
				
			Addr_to_AV(&addr);
			ic++;
		} else {		/* there is another index pointer */
			ip = ip->nip;	/* get it (nip=next index ponter) */
			ic = 0;		/* index cound to 0 and do it again */
			if (read_addr(db, ip->index[ic], &addr) == FAILURE) {
				alertbox(w, 107);
				return;
			}
			Addr_to_AV(&addr);
		}
		if ((ip->nip == NULL) && (ic == ip->count)) {
			last = TRUE;		/* last entry */
		}
	}
}

/*
 *	:DoAddrListWindow()
 *	open the address list window
 */
void	DoAddrListWindow()
{
	if (newflag) {		/* No databse loaded */
		alertbox(ListShellAV, 101);
		return;
	}
	FmUpdateAv(DListButtonAV);
	FmGetAv(DListButtonAV);		/* status of toggle button in menu */

	if (ListButtonAV == True) {
		BuildAddrList(ListWidgetAV);	/* build the list */
		FmShowWidget(ListShellAV);	/* show list window */
	} else {			/* window already open */
		FmHideWidget(ListShellAV);	/* close it */
	}
}

/*
 *	:ListCloseButon()
 *	close the list window
 */

void ListCloseButton()
{
	FmHideWidget(ListShellAV);

	ListButtonAV = False;		/* set toggle button in menu to false */

	FmSetAv(DListButtonAV);
	FmUpdateAv(DListButtonAV);
}

/*
 *	:BuildAddrList
 *	build the address list
 */

void	BuildAddrList(w)
Widget	w;	/*  list widget */
{
	long		size;		/* size in chars of all addresses */
	register long	i, number;
	register int	j;
	ADDRESS		*addrlst;	/* ptr to list of all addresses */

	size = sizeof(ADDRESS) * db->number;
	addrlst = (ADDRESS *) malloc(size);
	if (addrlst == NULL) {
		alertbox(w, 105);
		return;
	}
	if (load_addr_list(db, addrlst, size, 0) == NULL) {
		free(addrlst);
		alertbox(w, 106);
		return;
	}
	number = db->number;
	for (i = 0; i < number; i++) {
		UpdateList(w, addrlst, 0);	/* add addr to list */
		addrlst++;
	}
	free(addrlst);
	BuildListFlag = True;
}

/*
 *	:DestroyList()
 *	remove all items from the list
 */
void	DestroyList()
{
	/*
	 *	just set the number of items in the XmList to zero and
	 *	Motif deletes all items in the list
	 */
	ListItemCountAV = 0;
	FmSetAv(DListItemCountAV);
	FmUpdateAv(DListItemCountAV);

	BuildListFlag = False;
}

/*
 *	:UpdateList
 *	add an entry to the list window
 */

void	UpdateList(w, addrlst, pos)
Widget	w;		/* list widget */
ADDRESS	*addrlst;	/* to be added address */
int	pos;		/* position, where addr will be inserted */
			/* 0 = end of list */
{
	XmString	listline;
	static char	linebuf[512];
	register int	j;
	register char	*s, *t;

	/*
	 *	copy the whole address into a line buffer and add the
	 *	buffer to the XmList
	 */
	t = linebuf;
	s = addrlst->name;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->name) + 2); j++)
		*t++ = ' ';	
	s = addrlst->street;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->street) + 2); j++)
		*t++ = ' ';	
	s = addrlst->city;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->city) + 2); j++)
		*t++ = ' ';	
	s = addrlst->state;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->state) + 2); j++)
		*t++ = ' ';	
	s = addrlst->zip;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->zip) + 2); j++)
		*t++ = ' ';	
	s = addrlst->country;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->country) + 2); j++)
		*t++ = ' ';	
	s = addrlst->phone;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->phone) + 2); j++)
		*t++ = ' ';	
	s = addrlst->fax;
	for (j = 0; *s; j++)
		*t++ = *s++;
	for (; j < (sizeof(addrlst->fax) + 2); j++)
		*t++ = ' ';	
	*t = '\0';

	listline = (XmString) XmStringCreateLtoR(linebuf,
		XmSTRING_DEFAULT_CHARSET);
	XmListAddItemUnselected(w, listline, pos);
}

/*
 *	:ListSelect()
 *	an item in the list was selected, copy the data into the text fields
 *	[XmList single selection callback]
 */

void	ListSelect(items)
XmStringTable	items;		/* selected item (addr) in the list */
{
	char	*name;
	register char *s;

	if (items == NULL) {
		clear_textfields();	/* nothing selected */
		return;
	}
	/*
	 *	get selected string
	 */
	XmStringGetLtoR(items[0], XmSTRING_DEFAULT_CHARSET, &name);

	/*
	 *	just need the name, not the whole address,
	 *	so replace the first space (which followes the name) with
	 *	a '\0'
	 */
	for (s = name; *s != ' '; s++)
		;
	*s = '\0';

	NameAV = name;		/* copy the name into the Name text field */
	FmSetAv(DNameAV);
	FmUpdateAv(DNameAV);

	SearchButton(toplevel); /* do a search and display the read in addr. */
				/* in the main window */
}

/*
 *	:ClearButton()
 *	clear all text input fields
 */

void	ClearButton()
{
	clear_textfields();
	clear_addrFoundLine();
}

/*
 *	:CreateDB()
 *	create a new db
 */

void	CreateDB()
{

	FmGetAv(DNewDBFnameAV);		/* get name of new db */

	if (!newflag && (db != NULL)) {
		if (BuildListFlag)
			DestroyList();

		close_db(db);		/* close opened db */
	}
	newflag = TRUE;
	/*
	 *	create new db
	 */
	db = create_db(NewDBFnameAV);
	 
	if (db == NULL) {	/* no db was created, so  the sting */
				/* in the msg line (name of current db) */
				/* has to be set to none */
		CurrentDBAV = XmStringCreateLtoR(NoneDBstr, 
			XmSTRING_DEFAULT_CHARSET);
		FmSetAv(DCurrentDBAV);
		FmUpdateAv(DCurrentDBAV);

		alertbox(MainWindowAV, 108);	/* No database was created */

		return;
	}
	newflag = FALSE;

	/*
	 *	output the name of the new db in the msg line
	 */
	CurrentDBAV = XmStringCreateLtoR(NewDBFnameAV, 
			XmSTRING_DEFAULT_CHARSET);
	FmSetAv(DCurrentDBAV);
	FmUpdateAv(DCurrentDBAV);
	clear_addrFoundLine();

}

/*
 *	:OpenDB()
 *	open an existing db
 */

void	OpenDB()
{
	char	fname[FNAMELEN];
	char	*path;

	/*
	 *	get path (including file name) of selected file 
	 *	in the file selection box
	 */
	XmStringGetLtoR(FileDirSpecAV, XmSTRING_DEFAULT_CHARSET, &path);


	trunc_fname(path, fname);	/* get file name */


	if (!newflag && (db != NULL)) {
		if (BuildListFlag)
			DestroyList();

		close_db(db);		/* close opended db */
		newflag = TRUE;
	}
	/*
	 *	open db
	 */
	if ((db = open_db(fname)) == NULL) {
				/* no db was created, so  the sting */
				/* in the msg line (name of current db) */
				/* has to be set to none */
		CurrentDBAV = XmStringCreateLtoR(NoneDBstr, 
			XmSTRING_DEFAULT_CHARSET);
		FmSetAv(DCurrentDBAV);
		FmUpdateAv(DCurrentDBAV);
		
		alertbox(MainWindowAV, 109);	/* No database was opened */

		return;
	}
	newflag = FALSE;

	/*
	 *	output the name of the new db in the msg line
	 */
	CurrentDBAV = XmStringCreateLtoR(fname, XmSTRING_DEFAULT_CHARSET);
	FmSetAv(DCurrentDBAV);
	FmUpdateAv(DCurrentDBAV);
	clear_addrFoundLine();
	/*
	 *	if the list window is open, we have to display a new list
	 */
	FmUpdateAv(DListButtonAV);
	FmGetAv(DListButtonAV);		/* status of toggle button in menu */

	if (ListButtonAV == True) 
		BuildAddrList(ListWidgetAV);
}

/*
 *	:CloseDB()
 *	close db
 */

void CloseDB()
{
	if (!newflag && (db != NULL)) {
		if (BuildListFlag)
			DestroyList();

		/*
		 *	close database
		 */
		close_db(db);
		newflag = TRUE;

				/* set the sting */
				/* in the msg line (name of current db) */
				/* to none */
		CurrentDBAV = XmStringCreateLtoR(NoneDBstr, 
			XmSTRING_DEFAULT_CHARSET);
		FmSetAv(DCurrentDBAV);
		FmUpdateAv(DCurrentDBAV);
		clear_addrFoundLine();
	}
}

/*
 *	:AV_to_Addr()
 *	copy values of the text input fields into the ADDRESS struct
 */

void	AV_to_Addr(a)
ADDRESS	*a;
{
	FmUpdateAv(DNameAV);
	FmUpdateAv(DStreetAV);
	FmUpdateAv(DCityAV);
	FmUpdateAv(DStateAV);
	FmUpdateAv(DZIPAV);
	FmUpdateAv(DCountryAV);
	FmUpdateAv(DPhoneAV);
	FmUpdateAv(DFAXAV);

	FmGetAv(DNameAV);
	FmGetAv(DStreetAV);
	FmGetAv(DCityAV);
	FmGetAv(DStateAV);
	FmGetAv(DZIPAV);
	FmGetAv(DCountryAV);
	FmGetAv(DPhoneAV);
	FmGetAv(DFAXAV);

	strNcpy(a->name, NameAV, sizeof(a->name));
	strNcpy(a->street , StreetAV, sizeof(a->street));
	strNcpy(a->city, CityAV, sizeof(a->city));
	strNcpy(a->state, StateAV, sizeof(a->state));
	strNcpy(a->zip, ZIPAV, sizeof(a->zip));
	strNcpy(a->country, CountryAV, sizeof(a->country));
	strNcpy(a->phone, PhoneAV, sizeof(a->phone));
	strNcpy(a->fax, FAXAV, sizeof(a->fax));
}

/*
 *	:Addr_to_AV()
 *	copy the ADDRESS struct into the text fields
 */

void	Addr_to_AV(a)
ADDRESS	*a;
{
	NameAV= a->name;
	StreetAV= a->street;
	CityAV= a->city;
	StateAV= a->state;
	ZIPAV= a->zip;
	CountryAV= a->country;
	PhoneAV= a->phone;
	FAXAV= a->fax;

	FmSetAv(DNameAV);
	FmSetAv(DStreetAV);
	FmSetAv(DCityAV);
	FmSetAv(DStateAV);
	FmSetAv(DZIPAV);
	FmSetAv(DCountryAV);
	FmSetAv(DPhoneAV);
	FmSetAv(DFAXAV);
}


/*
 *	:clear_textfields()
 *	clear all text input fields
 */

void	clear_textfields()
{
	NameAV = "";
	StreetAV = "";
	CityAV = "";
	StateAV = "";
	ZIPAV = "";
	CountryAV = "";
	PhoneAV = "";
	FAXAV = "";

	FmSetAv(DNameAV);
	FmSetAv(DStreetAV);
	FmSetAv(DCityAV);
	FmSetAv(DStateAV);
	FmSetAv(DZIPAV);
	FmSetAv(DCountryAV);
	FmSetAv(DPhoneAV);
	FmSetAv(DFAXAV);

}

/*
 *	:clear_addrFoundLine()
 *	clear the message line of the main window
 */
void	clear_addrFoundLine()
{
	AddrFoundAV = XmStringCreateLtoR("", XmSTRING_DEFAULT_CHARSET);
	FmSetAv(DAddrFoundAV);
	FmUpdateAv(DAddrFoundAV);
}	

/*
 *	:alertbox()
 *	pop up an alert box and print an alert message
 */

int	alertbox(w, msgnr)
Widget	w;
int	msgnr;
{	

	PopupErrorAndWait(w, msgnr);
}

/*
 *	:alert()
 *	write an internal error message to stdout
 *	this fct is called by the db functions in addrdb.c
 */

int	alert(msg)
char	*msg;
{	
	LogWrite(msg);
}

/*
 *	:trunc_fname()
 *	get the filename
 */

trunc_fname(path, fname)
char	*path;
char	*fname;	
{
	register int len, i;
	register char *s, *p;

	s = &path[len = strlen(path)];

	for (i = 0; (*s != '/') && (len >= 0); len--, i++)
		s--;

	s++;	
	p = fname;

	for (; *s; )
		*p++ = *s++;
	*p = '\0';

}

/*
 *	:strNcpy()
 *	copy max. n characters of s2 into s1
 */

char	*strNcpy(s1, s2, n)
register char	*s1, *s2;
register int	n;
{
	register int i;
	char	 *s = s1;

	for (i = 1; (i < n) && *s2; i++)
		*s1++ = *s2++;
	*s1 = '\0';

	return s;
}
