/* $TOG: TxtPropCv.c /main/15 1997/06/18 17:46:05 samborn $ */
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



#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#ifndef X_NOT_STDC_ENV
#include <stdlib.h>
#endif
#include <string.h>
#include <ctype.h>
#include <Xm/XmosP.h>
#include "XmI.h"
#include "XmStringI.h"



static Atom
GetLocaleEncodingAtom(Display *dpy)
{
  int ret_status = 0;
  XTextProperty tmp_prop;
  char * tmp_string = "ABC";  /* these are characters in XPCS, so... safe */
  Atom encoding;

  tmp_prop.value = NULL; /* just in case X doesn't do it */
  ret_status = XmbTextListToTextProperty(dpy, &tmp_string, 1,
					 (XICCEncodingStyle)XTextStyle, 
					 &tmp_prop);
  if (ret_status == Success)
    encoding = tmp_prop.encoding;
  else
    encoding = None;        /* XmbTextList... should always be able
			   * to convert XPCS characters; but in
			   * case its broken, this prevents a core
			   * dump.
			   */
  if (tmp_prop.value != NULL) XFree((char *)tmp_prop.value);
  return(encoding);
}

/************************************************************************
 *
 *  TextPropertyToSingleTextItem
 *  
 ************************************************************************/

static int
TextPropertyToSingleTextItem(Display *display, XTextProperty *text_prop,
			     char **text_item)
{
    int result, count;
    char **textlist;

    result = XmbTextPropertyToTextList(display, text_prop, &textlist,
				       &count);
    if (result != Success) return(result);
    if (count == 1)
    {
	/* Otherwise just return the first element of the 
	   text list */
	*text_item = XtNewString(textlist[0]);
	if (*text_item == NULL)
	  {
	    XFreeStringList(textlist);
	    return(XNoMemory);
	  }
	XFreeStringList(textlist);
    }
    else if (count > 1)
    {
	/* If we got back more than one string, then
	   let's concatenate them all together. */
	int i;
	size_t length = 0;
	char *newstring;
	
	/* First figure out how big the final string will be */
	for (i = 0; i < count; ++i)
	  {
	    size_t item_length = strlen(textlist[i]);
	    if (item_length > SIZE_MAX - length ||
		length + item_length == SIZE_MAX)
	      {
		XFreeStringList(textlist);
		return(XNoMemory);
	      }
	    length += item_length;
	  }
	/* Allocate a buffer and jam all the strings into it */
	newstring = (char *)XtMalloc(length + 1);
	if (newstring == NULL)
	  {
	    XFreeStringList(textlist);
	    return(XNoMemory);
	  }
	newstring[0] = '\0';
	for (i = 0; i < count; ++i)
	  strcat(newstring, textlist[i]);
	*text_item = newstring;
	XFreeStringList(textlist);
    }
    return(Success);
}

/************************************************************************
 *
 *  GetTextSegment
 *
 *  This function gets the next full segment from the given xmstring using
 *  the given xmcontext and tries to extract either locale text or
 *  string text (i.e. ISO8859-1 text) based on the texttype argument.
 *
 *  This function can return one of three return values:
 *
 *    _VALID_SEGMENT: we found a segment that we could decompose. The
 *		appropriate text has been stored in the given buffer
 *
 *    _INVALID_SEGMENT: we could not decompose the segment. The given
 *		buffer does not contain valid data
 *
 *    _NO_MORE_SEGMENTS: there are no more segments available in the
 *		xmstring using the xmcontext.
 *
 ************************************************************************/

/* Segment type return value */
enum { _VALID_SEGMENT, _INVALID_SEGMENT, _NO_MORE_SEGMENTS };

/* Valid text types */
enum { _LOCALE_TEXT, _STRING_TEXT };

