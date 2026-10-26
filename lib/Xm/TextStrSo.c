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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: TextStrSo.c /main/14 1996/10/23 16:05:21 cde-osf $"
#endif
#endif

#define FIX_1320

#include <ctype.h>
#include <limits.h>
#include <stdint.h>
#include <X11/Xatom.h>
#include <X11/Xmd.h>
#include <Xm/AtomMgr.h>
#include <Xm/TextSelP.h>
#include <Xm/TextStrSoP.h>
#include <Xm/XmosP.h>
#include "XmI.h"		/* for _XmValidTimestamp() */
#include "TextI.h"
#include "TextStrSoI.h"
#include "TextUnicode.h"

/********    Static Function Declarations    ********/

static void AddWidget(XmTextSource source,
		      XmTextWidget tw);
static char * _XmStringSourceGetChar(XmSourceData data,
				     XmTextPosition position);
static int CountLines(XmTextSource source,
		      XmTextPosition start,
		      unsigned long length);
static void RemoveWidget(XmTextSource source,
			 XmTextWidget tw);
static void _XmStringSourceReadString(XmTextSource source,
				      int start,
				      XmTextBlock block);
static XmTextPosition ReadSource(XmTextSource source,
				 XmTextPosition position,
				 XmTextPosition last_position,
				 XmTextBlock block);
static XmTextStatus Replace(XmTextWidget initiator,
			    XEvent *event,
			    XmTextPosition *start,
			    XmTextPosition *end,
			    XmTextBlock block,
#if NeedWidePrototypes
			    int call_callbacks);
#else
                            Boolean call_callbacks);
#endif /* NeedsWidePrototypes */
static void ScanParagraph(XmSourceData data,
			  XmTextPosition *new_position,
			  XmTextScanDirection dir,
			  int ddir,
			  XmTextPosition *last_char);
static XmTextPosition Scan(XmTextSource source,
			   XmTextPosition pos,
			   XmTextScanType sType,
			   XmTextScanDirection dir,
			   int count,
#if NeedWidePrototypes
			   int include);
#else
                           Boolean include);
#endif /* NeedWidePrototypes */
static Boolean GetSelection(XmTextSource source,
			    XmTextPosition *left,
			    XmTextPosition *right);
static void SetSelection(XmTextSource source,
			 XmTextPosition left,
			 XmTextPosition right,
			 Time set_time);
static Boolean TextSourceRangeBytes(XmTextPosition from,
					XmTextPosition to,
					unsigned int char_size,
					size_t *bytes);
static Boolean TextSourceByteOffset(XmSourceData data,
					XmTextPosition position,
					size_t unit,
					size_t *offset);

/********    End Static Function Declarations    ********/

#define TEXT_INCREMENT 1024
#define TEXT_INITIAL_INCREM 64

static Boolean
TextSourceRangeBytes(XmTextPosition from,
		     XmTextPosition to,
		     unsigned int char_size,
		     size_t *bytes)
{
    XmTextPosition length;

    if (from < 0 || to < from || from > INT_MAX || to > INT_MAX ||
	char_size == 0)
	return False;
    length = to - from;
    if ((size_t)length > (SIZE_MAX / char_size) - 1)
	return False;
    *bytes = ((size_t)length + 1) * char_size;
    return True;
}

static Boolean
TextSourceByteOffset(XmSourceData data,
			     XmTextPosition position,
			     size_t unit,
			     size_t *offset)
{
    if (data == NULL || data->ptr == NULL || offset == NULL ||
	position < 0 || unit == 0 || data->maxlength < 0 ||
	(size_t)position > (size_t)data->maxlength ||
	(size_t)position > SIZE_MAX / unit)
	return False;
    *offset = (size_t)position * unit;
    return True;
}

/* Convert a stream of bytes into a char*, BITS16*, or wchar_t* array. 
 * Return number of characters created.
 *
 * If num_chars == 1, don't add a null terminator, else if a null terminator
 * is present on the byte stream, convert it and add it to the character
 * array;  Count returned does not include NULL terminator (just like strlen).
 *
 * This routine assumes that a BITS16 is two-bytes;
 * the routine must be modified if these assumptions are incorrect.
 */

/* ARGSUSED */
int
_XmTextBytesToCharacters(char * characters, 
			 char * bytes,
			 int num_chars, 
#if NeedWidePrototypes
			 int add_null_terminator,
#else
			 Boolean add_null_terminator,
#endif /* NeedWidePrototypes */
			 int max_char_size)
{
  unsigned char * tmp_bytes;
  int num_bytes; 
  int count=0;
  BITS16 *bits16_ptr, temp_bits16;
  wchar_t *wchar_t_ptr;
  
  
  /* If 0 characters requested, or a null pointer passed, dont do
   * anything... just return 0 characters converted.
   */
  
  if (num_chars <= 0 || characters == NULL || bytes == NULL ||
      max_char_size <= 0)
    return 0;
  
  switch (max_char_size) {
  case 1: {
    (void) memcpy((void*)characters, (void*)bytes, num_chars);
    count = num_chars;
    break;
  } /* end case 1 */
  case 2: {
    bits16_ptr = (BITS16 *) characters;
    tmp_bytes = (unsigned char*) bytes;
    for (
#ifndef NO_MULTIBYTE
	 num_bytes = mblen((char*)tmp_bytes, max_char_size), 
#else
	 num_bytes = *tmp_bytes ? 1 : 0,
#endif
	 temp_bits16 = 0; 
	 num_chars > 0 && num_bytes > 0;
	 num_chars--,
#ifndef NO_MULTIBYTE
	 num_bytes = mblen((char*)tmp_bytes, max_char_size), 
#else
	 num_bytes = *tmp_bytes ? 1 : 0,
#endif
	 temp_bits16 = 0, bits16_ptr ++) {
      if (num_bytes == 1) {
	temp_bits16 = (BITS16) *tmp_bytes++;
      } else {
	temp_bits16 = (BITS16) *tmp_bytes++;
	temp_bits16 = temp_bits16 << 8;
	temp_bits16 |= (BITS16) *tmp_bytes++;
      }
      *bits16_ptr = temp_bits16;
      count++;
    }
    /* if bytes is NULL terminated, characters should be too */
    if (add_null_terminator == True)
      *bits16_ptr = (BITS16) 0;  
    break;
  } /* end case 2 */
  default: {
    wchar_t_ptr = (wchar_t *)characters;
    count = mbstowcs(wchar_t_ptr, bytes, num_chars);
    if (add_null_terminator == True && count >= 0)
      wchar_t_ptr[count] = (wchar_t)0;
    break;
  } /* end default */
  } /* end switch */
  
  return count;
}

/* Convert an array of char*, BITS16*, or wchar_t* into a stream of bytes.
 * Return the number of bytes placed into 'bytes'
 *
 * Null terminate the byte stream - caller better have alloc'ed enough space!
 */

/* ARGSUSED */
int
_XmTextCharactersToBytes(char * bytes,
			 char * characters,
			 int num_chars,
			 int max_char_size)
{
  unsigned char *temp_char;
  unsigned char *byte_ptr;
  int count = 0;
  int i, j;
  BITS16 *bits16_ptr, temp_bits16;
  wchar_t *wchars;

  if (num_chars <= 0) {
    if (bytes != NULL)
      *bytes = '\0';
    return 0;
  }
  if (bytes == NULL || characters == NULL || max_char_size <= 0)
    return 0;
  
  switch (max_char_size) {
  case 1: {
    (void) memcpy((void*)bytes, (void*)characters, num_chars);
    count = num_chars;
    break;
  } /* end case 1 */
  case 2: {
    bits16_ptr = (BITS16 *) characters;
    byte_ptr = (unsigned char*) bytes;
    temp_char = (unsigned char*) XtMalloc (max_char_size);
    if (temp_char == NULL) {
	*bytes = '\0';
	return 0;
    }
    for (i = 0; i < num_chars && *bits16_ptr != 0; i++, bits16_ptr++) {
      temp_bits16 = *bits16_ptr;
      /* create an array of chars; char[max_char_size - 1] contains the 
       * low order byte */
      for (j = max_char_size - 1; j >= 0; j--) {
	temp_char[j] = (unsigned char)(temp_bits16 & 0377);
	temp_bits16 = temp_bits16 >> 8;
      }
      /* start with high order byte.  If any byte is 0, skip it. */
      for (j = 0; j < max_char_size; j++) { 
	if (temp_char[j] > 0) {
	  *byte_ptr = temp_char[j];
	  byte_ptr++; count++;
	}
      }
    }
    XtFree ((char*)temp_char);
    if (count < num_chars) *byte_ptr = '\0';
    break;
  } /* end case 2 */
  default: {
    int nbytes;

    wchars = (wchar_t *)characters;
    for (i = 0; i < num_chars && *wchars != 0L; i++, wchars++) {
      nbytes = wctomb(bytes, *wchars);
      if (nbytes < 0)
	break; /* illegal char */
      count += nbytes;
      bytes += nbytes;
    }
    if (count >= 0)
      bytes[count] = '\0';
    break;
  } /* end default */
  } /* end switch */
  return (count);    /* return the number of bytes placed in bptr */
}

char * 
_XmStringSourceGetString(XmTextWidget tw,
			 XmTextPosition from,
			 XmTextPosition to,
#if NeedWidePrototypes
			 int want_wchar)
#else
                         Boolean want_wchar)
