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
static char rcsid[] = "$XConsortium: XmStringSeg.c /main/7 1995/09/19 23:13:59 cde-sun $"
#endif
#endif

#include <Xm/XmosP.h>
#include <limits.h>
#include <stdint.h>

#include "XmI.h"
#include "XmStringI.h"
#include "XmRenderTI.h"

static Boolean
CopyRenditionTags(XmStringTag *source,
                  unsigned int count,
                  Boolean copy_data,
                  XmStringTag **result)
{
  XmStringTag *tags;
  unsigned int i;

  *result = NULL;
  if (count == 0)
    return True;
  if (source == NULL)
    return False;
  if ((size_t)count > SIZE_MAX / sizeof(*tags) ||
      (size_t)count > UINT_MAX / sizeof(*tags))
    return False;

  tags = (XmStringTag *)XtMalloc(
	(Cardinal)(sizeof(*tags) * (size_t)count));
  if (tags == NULL)
    return False;
  for (i = 0; i < count; i++) {
    if (source[i] == NULL) {
      if (copy_data)
	while (i != 0)
	  XtFree((char *)tags[--i]);
      XtFree((char *)tags);
      return False;
    }
    tags[i] = copy_data ? XtNewString(source[i]) : source[i];
    if (copy_data && tags[i] == NULL) {
      while (i != 0)
        XtFree((char *)tags[--i]);
      XtFree((char *)tags);
      return False;
    }
  }
  *result = tags;
  return True;
}


/*
 * _XmStringGetSegment: A generalized version of XmStringGetNextSegment.
 *	Returns char_count, and allows explicit control over peeking
 *	and copying of data.  Interleaving of calls to _XmStringGetSegment()
 *	and XmeStringGetComponent() is supported.
 */