/*ARGSUSED*/
static unsigned char
GetTextSegment(Display *display, /* unused */
	       XmStringContext xmcontext,
	       XmString xmstring, /* unused */
	       char **buffer, 
	       unsigned char texttype)
{
    XtPointer text;
    XmStringTag tag, *rendition_tags;
    XmTextType type;
    XmStringDirection direction;
    XmDirection push_before;
    Boolean separator, pop_after;
    unsigned int tag_count;
    unsigned char return_status, tabs;
    short char_count;
    char *encoding;
    int i;
    
    /* Initialize the locale_buffer just in case we need to return
       Failure at any point along the way. */
    *buffer = NULL;
    return_status = _VALID_SEGMENT;

    /* Decompose the xmstring and handle each segment separately. */
    if (_XmStringGetSegment(xmcontext, TRUE, FALSE, &text, &tag, &type,
			    &rendition_tags, &tag_count, &direction,
			    &separator, &tabs, &char_count,
			    &push_before, &pop_after))
    {
	switch (type)
	{
	  case XmMULTIBYTE_TEXT:
	  case XmWIDECHAR_TEXT:
	    if (texttype == _LOCALE_TEXT)
	    {
	      /* The text should be already be valid locale text */
	      char *tmp = XtMalloc(char_count + sizeof(wchar_t));
	      if (tmp == NULL)
		return(_INVALID_SEGMENT);
	      memcpy(tmp, text, char_count);
	      bzero(tmp + char_count, sizeof(wchar_t));

	      *buffer = tmp;
	    }
	    else if (texttype == _STRING_TEXT)
	    {
		*buffer = NULL;
		return(_INVALID_SEGMENT);
	    }
	    break;
	  case XmCHARSET_TEXT:
	    /* "tag" is the charset when type is CHARSET_TEXT */
	    encoding = XmMapSegmentEncoding(tag);
	    if (encoding != NULL)
	    {
		if (texttype == _LOCALE_TEXT &&
		    (strcmp(encoding, _MOTIF_DEFAULT_LOCALE) == 0 ||
		     strcmp(encoding, XmFONTLIST_DEFAULT_TAG) == 0))
		  /* @@ Is ISO8859-1 valid here? */
		{
		  /* Given the above charset encodings, the text should
		     already be valid locale text. */
		  char *tmp = XtMalloc(char_count + sizeof(wchar_t));
		  if (tmp == NULL)
		    return(_INVALID_SEGMENT);
		  memcpy(tmp, text, char_count);
		  bzero(tmp + char_count, sizeof(wchar_t));

		  *buffer = tmp;
		}
		else if (texttype == _STRING_TEXT &&
			 (strcmp(encoding, "ISO8859-1") == 0))
		{
		  /* The text is valid STRING text */
		  char *tmp = XtMalloc(char_count + sizeof(wchar_t));
		  if (tmp == NULL)
		    return(_INVALID_SEGMENT);
		  memcpy(tmp, text, char_count);
		  bzero(tmp + char_count, sizeof(wchar_t));

		  *buffer = tmp;
		}
		else return(_INVALID_SEGMENT);
	    }
	    else return(_INVALID_SEGMENT); /* the encoding was unregistered */
	    break;
	case XmNO_TEXT:
	    break;
	}
	/* Before returning Success, check to see whether we need to
	   prepend any tabs or append any newlines. */
	if ((return_status == _VALID_SEGMENT) &&
	    (separator == True || tabs != 0))
	{
	    size_t newlength;
	    size_t buffer_length = *buffer ? strlen(*buffer) : 0;
	    size_t prefix_length = (size_t)tabs + (separator ? 1U : 0U);
	    char *newstring;

	    if (buffer_length > SIZE_MAX - prefix_length - 1U)
	    {
		XtFree(*buffer);
		*buffer = NULL;
		return(_INVALID_SEGMENT);
	    }
	    newlength = buffer_length + prefix_length;
	    newstring = (char *)XtMalloc(sizeof(char) * (newlength + 1));
	    if (newstring == NULL)
	      {
		XtFree(*buffer);
		*buffer = NULL;
		return(_INVALID_SEGMENT);
	      }
	    for (i = 0; i < tabs; i++) newstring[i] = '\t';
	    if (*buffer != NULL)
	      strcpy(&newstring[i], *buffer);
	    else
	      newstring[i] = '\0';
	    strcat(newstring, "\n");
	    XtFree(*buffer);
	    *buffer = newstring;
	}
	return(return_status);
    }
    else
    {
	/* Return NULL text but a value of Success to designate that
	   there are no more segments to process. */
	*buffer = NULL;
	return(_NO_MORE_SEGMENTS);
    }
}
    