#endif /* NeedWidePrototypes */
{
  char *buf;
  wchar_t *wc_buf;
  XmTextBlockRec block;
  size_t destpos;
  XmTextPosition pos, ret_pos;
  size_t allocation;
  size_t return_val = 0;
  XmTextPosition start_pos;

  if (!TextSourceRangeBytes(from, to,
				want_wchar ? sizeof(wchar_t) : tw->text.char_size,
				&allocation))
	return NULL;
  destpos = 0;
  if (!want_wchar) {
    buf = XtMalloc(allocation);
    if (buf == NULL)
	return NULL;
    for (pos = from; pos < to; ) {
	start_pos = pos;
	block.length = 0;
      pos = ReadSource(tw->text.source, pos, to, &block);
	if (pos <= start_pos || block.length == 0 || block.ptr == NULL)
	break;
	  if ((size_t)block.length > allocation - 1 - destpos) {
	    XtFree(buf);
	    return NULL;
	  }
      (void)memcpy((void*)&buf[destpos], (void*)block.ptr, block.length);
      destpos += block.length;
    }
    buf[destpos] = 0;
    return buf;
  } else { /* want buffer of wchar_t * data */
    buf = XtMalloc(allocation);
    if (buf == NULL)
	return NULL;
    wc_buf = (wchar_t *)buf;
    for (pos = from; pos < to; ) {
	start_pos = pos;
	block.length = 0;
      ret_pos = ReadSource(tw->text.source, pos, to, &block);
	if (ret_pos <= start_pos || block.length == 0 || block.ptr == NULL)
	break;
	  if (ret_pos < pos ||
	      (size_t)(ret_pos - pos) >
	      (allocation / sizeof(wchar_t)) - 1 - destpos) {
	    XtFree(buf);
	    return NULL;
	  }
      return_val =  mbstowcs(&wc_buf[destpos], block.ptr,
			     (size_t) (ret_pos - pos));
	  if (return_val == (size_t)-1) {
	    XtFree(buf);
	    return NULL;
	  }
	  if (return_val > 0) destpos += return_val;
      pos = ret_pos;
    }
    wc_buf[destpos] = (wchar_t)0L;
    return ((char*)wc_buf);
  }
}


static void 
AddWidget(XmTextSource source,
	  XmTextWidget tw)
{
  XmSourceData data = source->data;
  XmTextWidget *new_widgets;
  _XmHighlightRec *new_highlights;
  size_t widget_bytes;
  size_t highlight_bytes;

  if (data->numwidgets < 0 || data->numwidgets == INT_MAX ||
      (size_t)(data->numwidgets + 1) >
	(size_t)UINT_MAX / sizeof(XmTextWidget))
    return;
  data->numwidgets++;
  widget_bytes = sizeof(XmTextWidget) * (size_t)data->numwidgets;
  new_widgets = (XmTextWidget *)XtRealloc((char *)data->widgets,
					 (Cardinal)widget_bytes);
  if (new_widgets == NULL) {
    data->numwidgets--;
    return;
  }
  data->widgets = new_widgets;
  data->widgets[data->numwidgets - 1] = tw;
  
  if (data->numwidgets == 1)
    _XmTextSetHighlight((Widget) tw, 0, tw->text.last_position, 
		       XmHIGHLIGHT_NORMAL);
  else {
    if (data->widgets[0]->text.highlight.maximum < 0 ||
	(size_t)data->widgets[0]->text.highlight.maximum >
	  SIZE_MAX / sizeof(_XmHighlightRec) ||
	(size_t)data->widgets[0]->text.highlight.maximum >
	  (size_t)UINT_MAX / sizeof(_XmHighlightRec)) {
      data->numwidgets--;
      return;
    }
    highlight_bytes = (size_t)data->widgets[0]->text.highlight.maximum *
	 sizeof(_XmHighlightRec);
    new_highlights = (_XmHighlightRec *)XtRealloc(
	(char *)tw->text.highlight.list, (Cardinal)highlight_bytes);
    if (new_highlights == NULL) {
      data->numwidgets--;
      return;
    }
    tw->text.highlight.list = new_highlights;
    tw->text.highlight.maximum = data->widgets[0]->text.highlight.maximum;
    tw->text.highlight.number = data->widgets[0]->text.highlight.number;
    memmove((void *) tw->text.highlight.list, 
	    (void *) data->widgets[0]->text.highlight.list,
	    (size_t) data->widgets[0]->text.highlight.number *
	    sizeof(_XmHighlightRec));
  }
  
  
  if (data->hasselection && data->numwidgets == 1) {
    Time select_time = XtLastTimestampProcessed(XtDisplay((Widget)tw));
    if (!select_time) select_time = _XmValidTimestamp((Widget)tw);
    if (!XmePrimarySource((Widget) data->widgets[0], select_time)) {
      (*source->SetSelection)(source, 1, 0, select_time);
    } else {
      XmAnyCallbackStruct cb;

      data->prim_time = select_time;
      cb.reason = XmCR_GAIN_PRIMARY;
      cb.event = NULL;
      XtCallCallbackList ((Widget) data->widgets[0],
			  data->widgets[0]->text.gain_primary_callback,
			  (XtPointer) &cb);
    }
  }
}

/********************************<->***********************************/
static char * 
_XmStringSourceGetChar(XmSourceData data,
		       XmTextPosition position)       /* starting position */
{
  /* gap_size is the number of character in the gap, not number of bytes */
  size_t gap_size, byte_position, unit, logical_position;
  XmTextWidget tw;

  if (data == NULL || data->widgets == NULL || data->numwidgets < 1)
    return ("");
  tw = (XmTextWidget) data->widgets[0];
  unit = tw->text.char_size < 3 ? (size_t)tw->text.char_size :
						 sizeof(wchar_t);
  if (!TextSourceByteOffset(data, position, unit, &byte_position))
    return ("");

  /* Regardless of its representation, source storage is byte-addressed. */
  if (data->ptr + byte_position < data->gap_start)
    return (&data->ptr[byte_position]);

  gap_size = (size_t)(data->gap_end - data->gap_start) / unit;
  if ((size_t)position > SIZE_MAX - gap_size)
    return ("");
  logical_position = (size_t)position + gap_size;
  if (logical_position >= (size_t)data->maxlength ||
      logical_position > SIZE_MAX / unit)
    return ("");
  return (&data->ptr[logical_position * unit]);
}

/* Inspect one native source character without flattening or moving its gap.
 * UTF-8 source storage is scalar wchar_t; the smaller native storage forms
 * still represent CR/LF directly. Bounds checks precede all pointer access. */
static unsigned long
SourceLineCharacter(XmSourceData data, XmTextPosition position)
{
  size_t unit, before;
  char *at;
  BITS16 short_character;
  wchar_t wide_character;
  int character_size;
  if (position < 0 || position >= data->length || data->numwidgets < 1)
    return 0;
  character_size = data->widgets[0]->text.char_size;
  unit = character_size < 3 ? (size_t)character_size : sizeof(wchar_t);
  if (!unit) return 0;
  before = (size_t)(data->gap_start - data->ptr) / unit;
  at = (size_t)position < before ? data->ptr + (size_t)position * unit :
       data->gap_end + ((size_t)position - before) * unit;
  if (character_size == 1) return (unsigned char)*at;
  if (character_size == 2) {
    memcpy(&short_character, at, sizeof(short_character));
    return (unsigned long)short_character;
  }
  memcpy(&wide_character, at, sizeof(wide_character));
  return (unsigned long)wide_character;
}

static unsigned long
SourceNewlineCharacter(XmSourceData data)
{
  int character_size = data->widgets[0]->text.char_size;
  BITS16 short_character;
  wchar_t wide_character;

  if (character_size == 1)
    return (unsigned long)(unsigned char)data->PSWC_NWLN[0];
  if (character_size == 2) {
    memcpy(&short_character, data->PSWC_NWLN, sizeof(short_character));
    return (unsigned long)short_character;
  }
  memcpy(&wide_character, data->PSWC_NWLN, sizeof(wide_character));
  return (unsigned long)wide_character;
}

/* UAX #14 mandatory separators (BK/CR/LF/NL). CRLF is paired by the
 * scanner; the other separators each consume one native scalar. */
static Boolean
SourceHardLineBreak(unsigned long character)
{
  return character == '\r' || character == '\n' || character == '\v' ||
         character == '\f' || character == 0x85 || character == 0x2028 ||
         character == 0x2029;
}

/* In a supported UTF-8 locale, mandatory separators terminate native hard lines,
 * preserving all source scalars. Right scans exclude/include a terminator at
 * its first/last outer boundary; left scans use its last/first boundary.
 * A CRLF pair has only its outer boundaries; scans never return its interior.
 * Normalize an interior starting position in the requested direction before
 * scanning, including count zero. Repeated scans cross whole terminators,
 * even when excluded. Other locales retain the original native line policy. */
static XmTextPosition
ScanUnicodeLine(XmSourceData data, XmTextPosition position,
                XmTextScanDirection direction, int count, Boolean include)
{
  int i;
  if (position < 0) position = 0;
  if (position > data->length) position = data->length;
  if (position > 0 && position < data->length &&
      SourceLineCharacter(data, position - 1) == '\r' &&
      SourceLineCharacter(data, position) == '\n')
    position += direction == XmsdRight ? 1 : -1;
  for (i = 0; i < count; ++i) {
    XmTextPosition first, last;
    if (direction == XmsdRight) {
      while (position < data->length &&
             !SourceHardLineBreak(SourceLineCharacter(data, position)))
        ++position;
      if (position == data->length) return position;
      first = position;
      last = first + 1;
      if (SourceLineCharacter(data, first) == '\r' &&
          SourceLineCharacter(data, last) == '\n') ++last;
      position = i + 1 == count && !include ? first : last;
    } else {
      while (position > 0 &&
             !SourceHardLineBreak(SourceLineCharacter(data, position - 1)))
        --position;
      if (!position) return position;
      last = position;
      first = last - 1;
      if (SourceLineCharacter(data, first) == '\n' && first > 0 &&
          SourceLineCharacter(data, first - 1) == '\r') --first;
      position = i + 1 == count && !include ? last : first;
    }
  }
  return position;
}


