/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: UilDB.c /main/11 1996/11/21 20:03:11 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


/*
 *++
 *  FACILITY:
 *
 *      UIL Bindary Database :
 *
 *  ABSTRACT:
 *
 *--
 */

/*
 * This file contains routines which change the internal tables of UIL based on
 * a binary data base parameter in the command line 
 */


/*
 *
 *  INCLUDE FILES
 *
 */

#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#include <unistd.h>
#endif




#include "UilDefI.h"

#define X_INCLUDE_PWD_H
#define XOS_USE_XT_LOCKING

#ifdef HAVE_X11_XOS_R_H
#include <X11/Xos_r.h>
#else
#include <Xm/Xmos_r.h>
#endif

#include <stdio.h>
#include <limits.h>
#include <stdint.h>

/*
 *
 *  TABLE OF CONTENTS
 *
 *
 */


/*
 *
 *  DEFINE and MACRO DEFINITIONS
 *
 */
#define _check_read( __number_returned ) \
	if (( (__number_returned) != 1) || (feof(dbfile)) || (ferror(dbfile)) ) \
	{  diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column ); } 




/*
 *
 *  EXTERNAL VARIABLE DECLARATIONS
 *
 */

/*
 *
 *  GLOBAL VARIABLE DECLARATIONS
 *
 */


/*
 *
 *  OWN VARIABLE DECLARATIONS
 *
 */
static FILE *dbfile;
static int  num_bits;

static Boolean
db_read_table(void *table, size_t table_size)
{
    int return_num_items;

    if (table_size == 0)
	return True;
    if (table == NULL)
	{
	diag_issue_diagnostic(d_bad_database, diag_k_no_source,
			      diag_k_no_column);
	return False;
	}

    return_num_items = (int)fread(table, table_size, 1, dbfile);
    if (return_num_items != 1 || feof(dbfile) || ferror(dbfile))
	{
	diag_issue_diagnostic(d_bad_database, diag_k_no_source,
			      diag_k_no_column);
	return False;
	}
    return True;
}

/* Allocate one byte for an empty serialized table so that every fread target
 * remains a valid object, even when the database legitimately has no entries.
 */
static void *
db_allocate_table(size_t table_size)
{
    void *table = XtMalloc((Cardinal)(table_size == 0 ? 1 : table_size));

    if (table == NULL)
	diag_issue_diagnostic(d_bad_database, diag_k_no_source,
			      diag_k_no_column);
    return table;
}

void db_incorporate()

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine incorporate the binary database passed in the command line.
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

/*
 *  External Functions
 */