/************************************************************************
 *
 *  GetUseableText
 *
 *  Given an XmString, decompose it into text specified by the texttype
 *  argument. Valid text types are _LOCALE_TEXT for text encoded in the
 *  current locale and _STRING_TEXT for text encoded in ISO8859-1 with
 *  newlines and tabs.
 *
 *  If the "strict" argument is False, then any compound strings which
 *  have a segment that can not be decomposed as specified will be converted
 *  to compound text and then converted to the appropriate text by using
 *  a series of XmbTextListToTextProperty and XmbTextPropertyToTextList
 *  calls to acchive the conversion.
 *
 *  If the "strict" argument is True, then all compound strings must be
 *  fully convertible to the specified text type. If one is not in any
 *  way, then XLocaleNotSupported is returned.
 *
 ************************************************************************/

static int
GetUseableText(Display *display, XmString xmstring, char **buffer,
	       Boolean strict, unsigned char texttype)
{
    _XmStringContextRec stack_context;
    XTextProperty text_prop_return;
    char *text = NULL, *final_string = NULL, *text_item=NULL, *compound_text;
    char *new_final_string;
    unsigned char return_status;
    int result;
    size_t size_so_far = 1; /* initialized for the ending NULL */
    XICCEncodingStyle encoding_style;

    /* Initialize the buffer in case we have to abort and return
       failure. */
    *buffer = NULL;
    switch(texttype)
    {
      case _LOCALE_TEXT:
	encoding_style = XTextStyle;
	break;
      case _STRING_TEXT:
	encoding_style = XStringStyle;
	break;
      default:
	return(XLocaleNotSupported);
    }

    /* Decompose the xmstring and handle each segment separately. */
    _XmStringContextReInit(&stack_context, xmstring);

    /* Get text for each segment of the compound string. Concatenate
       all the text from the segments together into one string that will
       be returned in the buffer. */
    while((return_status =
	   GetTextSegment(display, &stack_context, xmstring,
			  &text, texttype)) == _VALID_SEGMENT)
    {
	size_t text_length = strlen(text);
	if (text_length == SIZE_MAX ||
	    text_length > SIZE_MAX - size_so_far - 1)
	  {
	    XtFree(text);
	    if (final_string) XtFree(final_string);
	    _XmStringContextFree(&stack_context);
	    return(XNoMemory);
	  }
	size_so_far += text_length;
	new_final_string = (char *)XtRealloc(final_string, size_so_far + 1);
	if (new_final_string == NULL)
	  {
	    XtFree(text);
	    XtFree(final_string);
	    _XmStringContextFree(&stack_context);
	    return(XNoMemory);
	  }
	final_string = new_final_string;
	final_string[0] = '\0';
	strcat(final_string, text);
	XtFree(text);
	text = NULL;
    }

    /* If we encountered an invalid segment, then we will throw up our
       hands and try all over again by converting the compound string to
       compound text and then converting the compound text to a text 
       property and from a text property back to a text list. This should
       result in text but with possible loss of information. */
    if (return_status == _INVALID_SEGMENT)
    {
	size_t txt_len;
	char *txt_value;

	/* First, free what we've malloc'ed already */
	if (final_string) XtFree(final_string);

	/* If we are in strict mode, then return XLocaleNotSupported to
	   let the caller know that we could not decompose one of the
	   XmStrings in the context of the current locale */
	if (strict) 
	  {
	    _XmStringContextFree(&stack_context);
	    return(XLocaleNotSupported);
	  }

	/* Now convert the compound string to compound text ... */
	if ((compound_text = XmCvtXmStringToCT(xmstring))
	    == (char *)NULL)
	  {
	    _XmStringContextFree(&stack_context);
	    return (XLocaleNotSupported);
	  }
	/* then to a text property in TextStyle encoding ... */
	txt_len = strlen(compound_text);
	if (txt_len > SIZE_MAX - 2)
	  {
	    XtFree(compound_text);
	    _XmStringContextFree(&stack_context);
	    return XNoMemory;
	  }
	txt_value = XtMalloc((txt_len + 2) * sizeof(char));
	if (txt_value == NULL)
	  {
	    XtFree(compound_text);
	    _XmStringContextFree(&stack_context);
	    return XNoMemory;
	  }
	strcpy(txt_value, compound_text);
	text_prop_return.value = (unsigned char *) txt_value;
	text_prop_return.value[txt_len] = '\0';
	text_prop_return.nitems = txt_len;
	text_prop_return.encoding =
		XInternAtom(display, XmSCOMPOUND_TEXT, False);
	text_prop_return.format = 8;
	XtFree(compound_text);
	/* then to a text list, which should result in text. */
	result = TextPropertyToSingleTextItem(display, &text_prop_return,
					      &text_item);
	if (text_prop_return.value != NULL) 
	    XtFree((char *)text_prop_return.value);

	if (result != Success) 
	  {
	    _XmStringContextFree(&stack_context);
	    return(result);
	  }
	final_string = text_item;
    }

    /* Otherwise, we should have valid text */
    *buffer = final_string;
    _XmStringContextFree(&stack_context);
    return(Success);
}
    