/*DELTA: length IS NOW TREATED AS NUMBER OF CHARACTERS - CALLERS MUST CHANGE*/
static int 
CountLines(XmTextSource source,
	   XmTextPosition start,
	   unsigned long length)
{
  XmSourceData data = source->data;
  int num_lines = 0;
  unsigned long newline;
  XmTextPosition position, end;

  if (start < 0) {
    unsigned long skipped = (unsigned long)(-(start + 1)) + 1;
    if (length <= skipped) return 0;
    length -= skipped;
    start = 0;
  }
  if (start >= data->length) return 0;
  if (length > (unsigned long)(data->length - start))
    length = (unsigned long)(data->length - start);

  if (source->Scan == Scan && XmTextUnicodeLocaleAvailable()) {
    end = start + (XmTextPosition)length;
    /* Charge a pair when its LF completes, so adjacent partial ranges add
     * to the whole-range count even if the gap/range splits CR from LF. */
    for (position = start; position < end; ++position) {
      unsigned long character = SourceLineCharacter(data, position);
      if (SourceHardLineBreak(character) &&
          !(character == '\r' && SourceLineCharacter(data, position + 1) == '\n'))
        ++num_lines;
    }
    return num_lines;
  }

  if (length == 0)
    return num_lines;

  newline = SourceNewlineCharacter(data);
  end = start + (XmTextPosition)length;
  for (position = start; position < end; ++position)
    if (SourceLineCharacter(data, position) == newline)
      ++num_lines;
  return num_lines;
}

static void 
RemoveWidget(XmTextSource source,
	     XmTextWidget tw)
{
  XmSourceData data = source->data;
  int i;
  for (i=0; i<data->numwidgets; i++) {
    if (data->widgets[i] == tw) {
      XmTextPosition left, right;
      Boolean had_selection = False;
      Time select_time =
	XtLastTimestampProcessed(XtDisplay((Widget)tw));
      
      if (data->hasselection) {
	(*source->GetSelection)(source, &left, &right);
	(*source->SetSelection)(source, 1, -999, select_time);
	had_selection = True;
      }
      data->numwidgets--;
      data->widgets[i] = data->widgets[data->numwidgets];
      if (i == 0 && data->numwidgets > 0 && had_selection)
	(*source->SetSelection)(source, left, right, select_time);
      if (data->numwidgets == 0) _XmStringSourceDestroy(source);
      return;
    }
  }
}

Boolean *
_XmStringSourceGetPending(XmTextWidget tw)
{
  Boolean *pending;
  XmSourceData data = tw->text.source->data;
  int i;

  if (data->numwidgets <= 0 ||
      (size_t)data->numwidgets > (size_t)UINT_MAX / sizeof(Boolean))
    return NULL;
  pending = (Boolean *)XtMalloc((Cardinal)((size_t)data->numwidgets *
					 sizeof(Boolean)));
  if (pending == NULL)
    return NULL;
  for (i=0; i<data->numwidgets; i++) 
    pending[i] = ((XmTextWidget)data->widgets[i])->text.pendingoff;

  return pending;
}

void
_XmStringSourceSetPending(XmTextWidget tw,
			  Boolean *pending)
{
  XmSourceData data = tw->text.source->data;
  int i;
  
  if ((uintptr_t)pending > (uintptr_t)1)
    for (i=0; i<data->numwidgets; i++) 
      ((XmTextWidget)data->widgets[i])->text.pendingoff = pending[i];
  else
    for (i=0; i<data->numwidgets; i++) 
      ((XmTextWidget)data->widgets[i])->text.pendingoff = 
	(Boolean)(uintptr_t)pending;
}

/*
 * Determines where to move the gap and calls memmove to move the gap.
 */
/********************************<->***********************************/
void 
_XmStringSourceSetGappedBuffer(XmSourceData data,
			       XmTextPosition position) /* starting position */
{
  XmTextWidget tw = (XmTextWidget) data->widgets[0];
  size_t count, byte_position;
  size_t char_size = (tw->text.char_size < 3 ?
			      (size_t)tw->text.char_size : sizeof(wchar_t));

  if (!TextSourceByteOffset(data, position, char_size, &byte_position))
	return;
  
  /* if no change in gap placement, return */
  if (data->ptr + byte_position == data->gap_start)
    return;
  
	if (data->ptr + byte_position < data->gap_start) {
    /* move gap to the left */
    count = (size_t)(data->gap_start - (data->ptr + byte_position));
    memmove(data->gap_end - count, data->ptr + byte_position, count);
    data->gap_start -= count; /* ie, data->gap_start = position; */
    data->gap_end -= count;   /* ie, data->gap_end = position + gap_size; */
  } else {
    /* move gap to the right */
    count = (size_t)((data->ptr + byte_position) - data->gap_start);
    memmove(data->gap_start, data->gap_end, count);
    data->gap_start += count; /* ie, data->gap_start = position; */
    data->gap_end += count;   /* ie, data->gap_end = position + gap_size; */
  }
}

/********************************<->***********************************/
/* The only caller of this routine expects to get char* in block */
static void 
_XmStringSourceReadString(XmTextSource source,
			  int start,
			  XmTextBlock block)
{
  XmSourceData data = source->data;
  XmTextWidget tw = (XmTextWidget) data->widgets[0];
  size_t gap_size, byte_start, unit;

  unit = tw->text.char_size < 3 ? (size_t)tw->text.char_size :
						 sizeof(wchar_t);
  if (block == NULL ||
      !TextSourceByteOffset(data, (XmTextPosition)start, unit,
					    &byte_start)) {
    if (block != NULL) {
      block->ptr = NULL;
      block->length = 0;
    }
    return;
  }
  gap_size = (size_t)(data->gap_end - data->gap_start);
  
  if (data->ptr + byte_start + block->length <= data->gap_start)
    block->ptr = data->ptr + byte_start;
  else if (data->ptr + byte_start + gap_size >= data->gap_end)
    block->ptr = data->ptr + byte_start + gap_size;
  else {
    block->ptr = data->ptr + byte_start;
    block->length = data->gap_start - (data->ptr + byte_start);
  }
}

/* Caller wants block to contain char*; _XmStringSourceReadString provides
 * char*, BITS16* or wchar_t*; so we need to modify what it gives us.
 */
static XmTextPosition 
ReadSource(XmTextSource source,
	   XmTextPosition position,       /* starting position */
	   XmTextPosition last_position,  /* The last position we're interested
					     in.  Don't return info about any
					     later positions. */
	   XmTextBlock block)            /* RETURN: text read in */
{
  XmTextPosition return_pos;
  XmTextPosition available;
  int num_bytes;
  XmSourceData data = source->data;
  XmTextWidget tw = (XmTextWidget) data->widgets[0];
  int char_size = (tw->text.char_size < 3 ?
		   (int)tw->text.char_size :
		   sizeof(wchar_t));
  
  if (position < 0 || position > INT_MAX)
	return 0;
  if (last_position > data->length) last_position = data->length;
  if (last_position < position)
	return 0;
  available = last_position - position;
  if (available > INT_MAX / char_size)
	available = INT_MAX / char_size;
  last_position = position + available;
  block->length = (int)(available * char_size);
  if (block->length < 0 ) block->length = 0;
  block->format = XmFMT_8_BIT;
  _XmStringSourceReadString(source, (int)position, block);
  
  if (block->length > 0) {
    size_t block_bytes;
    char *new_value;

    if ((size_t)block->length >
	(SIZE_MAX - 1) / (size_t)tw->text.char_size)
	  return 0;
    block_bytes = (size_t)block->length * (size_t)tw->text.char_size + 1;
    if (data->old_length == 0) {
      new_value = (char *)XtMalloc(block_bytes);
      if (new_value == NULL)
	return 0;
      data->value = new_value;
      data->old_length = block->length;
    } else if (block->length > data->old_length) {
      new_value = (char *)XtRealloc(data->value, block_bytes);
      if (new_value == NULL)
	return 0;
      data->value = new_value;
      data->old_length = block->length;
    }

    if (data->value == NULL)
	return 0;
    
    if ((int)tw->text.char_size == 1) {
      return_pos = position + block->length;
    } else {
      return_pos = position + (block->length / char_size);
      num_bytes = _XmTextCharactersToBytes(data->value, block->ptr, 
					   block->length / char_size,
					   (int)tw->text.char_size);
      block->length = num_bytes;
      block->ptr = data->value;
    }
    return return_pos;
  } else
    return 0;
}

void
_XmTextValidate(XmTextPosition * start,
		XmTextPosition * end,
		int maxsize)
{
  if (*start < 0) *start = 0;
  if (*start > maxsize) {
    *start = maxsize;
  }
  if (*end < 0) *end = 0;
  if (*end > maxsize) {
    *end = maxsize;
  }
  
  if (*start > *end) { 
    XmTextPosition tmp; /* tmp variable for swapping positions */
    tmp = *end;    
    *end = *start;
    *start = tmp;
  }
}