/*
 *  Local variables
 */
{
    int			first_character;
    _db_header		header;
    _db_globals		globals;

    db_open_file();

    if (!db_read_table(&globals, sizeof(_db_globals)))
	goto db_incorporate_done;

    /*
     * Some heuristics to see if this is a reasonable database.
     * The magic numbers are about 10 times as big as the DXm database
     * for DECWindows V3. The diagnostic does a fatal exit.
     */
    if ( globals.version < 0 ||
	 globals.uil_max_arg < 0 || globals.uil_max_arg>5000 ||
	 globals.uil_max_charset < 0 || globals.uil_max_charset>200 ||
	 globals.charset_lang_table_max < 0 || globals.charset_lang_table_max>1000 ||
	 globals.uil_max_object < 0 || globals.uil_max_object>500 ||
	 globals.uil_max_reason < 0 || globals.uil_max_reason>1000 ||
	 globals.uil_max_enumval < 0 || globals.uil_max_enumval>3000 ||
	 globals.uil_max_enumset < 0 || globals.uil_max_enumset>1000 ||
	 globals.key_k_keyword_count < 0 || globals.key_k_keyword_count>10000 ||
	 globals.key_k_keyword_max_length < 0 || globals.key_k_keyword_max_length>200 ||
	 globals.uil_max_child < 0 || globals.uil_max_child>250)
	{
	diag_issue_diagnostic (d_bad_database,
			       diag_k_no_source,
			       diag_k_no_column);
	fclose (dbfile);
	return;
	}

    uil_max_arg = globals.uil_max_arg ;
    uil_max_charset = globals.uil_max_charset ;
    charset_lang_table_max = (unsigned short)globals.charset_lang_table_max ;
    uil_max_object = globals.uil_max_object ;
    uil_max_reason = globals.uil_max_reason ;
    uil_max_enumval = globals.uil_max_enumval ;
    uil_max_enumset = globals.uil_max_enumset ;
    key_k_keyword_count = globals.key_k_keyword_count ;
    key_k_keyword_max_length = globals.key_k_keyword_max_length ;
    uil_max_child = globals.uil_max_child;
    num_bits = (uil_max_object +7) / 8;

    if (globals.version > DB_Compiled_Version) 
	diag_issue_diagnostic( d_future_version, diag_k_no_source, diag_k_no_column );

    for (;;) 
	{
	first_character = fgetc(dbfile);
	if (first_character == EOF)
	    {
	    if (ferror(dbfile))
		{
		diag_issue_diagnostic(d_bad_database, diag_k_no_source,
				      diag_k_no_column);
		goto db_incorporate_done;
		}
	    break;
	    }
	if (ungetc(first_character, dbfile) == EOF ||
	    !db_read_table(&header, sizeof(_db_header)))
	    goto db_incorporate_done;
	if (header.num_items < 0 || header.table_size < 0)
	    {
	    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
				    diag_k_no_column );
	    goto db_incorporate_done;
	    }
	switch (header.table_id)
	    {
	    case Constraint_Tab:
		if ((size_t)header.num_items > (size_t)header.table_size)
		    {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		    goto db_incorporate_done;
		    }
		constraint_tab = (unsigned char *) db_allocate_table((size_t)header.table_size);
		if (constraint_tab == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(constraint_tab, (size_t)header.num_items))
		    goto db_incorporate_done;
		break;
	    case Argument_Type_Table_Value:
		if ((size_t)header.num_items > (size_t)header.table_size)
		    {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		    goto db_incorporate_done;
		    }
		/*
		 * NOTE: The first entry is not used but we copy it anyway
		 */
		argument_type_table = (unsigned char *) db_allocate_table((size_t)header.table_size);
		if (argument_type_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(argument_type_table, (size_t)header.num_items))
		    goto db_incorporate_done;
		break;
	    case Child_Class_Table:
		if ((size_t)header.num_items > (size_t)header.table_size)
		    {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		    goto db_incorporate_done;
		    }
		/*
		 * NOTE: The first entry is not used but we copy it anyway
		 */
		child_class_table = 
		  (unsigned char *) db_allocate_table((size_t)header.table_size);
		if (child_class_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(child_class_table, (size_t)header.num_items))
		    goto db_incorporate_done;
		break;
	    case Charset_Wrdirection_Table:
		if ((size_t)header.num_items > (size_t)header.table_size)
		    {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		    goto db_incorporate_done;
		    }
		charset_writing_direction_table = (unsigned char *) db_allocate_table((size_t)header.table_size);
		if (charset_writing_direction_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(charset_writing_direction_table,
				   (size_t)header.num_items))
		    goto db_incorporate_done;
		break;
	    case Charset_Parsdirection_Table:
		if ((size_t)header.num_items > (size_t)header.table_size)
		    {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		    goto db_incorporate_done;
		    }
		charset_parsing_direction_table = (unsigned char *) db_allocate_table((size_t)header.table_size);
		if (charset_parsing_direction_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(charset_parsing_direction_table,
				   (size_t)header.num_items))
		    goto db_incorporate_done;
		break;
	    case Charset_Charsize_Table:
		if ((size_t)header.num_items > (size_t)header.table_size)
		    {
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		    goto db_incorporate_done;
		    }
		charset_character_size_table = (unsigned char *) db_allocate_table((size_t)header.table_size);
		if (charset_character_size_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(charset_character_size_table,
				   (size_t)header.num_items))
		    goto db_incorporate_done;
		break;
	    case Key_Table:
	    case Key_Table_Case_Ins:
		if (!db_read_ints_and_string (&header))
		    goto db_incorporate_done;
		break;
	    case Allowed_Argument_Table:
	    case Allowed_Child_Table:
	    case Allowed_Control_Table:
	    case Allowed_Reason_Table:
		if (!db_read_char_table (&header))
		    goto db_incorporate_done;
		break;
	    case Charset_Xmstring_Names_Table:
	    case Charset_Lang_Names_Table:
	    case Uil_Widget_Names:
	    case Uil_Children_Names:
	    case Uil_Argument_Names:
	    case Uil_Reason_Names:
	    case Uil_Enumval_names:
	    case Uil_Charset_Names:
	    case Uil_Widget_Funcs:
	    case Uil_Argument_Toolkit_Names:
	    case Uil_Reason_Toolkit_Names:
		if (!db_read_length_and_string (&header))
		    goto db_incorporate_done;
		break;
	    case Charset_Lang_Codes_Table:
		charset_lang_codes_table = (unsigned short int *) db_allocate_table((size_t)header.table_size);
		if (charset_lang_codes_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(charset_lang_codes_table,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    case Argument_Enum_Set_Table:
		argument_enumset_table = (unsigned short int *) db_allocate_table((size_t)header.table_size);
		if (argument_enumset_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(argument_enumset_table,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    case Related_Argument_Table:
		related_argument_table = (unsigned short int *) db_allocate_table((size_t)header.table_size);
		if (related_argument_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(related_argument_table,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    case Uil_Gadget_Funcs:
		uil_gadget_variants = (unsigned short int *) db_allocate_table((size_t)header.table_size);
		if (uil_gadget_variants == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(uil_gadget_variants,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    case Uil_Urm_Nondialog_Class:
		uil_urm_nondialog_class = (unsigned short int *) db_allocate_table((size_t)header.table_size);
		if (uil_urm_nondialog_class == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(uil_urm_nondialog_class,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    case Uil_Urm_Subtree_Resource:
		uil_urm_subtree_resource = (unsigned short int *) db_allocate_table((size_t)header.table_size);
		if (uil_urm_subtree_resource == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(uil_urm_subtree_resource,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    case Enum_Set_Table:
		if (!db_read_int_and_shorts(&header))
		    goto db_incorporate_done;
		break;
	    case Enumval_Values_Table:
		enumval_values_table = (int *) db_allocate_table((size_t)header.table_size);
		if (enumval_values_table == NULL)
		    goto db_incorporate_done;
		if (!db_read_table(enumval_values_table,
				   (size_t)header.table_size))
		    goto db_incorporate_done;
		break;
	    default:
		diag_issue_diagnostic( d_bad_database, diag_k_no_source, diag_k_no_column );
		goto db_incorporate_done;
	    } /* end switch */
    } /* end for */

db_incorporate_done:
    fclose (dbfile);
    return;
}





Boolean db_read_ints_and_string(_db_header_ptr header)
/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in tables of integers and one string unsigned chars and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Key_Table:
 *	    Key_Table_Case_Ins:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */
{
	if (header == NULL)
	    return False;

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int			i;
	size_t		string_size = 0;
	key_keytable_entry_type	*table = NULL;
	char			*string_table;
	 
	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here to protect against bad
	     *	     pointers.
	     */
	    case Key_Table:
		key_table = (key_keytable_entry_type *) XtCalloc (1, (Cardinal)header->table_size);
		table = key_table;
		break;
	    case Key_Table_Case_Ins:
		key_table_case_ins = (key_keytable_entry_type *) XtCalloc (1, (Cardinal)header->table_size);
		table = key_table_case_ins;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_ints_and_string");
	    }

	/* 
	 * Get the entire table with one read. 
	 * Then loop through the table and up the length of the strings.
	 * Get all the strings with one read.
	 * Reassign the addresses
	 */
	if (table == NULL || header->num_items < 0 || header->table_size < 0 ||
	    (size_t)header->num_items >
		(size_t)header->table_size / sizeof(*table))
	    return False;
	if (!db_read_table(table, (size_t)header->table_size))
	    return False;

	for ( i=0 ; i<header->num_items; i++)
	    {
	    /*
	     * Add one for the null character on the string
	     */
	    if (string_size > (size_t)UINT_MAX -
		(size_t)table[i].b_length - 1)
		{
		diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		return False;
		}
	    string_size += (size_t)table[i].b_length + 1;
	    };

	string_table = XtMalloc ((Cardinal)string_size);
	if (!db_read_table(string_table, string_size))
	    return False;

	for ( i=0 ; i<header->num_items; i++)
	    {
	    table[i].at_name = string_table;
	    string_table +=  table[i].b_length + 1; 
	    };

	return True;
}





Boolean db_read_char_table(_db_header_ptr header)
/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in tables of unsigned chars and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Allowed_Argument_Table:
 *	    Allowed_Child_Table:
 *	    Allowed_Control_Table:
 *	    Allowed_Reason_Table:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */
{
/*
 *  External Functions
 */

/*
 *  Local variables
 */
	unsigned char	**ptr = NULL;
	int		i;
	unsigned char	*table = NULL;
	size_t		item_count, pointer_bytes, table_bytes;

	if (header == NULL || header->num_items < 0 ||
	    header->table_size < 0 || header->num_items == INT_MAX || num_bits <= 0)
	    return False;
	item_count = (size_t)header->num_items + 1;
	if (item_count > SIZE_MAX / sizeof(*ptr) ||
	    item_count * sizeof(*ptr) > (size_t)header->table_size ||
	    (size_t)header->num_items > SIZE_MAX / (size_t)num_bits)
	    return False;
	pointer_bytes = item_count * sizeof(*ptr);
	table_bytes = (size_t)header->num_items * (size_t)num_bits;
	if (table_bytes > UINT_MAX)
	    return False;
	 
	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here to protect against bad
	     *	     pointers.
	     */
	    case Allowed_Argument_Table:
		allowed_argument_table = (unsigned char **) XtCalloc (1, (Cardinal)header->table_size);
		ptr = allowed_argument_table;
		break;
	    case Allowed_Child_Table:
		allowed_child_table = 
		  (unsigned char **) XtCalloc (1, (Cardinal)header->table_size);
		ptr = allowed_child_table;
		break;
	    case Allowed_Control_Table:
		allowed_control_table = (unsigned char **) XtCalloc (1, (Cardinal)header->table_size);
		ptr = allowed_control_table;
		break;
	    case Allowed_Reason_Table:
		allowed_reason_table = (unsigned char **) XtCalloc (1, (Cardinal)header->table_size);
		ptr = allowed_reason_table;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_char_table");
	}

	/*
	 * Read in the entire table contents in one whack.
	 * Then go through the table and set the addresses
	 */
	table = (unsigned char *) XtMalloc ((Cardinal)table_bytes);
	if (ptr == NULL)
	    {
	    diag_issue_diagnostic(d_bad_database, diag_k_no_source,
				  diag_k_no_column);
	    return False;
	    }
	if (!db_read_table(table, table_bytes))
	    return False;
	for ( i=1 ; i<=header->num_items; i++ )
	    {
	    ptr[i] = table;
	    table += num_bits;
	    };

	return True;
}



Boolean db_read_length_and_string(_db_header_ptr header)
/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in length and strings of unsigned chars and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Charset_Xmstring_Names_Table:
 *	    Charset_Lang_Names_Table:
 *	    Uil_Widget_Names:
 *	    Uil_Children_Names:
 *	    Uil_Argument_Names:
 *	    Uil_Reason_Names:
 *	    Uil_Enumval_names:
 *	    Uil_Charset_Names:
 *	    Uil_Widget_Funcs:
 *	    Uil_Argument_Toolkit_Names:
 *	    Uil_Reason_Toolkit_Names:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */
{
/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int		i;
	size_t		item_count, lengths_bytes, string_size = 0;
	int		*lengths;
	char		*string_table;
	char		**table = NULL;

	if (header == NULL || header->num_items < 0 ||
	    header->table_size < 0 || header->num_items == INT_MAX)
	    return False;
	item_count = (size_t)header->num_items + 1;
	if (item_count > SIZE_MAX / sizeof(*table) ||
	    item_count * sizeof(*table) > (size_t)header->table_size ||
	    item_count > SIZE_MAX / sizeof(*lengths) ||
	    item_count * sizeof(*lengths) > UINT_MAX)
	    return False;
	lengths_bytes = item_count * sizeof(*lengths);
	 
	switch (header->table_id)
	    {
	    /*
	     * NOTE: Calloc is used here because it might be possible to
	     *	     have a string of zero length, particularly for the
	     *	     first record. Ergo we Calloc to protect against bad
	     *	     pointers.
	     */
	    case Charset_Xmstring_Names_Table:
		charset_xmstring_names_table = (char **) XtCalloc (1, header->table_size);
		table = charset_xmstring_names_table;
		break;
	    case Charset_Lang_Names_Table:
		charset_lang_names_table = (char **) XtCalloc (1, header->table_size);
		table = charset_lang_names_table;
		break;
	    case Uil_Widget_Names:
		uil_widget_names = (char **) XtCalloc (1, header->table_size);
		table = uil_widget_names ;
		break;
	    case Uil_Children_Names:
		uil_child_names = (char **) XtCalloc (1, header->table_size);
		table = uil_child_names ;
		break;
	    case Uil_Argument_Names:
		uil_argument_names = (char **) XtCalloc (1, header->table_size);
		table = uil_argument_names;
		break;
	    case Uil_Reason_Names:
		uil_reason_names = (char **) XtCalloc (1, header->table_size);
		table = uil_reason_names;
		break;
	    case Uil_Enumval_names:
		uil_enumval_names = (char **) XtCalloc (1, header->table_size);
		table = uil_enumval_names;
		break;
	    case Uil_Charset_Names:
		uil_charset_names = (char **) XtCalloc (1, header->table_size);
		table = uil_charset_names;
		break;
	    case Uil_Widget_Funcs:
		uil_widget_funcs = (char **) XtCalloc (1, header->table_size);
		table = uil_widget_funcs;
		break;
	    case Uil_Argument_Toolkit_Names:
		uil_argument_toolkit_names = (char **) XtCalloc (1, header->table_size);
		table = uil_argument_toolkit_names;
		break;
	    case Uil_Reason_Toolkit_Names:
		uil_reason_toolkit_names = (char **) XtCalloc (1, header->table_size);
		table = uil_reason_toolkit_names;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_length_and_string");
	    }

	/* 
	 * Get the lengths of all the strings with one read.  
	 * Then loop through the table and up the length of the strings.
	 * Get all the strings with one read. 
	 * Reassign the addresses using the length table and string table.
	 * Cleanup by Freeing length table.
	 * 
	 * NOTE: In some tables the counting starts at 1 not 0 so you
	 *	 have to be carefull.
	 */

	lengths = (int *) XtMalloc ((Cardinal)lengths_bytes);
	if (table == NULL)
	    {
	    diag_issue_diagnostic(d_bad_database, diag_k_no_source,
				  diag_k_no_column);
	    return False;
	    }
	if (!db_read_table(lengths, lengths_bytes))
	    return False;
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    /*
	     * Add one for the null terminator
	     */
	    if (lengths[i] < 0)
		{
		XtFree ((char *)lengths);
		diag_issue_diagnostic( d_bad_database, diag_k_no_source,
					    diag_k_no_column );
		return False;
		}
	    if (lengths[i])
		{
		if (string_size > (size_t)UINT_MAX - (size_t)lengths[i] - 1)
		    {
		    XtFree ((char *)lengths);
		    diag_issue_diagnostic( d_bad_database, diag_k_no_source,
						 diag_k_no_column );
		    return False;
		    }
		string_size += (size_t)lengths[i] + 1;
		}
	    }

	string_table = XtMalloc ((Cardinal)string_size);
	if (!db_read_table(string_table, string_size))
	    {
	    XtFree ((char *)lengths);
	    return False;
	    }
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (lengths[i])
		{
		table[i] = string_table;
/* BEGIN HaL Fix CR 5618 */
		  if ((header->table_id == Uil_Widget_Names) &&
		      (strcmp(table[i], "user_defined") == 0))
		    uil_sym_user_defined_object = (unsigned short)i;
/* END HaL Fix CR 5618 */
		string_table +=  lengths[i] + 1;
		}
	    }

	XtFree ((char *)lengths);

	return True;
}



Boolean db_read_int_and_shorts(_db_header_ptr header)
/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine reads in a structure consisting of one integer and a 
 *	pointer to a table of integer and places them into 
 *	memory. It will Malloc new space for the table. The tables supported
 *	this routine are:
 *
 *	    Enum_Set_Table:
 *
 *
 *  FORMAL PARAMETERS:
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */
{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	int			i;
	size_t		item_count, int_table_count = 0;
	UilEnumSetDescDef	*table = NULL;
	unsigned short int 	*int_table = NULL;

	if (header == NULL || header->num_items < 0 ||
	    header->table_size < 0 || header->num_items == INT_MAX)
	    return False;
	item_count = (size_t)header->num_items + 1;
	if (item_count > SIZE_MAX / sizeof(*table) ||
	    item_count * sizeof(*table) > (size_t)header->table_size)
	    return False;
	 
	switch (header->table_id)
	    {
	    case Enum_Set_Table:
		enum_set_table = (UilEnumSetDescDef *) XtCalloc (1, (Cardinal)header->table_size);
		table = enum_set_table;
		break;
	    default:
		diag_issue_internal_error ("Bad table_id in db_read_int_shorts");
	    }

	/* 
	 * Get the entire table with one read. 
	 * Then loop through the table and add up the number of ints in each int table.
	 * Get all the integer tables with one read.
	 * Reassign the addresses of the tables.
	 */
	if (!db_read_table(table, (size_t)header->table_size))
	    return False;
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (table[i].values_cnt < 0 ||
		int_table_count > (size_t)UINT_MAX / sizeof(unsigned short) -
		    (size_t)table[i].values_cnt)
		return False;
	    int_table_count += (size_t)table[i].values_cnt;
	    }

	int_table = (unsigned short int *) XtCalloc
	    (1, (Cardinal)(sizeof(unsigned short) * int_table_count));
	if (!db_read_table(int_table,
			   sizeof(unsigned short) * int_table_count))
	    return False;
	for ( i=0 ; i<=header->num_items; i++)
	    {
	    if (table[i].values_cnt)
		{
		table[i].values = int_table;
		int_table += table[i].values_cnt;
		}
	    }

	return True;
}



void db_open_file ()

/*
 *++
 *
 *  PROCEDURE DESCRIPTION:
 *
 *	This routine opens the binary database file in a platform-dependent way,
 *	performing i18n language switching in order to do so.
 *
 *	Per the latest agreement on semantics, this routine does:
 *		- first, try to open in the local directory (that is, with
 *		  no switching).
 *		- second, try language switching and open
 *
 *  FORMAL PARAMETERS:
 *
 *	name		A system-dependent string specifying the IDB file
 *			to be opened.
 *
 *  IMPLICIT INPUTS:
 *
 *  IMPLICIT OUTPUTS:
 *
 *  FUNCTION VALUE:
 *
 *  SIDE EFFECTS:
 *
 *--
 */

{

/*
 *  External Functions
 */

/*
 *  Local variables
 */
	char			*resolvedname;		/* current resolved name */
	SubstitutionRec		subs[3];
	char			*wmdPath;

	/*
	 * Use XtFindFile instead of XtResolvePathName. XtResolvePathName requires a
	 * display which UIL doesn't have. At the current time there is no support for
	 * $LANG in the path string. If such support was deamed necessary, the %L, %l,
	 * %t, %c values would be set up as subs here using globals from the fetch of
	 * LANG variable used to determine the default codeset (or vice versa depending
	 * on which is called first) 
	 *
	 * If the last 4 characters of the file name are not .bdb
	 * then pass in the suffix of .bdb. If a file isn't found with the suffix passed
	 * in then try without the suffix.
	 */

	/*
	 * Make sure 'S' is the last one so we can remove the suffix for the first pass.
	 */
	subs[0].match = 'N';
	subs[0].substitution = Uil_cmd_z_command.ac_database;
	subs[1].match = 'T';
	subs[1].substitution = "wmd";
	subs[2].match = 'S';
	subs[2].substitution = ".wmd";

	wmdPath = init_wmd_path(Uil_cmd_z_command.ac_database);

	resolvedname = 0;

	/*
	 * Check and see if the .wmd suffix is already on the file. If not then try to
	 * resolve the pathname with .wmd suffix first. If that fails or the suffix is
	 * already on the file then just try to resolve the pathname.
	 */
	if (strlen(Uil_cmd_z_command.ac_database) < 4 ||
            strcmp(Uil_cmd_z_command.ac_database +
                   strlen(Uil_cmd_z_command.ac_database) - 4, ".wmd") != 0)
		resolvedname = XtFindFile(wmdPath,
					      subs,
					      XtNumber(subs),
					      (XtFilePredicate)NULL);

	/*
	 * No .wmd suffix or a failure to resolve the pathname with the .wmd suffix
	 * Try without the suffix.
	 */
	subs[2].substitution = "";
	if (resolvedname == 0) 
		resolvedname = XtFindFile(wmdPath,
					      subs,
					      XtNumber(subs),
					      (XtFilePredicate)NULL);

	if (resolvedname == 0)
	    {
	    diag_issue_diagnostic( d_wmd_open,
				   diag_k_no_source, diag_k_no_column,
				   Uil_cmd_z_command.ac_database);
	    }

	dbfile = fopen (resolvedname, "rb");

	/* If the file is not found, a fatal error is generated.	*/
	if (dbfile == NULL)
	    {
	    diag_issue_diagnostic( d_src_open,
				   diag_k_no_source, diag_k_no_column,
				   resolvedname);
	    }

	XtFree(wmdPath);
        XtFree(resolvedname);
	return;
}




String get_root_dir_name()
{
	int uid;
	_Xgetpwparams pwd_buf;
	struct passwd *pwd_value;
	static char *ptr = NULL;
	char *outptr;
	size_t length;

	if (ptr == NULL)
	{
	if((ptr = (char *)getenv("HOME")) == NULL) 
	    {
	    if((ptr = (char *)getenv(USER_VAR)) != NULL) 
		{
		pwd_value = _XGetpwnam(ptr, pwd_buf);
		}
	    else 
		{
		uid = getuid();
		pwd_value = _XGetpwuid(uid, pwd_buf);
		}
	    if (pwd_value != NULL)
		{
		ptr = pwd_value->pw_dir;
		}
	    else 
		{
		 ptr = "";
		}
	    }
	}

	length = strlen(ptr);
	if (length > (size_t)UINT_MAX - 2)
	    return NULL;
	outptr = XtMalloc ((Cardinal)(length + 2));
	if (outptr == NULL)
	    return NULL;
	strcpy (outptr, ptr);
	strcat (outptr, "/");
	return outptr; 
}

/*
 * XAPPLRES_DEFAULT and UIDPATH_DEFAULT are intentionally split to support
 * SCCS. DO NOT reformat the lines else %-N-%-S could be converted by SCCS into
 * something totally bizarre causing MrmOpenHierarchy failures.
 */

/* The following are usually defined in the Makefile */

#ifndef LIBDIR
#define LIBDIR "/usr/lib/X11"
#endif
#ifndef INCDIR
#define INCDIR "/usr/include/X11"
#endif

static char libdir[] = LIBDIR;
static char incdir[] = INCDIR;

static char XAPPLRES_DEFAULT[] = "\
%%N\
%%S:\
%s/%%T/%%N\
%%S:\
%s%%T/%%N\
%%S:\
%s%%N\
%%S:\
%s/%%T/%%N\
%%S:\
%s/%%T/%%N\
%%S";

static char WMDPATH_DEFAULT[] = "\
%%N\
%%S:\
%s%%T/%%N\
%%S:\
%s%%N\
%%S:\
%s/%%L/%%T/%%N\
%%S:\
%s/%%T/%%N\
%%S";

static char ABSOLUTE_PATH[] = "\
%N\
%S";

String init_wmd_path(String filename)
{
    String path;
    String old_path;
    String homedir;
    String wmd_path;
    int path_length;


    if (filename[0] == '/')
	{
	wmd_path = XtNewString(ABSOLUTE_PATH);
	}
    else
	{
	path = (char *)getenv ("WMDPATH");
	if (path  == NULL) 
	    {
	    homedir = get_root_dir_name();
	    old_path = (char *)getenv ("XAPPLRESDIR");
	    if (old_path == NULL) 
		{
	    path_length = snprintf(NULL, 0, WMDPATH_DEFAULT,
			 homedir, homedir, libdir, incdir);
	    if (path_length < 0)
		{
		XtFree(homedir);
		return NULL;
		}
	    wmd_path = XtMalloc((size_t)path_length + 1);
	    (void)snprintf(wmd_path, (size_t)path_length + 1, WMDPATH_DEFAULT,
		    homedir, homedir, libdir, incdir);
		} 
	    else 
		{
	    path_length = snprintf(NULL, 0, XAPPLRES_DEFAULT,
			old_path,
			homedir, homedir, libdir, incdir);
	    if (path_length < 0)
		{
		XtFree(homedir);
		return NULL;
		}
	    wmd_path = XtMalloc((size_t)path_length + 1);
	    (void)snprintf(wmd_path, (size_t)path_length + 1, XAPPLRES_DEFAULT,
		    old_path, homedir, homedir, libdir, incdir);
		}
	    XtFree (homedir);
	    }
	else
	    {
	    wmd_path = XtNewString(path);
	    }
	}
    return (wmd_path);
}
