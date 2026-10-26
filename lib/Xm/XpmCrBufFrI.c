/*
 * Copyright (C) 1989-95 GROUPE BULL
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * GROUPE BULL BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
 * AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * Except as contained in this notice, the name of GROUPE BULL shall not be
 * used in advertising or otherwise to promote the sale, use or other dealings
 * in this Software without prior written authorization from GROUPE BULL.
 */

/*****************************************************************************\
*  CrBufFrI.c:                                                                *
*                                                                             *
*  XPM library                                                                *
*  Scan an image and possibly its mask and create an XPM buffer               *
*                                                                             *
*  Developed by Arnaud Le Hors                                                *
\*****************************************************************************/

/* October 2004, source code review by Thomas Biege <thomas@suse.de> */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "XpmI.h"

LFUNC(WriteColors, int, (char **dataptr, unsigned int *data_size,
			 unsigned int *used_size, XpmColor *colors,
			 unsigned int ncolors, unsigned int cpp));

LFUNC(WritePixels, int, (char *dataptr, unsigned int data_size,
			  unsigned int *used_size,
			  unsigned int width, unsigned int height,
			  unsigned int cpp, unsigned int *pixels,
			  XpmColor *colors, unsigned int ncolors));

LFUNC(WriteExtensions, int, (char *dataptr, unsigned int data_size,
			     unsigned int *used_size,
			     XpmExtension *ext, unsigned int num));

LFUNC(ExtensionsSize, unsigned int, (XpmExtension *ext, unsigned int num));
LFUNC(CommentsSize, int, (XpmInfo *info));

int
XpmCreateBufferFromImage(
    Display		 *display,
    char		**buffer_return,
    XImage		 *image,
    XImage		 *shapeimage,
    XpmAttributes	 *attributes)
{
    XpmImage xpmimage;
    XpmInfo info;
    int ErrorStatus;

    if (display == NULL || buffer_return == NULL ||
	(image == NULL && shapeimage == NULL))
	return XpmFileInvalid;

    /* initialize return value */
    *buffer_return = NULL;

    /* create an XpmImage from the image */
    ErrorStatus = XpmCreateXpmImageFromImage(display, image, shapeimage,
					     &xpmimage, attributes);
    if (ErrorStatus != XpmSuccess)
	return (ErrorStatus);

    /* create the buffer from the XpmImage */
    if (attributes) {
	xpmSetInfo(&info, attributes);
	ErrorStatus =
	    XpmCreateBufferFromXpmImage(buffer_return, &xpmimage, &info);
    } else
	ErrorStatus =
	    XpmCreateBufferFromXpmImage(buffer_return, &xpmimage, NULL);

    /* free the XpmImage */
    XpmFreeXpmImage(&xpmimage);

    return (ErrorStatus);
}


#undef RETURN
#define RETURN(status) \
do \
{ \
      ErrorStatus = status; \
      goto error; \
}while(0)