Boolean
_XmStringGetSegment(_XmStringContext   context, 
		    Boolean	       update_context,
		    Boolean	       copy_data,
		    XtPointer         *text, 
		    XmStringTag       *tag, 
		    XmTextType        *type, 
		    XmStringTag      **rendition_tags, 
		    unsigned int      *tag_count,
		    XmStringDirection *direction,
		    Boolean           *separator, 
		    unsigned char     *tabs,
		    short             *char_count,
		    XmDirection       *push_before,
		    Boolean	      *pop_after)
{
  _XmStringContextRec	local_context_data;
  _XmStringContext	local_context = context;
  Boolean		result = FALSE;
  Boolean 		done;
  Boolean		new_renditions;
  XmStringComponentType ctype;
  unsigned int 		len;
  XtPointer 		val;

  /* Validate context before compatibility output initialization reads it. */
  if (! (context && text && tag && type && rendition_tags && tag_count
	 && direction && separator && tabs && char_count && push_before
	 && pop_after))
    return False;
  
  /* Initialize the out parameters */
  if (text)           *text           = NULL;
  if (tag)            *tag            = NULL;
  if (type)           *type           = XmCHARSET_TEXT;
  if (rendition_tags) *rendition_tags = NULL;
  if (tag_count)      *tag_count      = 0;
  if (direction)      *direction      = _XmStrContDir(context);	/* for BC */
  if (separator)      *separator      = False;
  if (tabs)           *tabs           = 0;
  if (char_count)     *char_count     = 0;
  if (push_before)    *push_before    = 0;
  if (pop_after)      *pop_after      = False;
  
  /* No NULL pointers allowed. */
  if (! (context && text && tag && type && rendition_tags && tag_count
	 && direction && separator && tabs && char_count && push_before
	 && pop_after))
    return False;

  if (_XmStrContError(context))
    return False;

  /* Setup a writable context. */
  if (!update_context)
    {
      local_context = &local_context_data;
      _XmStringContextCopy(local_context, context);
    }

  /* N.B.: This code relies on the order of components from XmeStringGetComponent()! */
  done = new_renditions = FALSE;
  while (!done)
    {
      /* Peek at components before consuming them. */
      ctype = XmeStringGetComponent(local_context, FALSE, FALSE, &len, &val);
      switch (ctype)
	{
	case XmSTRING_COMPONENT_LAYOUT_PUSH:
	  if (*tabs || *text)
	    done = TRUE;
	  else
	    *push_before = *((XmDirection *) val);
	  break;

	case XmSTRING_COMPONENT_RENDITION_BEGIN:
	  if (*text)
	    done = TRUE;
	  else if (*tabs)
	    new_renditions = TRUE;
	  break;

	case XmSTRING_COMPONENT_CHARSET:
	case XmSTRING_COMPONENT_LOCALE:
	  if (*text)
	    done = TRUE;
	  else
	    *tag = (XmStringTag) val;
	  break;

	case XmSTRING_COMPONENT_TAB:
	  if (*text)
	    done = TRUE;
	  else
	    {
	      /* Save the renditions now. */
	      if ((*tag_count == 0) && _XmStrContRendCount(local_context))
		{
		  *tag_count = _XmStrContRendCount(local_context);
		  if ((size_t)*tag_count > SIZE_MAX / sizeof(XmStringTag) ||
		      (size_t)*tag_count > UINT_MAX / sizeof(XmStringTag))
		    {
		      *tag_count = 0;
		      result = FALSE;
		      done = TRUE;
		      break;
		    }
		  if (!CopyRenditionTags(_XmStrContRendTags(local_context),
					  *tag_count, copy_data, rendition_tags))
		    {
		      *tag_count = 0;
		      result = FALSE;
		      done = TRUE;
		      break;
		    }
		}

	      /* Return at the end of this line. */
	      (*tabs)++;
	      result = TRUE;
	    }
	  break;

	case XmSTRING_COMPONENT_DIRECTION:
	  if (*text)
	    done = TRUE;
	  else
	    *direction = *((XmStringDirection *) val);
	  break;

	case XmSTRING_COMPONENT_TEXT:
	case XmSTRING_COMPONENT_LOCALE_TEXT:
	case XmSTRING_COMPONENT_WIDECHAR_TEXT:
	  if (*text)
	    done = TRUE;
	  else if (len > (unsigned int)SHRT_MAX)
	    {
	      /* The legacy API exposes the component length as a short. */
	      result = FALSE;
	      done = TRUE;
	    }
	  else if (*tabs && new_renditions)
	    {
	      /* Tabs had a different set of renditions than the text, */
	      /* so we can't return both tabs and text at once. */
	      done = TRUE;
	    }
	  else
	    {
	      *char_count = len;
	      *text = val;
	      
	      if (ctype == XmSTRING_COMPONENT_TEXT)
		*type = XmCHARSET_TEXT;
	      else if (ctype == XmSTRING_COMPONENT_LOCALE_TEXT)
		*type = XmMULTIBYTE_TEXT;
	      else if (ctype == XmSTRING_COMPONENT_WIDECHAR_TEXT)
		*type = XmWIDECHAR_TEXT;
	      else
		{
		  result = FALSE;
		  done = TRUE;
		  break;
		}
	      
	      /* Force a tag for backward compatibility with Motif 1.2 */
	      if (! *tag)
		*tag = _XmStrContTag(local_context);

	      result = TRUE;
	      
	      /* Save the renditions now. */
	      if ((*tag_count == 0) && _XmStrContRendCount(local_context))
		{
		  *tag_count = _XmStrContRendCount(local_context);
		    if ((size_t)*tag_count > SIZE_MAX / sizeof(XmStringTag) ||
			(size_t)*tag_count > UINT_MAX / sizeof(XmStringTag))
		      {
			*tag_count = 0;
			result = FALSE;
			done = TRUE;
			break;
		      }
		  if (!CopyRenditionTags(_XmStrContRendTags(local_context),
					  *tag_count, copy_data, rendition_tags))
		    {
		      *tag_count = 0;
		      result = FALSE;
		      done = TRUE;
		      break;
		    }
		}
	    }
	  break;
	      
	case XmSTRING_COMPONENT_RENDITION_END:
	  break;
	      
	case XmSTRING_COMPONENT_LAYOUT_POP:
	  if (*tabs || *text)
	    {
	      /* We're almost done, so record this pop. */
	      *pop_after = TRUE;
	    }
	  else
	    {
	      /* We're ignoring this pop, so discard any recorded push. */
	      *push_before = 0;
	    }
	  break;

	case XmSTRING_COMPONENT_SEPARATOR:
	  if (*tabs || *text)
	    {
	      *separator = TRUE;
	      done = TRUE;
	    }
	  break;
	  
	case XmSTRING_COMPONENT_END:
	default:
	  done = TRUE;
	  break;
	}

      /* Consume the component if we aren't done. */
      if (!done)
	(void) XmeStringGetComponent(local_context, TRUE, FALSE, &len, &val);
    }

  if (copy_data && result)
    {
      /* Copy the tag. */
      if (*tag)
	{
	  XmStringTag copied_tag = XtNewString(*tag);
	  if (copied_tag == NULL) {
	    *tag = NULL;
	    result = FALSE;
	  } else {
	    *tag = copied_tag;
	  }
	}

      /* Copy the text. */
	if (*text && result)
	{
	  if (*char_count < 0)
	    {
	    *text = NULL;
	    XtFree((char *)*tag);
	    *tag = NULL;
	    result = FALSE;
	    }
	  else
	  {
	  size_t text_bytes = (*type == XmWIDECHAR_TEXT) ?
	      (size_t)*char_count * sizeof(wchar_t) : (size_t)*char_count;
	  if ((*type == XmWIDECHAR_TEXT &&
	       (size_t)*char_count > SIZE_MAX / sizeof(wchar_t)) ||
	      text_bytes > SIZE_MAX - sizeof(wchar_t) ||
	      text_bytes + sizeof(wchar_t) > UINT_MAX)
	    {
	    *text = NULL;
	    XtFree((char *)*tag);
	    *tag = NULL;
	    result = FALSE;
	    }
	  else
	  {
	  char *tmp = XtMalloc((Cardinal)(text_bytes + sizeof(wchar_t)));
	  if (tmp == NULL) {
	    *text = NULL;
	    XtFree((char *)*tag);
	    *tag = NULL;
	    result = FALSE;
	  } else {
	    memcpy(tmp, *text, text_bytes);
	    bzero(tmp + text_bytes, sizeof(wchar_t));
	    *text = (XtPointer) tmp;
	  }
	  }
	  }
	}
    }

  /* Free the local context. */
  if (local_context == &local_context_data)
    _XmStringContextFree(local_context);

  return result;
}