Boolean 
_XmTextModifyVerify(XmTextWidget initiator,
		    XEvent *event,
		    XmTextPosition *start,
		    XmTextPosition *end,
		    XmTextPosition *cursorPos,	/* RETURN, may be NULL if not */
		    XmTextBlock block,
		    XmTextBlock newblock,	/* RETURN */
		    Boolean *freeBlock)
{
  register XmSourceData data = initiator->text.source->data;
  register long delta;
  register int block_num_chars;  /* number of characters in the block */
  XmTextPosition newInsert = initiator->text.cursor_position;
  XmTextVerifyCallbackStruct tvcb;
  XmTextVerifyCallbackStructWcs wcs_tvcb;
  XmTextBlockRecWcs wcs_newblock;
  
  *freeBlock = False;
  
  if (*start == *end && block->length == 0) return False;
  
  _XmTextValidate(start, end, data->length);
  
    newblock->length = block->length; 	/* RETURNed values */
    newblock->format = block->format; 
    newblock->ptr = block->ptr; 
    
    if (!initiator->text.modify_verify_callback &&
        !initiator->text.wcs_modify_verify_callback) 
    {
        /* we have neither callback, so do expensive 
        ** computation only if cursorPos is needed 
        */
        if (cursorPos)
  	{
    	block_num_chars = _XmTextCountCharacters(block->ptr, block->length);
    	*cursorPos = *start + block_num_chars;
  	}
      return True;
    }
  
    /* cursorPos may be needed to be returned. If not, and if the text is
    ** not editable, can drop out now
    */
    if (!cursorPos && !data->editable)
      return False;
  
    /* there is at least one callback, so block_num_chars is needed; 
    ** this is the potentially-expensive operation that we're trying to avoid 
    */
    block_num_chars = _XmTextCountCharacters(block->ptr, block->length);
    if (cursorPos) *cursorPos = *start + block_num_chars;
    
    if (!data->editable)	/* if cursorPos was needed, it's set; can now drop out */
      return False;
  
    /* we have at least one callback on editable text, and cursorPos may or may
    ** not need to be set. block_num_chars has been set, so perform some other 
    ** quick evaluations on whether or not the modify is reasonable. Then
    ** call one or both of the callbacks, and if necessary reset cursorPos.
    */
  
    delta = block_num_chars - (*end - *start);
    if (delta > 0 && (data->length + delta > data->maxallowed))
      return False;
  
  /* If both modify_verify and modify_verify_wcs are registered:
   *    - first call the char* callback, then
   *    - pass the modified data from the char* callback to the
   *      wchar_t callback.
   * If programmers set both callback lists, they get's what they asked for.
   */
  
  wcs_newblock.wcsptr = (wchar_t *)NULL;
  wcs_newblock.length = 0;
  
  /* If there are char* callbacks registered, call them. */
  if (initiator->text.modify_verify_callback) {
    /* Fill in the block to pass to the callback. */
    if (block->length) {
      if (block->length < 0 ||
          (size_t)block->length >= (size_t)UINT_MAX)
	{
	  *freeBlock = False;
	  return False;
	}
      newblock->ptr = (char *) XtMalloc((Cardinal)((size_t)block->length + 1));
      if (newblock->ptr == NULL) {
	*freeBlock = False;
	return False;
      }
      *freeBlock = True;
      (void) memcpy((void*) newblock->ptr, (void*) block->ptr,
		    block->length);
      newblock->ptr[block->length] = '\0';
    }
    
    /* Call Verification Callback to indicate that text is being modified */
    tvcb.reason = XmCR_MODIFYING_TEXT_VALUE;
    tvcb.event = event;
    tvcb.currInsert = (XmTextPosition) (initiator->text.cursor_position);
    tvcb.newInsert = (XmTextPosition) (initiator->text.cursor_position);
    tvcb.startPos = *start;
    tvcb.endPos = *end;
    tvcb.doit = True;
    tvcb.text = newblock;
    XtCallCallbackList ((Widget) initiator,
			initiator->text.modify_verify_callback,
			(XtPointer) &tvcb);
    /* If doit flag is false, application wants to negate the action,
     * so free allocate space and return False.
     */
    if (!tvcb.doit) {
      if (newblock->ptr && newblock->ptr != block->ptr) 
	XtFree(newblock->ptr);
      *freeBlock = False;
      return False;
    } else {
      *start = tvcb.startPos;
      *end = tvcb.endPos;
      newInsert = tvcb.newInsert;
      _XmTextValidate (start, end, data->length);
      if (tvcb.text != newblock || tvcb.text->ptr != newblock->ptr) {
	newblock->length = tvcb.text->length;
	if (newblock->ptr && newblock->ptr != block->ptr) 
	  XtFree(newblock->ptr);
	*freeBlock = False;
	if (newblock->length) {
	  if (newblock->length < 0 ||
	      (size_t)newblock->length >= (size_t)UINT_MAX)
	    return False;
	  newblock->ptr = XtMalloc((Cardinal)((size_t)newblock->length + 1));
	  if (newblock->ptr == NULL) {
	    return False;
	  }
	  *freeBlock = True;
	  (void)memcpy((void*)newblock->ptr, (void*)tvcb.text->ptr, 
		       tvcb.text->length);
	} else newblock->ptr = NULL;
      }
      newblock->format = tvcb.text->format;
      block_num_chars = _XmTextCountCharacters(newblock->ptr,
					       newblock->length);
      
      delta = block_num_chars - (*end - *start);
      
      if (delta > 0 && data->length + delta > data->maxallowed &&
	  (!UnderVerifyPreedit(initiator))) {
	if (newblock->ptr && newblock->ptr != block->ptr) 
	  XtFree(newblock->ptr);
	*freeBlock = False;
	return False;
      }
    }
  }  /* end if there are char* modify verify callbacks */
  
  if (initiator->text.wcs_modify_verify_callback) {
    size_t wcs_capacity;

    if (newblock->length < 0 ||
	(size_t)newblock->length > (SIZE_MAX / sizeof(wchar_t)) - 1 ||
	(size_t)newblock->length + 1 > UINT_MAX / sizeof(wchar_t)) {
	  if (newblock->ptr && newblock->ptr != block->ptr)
	    XtFree(newblock->ptr);
	  *freeBlock = False;
	  return False;
    }
    wcs_capacity = ((size_t)newblock->length + 1) * sizeof(wchar_t);
    wcs_newblock.wcsptr = (wchar_t *)XtMalloc(wcs_capacity);
    if (wcs_newblock.wcsptr == NULL) {
      if (newblock->ptr && newblock->ptr != block->ptr)
	XtFree(newblock->ptr);
      *freeBlock = False;
      return False;
    }
    wcs_newblock.length = mbstowcs(wcs_newblock.wcsptr, newblock->ptr,
				   block_num_chars);
    if (wcs_newblock.length < 0) wcs_newblock.length = 0;
    wcs_tvcb.reason = XmCR_MODIFYING_TEXT_VALUE;
    wcs_tvcb.event = event;
    wcs_tvcb.currInsert = initiator->text.cursor_position;
    wcs_tvcb.newInsert = initiator->text.cursor_position;
    wcs_tvcb.startPos = *start;
    wcs_tvcb.endPos = *end;
    wcs_tvcb.doit = True;
    wcs_tvcb.text = &wcs_newblock;
    
    XtCallCallbackList((Widget) initiator,
		       initiator->text.wcs_modify_verify_callback,
		       (XtPointer) &wcs_tvcb);
    if (!wcs_tvcb.doit) {
      if (newblock->ptr && newblock->ptr != block->ptr)
	XtFree(newblock->ptr);
      *freeBlock = False;
      if (wcs_newblock.wcsptr) XtFree((char*)wcs_newblock.wcsptr);
      return False;
    } else {
      *start = wcs_tvcb.startPos;
      *end = wcs_tvcb.endPos;
      newInsert = wcs_tvcb.newInsert;
      _XmTextValidate (start, end, data->length);
      /* use newblock as a temporary holder and put the char*
       * data there */
      if (newblock->ptr && newblock->ptr != block->ptr) {
	XtFree(newblock->ptr);
	newblock->ptr = NULL;
      }
      *freeBlock = False;
      if (wcs_tvcb.text->length) {
	size_t wide_length = (size_t)wcs_tvcb.text->length;
	size_t capacity = 0;
	{
	    size_t unit_size = (size_t)(initiator->text.char_size < 3 ?
		(int)initiator->text.char_size : sizeof(wchar_t));
	    if (unit_size != 0 && wide_length <= (SIZE_MAX / unit_size) - 1)
		capacity = (wide_length + 1) * unit_size;
	}
	if (capacity == 0 || capacity > (size_t)UINT_MAX) {
	    newblock->ptr = NULL;
	    newblock->length = -1;
	} else {
	    newblock->ptr = (char *)XtMalloc((Cardinal)capacity);
	    if (newblock->ptr == NULL) {
		newblock->length = -1;
	    } else {
		*freeBlock = True;
		wcs_tvcb.text->wcsptr[wcs_tvcb.text->length] = (wchar_t)0L;
		{
		    size_t converted = wcstombs(newblock->ptr,
				wcs_tvcb.text->wcsptr, capacity);
		    if (converted == (size_t)-1 || converted >= capacity ||
			converted > (size_t)INT_MAX)
			newblock->length = -1;
		    else
			newblock->length = (int)converted;
		}
	    }
	}
      } else {
	newblock->ptr = NULL;
	newblock->length = 0;
      }
      
      block_num_chars = wcs_tvcb.text->length;
      delta = block_num_chars - (*end - *start);
      
      /* if the wcstombs found bad data, newblock->length is negative */
      if ((delta > 0 && data->length + delta > data->maxallowed &&
	  (!UnderVerifyPreedit(initiator))) ||
	  newblock->length < 0) {
	
	if (newblock->ptr && newblock->ptr != block->ptr)
	  XtFree(newblock->ptr);
	*freeBlock = False;
	if (wcs_newblock.wcsptr) XtFree((char*)wcs_newblock.wcsptr);
	return False;
      }
    }
    
    /* If we alloced space for the wcs_newblock, we need to clean it up */
    if (wcs_newblock.wcsptr) XtFree((char*)wcs_newblock.wcsptr);
    
  }  /* end if there are wide char modify verify callbacks */
  
  if (cursorPos)
  {
	  if (initiator->text.cursor_position != newInsert)	/* true only if we have callbacks */
	  {
	    if (newInsert > data->length + delta) {
	      *cursorPos = data->length + delta;
	    } else if (newInsert < 0) {
	      *cursorPos = 0;
	    } else {
	      *cursorPos = newInsert;
	    }
	  }
	  else
	    *cursorPos = *start + block_num_chars;
  }
  
  return True;
}