int
XpmCreateBufferFromXpmImage(
    char	**buffer_return,
    XpmImage	 *image,
    XpmInfo	 *info)
{
    /* calculation variables */
    int ErrorStatus;
    char buf[BUFSIZ];
    unsigned int cmts, extensions, ext_size = 0;
    unsigned int l, cmt_size = 0;
    char *ptr = NULL, *p;
    unsigned int ptr_size, used_size, tmp;

    if (buffer_return == NULL || image == NULL)
	return XpmFileInvalid;

    *buffer_return = NULL;

    cmts = info && (info->valuemask & XpmComments);
    extensions = info && (info->valuemask & XpmExtensions)
	&& info->nextensions;

    /* compute the extensions and comments size */
    if (extensions)
	ext_size = ExtensionsSize(info->extensions, info->nextensions);
    if (extensions && ext_size == 0)
	RETURN(XpmNoMemory);
    if (cmts) {
	int comment_size = CommentsSize(info);
	if (comment_size < 0)
	    RETURN(XpmNoMemory);
	cmt_size = (unsigned int)comment_size;
    }

    /* write the header line */
#ifndef VOID_SPRINTF
    used_size =
#endif
    sprintf(buf, "/* XPM */\nstatic char * image_name[] = {\n");
#ifdef VOID_SPRINTF
    used_size = strlen(buf);
#endif
    ptr_size = used_size + ext_size + cmt_size + 1; /* ptr_size can't be 0 */
    if(ptr_size <= used_size ||
       ptr_size <= ext_size  ||
       ptr_size <= cmt_size)
    {
        return XpmNoMemory;
    }
    ptr = (char *) XpmMalloc(ptr_size);
    if (!ptr)
	return XpmNoMemory;
    strcpy(ptr, buf);

    /* write the values line */
    if (cmts && info->hints_cmt) {
#ifndef VOID_SPRINTF
	used_size +=
#endif
	snprintf(ptr + used_size, ptr_size-used_size, "/*%s*/\n", info->hints_cmt);
#ifdef VOID_SPRINTF
	used_size += strlen(info->hints_cmt) + 5;
#endif
    }
#ifndef VOID_SPRINTF
    l =
#endif
    sprintf(buf, "\"%d %d %d %d", image->width, image->height,
	    image->ncolors, image->cpp);
#ifdef VOID_SPRINTF
    l = strlen(buf);
#endif

    if (info && (info->valuemask & XpmHotspot)) {
#ifndef VOID_SPRINTF
	l +=
#endif
	snprintf(buf + l, sizeof(buf)-l, " %d %d", info->x_hotspot, info->y_hotspot);
#ifdef VOID_SPRINTF
	l = strlen(buf);
#endif
    }
    if (extensions) {
#ifndef VOID_SPRINTF
	l +=
#endif
	sprintf(buf + l, " XPMEXT");
#ifdef VOID_SPRINTF
	l = strlen(buf);
#endif
    }
#ifndef VOID_SPRINTF
    l +=
#endif
    sprintf(buf + l, "\",\n");
#ifdef VOID_SPRINTF
    l = strlen(buf);
#endif
    ptr_size += l;
    if(ptr_size <= l)
        RETURN(XpmNoMemory);
    p = (char *) XpmRealloc(ptr, ptr_size);
    if (!p)
	RETURN(XpmNoMemory);
    ptr = p;
    strcpy(ptr + used_size, buf);
    used_size += l;

    /* write colors */
    if (cmts && info->colors_cmt) {
#ifndef VOID_SPRINTF
	used_size +=
#endif
	snprintf(ptr + used_size, ptr_size-used_size, "/*%s*/\n", info->colors_cmt);
#ifdef VOID_SPRINTF
	used_size += strlen(info->colors_cmt) + 5;
#endif
    }
    ErrorStatus = WriteColors(&ptr, &ptr_size, &used_size,
			      image->colorTable, image->ncolors, image->cpp);

    if (ErrorStatus != XpmSuccess)
	RETURN(ErrorStatus);

    /*
     * now we know the exact size we need, realloc the data
     * 4 = 1 (for '"') + 3 (for '",\n')
     * 1 = - 2 (because the last line does not end with ',\n') + 3 (for '};\n')
     */
     if(image->width  > UINT_MAX / image->cpp ||
       (tmp = image->width * image->cpp + 4) <= 4 ||
        image->height > UINT_MAX / tmp ||
       (tmp = image->height * tmp + 1) <= 1 ||
       (ptr_size += tmp) <= tmp)
	RETURN(XpmNoMemory);

    p = (char *) XpmRealloc(ptr, ptr_size);
    if (!p)
	RETURN(XpmNoMemory);
    ptr = p;

    /* print pixels */
    if (cmts && info->pixels_cmt) {
#ifndef VOID_SPRINTF
	used_size +=
#endif
	snprintf(ptr + used_size, ptr_size-used_size, "/*%s*/\n", info->pixels_cmt);
#ifdef VOID_SPRINTF
	used_size += strlen(info->pixels_cmt) + 5;
#endif
    }
    if (!WritePixels(ptr + used_size, ptr_size - used_size, &used_size,
		     image->width, image->height, image->cpp, image->data,
		     image->colorTable, image->ncolors))
	RETURN(XpmFileInvalid);

    /* print extensions */
    if (extensions &&
	!WriteExtensions(ptr + used_size, ptr_size-used_size, &used_size,
			 info->extensions, info->nextensions))
	RETURN(XpmNoMemory);

    /* close the array */
    strcpy(ptr + used_size, "};\n");

    *buffer_return = ptr;

    return (XpmSuccess);

/* exit point in case of error, free only locally allocated variables */
error:
    if (ptr)
	XpmFree(ptr);
    return (ErrorStatus);
}