Boolean 
_XmStringGetNextSegment(
        _XmStringContext context,
        XmStringTag *tag,
        XmStringDirection *direction,
        char **text,
        short *char_count,
        Boolean *separator )
{
  Boolean       result;
  XmTextType    type;
  XmStringTag * rendition_tags;
  unsigned int  tag_count;
  unsigned char tabs;
  XmDirection   push_before;
  Boolean	pop_after;

  /* Get all the fields and discard the ones we don't want. */
  result = _XmStringGetSegment(context, True, True, (XtPointer*) text, tag,
		       &type, &rendition_tags, &tag_count, direction,
		       separator, &tabs, char_count, &push_before, &pop_after);
  if (result) {
    if (rendition_tags)
      {
	while (tag_count-- > 0)
	  XtFree((char*) rendition_tags[tag_count]);
	XtFree((char*) rendition_tags);
      }
    
    if (type == XmWIDECHAR_TEXT && *text) {
      /* must convert (this should be done in segment's locale instead) */
      size_t len;
      size_t converted;
      wchar_t *wtext = (wchar_t *)(*text);

	if ((size_t)*char_count > (SIZE_MAX - 1) / MB_CUR_MAX ||
	    (size_t)*char_count * MB_CUR_MAX + 1 > UINT_MAX) {
	result = False;
	*text = NULL;
	XtFree((char *)*tag); *tag = NULL;
      } else {
	len = (size_t)*char_count * MB_CUR_MAX;
	*text = (char *)XtMalloc((Cardinal)(len + 1));
	if (*text == NULL) {
	  result = False;
	  XtFree((char *)*tag); *tag = NULL;
	} else {
	converted = wcstombs(*text, wtext, len);
	if (converted == (size_t)-1 || converted > SHRT_MAX) {
	  result = False;
	  XtFree(*text); *text = NULL;
	  XtFree((char *)*tag); *tag = NULL;
	} else
	{
	  *char_count = (short)converted;
	  (*text)[converted] = '\0';
	}
	}
	}
      XtFree((char *)wtext);
    } 
  }
	  
  return result;
}

/*
 * fetch the next 'segment' of the external TCS
 */
Boolean 
XmStringGetNextSegment(XmStringContext context,
		       char **text,
		       XmStringTag *tag,
		       XmStringDirection *direction,
		       Boolean *separator )
{
  short char_count;
  Boolean ret_val;

  if (context == NULL || text == NULL || tag == NULL ||
      direction == NULL || separator == NULL)
	return(False);
  
  _XmProcessLock();
  ret_val = _XmStringGetNextSegment((_XmStringContext)context, 
		 tag, direction, text, &char_count, separator);
  _XmProcessUnlock();
  return ret_val;
}