/*ARGSUSED*/
static XmTextStatus 
Replace(XmTextWidget initiator,
        XEvent * event,		/* unused */
        XmTextPosition *start,
        XmTextPosition *end,
        XmTextBlock block,
#if NeedWidePrototypes
        int call_callbacks)	/* unused */
#else
        Boolean call_callbacks)	/* unused */
#endif
{
  register XmSourceData data = initiator->text.source->data;
  register int i;
  register long delta;
  register int block_num_chars;  /* number of characters in the block */
  size_t gap_size, gap_start_offset, gap_end_offset, end_byte_offset;
  size_t removed_bytes;
  int old_maxlength;
  char *resized_ptr;
  XmTextUnicodeMap *replacement_map = NULL;
  int char_size = (initiator->text.char_size < 3 ?
		   (int)initiator->text.char_size :
		   sizeof(wchar_t));
  
  if (*start == *end && block->length == 0) return EditReject;
  
  _XmTextValidate(start, end, data->length);
	if (!TextSourceByteOffset(data, *end, (size_t)char_size,
				   &end_byte_offset))
	return EditError;
	removed_bytes = (size_t)(*end - *start) * (size_t)char_size;
  
  block_num_chars = _XmTextCountCharacters(block->ptr, block->length);
  delta = block_num_chars - (*end - *start);

  if (delta > (long)INT_MAX - data->length ||
      delta < -(long)data->length)
    return EditError;
  
  if (!data->editable ||
      (delta > 0 && data->length + delta > data->maxallowed &&
	(!UnderVerifyPreedit(initiator))))
    return EditError;
  
  /**********************************************************************/
  
  initiator->text.output->DrawInsertionPoint(initiator,
					     initiator->text.cursor_position,
					     off);
  
  /* Move the gap to the editing position (*start). */
  _XmStringSourceSetGappedBuffer(data, *start);
  
  for (i=0; i<data->numwidgets; i++) {
    _XmTextDisableRedisplay(data->widgets[i], TRUE);
    if (data->hasselection)
      _XmTextSetHighlight((Widget)data->widgets[i], data->left, 
			 data->right, XmHIGHLIGHT_NORMAL);
  }
  
  old_maxlength = data->maxlength;
  if (data->length + delta >= data->maxlength) {
    while (data->length + delta >= data->maxlength) {
	if (data->maxlength <= 0 ||
	    (data->maxlength < TEXT_INCREMENT &&
	     data->maxlength > INT_MAX / 2) ||
	    (data->maxlength >= TEXT_INCREMENT &&
	     data->maxlength > INT_MAX - TEXT_INCREMENT)) {
	  for (i = 0; i < data->numwidgets; i++) {
	    if (data->hasselection)
	      _XmTextSetHighlight((Widget)data->widgets[i], data->left,
				  data->right, XmHIGHLIGHT_SELECTED);
	    _XmTextEnableRedisplay(data->widgets[i]);
	  }
	  initiator->text.output->DrawInsertionPoint(initiator,
						     initiator->text.cursor_position,
						     on);
	  return EditError;
	}
	if (data->maxlength < TEXT_INCREMENT)
	data->maxlength *= 2;
      else
	data->maxlength += TEXT_INCREMENT;
    }
    
    gap_start_offset = (size_t)(data->gap_start - data->ptr);
    gap_end_offset = (size_t)(data->gap_end - data->ptr);
	if (char_size <= 0 ||
	    (size_t)data->maxlength >
	      (size_t)UINT_MAX / (size_t)char_size) {
	  for (i = 0; i < data->numwidgets; i++) {
	    if (data->hasselection)
	      _XmTextSetHighlight((Widget)data->widgets[i], data->left,
				  data->right, XmHIGHLIGHT_SELECTED);
	    _XmTextEnableRedisplay(data->widgets[i]);
	  }
	  initiator->text.output->DrawInsertionPoint(initiator,
						     initiator->text.cursor_position,
						     on);
	  return EditError;
	}
	resized_ptr = XtRealloc(data->ptr,
				(Cardinal)((size_t)data->maxlength *
					   (size_t)char_size));
	if (resized_ptr == NULL) {
	  for (i = 0; i < data->numwidgets; i++) {
	    if (data->hasselection)
	      _XmTextSetHighlight((Widget)data->widgets[i], data->left,
				  data->right, XmHIGHLIGHT_SELECTED);
	    _XmTextEnableRedisplay(data->widgets[i]);
	  }
	  initiator->text.output->DrawInsertionPoint(initiator,
						     initiator->text.cursor_position,
						     on);
	  return EditError;
	}
	data->ptr = resized_ptr;
	data->gap_start = data->ptr + gap_start_offset;
	data->gap_end = data->ptr + gap_end_offset +
      ((size_t)char_size * (size_t)(data->maxlength - old_maxlength));
    if (gap_end_offset != ((size_t)old_maxlength * (size_t)char_size))
      memmove(data->gap_end, data->ptr + gap_end_offset, 
	      ((size_t)old_maxlength * (size_t)char_size) - gap_end_offset);
    /* Do something to move the allocated space into the buffer */
  } 
  
  /* Replace receives the final approved range/block, after modify-verify.
   * Preserve an initialized immutable map through the exact accepted edit;
   * MapReplace validates the UTF-8 and leaves its input unchanged on failure.
   * Source positions count locale characters, so only a supported UTF-8
   * locale may use the bytes directly as Unicode scalar coordinates. */
  if (data->unicode_map && XmTextUnicodeLocaleAvailable() &&
      XmTextUnicodeScalarCount(data->unicode_map) == (size_t)data->length) {
    replacement_map = XmTextUnicodeMapReplace(data->unicode_map,
        (size_t)*start, (size_t)*end, block->ptr, (size_t)block->length);
    if (replacement_map &&
        XmTextUnicodeScalarCount(replacement_map) != (size_t)(data->length + delta)) {
      XmTextUnicodeMapFree(replacement_map);
      replacement_map = NULL;
    }
  }
  data->length += (int) delta;
  
  if (data->hasselection && *start < data->right && *end > data->left) {
    if (*start <= data->left) {
      if (*end < data->right) {
	data->left = *end; /* delete encompasses left half of the
			      selection so move left endpoint */
      } else {
	data->right = data->left; /* delete encompasses the selection
				     so set selection to NULL */
      }
    } else {
      if (*end >= data->right) {
	data->right = *start; /* delete encompasses the right half of the
				 selection so move right endpoint */
      } else {
	data->right -= (*end - *start); /* delete is completely within the
					   selection so shrink the selection */
      }
    }
  }
  
  /* delete data */
  gap_size = data->gap_end - data->gap_start;
  /* expand the end of the gap to the right */
  if ((data->ptr + gap_size + end_byte_offset) > data->gap_end)
    data->gap_end += removed_bytes;
  
  /* add data */
  /* copy the data into the gap_start and increment the gap start pointer */
  /* convert data from char* to characters and copy into the gapped buffer */
  if ((int)initiator->text.char_size == 1) {
    for (i=0; i < block->length; i++) {
      /* if (data->gap_start == data->gap_end) break; */
      *data->gap_start++ = block->ptr[i];
    }
    
  } else {
	data->gap_start += (size_t)char_size *
	  (size_t)_XmTextBytesToCharacters(data->gap_start,
			       &block->ptr[0], 
			       block_num_chars, False,
			       (int)initiator->text.char_size);
  }

  /* Publish the cache only after the native source bytes/length commit, and
   * before invalidation/redisplay can reenter through another sharing widget.
   * A failed optional cache update never leaves old metadata on new bytes;
   * the next interactive query may retry building a correct map lazily. */
  {
    XmTextUnicodeMap *previous_map = data->unicode_map;
    data->unicode_map = replacement_map;
    XmTextUnicodeMapFree(previous_map);
  }
  
  if (data->hasselection && data->left != data->right) {
    if (*end <= data->left) {
      data->left += delta;
      data->right += delta;
    }
    if (data->left > data->right)
      data->right = data->left;
  }
  
  for (i=0; i<data->numwidgets; i++) {
    _XmTextInvalidate(data->widgets[i], *start, *end, delta);
    _XmTextUpdateLineTable((Widget) data->widgets[i], *start,
			   *end, block, True);
    if (data->hasselection)
      _XmTextSetHighlight((Widget)data->widgets[i], data->left, 
			 data->right, XmHIGHLIGHT_SELECTED);
    
    _XmTextEnableRedisplay(data->widgets[i]);
  }
  initiator->text.output->DrawInsertionPoint(initiator,
					     initiator->text.cursor_position, 
					     on);
  
  if (data->maxlength != TEXT_INITIAL_INCREM &&
      ((data->maxlength > TEXT_INCREMENT &&
	data->length <= data->maxlength - TEXT_INCREMENT) ||
       data->length <= data->maxlength >> 1)) {
    int compact_maxlength;
    size_t compact_bytes;
    char *compact_ptr;

    /* Move the gap to the last position. */
    _XmStringSourceSetGappedBuffer(data, data->length);

    compact_maxlength = TEXT_INITIAL_INCREM;
    

    while (data->length >= compact_maxlength) {
	if ((compact_maxlength < TEXT_INCREMENT &&
	     compact_maxlength > INT_MAX / 2) ||
	    (compact_maxlength >= TEXT_INCREMENT &&
	     compact_maxlength > INT_MAX - TEXT_INCREMENT))
	  break;
	if (compact_maxlength < TEXT_INCREMENT)
	  compact_maxlength *= 2;
      else
	  compact_maxlength += TEXT_INCREMENT;
    }

    if (compact_maxlength > 0 && char_size > 0 &&
	(size_t)compact_maxlength <=
	  (size_t)UINT_MAX / (size_t)char_size) {
	compact_bytes = (size_t)compact_maxlength * (size_t)char_size;
	compact_ptr = XtRealloc(data->ptr, (Cardinal)compact_bytes);
	if (compact_ptr != NULL) {
	  data->ptr = compact_ptr;
	  data->maxlength = compact_maxlength;
	  data->gap_start = data->ptr +
	      ((size_t)data->length * (size_t)char_size);
	  data->gap_end = data->ptr +
	      ((size_t)(data->maxlength - 1) * (size_t)char_size);
	}
    }
  }
  
  return EditDone;
}