/************************************************************************
 *
 *  XmCvtXmStringTableToTextProperty
 *  
 ************************************************************************/
int
XmCvtXmStringTableToTextProperty(Display *display,
				 XmStringTable string_table,
				 int count,
				 XmICCEncodingStyle style,
				 XTextProperty *text_prop_return)
{
  char **compound_text, **useable_text, *xm_compound_text, *ptr;
  unsigned char *ubufptr, *bufptr,  texttype =_LOCALE_TEXT , *final_string;
  int i, result, total_size;
  Boolean strict = True;
  Atom encoding = 0;
  _XmDisplayToAppContext(display);

  _XmAppLock(app);
  if (text_prop_return == NULL || count < 0 ||
      (count != 0 && string_table == NULL)) {
    _XmAppUnlock(app);
    return BadValue;
  }
  switch (style)
    {
    case XmSTYLE_COMPOUND_TEXT:
      if ((size_t)count > SIZE_MAX / sizeof(char *) ||
	  (size_t)count > UINT_MAX / sizeof(char *)) {
	_XmAppUnlock(app);
	return BadAlloc;
      }
      compound_text = (char **)XtMalloc(sizeof(char *) * count);
      if (compound_text == NULL) {
	_XmAppUnlock(app);
	return BadAlloc;
      }
      /* Convert each XmString to Compound Text */
      for (i = 0, total_size = 0; i < count; ++i) {
	size_t item_size;
	compound_text[i] = XmCvtXmStringToCT(string_table[i]);
	item_size = (compound_text[i] ? strlen(compound_text[i]) : 0) + 1;
	if (item_size > (size_t)INT_MAX - (size_t)total_size) {
	    int cleanup;
	    XtFree(compound_text[i]);
	    for (cleanup = 0; cleanup < i; ++cleanup)
		XtFree(compound_text[cleanup]);
	    XtFree((char *)compound_text);
	    _XmAppUnlock(app);
	    return BadAlloc;
	}
	total_size += (int)item_size;
      }
      /* Generate the resulting XTextProperty value as a set of
	 null-separated compound text elements. A final terminating 
	 null is stored at the end of the value field of text_prop_return 
	 but is not included in the nitems member. */
      xm_compound_text = (char *) XtMalloc(sizeof(char) * (total_size + 1));
      if (xm_compound_text == NULL) {
	int cleanup;
	for (cleanup = 0; cleanup < count; ++cleanup)
	    XtFree(compound_text[cleanup]);
	XtFree((char *)compound_text);
	_XmAppUnlock(app);
	return BadAlloc;
      }
      ptr = xm_compound_text;
      for (i = 0; i < count; i++)
      {
	if (compound_text[i])
	{
	  strcpy(ptr, compound_text[i]);
	  XtFree(compound_text[i]);
	}
	else
	{
	  *ptr = '\0';
	}
	ptr += strlen(ptr) + 1;
      }
      *ptr = '\0';
      XtFree((char *) compound_text);
      text_prop_return->value = (unsigned char *) xm_compound_text;
      text_prop_return->encoding = 
	  XInternAtom(display, XmSCOMPOUND_TEXT, False);
      text_prop_return->format = 8;
      text_prop_return->nitems = total_size;
      _XmAppUnlock(app);
      return(Success);
      
    case XmSTYLE_COMPOUND_STRING:
      /* First calculate how much space the compound strings will occupy
	 when they are all converted to ASN1 strings. */
      for (i = 0, total_size = 0; i < count; ++i) {
	int item_size = XmCvtXmStringToByteStream(string_table[i],NULL);
	if (item_size < 0 || item_size > INT_MAX - total_size) {
	    _XmAppUnlock(app);
	    return BadAlloc;
	}
	total_size += item_size;
      }
      /* Allocate that amount of space and convert the compound strings
	 to ASN1 strings, putting them directly into the buffer. */
      text_prop_return->value = ubufptr =
	(unsigned char *) XtMalloc(sizeof(unsigned char) * ((size_t)total_size + 1));
      if (text_prop_return->value == NULL) {
	_XmAppUnlock(app);
	return BadAlloc;
      }
      for (i = 0; i < count; ++i)
	{
	  int size;
	  size = XmCvtXmStringToByteStream(string_table[i],&bufptr);
	  if (size < 0 || size > total_size - (int)(ubufptr - text_prop_return->value)) {
	    XtFree((char *)bufptr);
	    XtFree((char *)text_prop_return->value);
	    text_prop_return->value = NULL;
	    _XmAppUnlock(app);
	    return BadAlloc;
	  }
	  memcpy(ubufptr, bufptr, size);
	  XtFree((char *) bufptr);
	  ubufptr += size;
	}
      *ubufptr = '\0';
      text_prop_return->nitems = total_size;
      text_prop_return->format = 8;
      text_prop_return->encoding =
	XInternAtom(display, XmS_MOTIF_COMPOUND_STRING, False);
      _XmAppUnlock(app);
      return(Success);
      
    case XmSTYLE_LOCALE:
    case XmSTYLE_TEXT:
    case XmSTYLE_STRING:
    case XmSTYLE_STANDARD_ICC_TEXT:
      /* We do mostly the same thing for these four styles. Do another
	 switch to set up some parameters which will change the behavior
	 slightly depending on which style is being requested. */
      switch (style) {
      case XmSTYLE_LOCALE:
	  strict = False;
	  texttype = _LOCALE_TEXT;
	  encoding = GetLocaleEncodingAtom(display);
	  break;
      case XmSTYLE_TEXT:
	  strict = True;
	  texttype = _LOCALE_TEXT;
	  encoding = GetLocaleEncodingAtom(display);
	  break;
      case XmSTYLE_STRING:
	  strict = False;
	  texttype = _STRING_TEXT;
	  encoding = XA_STRING;
	  break;
      case XmSTYLE_STANDARD_ICC_TEXT:
	  strict = True;
	  texttype = _STRING_TEXT;
	  encoding = XA_STRING;
	  break;
      case XmSTYLE_COMPOUND_TEXT:
      case XmSTYLE_COMPOUND_STRING:
	  break;
	}
      /* Get useable text for each compound string */
      if ((size_t)count > SIZE_MAX / sizeof(char *)) {
	_XmAppUnlock(app);
	return BadAlloc;
      }
      useable_text = (char **)XtMalloc(sizeof(char *) * count);
      if (count > 0 && useable_text == NULL) {
	_XmAppUnlock(app);
	return BadAlloc;
      }
      for (i = 0; i < count; ++i)
	{
	  result = GetUseableText(display, string_table[i],
				  &useable_text[i], strict, texttype);
	  if (result != Success)
	    {
	      /* Free up what we have so far ... */
	      --i; /* skip the one that failed */
	      while (i >= 0) XtFree(useable_text[i--]);
	      if (strict)
		{
		  /* If we got back XLocaleNotSupported, then pretend the
		     caller asked for COMPOUND_TEXT style. */
		  if (result == XLocaleNotSupported)
		    {
		      /* Try to do a straight COMPOUND_TEXT conversion
			 and return whatever we got back. */
		      _XmAppUnlock(app);
		      return (XmCvtXmStringTableToTextProperty
			      (display, string_table, count,
			       XmSTYLE_COMPOUND_TEXT, text_prop_return));
		    }
		  else
		    {
		      /* Otherwise, we got back an error we didn't know how
			 to handle. Just return it. */
		      _XmAppUnlock(app);
		      return(result);
		    }
		}
	      else
		{
		  /* If we are not being strict, then an error was really
		     an error that we could not handle. */
		  _XmAppUnlock(app);
		  return(result);
		}
	    }
	}
      /* Now take the useable_text array and convert it to one
	 long string with null separated elements. */
      for (i = 0, total_size = 0; i < count; ++i) {
	size_t item_size = strlen(useable_text[i]) + 1;
	if (item_size > (size_t)INT_MAX - (size_t)total_size) {
	    int cleanup;
	    for (cleanup = 0; cleanup <= i; ++cleanup)
		XtFree(useable_text[cleanup]);
	    XtFree((char *)useable_text);
	    _XmAppUnlock(app);
	    return BadAlloc;
	}
	total_size += (int)item_size;
      }
      final_string =
	(unsigned char *)XtMalloc(sizeof(char) * (total_size + 1));
      if (final_string == NULL) {
	for (i = 0; i < count; ++i) XtFree(useable_text[i]);
	XtFree((char *)useable_text);
	_XmAppUnlock(app);
	return BadAlloc;
      }
      final_string[0] = '\0';
      bufptr = final_string;
      for (i = 0; i < count; ++i)
	{
	  strcpy((char *)bufptr, useable_text[i]);
	  bufptr += strlen(useable_text[i]) + 1;
	}
      *bufptr = '\0';
      
      /* Fill in the text property with the data */
      text_prop_return->encoding = encoding;
      text_prop_return->value = final_string;
      text_prop_return->nitems = total_size;
      text_prop_return->format = 8;
      
      /* Clean up and leave town */
      for (i = 0; i < count; ++i) XtFree(useable_text[i]);
      XtFree((char *) useable_text);
      _XmAppUnlock(app);
      return(Success);
      
    default:
      _XmAppUnlock(app);
      return(XLocaleNotSupported);
    }
}

