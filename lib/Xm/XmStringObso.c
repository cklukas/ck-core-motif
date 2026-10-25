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
 * 
 */
/*
 * HISTORY
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: XmStringObso.c /main/6 1995/09/19 23:13:52 cde-sun $"
#endif
#endif

#include "XmStringI.h"
#include "XmI.h"

/*
 * as close as we can come to Latin1Create without knowing the charset of
 * Latin1.  This imposes the semantic of \n meaning separator.
 */
XmString 
XmStringLtoRCreate(
        char *text,
        XmStringTag tag )
{
  char *start, *end;
  Boolean done;
  XmString string;

  if (!text) {
	return (NULL);
  }

  start = text;
  done = FALSE;

  /* Set the direction once only at the beginning. */
  string = XmStringDirectionCreate(XmSTRING_DIRECTION_L_TO_R);
    
  while ( ! done)		/* loop thu local copy */
    {				/* looking for \n */
      end = start;

      while ((*end != '\0') && (*end != '\n'))  end++;

      if (*end == '\0')
	done = TRUE;		/* we are at the end */

      /* Don't convert empty string unless it's an initial newline. */
      /* Done so StringHeight has clue to size of empty lines. */
      if ((start != end) || (start == text))
        string = XmStringConcatAndFree (string, 
					_XmStringNCreate(start, tag, 
							 end - start));

      if (string == NULL)
	return (NULL);
      
      /* Make a separator if this isn't the last segment. */
      if (!done) {
        string = XmStringConcatAndFree(string, XmStringSeparatorCreate());
	if (string == NULL)
	  return (NULL);
	start = ++end;		/* start at next char */
      }
    }

  return (string);
}

XmString 
XmStringCreateLtoR(
        char *text,
        XmStringTag tag )
{
  return (XmStringLtoRCreate (text, tag));
}


/*
 * build an external TCS 'segment', just a high level create
 */
XmString 
XmStringSegmentCreate(
        char *text,
        XmStringTag tag,
#if NeedWidePrototypes
        int direction,
        int separator )
#else
        XmStringDirection direction,
        Boolean separator )
#endif /* NeedWidePrototypes */
{
  XmString result;

  result = XmStringConcatAndFree (XmStringDirectionCreate (direction),
				  XmStringCreate (text, tag));

  if (result == NULL)
    return (NULL);

  if (separator)
    result = XmStringConcatAndFree (result, XmStringSeparatorCreate ());

  return result;
}

/*
 * Convenience routine to create an XmString from a NULL terminated string.
 */
XmString 
XmStringCreateSimple(
        char *text )
{
  return (XmStringCreate(text, XmSTRING_DEFAULT_CHARSET));
}
/*
 * concat two external strings.  Only concat a component at a time
 * so that we always wind up with a meaningful string
 */
XmString 
XmStringNConcat(XmString first,
		XmString second,
		int n )
{
  XmString	tmp, ret_val;
  
  tmp = XmStringConcat(first, second);
  if (tmp == NULL)
    return (NULL);

  ret_val = XmStringNCopy(tmp, XmStringLength(first) + n);
  
  XmStringFree(tmp);
  
  return(ret_val);
}

/*
 * Copy a compound string, such that the equivalent ASN.1 form
 * has <= n bytes.  Only copy a component at a time
 * so that we always wind up with a meaningful string
 */
XmString 
XmStringNCopy(
        XmString str,
        int n )
{
  unsigned char	*tmp;
  unsigned int	len;
  XmString	ret_val;
  
  len = XmCvtXmStringToByteStream(str, &tmp);
  
  if (n >= len) /* No need to truncate */
    {
      ret_val = XmStringCopy(str);
    }
  else /* Truncate and convert */
    {
      tmp = _XmStringTruncateASN1(tmp, n);
      ret_val = XmCvtByteStreamToXmString(tmp);
    }
  
  XtFree((char *)tmp);
  
  return(ret_val);
}

/* Compare ASN.1 form of strings. */
Boolean 
XmStringByteCompare(
        XmString a1,
        XmString b1 )
{
    unsigned char  *a = NULL;
    unsigned char  *b = NULL;
    unsigned short a_length, b_length;
    Boolean	   ret_val;

    if ((a1 == NULL) && (b1 == NULL)) {
	return (TRUE);
    }
    if ((a1 == NULL) || (b1 == NULL)) {
	return (FALSE);
    }

    a_length = XmCvtXmStringToByteStream(a1, &a);
    b_length = XmCvtXmStringToByteStream(b1, &b);

    if ((a_length == 0 && a == NULL) || (b_length == 0 && b == NULL)) {
        XtFree((char *)a);
        XtFree((char *)b);
        return (FALSE);
    }

    if ((a_length != b_length) || (memcmp(a, b, a_length) != 0))
      ret_val = FALSE;
    else ret_val = TRUE;

    XtFree((char *)a);
    XtFree((char *)b);

    return(ret_val);
}