#define Increment(data, position, direction)\
{\
  if (direction == XmsdLeft) {\
    if (position > 0) \
      position--;\
  } else {\
    if (position < data->length)\
      position++;\
  }\
}

#define Look(data, position, dir) \
    ((dir == XmsdLeft) \
      ? ((position) ? _XmStringSourceGetChar(data, position - 1) \
	            : NULL) \
      : ((position == data->length) ? NULL \
	                            : _XmStringSourceGetChar(data, position)))
   
static void 
ScanParagraph(XmSourceData data,
	      XmTextPosition *new_position,
	      XmTextScanDirection dir,
	      int ddir,
	      XmTextPosition *last_char)
{
  Boolean found = False;
  XmTextPosition position = *new_position;
  char mb_char[1 + MB_LEN_MAX];
  char * c;
  
  while (position >= 0 && position <= data->length) {
    /* DELTA: Look now returns a pointer */
    /* DELTA: EFFECIENCY: LEAVE AS SHORT*, INT*, ... COMPARE TO PSWC_NWLN */
    c = Look(data, position, dir);
    (void) _XmTextCharactersToBytes(mb_char, c, 1, 
				    (int)data->widgets[0]->text.char_size);
    if (*mb_char == '\n') {
      /* DELTA: Look now returns a pointer */
      c = Look(data, position + ddir, dir);
      (void) _XmTextCharactersToBytes(mb_char, c, 1,
				      (int)data->widgets[0]->text.char_size);
      while (isspace((unsigned char)*mb_char)) {
	if (*mb_char == '\n') {
	  found = True;
	  while (isspace((unsigned char)*mb_char)) {
	    /* DELTA: Look now returns a pointer */
	    c = Look(data, position + ddir, dir);
	    (void) _XmTextCharactersToBytes(mb_char, c, 1,
					(int)data->widgets[0]->text.char_size);
	    Increment(data, position, dir);
	  }
	  break;
	}
	/* DELTA: Look now returns a pointer */
	c = Look(data, position + ddir, dir);
	(void) _XmTextCharactersToBytes(mb_char, c, 1,
					(int)data->widgets[0]->text.char_size);
	Increment(data, position, dir);
	/* BEGIN 3145 fix -- Do not bypass a nonspace character */
	if (!isspace((unsigned char)*c))
	  *last_char = (position) + ddir;
	/* END 3145 */
      }
      if (found) break;
    } else if (!isspace((unsigned char)*mb_char)) {
      *last_char = (position) + ddir;
    }
    
    if(((dir == XmsdRight) && (position == data->length)) ||
       ((dir == XmsdLeft) && (position == 0)))
      break;
    Increment(data, position, dir);
  }
  
  *new_position = position;
}

static XmTextPosition 
Scan(XmTextSource source,
     XmTextPosition pos,
     XmTextScanType sType,
     XmTextScanDirection dir,
     int count,
#if NeedWidePrototypes
     int include)
#else
     Boolean include)
#endif /* NeedWidePrototypes */
{
  register long whiteSpace = -1;
  register XmTextPosition position = pos;
  register int i;
  XmTextPosition temp;
  XmSourceData data = source->data;
  XmTextWidget tw = (XmTextWidget)data->widgets[0];
  char * c;
  BITS16 * bits16_ptr;
  wchar_t * wchar_t_ptr;
  char mb_char[1 + MB_LEN_MAX];
  Boolean start_is_mb, cur_is_mb;  /* False == 1-byte char, else multi-byte */
  int num_bytes = 0;
  int ddir = (dir == XmsdRight) ? 1 : -1;
  
  switch (sType) {
  case XmSELECT_POSITION: 
    if (!include && count > 0)
      count -= 1;
    for (i = 0; i < count; i++) {
      Increment(data, position, dir);
    }
    break;
  case XmSELECT_WHITESPACE: 
  case XmSELECT_WORD:
    if (tw->text.char_size == 1) {
      char * c;
      for (i = 0; i < count; i++) {
	whiteSpace = -1;
	while (position >= 0 && position <= data->length) {
	  c = Look(data, position, dir);
	  if (c && isspace((unsigned char)*c)) {
	    if (whiteSpace < 0) whiteSpace = position;
#ifdef FIX_1320
	    if (*c == '\n') break;
#endif
	  } else if (whiteSpace >= 0)
	    break;
	  position += ddir;
	}
      }
    } else {
      for (i = 0; i < count; i++) {
	whiteSpace = -1;
	num_bytes = _XmTextCharactersToBytes(mb_char,
					     Look(data, position, dir),
					     1,(int)tw->text.char_size);
	start_is_mb = (num_bytes < 2 ? False : True);
	while (position >= 0 && position <= data->length) {
	  num_bytes = _XmTextCharactersToBytes(mb_char,
					       Look(data, position, dir),
					       1, (int)tw->text.char_size);
	  cur_is_mb = (num_bytes < 2 ? False : True);
	  if (!cur_is_mb && 
	      isspace((unsigned char)*mb_char)) {
	    if (whiteSpace < 0) whiteSpace = position;
	  } else if ((sType == XmSELECT_WORD) &&
		     (start_is_mb ^ cur_is_mb)) {
	    if (whiteSpace < 0) whiteSpace = position;
	    break;
	  } else if (whiteSpace >= 0)
	    break;
	  position += ddir;
	}
      }
    }
    if (!include) {
      if(whiteSpace < 0 && dir == XmsdRight)
	whiteSpace = data->length;
      position = whiteSpace;
    }
    break;
  case XmSELECT_LINE: 
    if (XmTextUnicodeLocaleAvailable())
      return ScanUnicodeLine(data, position, dir, count, (Boolean)include);
    for (i = 0; i < count; i++) {
      while (position >= 0 && position <= data->length) {
	/* DELTA: Look now returns a pointer */
	if ((int)tw->text.char_size == 1) {
	  c = Look(data, position, dir);
	  if ((c == NULL) || (*c == *data->PSWC_NWLN))
	    break;
	}
	else if ((int)tw->text.char_size == 2) {
	  bits16_ptr = (BITS16 *) Look(data, position, dir);
	  if ((bits16_ptr == NULL) ||
	      (*bits16_ptr == *(BITS16 *)data->PSWC_NWLN))
	    break;
	}
	else { /* MB_CUR_MAX == 3 or 4 or more */
	  wchar_t_ptr = (wchar_t *) Look(data, position, dir);
	  if ((wchar_t_ptr == NULL) ||
	      (*wchar_t_ptr == *(wchar_t *)data->PSWC_NWLN))
	    break;
	}
	
	if(((dir == XmsdRight) && (position == data->length)) || 
	   ((dir == XmsdLeft) && (position == 0)))
	  break;
	Increment(data, position, dir);
      }
      if (i + 1 != count)
	Increment(data, position, dir);
    }
    if (include) {
      /* later!!!check for last char in file # eol */
      Increment(data, position, dir);
    }
    break;
  case XmSELECT_PARAGRAPH: 
    /* Muliple paragraph scanning is not guarenteed to work. */
    for (i = 0; i < count; i++) {
      XmTextPosition start_position = position; 
      XmTextPosition last_char = position; 
      
      /* if scanning forward, check for between paragraphs condition */
      if (dir == XmsdRight) {
	/* DELTA: Look now returns a pointer */
	c = Look(data, position, dir);
	(void) _XmTextCharactersToBytes(mb_char, 
					Look(data, position, dir), 
					1, (int)tw->text.char_size);
	/* if is space, go back to first non-space */
	while (isspace((unsigned char)*mb_char)) {
	  if (position > 0)
	    position--;
	  else if (position == 0)
	    break;
	  (void) _XmTextCharactersToBytes(mb_char, 
					  Look(data, position, dir),
					  1, (int)tw->text.char_size);
	}
      }
      
      temp = position;
      ScanParagraph(data, &temp, dir, ddir, &last_char);
      position = temp;
      
      /*
       * If we are at the beginning of the paragraph and we are
       * scanning left, we need to rescan to find the character
       * at the beginning of the next paragraph.
       */  
      if (dir == XmsdLeft) {
	/* If we started at the beginning of the paragraph, rescan */
	if (last_char == start_position) {
	  temp = position;
	  ScanParagraph(data, &temp, dir, ddir, &last_char);
	}
	/*
	 * Set position to the last non-space 
	 * character that was scanned.
	 */
	position = last_char;
      }
      
      if (i + 1 != count)
	Increment(data, position, dir);
      
    }
    if (include) {
      Increment(data, position, dir);
    }
    break;
  case XmSELECT_ALL:
  default: 
    if (dir == XmsdLeft)
      position = 0;
    else
      position = data->length;
  }
  if (position < 0) position = 0;
  if (position > data->length) position = data->length;
  return(position);
}