/************************************************************************
 *
 *  XmCvtTextPropertyToXmStringTable
 *  
 ************************************************************************/
int
XmCvtTextPropertyToXmStringTable(Display *display,
				 XTextProperty *text_prop,
				 XmStringTable *string_table_return,
				 int *count_return)
{
    enum { XmACOMPOUND_TEXT, XmA_MOTIF_COMPOUND_STRING,
#ifdef UTF8_SUPPORTED
        XmAUTF8_STRING,
#endif
        NUM_ATOMS };
    static char* atom_names[] = { XmSCOMPOUND_TEXT, XmS_MOTIF_COMPOUND_STRING,
#ifdef UTF8_SUPPORTED
        XmSUTF8_STRING
#endif
	};

    char **text_list;
    int i, elements = 0;
    XmStringTable string_table;
    XmStringTag tag;
    XmTextType type;
    Atom LOCALE_ATOM = GetLocaleEncodingAtom(display);
    Atom atoms[XtNumber(atom_names)];
    _XmDisplayToAppContext(display);

    assert(XtNumber(atom_names) == NUM_ATOMS);
    XInternAtoms(display, atom_names, XtNumber(atom_names), False, atoms);

    _XmAppLock(app);
    if (text_prop == NULL || string_table_return == NULL ||
	count_return == NULL || text_prop->nitems < 0 ||
	(text_prop->nitems != 0 && text_prop->value == NULL)) {
	_XmAppUnlock(app);
	return BadValue;
    }
    if (text_prop->encoding == atoms[XmACOMPOUND_TEXT])
    {
	size_t offset, segment_length, remaining;
	char *segment;
	unsigned char *nul;

	if (text_prop->nitems > (unsigned long)INT_MAX) {
	    _XmAppUnlock(app);
	    return BadAlloc;
	}

	/* First found how many XmString we need to allocate. */
	for (*count_return = 1, i = 0; i < text_prop->nitems; i++)
	{
	    if (text_prop->value[i] == '\0')
	    {
		if (*count_return == INT_MAX) {
		    _XmAppUnlock(app);
		    return BadAlloc;
		}
		(*count_return)++;
	    }
	}
	if ((size_t)*count_return > SIZE_MAX / sizeof(XmString)) {
	    _XmAppUnlock(app);
	    return BadAlloc;
	}
	string_table =
	    (XmStringTable)XtMalloc(sizeof(XmString) * (*count_return));
	if (string_table == NULL) {
	    _XmAppUnlock(app);
	    return BadAlloc;
	}
	if (text_prop->nitems == 0 && text_prop->value == NULL) {
	    string_table[0] = XmCvtCTToXmString("");
	    *string_table_return = string_table;
	    _XmAppUnlock(app);
	    return Success;
	}
	/* Now convert each bounded compound-text segment to an XmString. */
	offset = 0;
	for (i = 0; i < *count_return; i++)
	{
	    XmString tempstr;
	    remaining = (size_t)text_prop->nitems - offset;
	    nul = (unsigned char *)memchr(text_prop->value + offset, '\0',
					 remaining);
	    segment_length = nul ? (size_t)(nul - (text_prop->value + offset))
				  : remaining;
	    segment = (char *)XtMalloc(segment_length + 1);
	    if (segment == NULL) {
		int cleanup;
		for (cleanup = 0; cleanup < i; ++cleanup)
		    XmStringFree(string_table[cleanup]);
		XtFree((char *)string_table);
		_XmAppUnlock(app);
		return BadAlloc;
	    }
	    memcpy(segment, text_prop->value + offset, segment_length);
	    segment[segment_length] = '\0';
	    tempstr = XmCvtCTToXmString(segment);
	    XtFree(segment);
	    string_table[i] = tempstr;
	    if (nul != NULL)
		offset += segment_length + 1;
	}
	*string_table_return = string_table;

	_XmAppUnlock(app);
	return(Success);
    }
    else if (text_prop->encoding == atoms[XmA_MOTIF_COMPOUND_STRING])
    {
	unsigned char *asn1_head;
	size_t offset, remaining;
	unsigned int stream_length;

	if (text_prop->nitems == 0) {
	    *string_table_return = (XmStringTable)XtMalloc(sizeof(XmString));
	    if (*string_table_return == NULL) {
		_XmAppUnlock(app);
		return BadAlloc;
	    }
	    *count_return = 0;
	    _XmAppUnlock(app);
	    return Success;
	}

	/* First calculate how many elements there are */
	offset = 0;
	for (elements = 0; offset < (size_t)text_prop->nitems &&
	     text_prop->value[offset] != '\0'; ++elements) {
	    if (elements == INT_MAX) {
		_XmAppUnlock(app);
		return BadAlloc;
	    }
	    remaining = (size_t)text_prop->nitems - offset;
	    if (remaining < 4 ||
		((text_prop->value[offset + 3] & 0x80) && remaining < 6)) {
		_XmAppUnlock(app);
		return XConverterNotFound;
	    }
	    stream_length = XmStringByteStreamLength(text_prop->value + offset);
	    if (stream_length == 0 || stream_length > remaining) {
		_XmAppUnlock(app);
		return XConverterNotFound;
	    }
	    offset += stream_length;
	}

	/* Now allocate a string table to put them in */
	string_table = (XmStringTable)XtMalloc(sizeof(XmString) *
					(elements ? elements : 1));
	if (string_table == NULL) {
	    _XmAppUnlock(app);
	    return BadAlloc;
	}

	/* Run through again, converting the strings. */
	offset = 0;
	for (elements = 0; offset < (size_t)text_prop->nitems &&
	     text_prop->value[offset] != '\0'; ++elements)
	{
	    asn1_head = text_prop->value + offset;
	    remaining = (size_t)text_prop->nitems - offset;
	    if (remaining < 4 ||
		((asn1_head[3] & 0x80) && remaining < 6)) {
		while (elements > 0) XtFree((char *)string_table[--elements]);
		XtFree((char *)string_table);
		_XmAppUnlock(app);
		return XConverterNotFound;
	    }
	    string_table[elements] = XmCvtByteStreamToXmString(asn1_head);
	    /* If the string is NULL, then we don't know what to do */
	    if (string_table[elements] == (XmString) NULL)
	    {
		while (elements > 0) XtFree((char *)string_table[--elements]);
		XtFree((char *)string_table);
		_XmAppUnlock(app);
		return(XConverterNotFound);
	    }
	    /* Find the next asn1 string header */
	    stream_length = XmStringByteStreamLength(asn1_head);
	    if (stream_length == 0 ||
		stream_length > (size_t)text_prop->nitems - offset) {
		XtFree((char *)string_table[elements]);
		while (elements > 0) XtFree((char *)string_table[--elements]);
		XtFree((char *)string_table);
		_XmAppUnlock(app);
		return XConverterNotFound;
	    }
	    offset += stream_length;
	}
	*string_table_return = string_table;
	*count_return = elements;
	_XmAppUnlock(app);
	return(Success);
    }
    else if (text_prop->encoding == LOCALE_ATOM)
    {
	tag = _MOTIF_DEFAULT_LOCALE;
	type = XmMULTIBYTE_TEXT;
    }
    else if (text_prop->encoding == XA_STRING)
    {
	tag = "ISO8859-1";
	type = XmCHARSET_TEXT;
    }
#ifdef UTF8_SUPPORTED
    else if (text_prop->encoding == atoms[XmAUTF8_STRING])
    {
	tag = "UTF-8";
	type = XmCHARSET_TEXT;
    }
#endif
    else {
	_XmAppUnlock(app);
        return(XLocaleNotSupported);
    }


    if (text_prop->nitems > (unsigned long)INT_MAX) {
	_XmAppUnlock(app);
	return BadAlloc;
    }
    if (text_prop->nitems == 0) {
	string_table = (XmStringTable)XtMalloc(sizeof(XmString));
	if (string_table == NULL) {
	    _XmAppUnlock(app);
	    return BadAlloc;
	}
	string_table[0] = XmStringGenerate((XtPointer)"", tag, type, NULL);
	*string_table_return = string_table;
	*count_return = 1;
	_XmAppUnlock(app);
	return Success;
    }

    /* We fell through the else-if's so pull apart the data in the text
       property and set up a return string table. */

    /* First count up how many string elements there are in the value. */
    for (i = 0, elements = 1; i + 1 < (int)text_prop->nitems; ++i)
    {
	/* The text prop value will have two NULL's at the end,
	   one for the end of the last string and one to terminate
	   the entire value. The terminating NULL will be excluded
	   by looping until i == nitems since the terminating NULL
	   is not included in the nitems calculation. */
	if (text_prop->value[i] == '\0')
	  ++elements;
    }
    if ((size_t)elements > SIZE_MAX / sizeof(XmString)) {
	_XmAppUnlock(app);
	return BadAlloc;
    }
    /* Create an appropriately sized array of xmstrings */
    string_table =
      (XmStringTable)XtMalloc(sizeof(XmString) * (elements));
    if (string_table == NULL) {
	_XmAppUnlock(app);
	return BadAlloc;
    }
    
    /* Create XmStrings from each string in the value field */
    string_table[0] = XmStringGenerate((XtPointer)text_prop->value, 
				       tag, type, NULL);
    if (string_table[0] == NULL) {
	XtFree((char *)string_table);
	_XmAppUnlock(app);
	return BadAlloc;
    }
    for (i = 0, elements = 1; i + 1 < (int)text_prop->nitems; ++i)
    {
	if (text_prop->value[i] == '\0')
	  {
	    XmString generated = XmStringGenerate(
		(XtPointer) &(text_prop->value[i + 1]), tag, type, NULL);
	    if (generated == NULL) {
		int cleanup;
		for (cleanup = 0; cleanup < elements; ++cleanup)
		    XmStringFree(string_table[cleanup]);
		XtFree((char *)string_table);
		_XmAppUnlock(app);
		return BadAlloc;
	    }
	    string_table[elements++] = generated;
	  }
    }
    *string_table_return = string_table;
    *count_return = elements;
    _XmAppUnlock(app);
    return(Success);
}