static int
WriteColors(
    char		**dataptr,
    unsigned int	 *data_size,
    unsigned int	 *used_size,
    XpmColor		 *colors,
    unsigned int	  ncolors,
    unsigned int	  cpp)
{
    char buf[BUFSIZ] = {0};
    unsigned int a, key, l;
    char *s, *s2;
    char **defaults;

    *buf = '"';
    for (a = 0; a < ncolors; a++, colors++) {

	defaults = (char **) colors;
	s = buf + 1;
	if(cpp > (sizeof(buf) - (s-buf)))
		return(XpmNoMemory);
	strncpy(s, *defaults++, cpp);
	s += cpp;

	for (key = 1; key <= NKEYS; key++, defaults++) {
	    if ((s2 = *defaults)) {
		size_t used = (size_t)(s - buf);
		size_t available;
		int written;

		if (used >= sizeof(buf))
			return(XpmNoMemory);
		available = sizeof(buf) - used;
		/* Do not advance by snprintf's required length after truncation. */
		written = snprintf(s, available, "\t%s %s",
				   xpmColorKeys[key - 1], s2);
		if (written < 0 || (size_t)written >= available)
			return(XpmNoMemory);
		s += written;
	    }
	}
	if(sizeof(buf) - (s-buf) < 4)
		return(XpmNoMemory);
	strcpy(s, "\",\n");
	l = s + 3 - buf;
	if( *data_size                   >= UINT_MAX-l ||
	    *data_size + l               <= *used_size ||
	   (*data_size + l - *used_size) <= sizeof(buf))
		return(XpmNoMemory);
	s = (char *) XpmRealloc(*dataptr, *data_size + l);
	if (!s)
	    return (XpmNoMemory);
	*data_size += l;
	strcpy(s + *used_size, buf);
	*used_size += l;
	*dataptr = s;
    }
    return (XpmSuccess);
}

static int
WritePixels(
    char		*dataptr,
    unsigned int	 data_size,
    unsigned int	*used_size,
    unsigned int	 width,
    unsigned int	 height,
    unsigned int	 cpp,
    unsigned int	*pixels,
    XpmColor		*colors,
    unsigned int	 ncolors)
{
    char *s = dataptr;
    unsigned int x, y, h;

    if (height <= 1)
	return 1;
    if (dataptr == NULL || used_size == NULL || pixels == NULL ||
	    colors == NULL || ncolors == 0)
	return 0;

    h = height - 1;
    for (y = 0; y < h; y++) {
	*s++ = '"';
	for (x = 0; x < width; x++, pixels++) {
	    if ((size_t)(s - dataptr) > data_size ||
		cpp >= data_size - (size_t)(s - dataptr) || *pixels >= ncolors)
		return 0;
	    strncpy(s, colors[*pixels].string, cpp);
	    s += cpp;
	}
	if ((size_t)(s - dataptr) > data_size ||
	    data_size - (size_t)(s - dataptr) < 4)
		return 0;
	strcpy(s, "\",\n");
	s += 3;
    }
    /* duplicate some code to avoid a test in the loop */
    *s++ = '"';
    for (x = 0; x < width; x++, pixels++) {
	if ((size_t)(s - dataptr) > data_size ||
	    cpp >= data_size - (size_t)(s - dataptr) || *pixels >= ncolors)
	    return 0;
	strncpy(s, colors[*pixels].string, cpp);
	s += cpp;
    }
    *s++ = '"';
    *used_size += (unsigned int)(s - dataptr);
    return 1;
}