static Boolean 
GetSelection(XmTextSource source,
	     XmTextPosition *left,
	     XmTextPosition *right)
{
  XmSourceData data = source->data;
  
  if (data->hasselection && data->left < data->right && data->left >= 0) {
    *left = data->left;
    *right = data->right;
    return True;
  } else {
    *left = *right = 0;
    data->hasselection = False;
    data->take_selection = True;
  }
  return False;
}

static void 
SetSelection(XmTextSource source,
	     XmTextPosition left,
	     XmTextPosition right, /* if right == -999, then  we're in 
				      LoseSelection, so don't call 
				      XtDisownSelection.*/
	     Time set_time)
{
  XmSourceData data = source->data;
  XmTextWidget tw;
  int i;
  int oldleft, oldright;
  
  if (!XtIsRealized((Widget)data->widgets[0]) ||
      (left > right && !data->hasselection)) return;
  
  if (left < 0) left = right = 0;
  
  for (i=0; i<data->numwidgets; i++) {
    tw = (XmTextWidget)(data->widgets[i]);
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position,
					   off);
    _XmTextDisableRedisplay(data->widgets[i], FALSE);
    if (data->hasselection)
      _XmTextSetHighlight((Widget)data->widgets[i], data->left, 
			 data->right, XmHIGHLIGHT_NORMAL);
    
    data->widgets[i]->text.output->data->refresh_ibeam_off = True;
  }
  
  oldleft = data->left;
  oldright = data->right;
  data->left = left;
  data->right = right;
  if (data->numwidgets > 0) {
    Widget widget = (Widget) data->widgets[0];
    tw = (XmTextWidget)widget;
    if (!set_time) set_time = _XmValidTimestamp(widget);
    
    if (left <= right) {
      if (data->take_selection || (oldleft == oldright && left != right)) {
	if (!XmePrimarySource(widget, set_time)) {
	  (*source->SetSelection)(source, 1, 0, set_time);
	} else {
	  XmAnyCallbackStruct cb;
	  
	  data->prim_time = set_time;
	  data->hasselection = True;
	  data->take_selection = False;
	  
	  cb.reason = XmCR_GAIN_PRIMARY;
	  cb.event = NULL;
	  XtCallCallbackList ((Widget) data->widgets[0], 
			      data->widgets[0]->text.gain_primary_callback,
			      (XtPointer) &cb);
	}
      }
      if (data->hasselection && data->left < data->right) {
	for (i=0; i<data->numwidgets; i++) {
	  _XmTextSetHighlight((Widget) data->widgets[i], data->left,
			     data->right, XmHIGHLIGHT_SELECTED);
	}
      }
      if (left == right) tw->text.add_mode = False;
    } else {
      if (right != -999)
	XtDisownSelection(widget, XA_PRIMARY, set_time);
      data->hasselection = False;
      data->take_selection = True;
      tw->text.add_mode = False;
    }
  }
  for (i=0; i<data->numwidgets; i++) {
    tw = (XmTextWidget)(data->widgets[i]);
    _XmTextEnableRedisplay(data->widgets[i]);
    (*tw->text.output->DrawInsertionPoint)(tw, tw->text.cursor_position,
					   on);
  }
}

/* Check the source method identity before inspecting any native source data. */
Boolean
_XmStringSourceUsesUnicodeLines(XmTextSource source)
{
  return source && source->Scan == Scan && XmTextUnicodeLocaleAvailable();
}

/* Source Scan(POSITION,count) retains its scalar-count contract for layout
 * callers. Interactive movement/deletion explicitly requests a grapheme step. */
XmTextPosition
_XmStringSourceGraphemeStep(XmTextSource source, XmTextPosition position,
                           XmTextScanDirection direction)
{
  XmSourceData data;
  size_t next;
  if (source->Scan != Scan || !XmTextUnicodeLocaleAvailable())
    return (*source->Scan)(source, position, XmSELECT_POSITION, direction, 1, True);
  data = source->data;
  if (direction == XmsdRight && position >= data->length)
    return data->length;
  if (direction == XmsdLeft && position <= 0)
    return 0;
  if (!data->unicode_map) {
    char *utf8 = _XmStringSourceGetValue(source, False);
    data->unicode_map = XmTextUnicodeMapCreate(utf8, strlen(utf8));
    XtFree(utf8);
    if (data->unicode_map && XmTextUnicodeScalarCount(data->unicode_map) != (size_t)data->length) {
      XmTextUnicodeMapFree(data->unicode_map); data->unicode_map = NULL;
    }
  }
  if (data->unicode_map && position >= 0 && position <= data->length &&
      (direction == XmsdRight ? XmTextUnicodeNextGrapheme(data->unicode_map, (size_t)position, &next)
                             : XmTextUnicodePreviousGrapheme(data->unicode_map, (size_t)position, &next)))
    return (XmTextPosition)next;
  /* An enabled Unicode map failure must not silently turn a grapheme action
   * into one scalar of a combining sequence. Leave the caret/source intact;
   * retry on a later query after transient allocation failure. Custom sources
   * and unsupported locales retain their native Scan contract above. */
  XmeWarning(data->numwidgets ? (Widget)data->widgets[0] : NULL,
             "Cannot map the text source for a Unicode grapheme action");
  return position;
}

/* Public routines. */

void
_XmTextValueChanged(XmTextWidget initiator,
		    XEvent *event)
{
  XmAnyCallbackStruct cb;
  
  cb.reason = XmCR_VALUE_CHANGED;
  cb.event = event;
  if (initiator->text.value_changed_callback)
    XtCallCallbackList((Widget)initiator,
		       initiator->text.value_changed_callback, (XtPointer)&cb);
}

XmTextSource 
_XmStringSourceCreate(char *value,
#if NeedWidePrototypes
		      int is_wchar)
#else
                      Boolean is_wchar)
#endif /* NeedWidePrototypes */
{
  XmTextSource source;
  XmSourceData data;
  int num_chars;
  char newline = '\n';
  wchar_t *wc_value = NULL;
  char * tmp_value = NULL;
  int char_size, max_char_size;
  int ret_value = 0;
  size_t allocation_bytes;

  source = (XmTextSource) XtMalloc((unsigned) sizeof(XmTextSourceRec));
  if (source == NULL)
    return NULL;
  data = (XmSourceData) XtMalloc((unsigned) sizeof(XmSourceDataRec));
  if (data == NULL)
    {
      XtFree((char *)source);
      return NULL;
    }
  memset(data, 0, sizeof(*data));
  source->data = data;
  source->AddWidget = AddWidget;
  source->CountLines = CountLines;
  source->RemoveWidget = RemoveWidget;
  source->ReadSource = ReadSource;
  source->Replace = (ReplaceProc) Replace;
  source->Scan = Scan;
  source->GetSelection = GetSelection;
  source->SetSelection = SetSelection;
  
  data->source = source;
  data->unicode_map = NULL;
  switch (MB_CUR_MAX) {
  case 1: case 2: 
    max_char_size = char_size = MB_CUR_MAX;
    break;
  case 0: 
    max_char_size = char_size = 1;
    break;
  default: 
    max_char_size = MB_CUR_MAX;
    char_size = sizeof(wchar_t);
  }
  
  if (is_wchar) {
    if (value == NULL)
      num_chars = 0;
    else
      for (num_chars = 0, wc_value = (wchar_t*)value;
	   wc_value[num_chars] != 0L;
	   num_chars++) {
	if (num_chars == INT_MAX)
	  goto allocation_failure;
	   }
	/*EMPTY*/;
    
    if (num_chars == INT_MAX)
	goto allocation_failure;
    data->maxlength = TEXT_INITIAL_INCREM;
    
    while ((num_chars + 1) >= data->maxlength) {
	if (data->maxlength <= 0 ||
	    (data->maxlength < TEXT_INCREMENT &&
	     data->maxlength > INT_MAX / 2) ||
	    (data->maxlength >= TEXT_INCREMENT &&
	     data->maxlength > INT_MAX - TEXT_INCREMENT))
	  goto allocation_failure;
      if (data->maxlength < TEXT_INCREMENT)
	data->maxlength *= 2;
      else
	data->maxlength += TEXT_INCREMENT;
    }
    
    data->old_length = 0;
    if (char_size <= 0 ||
	(size_t)data->maxlength > (size_t)UINT_MAX / (size_t)char_size)
	  goto allocation_failure;
    allocation_bytes = (size_t)data->maxlength * (size_t)char_size;
    data->ptr = XtMalloc((Cardinal)allocation_bytes);
    if (data->ptr == NULL)
      goto allocation_failure;
    if (max_char_size <= 0 ||
	(size_t)num_chars + 1 >
	  (size_t)UINT_MAX / (size_t)max_char_size)
	  goto allocation_failure;
    allocation_bytes = ((size_t)num_chars + 1) * (size_t)max_char_size;
    tmp_value = XtMalloc((Cardinal)allocation_bytes);
    if (tmp_value == NULL)
      goto allocation_failure;
    ret_value = value == NULL ? 0 :
      wcstombs(tmp_value, wc_value, allocation_bytes);
    data->value = NULL;  /* Scratch area for block->ptr conversions */
    /* Doesnt include NULL */
    if (ret_value < 0)
      data->length = 0;
    else 
      data->length = _XmTextBytesToCharacters(data->ptr, tmp_value, num_chars,
					      False, max_char_size);
    XtFree(tmp_value);
  } else {
    if (value)
      num_chars = _XmTextCountCharacters(value, strlen(value));
    else
      num_chars = 0;
    if (num_chars < 0 || num_chars == INT_MAX)
	goto allocation_failure;
    data->maxlength = TEXT_INITIAL_INCREM;
    
    while ((num_chars + 1) >= data->maxlength) {
	if (data->maxlength <= 0 ||
	    (data->maxlength < TEXT_INCREMENT &&
	     data->maxlength > INT_MAX / 2) ||
	    (data->maxlength >= TEXT_INCREMENT &&
	     data->maxlength > INT_MAX - TEXT_INCREMENT))
	  goto allocation_failure;
      if (data->maxlength < TEXT_INCREMENT)
	data->maxlength *= 2;
      else
	data->maxlength += TEXT_INCREMENT;
    }
    
    data->old_length = 0;
    if (char_size <= 0 ||
	(size_t)data->maxlength > (size_t)UINT_MAX / (size_t)char_size)
	  goto allocation_failure;
    allocation_bytes = (size_t)data->maxlength * (size_t)char_size;
    data->ptr = XtMalloc((Cardinal)allocation_bytes);
    if (data->ptr == NULL)
      goto allocation_failure;
    
    data->value = NULL;  /* Scratch area for block->ptr conversions */
    data->length = _XmTextBytesToCharacters(data->ptr, value, num_chars,
					    False, max_char_size);
  }
  
  data->PSWC_NWLN = (char *) XtMalloc(char_size);
  if (data->PSWC_NWLN == NULL)
    goto allocation_failure;
  _XmTextBytesToCharacters(data->PSWC_NWLN, &newline, 1, False, 
			   max_char_size);
  
  data->numwidgets = 0;
  data->widgets = (XmTextWidget *) XtMalloc((unsigned) sizeof(XmTextWidget));
  if (data->widgets == NULL)
    goto allocation_failure;
  data->hasselection = False;
  data->take_selection = True;
  data->left = data->right = 0;
  data->editable = True;
  data->maxallowed = INT_MAX;
  data->gap_start = data->ptr +
    ((size_t)data->length * (size_t)char_size);
  data->gap_end = data->ptr +
    ((size_t)(data->maxlength - 1) * (size_t)char_size);
  data->prim_time = 0;
  return source;

allocation_failure:
  XtFree(tmp_value);
  XtFree((char *)data->widgets);
  XtFree((char *)data->PSWC_NWLN);
  XtFree((char *)data->value);
  XtFree((char *)data->ptr);
  XtFree((char *)data);
  XtFree((char *)source);
  return NULL;
}

void 
_XmStringSourceDestroy(XmTextSource source)
{
  XmTextUnicodeMapFree(source->data->unicode_map);
  XtFree((char *) source->data->ptr);
  XtFree((char *) source->data->value);
  XtFree((char *) source->data->widgets);
  XtFree((char *) source->data->PSWC_NWLN);
  XtFree((char *) source->data);
  XtFree((char *) source);
  source = NULL;
}

char *
_XmStringSourceGetValue(XmTextSource source,
#if NeedWidePrototypes
			int want_wchar)
#else
                        Boolean want_wchar)
#endif /* NeedWidePrototypes */
{
  XmSourceData data = source->data;
  XmTextWidget tw = (XmTextWidget) data->widgets[0];
  XmTextBlockRec block;
  int length = 0;
  XmTextPosition pos = 0;
  XmTextPosition ret_pos = 0;
  XmTextPosition last_pos = 0;
  char * temp;
  wchar_t * wc_temp;
  int return_val;
  size_t allocation;

  if (!want_wchar) {
    if (data->length > 0) {
      if (!TextSourceRangeBytes(0, data->length, tw->text.char_size,
				 &allocation))
	return NULL;
      temp = (char *) XtMalloc(allocation);
      if (temp == NULL)
	return NULL;
    } else
      return(XtNewString(""));
    
    last_pos = (XmTextPosition) data->length;
    
    while (pos < last_pos) {
      ret_pos = ReadSource(source, pos, last_pos, &block);
	if (ret_pos <= pos || block.length <= 0 || block.ptr == NULL ||
	    (size_t)block.length > allocation - 1 - (size_t)length) {
	XtFree(temp);
	break;
	}
      
      (void)memcpy((void*)&temp[length], (void*)block.ptr, block.length);
      length += block.length;
      pos = ret_pos;
    }
	if (pos != last_pos)
	  return NULL;
    temp[length] = '\0';
    return (temp);
  } else {
    
    if (data->length > 0) {
      if (!TextSourceRangeBytes(0, data->length, sizeof(wchar_t),
				 &allocation))
	return NULL;
      wc_temp = (wchar_t*)XtMalloc(allocation);
      if (wc_temp == NULL)
	return NULL;
    } else {
      wc_temp = (wchar_t*)XtMalloc(sizeof(wchar_t));
      if (wc_temp == NULL)
	return NULL;
      wc_temp[0] = (wchar_t)0L;
      return (char*) wc_temp;
    }
    
    last_pos = (XmTextPosition) data->length;
    
    while (pos < last_pos) {
      ret_pos = ReadSource(source, pos, last_pos, &block);
	  if (ret_pos <= pos || block.length <= 0 || block.ptr == NULL ||
	  (size_t)(ret_pos - pos) >
	    (allocation / sizeof(wchar_t)) - 1 - (size_t)length) {
	XtFree((char *)wc_temp);
	return NULL;
	  }

      return_val = mbstowcs(&wc_temp[length], block.ptr,
			     (size_t)(ret_pos - pos));
	  if (return_val == (size_t)-1 ||
	      return_val > (allocation / sizeof(wchar_t)) - 1 - (size_t)length) {
	XtFree((char *)wc_temp);
	return NULL;
	  }
      if (return_val > 0) length += return_val;
      pos = ret_pos;
    }
    wc_temp[length] = (wchar_t)0L;
    return ((char*)wc_temp);
  }
}

void 
_XmStringSourceSetValue(XmTextWidget tw,
			char *value)
{
  XmTextSource source = tw->text.source;
  XmSourceData data = source->data;
  Boolean editable, freeBlock;
  int maxallowed;
  XmTextBlockRec block, newblock;
  XmTextPosition fromPos = 0; 
  XmTextPosition toPos = data->length;
  size_t value_length;

  if (value == NULL)
    value = "";
  value_length = strlen(value);
  if (value_length > (size_t)INT_MAX)
    return;
  
  (*source->SetSelection)(source, 1, 0,
			  XtLastTimestampProcessed(XtDisplay(tw)));
  block.format = XmFMT_8_BIT;
  block.length = (int)value_length;
  block.ptr = value;
  editable = data->editable;
  maxallowed = data->maxallowed;
  data->editable = TRUE;
  data->maxallowed = INT_MAX;
  _XmTextSetHighlight((Widget)tw, 0, tw->text.last_position, 
		     XmHIGHLIGHT_NORMAL);
  
  if (_XmTextModifyVerify(tw, NULL, &fromPos, &toPos,
			  NULL, &block, &newblock, &freeBlock)) {
    (void)(source->Replace)(tw, NULL, &fromPos, &toPos, &newblock, False);
    if (freeBlock && newblock.ptr) XtFree(newblock.ptr);
    _XmTextValueChanged(tw, NULL);
  }
  
  data->editable = editable;
  data->maxallowed = maxallowed;
}


Boolean 
_XmStringSourceHasSelection(XmTextSource source)
{
  return source->data->hasselection;
}

Boolean 
_XmStringSourceGetEditable(XmTextSource source)
{
  return source->data->editable;
}

void 
_XmStringSourceSetEditable(XmTextSource source,
#if NeedWidePrototypes
			   int editable)
#else
                           Boolean editable)
#endif /* NeedWidePrototypes */
{
  source->data->editable = editable;
}

int 
_XmStringSourceGetMaxLength(XmTextSource source)
{
  return source->data->maxallowed;
}

void 
_XmStringSourceSetMaxLength(XmTextSource source,
			    int max)
{
  source->data->maxallowed = max;
}