static unsigned int
ExtensionsSize(
    XpmExtension	*ext,
    unsigned int	 num)
{
    unsigned int x, y, a, size;
    size_t line_length;
    char **line;

    size = 0;
	if (num == 0) {
	    return(0); /* ok? */
	}
	if (ext == NULL)
	    return 0;
    for (x = 0; x < num; x++, ext++) {
	/* 11 = 10 (for ',\n"XPMEXT ') + 1 (for '"') */
	if (ext->name == NULL)
	    return 0;
	line_length = strlen(ext->name);
	if (size > UINT_MAX - 11 || line_length > UINT_MAX - size - 11)
	    return 0;
	size += (unsigned int)line_length + 11;
	a = ext->nlines; /* how can we trust ext->nlines to be not out-of-bounds? */
	if (a != 0 && ext->lines == NULL)
	    return 0;
	for (y = 0, line = ext->lines; y < a; y++, line++)
	    /* 4 = 3 (for ',\n"') + 1 (for '"') */
	    {
		if (*line == NULL)
		    return 0;
		line_length = strlen(*line);
		if (size > UINT_MAX - 4 || line_length > UINT_MAX - size - 4)
		    return 0;
		size += (unsigned int)line_length + 4;
	    }
    }
    /* 13 is for ',\n"XPMENDEXT"' */
    if(size > UINT_MAX - 13) /* unlikely */
    	return(0);
    return size + 13;
}

static int
WriteExtensions(
    char		*dataptr,
    unsigned int	 data_size,
    unsigned int	*used_size,
    XpmExtension	*ext,
    unsigned int	 num)
{
    unsigned int x, y, a;
    char **line;
    char *s = dataptr;
    size_t used;
    size_t available;
    int written;

    if (dataptr == NULL || used_size == NULL || ext == NULL ||
	*used_size > data_size)
	return 0;

    for (x = 0; x < num; x++, ext++) {
	if (ext->name == NULL)
	    return 0;
	used = (size_t)(s - dataptr);
	if (used >= data_size)
	    return 0;
	available = (size_t)data_size - used;
	written = snprintf(s, available, ",\n\"XPMEXT %s\"", ext->name);
	if (written < 0 || (size_t)written >= available)
	    return 0;
	s += written;
	a = ext->nlines;
	if (a != 0 && ext->lines == NULL)
	    return 0;
	for (y = 0, line = ext->lines; y < a; y++, line++) {
	    if (*line == NULL)
		return 0;
	    used = (size_t)(s - dataptr);
	    if (used >= data_size)
		return 0;
	    available = (size_t)data_size - used;
	    written = snprintf(s, available, ",\n\"%s\"", *line);
	    if (written < 0 || (size_t)written >= available)
		return 0;
	    s += written;
	}
    }
    used = (size_t)(s - dataptr);
    if (used > (size_t)data_size ||
	(size_t)data_size - used < sizeof(",\n\"XPMENDEXT\""))
	return 0;
    memcpy(s, ",\n\"XPMENDEXT\"", sizeof(",\n\"XPMENDEXT\""));
    *used_size = (unsigned int)(used + sizeof(",\n\"XPMENDEXT\"") - 1);
    return 1;
}

static int
CommentsSize(XpmInfo *info)
{
    size_t size = 0;
    size_t comment_length;

    /* 5 = 2 (for "/_*") + 3 (for "*_/\n") */
    if (info == NULL)
	return -1;
    if (info->hints_cmt) {
	comment_length = strlen(info->hints_cmt);
	if (size > UINT_MAX - 5 || comment_length > UINT_MAX - size - 5)
	    return -1;
	size += comment_length + 5;
    }

    if (info->colors_cmt) {
	comment_length = strlen(info->colors_cmt);
	if (size > UINT_MAX - 5 || comment_length > UINT_MAX - size - 5)
	    return -1;
	size += comment_length + 5;
    }

    if (info->pixels_cmt) {
	comment_length = strlen(info->pixels_cmt);
	if (size > UINT_MAX - 5 || comment_length > UINT_MAX - size - 5)
	    return -1;
	size += comment_length + 5;
    }

    if (size > INT_MAX)
	return -1;
    return (int)size;
}
