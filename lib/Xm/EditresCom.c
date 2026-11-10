/* $XConsortium: EditresCom.c /main/34 1995/12/11 00:19:42 gildea $ */

/*

Copyright (c) 1989  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.

*/
/* $XFree86: xc/lib/Xmu/EditresCom.c,v 1.3.2.4 1999/07/23 13:22:15 hohndel Exp $ */

/*
 * Author:  Chris D. Peterson, Dave Sternlicht, MIT X Consortium
 */

/*
    Replacement EditresCom.c for the one in Xmu to add the GetValues
    feature, and to modify the PositionInChild function for Motif. This
    modification is needed because Motif's VendorShell lies about the
    real position of it's child.
 */

/*
 * This file owns the whole Editres wire protocol: the request parser, the
 * reply writer and every command handler.  Nothing here calls the protocol
 * helpers that libXmu exports, because those trust a length field taken from
 * the request itself and read one byte past the data they were given.  A
 * request is parsed with a reader that is bound to the bytes actually
 * received, replies are built in a stream with a hard size limit, and a
 * widget named by a request is only ever found by searching the live widget
 * tree, never by turning a number from the wire into a pointer.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <X11/IntrinsicP.h>	/* To get into the composite and core widget
				   structures. */
#include <X11/ObjectP.h>	/* For XtIs<Classname> macros. */
#include <X11/StringDefs.h>	/* for XtRString. */
#define  XK_LATIN1
#include <X11/keysymdef.h>
#include <X11/ShellP.h>		/* for Application Shell Widget class. */

#include <X11/Xatom.h>
#include <X11/Xos.h>		/* for strcpy declaration */
#include <X11/Xmu/CharSet.h>

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "EditresComI.h"

/************************************************************
 *
 * Protocol constants.
 *
 ************************************************************/

#define EDITRES_NAME		"Editres"
#define EDITRES_COMMAND_ATOM	"EditresCommand"
#define EDITRES_COMM_ATOM	"EditresComm"
#define EDITRES_PROTOCOL_ATOM	"EditresProtocol"
#define EDITRES_CLIENT_VALUE	"EditresClientVal"

#define EDITRES_FORMAT			8
#define EDITRES_SEND_EVENT_FORMAT	32

#define EDITRES_IS_OBJECT		2
#define EDITRES_IS_UNREALIZED		0

#define HEADER_SIZE			6

#define CURRENT_PROTOCOL_VERSION	5L

/* The largest reply that will be built.  A reply that would grow beyond this
 * is replaced by a short failure message. */
#define EDITRES_MAX_REPLY		((size_t)32 * 1024 * 1024)

/* A reply buffer larger than this is released when the next command starts
 * instead of being kept for reuse. */
#define EDITRES_KEEP_REPLY		((size_t)1024 * 1024)

/* Widgets may keep the pointer to a String resource value they were given, so
 * the copies made for SetValues requests on such resources are never freed.
 * The total is therefore limited. */
#define EDITRES_MAX_RETAINED		((size_t)1024 * 1024)

/* Largest string, widget count or entry count a Card16 field can carry. */
#define EDITRES_MAX_CARD16		65535u

typedef enum {
    SendWidgetTree = 0,
    SetValues      = 1,
    GetResources   = 2,
    GetGeometry    = 3,
    FindChild      = 4,
    GetValues      = 5
} EditresCommand;

typedef enum {
    NormalResource     = 0,
    ConstraintResource = 1
} EditresResourceKind;

/* The type of a request identifier. */
typedef unsigned char ResIdent;

typedef enum {
    PartialSuccess   = 0,
    Failure	     = 1,
    ProtocolMismatch = 2
} EditResError;

/************************************************************
 *
 * Local structure definitions.
 *
 ************************************************************/

typedef enum { BlockNone, BlockSetValues, BlockAll } EditresBlock;

/*
 * A widget named by a request: the instance hierarchy from the root of the
 * tree down to the widget, each element the low 32 bits of the widget's
 * address.
 */
typedef struct _WidgetPath {
    unsigned short count;
    uint32_t * ids;
    Widget widget;		/* found by VerifyWidget. */
} WidgetPath;

/*
 * A parsed request.  Which fields are used depends on the command.
 */
typedef struct _EditresEvent {
    EditresCommand type;
    WidgetPath * paths;		/* SetValues, GetResources, GetGeometry,
				   FindChild, GetValues. */
    unsigned int num_paths;
    char * name;		/* SetValues, GetValues. */
    char * res_type;		/* SetValues. */
    char * value;		/* SetValues, NUL terminated. */
    unsigned short value_len;	/* SetValues. */
    short x, y;			/* FindChild. */
} EditresEvent;

/* The bytes of a request, from the current position to the end. */
typedef struct _WireReader {
    const unsigned char * cur;
    const unsigned char * end;
} WireReader;

/* A reply under construction.  HEADER_SIZE bytes in front of the payload are
 * reserved for the reply header. */
typedef struct _ReplyStream {
    unsigned char * buf;
    size_t size;		/* payload bytes written. */
    size_t alloc;		/* payload bytes allocated. */
    Boolean failed;		/* the reply no longer fits or memory ran out. */
} ReplyStream;

typedef struct _SVErrorInfo {
    const EditresEvent * event;
    ReplyStream * stream;
    unsigned int * count;
    const WidgetPath * entry;
} SVErrorInfo;

typedef struct _RetainedString {
    struct _RetainedString * next;
    size_t size;
    char text[1];
} RetainedString;

typedef struct _Globals {
    EditresBlock block;
    SVErrorInfo error_info;
    ReplyStream stream;
    Boolean reply_ready;
    RetainedString * retained;
    size_t retained_size;
} Globals;

typedef struct _EditresAtoms {
    Display * display;
    Atom editor;
    Atom comm;
    Atom command;
    Atom protocol;
    Atom client_value;
} EditresAtoms;

#define streq(a,b) (strcmp( (a), (b) ) == 0)

static EditresAtoms atoms;

static Globals globals;

static void SendFailure(Widget w, Atom sel, ResIdent ident, const char * str);
static void SendCommand(Widget w, Atom sel, ResIdent ident, EditResError reply_type);
static void ExecuteCommand(Widget w, Atom sel, ResIdent ident, EditresEvent * event);
static void FreeEvent(EditresEvent * event);
static void ExecuteSetValues(Widget w, const EditresEvent * sv_event, const WidgetPath * entry, ReplyStream * stream, unsigned int * count);
static void ExecuteGetGeometry(Widget w, ReplyStream * stream);
static void ExecuteGetResources(Widget w, ReplyStream * stream);
static void GetCommand(Widget w, XtPointer data, Atom *selection, Atom *type, XtPointer value, unsigned long *length, int * format);
static void LoadResources(Widget w);
static void DumpChildren(Widget w, ReplyStream * stream, unsigned int *count, unsigned int depth);
static const char * DumpWidgets(Widget w, const EditresEvent * event, ReplyStream * stream);
static const char * DoSetValues(Widget w, const EditresEvent * event, ReplyStream * stream);
static const char * DoFindChild(Widget w, const EditresEvent * event, ReplyStream * stream);
static const char * DoGetGeometry(Widget w, const EditresEvent * event, ReplyStream * stream);
static const char * DoGetResources(Widget w, const EditresEvent * event, ReplyStream * stream);
static const char * DumpValues(Widget w, const EditresEvent * event, ReplyStream * stream);

#ifndef HAVE_XMU_N_COPY_ISO
void _XmNCopyISOLatin1Lowered(char *dst, char *src, int size)
{
    register unsigned char *dest, *source;
    int bytes;

    if (size <= 0)
	return;

    for (dest = (unsigned char *)dst, source = (unsigned char *)src, bytes = 0;
	 *source && bytes < size - 1;
	 source++, dest++, bytes++)
    {
	if ((*source >= XK_A) && (*source <= XK_Z))
	    *dest = *source + (XK_a - XK_A);
	else if ((*source >= XK_Agrave) && (*source <= XK_Odiaeresis))
	    *dest = *source + (XK_agrave - XK_Agrave);
	else if ((*source >= XK_Ooblique) && (*source <= XK_Thorn))
	    *dest = *source + (XK_oslash - XK_Ooblique);
	else
	    *dest = *source;
    }
    *dest = '\0';
}
#endif

/************************************************************
 *
 * Reading a request.
 *
 * Every read is checked against the end of the bytes that were received.
 * Nothing the request says about its own size can move that end.
 *
 ************************************************************/

/*	Function Name: WireGet8
 *	Description: Retrieves an unsigned 8 bit value from the request.
 *	Arguments: reader - the request bytes.
 *                 val - a pointer to the value to return.
 *	Returns: TRUE if successful.
 */

static Boolean
WireGet8(WireReader * reader, unsigned char * val)
{
    if (reader->cur >= reader->end)
	return(FALSE);

    *val = *reader->cur++;
    return(TRUE);
}

/*	Function Name: WireGet16
 *	Description: Retrieves an unsigned 16 bit value, most significant
 *                   byte first.
 */

static Boolean
WireGet16(WireReader * reader, unsigned short * val)
{
    unsigned char hi, lo;

    if (!(WireGet8(reader, &hi) && WireGet8(reader, &lo)))
	return(FALSE);

    *val = (unsigned short)(((unsigned int)hi << 8) | (unsigned int)lo);
    return(TRUE);
}

/*	Function Name: WireGetSigned16
 *	Description: Retrieves a signed 16 bit value in two's complement.
 */

static Boolean
WireGetSigned16(WireReader * reader, short * val)
{
    unsigned short raw;

    if (!WireGet16(reader, &raw))
	return(FALSE);

    if (raw & 0x8000u)
	*val = (short)((int)raw - 0x10000);
    else
	*val = (short)raw;
    return(TRUE);
}

/*	Function Name: WireGet32
 *	Description: Retrieves an unsigned 32 bit value.
 */

static Boolean
WireGet32(WireReader * reader, uint32_t * val)
{
    unsigned short hi, lo;

    if (!(WireGet16(reader, &hi) && WireGet16(reader, &lo)))
	return(FALSE);

    *val = ((uint32_t)hi << 16) | (uint32_t)lo;
    return(TRUE);
}

/*	Function Name: WireGetBytes
 *	Description: Copies the next `count' bytes of the request.
 *	Returns: TRUE if that many bytes are left.
 */

static Boolean
WireGetBytes(WireReader * reader, void * dest, size_t count)
{
    if ((size_t)(reader->end - reader->cur) < count)
	return(FALSE);

    if (count != 0)
	memcpy(dest, reader->cur, count);
    reader->cur += count;
    return(TRUE);
}

/*	Function Name: WireGetString8
 *	Description: Retrieves a string made of a 16 bit length and that many
 *                   bytes.  The result is allocated with malloc and ends in
 *                   a NUL; a NUL inside the string ends it early.
 *	Returns: TRUE if retrieval was successful.
 */

static Boolean
WireGetString8(WireReader * reader, char ** str)
{
    unsigned short len;
    char * text;

    *str = NULL;
    if (!WireGet16(reader, &len))
	return(FALSE);

    if ((size_t)(reader->end - reader->cur) < (size_t)len)
	return(FALSE);

    text = (char *) malloc((size_t)len + 1);
    if (text == NULL)
	return(FALSE);

    memcpy(text, reader->cur, len);
    text[len] = '\0';
    reader->cur += len;
    *str = text;
    return(TRUE);
}

/*	Function Name: WireGetWidgetPath
 *	Description: Retrieves a widget path: a 16 bit count followed by that
 *                   many 32 bit widget identifiers.  A path names at least
 *                   the root of the tree, so an empty one is refused.
 *	Returns: TRUE if retrieval was successful.
 */

#define WIRE_MIN_PATH_BYTES	(2 + 4)

static Boolean
WireGetWidgetPath(WireReader * reader, WidgetPath * path)
{
    unsigned short count;
    unsigned int i;

    path->count = 0;
    path->ids = NULL;
    path->widget = NULL;

    if (!WireGet16(reader, &count) || count == 0)
	return(FALSE);

    if ((size_t)(reader->end - reader->cur) / 4 < (size_t)count)
	return(FALSE);

    path->ids = (uint32_t *) malloc(sizeof(uint32_t) * (size_t)count);
    if (path->ids == NULL)
	return(FALSE);

    for (i = 0; i < count; i++) {
	if (!WireGet32(reader, path->ids + i)) {
	    free(path->ids);
	    path->ids = NULL;
	    return(FALSE);
	}
    }
    path->count = count;
    return(TRUE);
}

/*	Function Name: WireGetPathList
 *	Description: Allocates and reads `num_paths' widget paths.  The count
 *                   is checked against the bytes left before anything is
 *                   allocated, since every path takes at least
 *                   WIRE_MIN_PATH_BYTES.
 *	Returns: TRUE if all paths were read.  On failure the event owns
 *                   whatever was read so far and FreeEvent releases it.
 */

static Boolean
WireGetPathList(WireReader * reader, EditresEvent * event, unsigned int num_paths)
{
    unsigned int i;

    if ((size_t)(reader->end - reader->cur) / WIRE_MIN_PATH_BYTES <
	(size_t)num_paths)
	return(FALSE);

    if (num_paths == 0)
	return(TRUE);

    event->paths = (WidgetPath *) calloc(num_paths, sizeof(WidgetPath));
    if (event->paths == NULL)
	return(FALSE);
    event->num_paths = num_paths;

    for (i = 0; i < num_paths; i++) {
	if (!WireGetWidgetPath(reader, event->paths + i))
	    return(FALSE);
    }
    return(TRUE);
}

/************************************************************
 *
 * Writing a reply.
 *
 ************************************************************/

/*	Function Name: ReplyReset
 *	Description: Empties the reply.  A buffer that grew large is released
 *                   instead of being kept.
 */

static void
ReplyReset(ReplyStream * stream)
{
    stream->size = 0;
    stream->failed = FALSE;
    if (stream->alloc > EDITRES_KEEP_REPLY) {
	free(stream->buf);
	stream->buf = NULL;
	stream->alloc = 0;
    }
}

/*	Function Name: BeginReply
 *	Description: Starts the reply of a new command.  The previous reply is
 *                   no longer offered while the new one is built.
 */

static void
BeginReply(void)
{
    ReplyReset(&globals.stream);
    globals.reply_ready = FALSE;
}

/*	Function Name: ReplyReserve
 *	Description: Makes room for `extra' more payload bytes.  Fails, and
 *                   marks the reply failed, when the reply would exceed
 *                   EDITRES_MAX_REPLY or memory is exhausted.
 *	Returns: TRUE if the bytes can be written.
 */

static Boolean
ReplyReserve(ReplyStream * stream, size_t extra)
{
    size_t need, new_alloc;
    unsigned char * new_buf;

    if (stream->failed)
	return(FALSE);

    if (extra > EDITRES_MAX_REPLY || stream->size > EDITRES_MAX_REPLY - extra) {
	stream->failed = TRUE;
	return(FALSE);
    }

    need = stream->size + extra;
    if (stream->buf != NULL && need <= stream->alloc)
	return(TRUE);

    new_alloc = (stream->alloc != 0) ? stream->alloc : 256;
    while (new_alloc < need)
	new_alloc *= 2;
    if (new_alloc > EDITRES_MAX_REPLY)
	new_alloc = EDITRES_MAX_REPLY;

    new_buf = (unsigned char *) realloc(stream->buf, HEADER_SIZE + new_alloc);
    if (new_buf == NULL) {
	stream->failed = TRUE;
	return(FALSE);
    }
    stream->buf = new_buf;
    stream->alloc = new_alloc;
    return(TRUE);
}

static void
ReplyPut8(ReplyStream * stream, unsigned int value)
{
    if (!ReplyReserve(stream, 1))
	return;
    stream->buf[HEADER_SIZE + stream->size++] = (unsigned char)(value & 0xFFu);
}

static void
ReplyPut16(ReplyStream * stream, unsigned int value)
{
    ReplyPut8(stream, (value >> 8) & 0xFFu);
    ReplyPut8(stream, value & 0xFFu);
}

static void
ReplyPut32(ReplyStream * stream, uint32_t value)
{
    ReplyPut16(stream, (unsigned int)((value >> 16) & 0xFFFFu));
    ReplyPut16(stream, (unsigned int)(value & 0xFFFFu));
}

static void
ReplyPutBool(ReplyStream * stream, Boolean value)
{
    ReplyPut8(stream, value ? 1u : 0u);
}

/*	Function Name: ReplyPutString8
 *	Description: Inserts a string.  The wire length is 16 bits, so a longer
 *                   string is cut and the length that is sent matches the
 *                   bytes that follow.
 */

static void
ReplyPutString8(ReplyStream * stream, const char * str)
{
    size_t len = (str != NULL) ? strlen(str) : 0;

    if (len > EDITRES_MAX_CARD16)
	len = EDITRES_MAX_CARD16;

    ReplyPut16(stream, (unsigned int)len);
    if (len != 0 && ReplyReserve(stream, len)) {
	memcpy(stream->buf + HEADER_SIZE + stream->size, str, len);
	stream->size += len;
    }
}

/*	Function Name: ReplyPutPath
 *	Description: Echoes a widget path as the request gave it.
 */

static void
ReplyPutPath(ReplyStream * stream, const WidgetPath * path)
{
    unsigned int i;

    ReplyPut16(stream, path->count);
    for (i = 0; i < path->count; i++)
	ReplyPut32(stream, path->ids[i]);
}

/*	Function Name: ReplyPatch16
 *	Description: Overwrites a 16 bit value already written, such as a
 *                   count that was not known when it was reserved.
 */

static void
ReplyPatch16(ReplyStream * stream, size_t offset, unsigned int value)
{
    if (stream->failed || stream->buf == NULL || stream->size < offset + 2)
	return;

    stream->buf[HEADER_SIZE + offset] = (unsigned char)((value >> 8) & 0xFFu);
    stream->buf[HEADER_SIZE + offset + 1] = (unsigned char)(value & 0xFFu);
}

/************************************************************
 *
 * Resource Editor Communication Code
 *
 ************************************************************/

/*	Function Name: InternAtoms
 *	Description: Looks up the protocol atoms of a display.  The result is
 *                   kept for the last display used.
 *	Returns: TRUE if the atoms are known for this display.
 */

static Boolean
InternAtoms(Display * dpy)
{
    static char* names[] = {
	EDITRES_NAME, EDITRES_COMM_ATOM, EDITRES_COMMAND_ATOM,
	EDITRES_PROTOCOL_ATOM, EDITRES_CLIENT_VALUE };
    Atom found[5];

    if (atoms.display == dpy)
	return(TRUE);

    if (XInternAtoms(dpy, names, 5, FALSE, found) == 0)
	return(FALSE);

    atoms.display = dpy;
    atoms.editor = found[0];
    atoms.comm = found[1];
    atoms.command = found[2];
    atoms.protocol = found[3];
    atoms.client_value = found[4];
    return(TRUE);
}

/*	Function Name: _XmEditResCheckMessages
 *	Description: This callback routine is set on all shell widgets,
 *                   and checks to see if a client message event
 *                   has come from the resource editor.
 *	Arguments: w - the shell widget.
 *                 data - *** UNUSED ***
 *                 event - The X Event that triggered this handler.
 *                 cont - *** UNUSED ***.
 *	Returns: none.
 *
 * Any client of the display can send this message, so the selection it names
 * is only used when it is the Editres communication selection; the message
 * must not make the application take over PRIMARY, CLIPBOARD or any other
 * selection.
 */

/* ARGSUSED */
void
_XmEditResCheckMessages(Widget w, XtPointer data, XEvent *event, Boolean *cont)
{
    Time time;
    ResIdent ident;
    static Boolean resources_loaded = FALSE;
    Atom res_comm;

    if (event->type == ClientMessage) {
	XClientMessageEvent * c_event = (XClientMessageEvent *) event;

	if (!InternAtoms(XtDisplay(w)))
	    return;

	if ((c_event->message_type != atoms.editor) ||
	    (c_event->format != EDITRES_SEND_EVENT_FORMAT))
	    return;

	if (!resources_loaded) {
	    resources_loaded = TRUE;
	    LoadResources(w);
	}

	time = (Time) c_event->data.l[0];
	res_comm = (Atom) c_event->data.l[1];
	ident = (ResIdent) c_event->data.l[2];

	if (res_comm != atoms.comm)
	    return;

	if (c_event->data.l[3] != CURRENT_PROTOCOL_VERSION) {
	    BeginReply();
	    ReplyPut8(&globals.stream, (unsigned int) CURRENT_PROTOCOL_VERSION);
	    SendCommand(w, res_comm, ident, ProtocolMismatch);
	    return;
	}

	XtGetSelectionValue(w, res_comm, atoms.command,
			    GetCommand, (XtPointer)(uintptr_t) ident, time);
    }
}

/*	Function Name: BuildEvent
 *	Description: Takes the info out of the request and constructs
 *                   the proper event structure.
 *	Arguments: w - widget to own selection, in case of error.
 *                 sel - selection to send error message back in.
 *                 data - the data for the request.
 *                 ident - the id number we are looking for.
 *                 length - length of request.
 *	Returns: the event, or NULL.
 *
 * The request carries its own length.  It is only accepted when it fits in
 * the bytes that were received.
 */

#define ERROR_MESSAGE ("Client: Improperly formatted protocol request")

static EditresEvent *
BuildEvent(Widget w, Atom sel, const unsigned char * data, ResIdent ident, unsigned long length)
{
    EditresEvent * event;
    WireReader header, body;
    unsigned char temp, command;
    uint32_t size;

    if (data == NULL || length < HEADER_SIZE) {
	SendFailure(w, sel, ident, ERROR_MESSAGE);
	return(NULL);
    }

    /*
     * Retrieve the Header.
     */

    header.cur = data;
    header.end = data + HEADER_SIZE;

    (void) WireGet8(&header, &temp);
    if (temp != ident)		/* Id's don't match, ignore request. */
	return(NULL);

    (void) WireGet8(&header, &command);
    (void) WireGet32(&header, &size);

    if ((unsigned long)size > length - HEADER_SIZE) {
	SendFailure(w, sel, ident, ERROR_MESSAGE);
	return(NULL);
    }

    body.cur = data + HEADER_SIZE;
    body.end = body.cur + size;

    event = (EditresEvent *) calloc(1, sizeof(EditresEvent));
    if (event == NULL) {
	SendFailure(w, sel, ident, ERROR_MESSAGE);
	return(NULL);
    }
    event->type = (EditresCommand) command;

    /*
     * Now retrieve the data segment.
     */

    switch(command) {
    case SendWidgetTree:
	break;			/* no additional info */
    case SetValues:
	{
	    unsigned short num_entries;

	    if ( !(WireGetString8(&body, &(event->name)) &&
		   WireGetString8(&body, &(event->res_type))))
	    {
		goto done;
	    }

	    /*
	     * The value is a string whose end we do not rely on.
	     */

	    if (!WireGet16(&body, &(event->value_len)))
		goto done;

	    event->value = (char *) malloc((size_t)event->value_len + 1);
	    if (event->value == NULL)
		goto done;
	    if (!WireGetBytes(&body, event->value, event->value_len))
		goto done;
	    event->value[event->value_len] = '\0';
	    event->value_len = (unsigned short) strlen(event->value);

	    if (!WireGet16(&body, &num_entries))
		goto done;
	    if (!WireGetPathList(&body, event, num_entries))
		goto done;
	}
	break;
    case FindChild:
	{
	    event->paths = (WidgetPath *) calloc(1, sizeof(WidgetPath));
	    if (event->paths == NULL)
		goto done;
	    event->num_paths = 1;

	    if (!(WireGetWidgetPath(&body, event->paths) &&
		  WireGetSigned16(&body, &(event->x)) &&
		  WireGetSigned16(&body, &(event->y))))
	    {
		goto done;
	    }
	}
	break;
    case GetGeometry:
    case GetResources:
	{
	    unsigned short num_entries;

	    if (!WireGet16(&body, &num_entries))
		goto done;
	    if (!WireGetPathList(&body, event, num_entries))
		goto done;
	}
	break;

    case GetValues:
	{
	    unsigned short num_entries;

	    if (!WireGetString8(&body, &(event->name)) ||
		!WireGet16(&body, &num_entries) ||
		num_entries == 0)
		goto done;

	    /*
	     * Only the first widget is looked at, so only its path is read.
	     */

	    event->paths = (WidgetPath *) calloc(1, sizeof(WidgetPath));
	    if (event->paths == NULL)
		goto done;
	    event->num_paths = 1;
	    if (!WireGetWidgetPath(&body, event->paths))
		goto done;
	}
	break;

    default:
	{
	    char buf[64];

	    snprintf(buf, sizeof(buf), "Unknown Protocol request %d.",
		     (int) command);
	    FreeEvent(event);
	    SendFailure(w, sel, ident, buf);
	    return(NULL);
	}
    }
    return(event);

 done:

    FreeEvent(event);
    SendFailure(w, sel, ident, ERROR_MESSAGE);
    return(NULL);
}

/*	Function Name: FreeEvent
 *	Description: Frees the event structure and any other pieces
 *                   in it that need freeing.
 *	Arguments: event - the event to free.
 *	Returns: none.
 */

static void
FreeEvent(EditresEvent * event)
{
    unsigned int i;

    if (event->paths != NULL) {
	for (i = 0; i < event->num_paths; i++)
	    free(event->paths[i].ids);
	free(event->paths);
    }
    free(event->name);
    free(event->res_type);
    free(event->value);
    free(event);
}

/*	Function Name: GetCommand
 *	Description: Gets the Command out of the selection asserted by the
 *                   resource manager.
 *	Arguments: (See Xt XtSelectionCallbackProc)
 *                 data - contains the ident number for the command.
 *	Returns: none.
 */

/* ARGSUSED */
static void
GetCommand(Widget w, XtPointer data, Atom *selection, Atom *type, XtPointer value, unsigned long *length, int * format)
{
    ResIdent ident = (ResIdent)(uintptr_t) data;
    EditresEvent * event;

    /*
     * The toolkit hands the selection value over to this procedure: it is
     * released here on every path, once the request has been parsed.
     */

    if (value == NULL)
	return;

    if (!InternAtoms(XtDisplay(w)) ||
	(*type != atoms.protocol) || (*format != EDITRES_FORMAT)) {
	XtFree((char *) value);
	return;
    }

    if ((event = BuildEvent(w, *selection, (const unsigned char *) value,
			    ident, *length)) != NULL) {
	XtFree((char *) value);
	value = NULL;
	ExecuteCommand(w, *selection, ident, event);
	FreeEvent(event);
    }
    else
	XtFree((char *) value);
}

/*	Function Name: ExecuteCommand
 *	Description: Executes a command received from the resource editor.
 *	Arguments: w       - a widget.
 *                 sel     - the selection to answer in.
 *                 ident   - the request identifier.
 *                 event   - the command to execute.
 *	Returns: none.
 */

/* ARGSUSED */
static void
ExecuteCommand(Widget w, Atom sel, ResIdent ident, EditresEvent * event)
{
    const char * (*func)(Widget, const EditresEvent *, ReplyStream *);
    const char * str;

    if (globals.block == BlockAll) {
	SendFailure(w, sel, ident,
		    "This client has blocked all Editres commands.");
	return;
    }
    else if ((globals.block == BlockSetValues) &&
	(event->type == SetValues)) {
	SendFailure(w, sel, ident,
		    "This client has blocked all SetValues requests.");
	return;
    }

    switch(event->type) {
    case SendWidgetTree:
	func = DumpWidgets;
	break;
    case SetValues:
	func = DoSetValues;
	break;
    case FindChild:
	func = DoFindChild;
	break;
    case GetGeometry:
	func = DoGetGeometry;
	break;
    case GetResources:
	func = DoGetResources;
	break;
    case GetValues:
	func = DumpValues;
	break;
    default:
	{
	    char buf[64];
	    snprintf(buf, sizeof(buf), "Unknown Protocol request %d.",
		     (int) event->type);
	    SendFailure(w, sel, ident, buf);
	    return;
	}
    }

    BeginReply();
    if ((str = (*func)(w, event, &globals.stream)) == NULL)
	SendCommand(w, sel, ident, PartialSuccess);
    else
	SendFailure(w, sel, ident, str);
}

/*	Function Name: ConvertReturnCommand
 *	Description: Converts a selection.
 *	Arguments: w - the widget that owns the selection.
 *                 selection - selection to convert.
 *                 target - target type for this selection.
 *                 type_ret - type of the selection.
 *                 value_ret - selection value;
 *                 length_ret - length of this selection.
 *                 format_ret - the format the selection is in.
 *	Returns: True if conversion was successful.
 *
 * Every conversion hands the toolkit its own copy of the reply, which the
 * toolkit frees.  A later command that rebuilds the reply stream therefore
 * cannot change or free a value that is still being transferred.
 */

/* ARGSUSED */
static Boolean
ConvertReturnCommand(Widget w, Atom * selection, Atom * target, Atom * type_ret, XtPointer *value_ret, unsigned long * length_ret, int * format_ret)
{
    size_t total;
    char * copy;

    if (!InternAtoms(XtDisplay(w)))
	return(FALSE);

    if ((*target != atoms.client_value) || !globals.reply_ready ||
	globals.stream.buf == NULL || globals.stream.failed)
	return(FALSE);

    total = HEADER_SIZE + globals.stream.size;
    copy = XtMalloc((Cardinal) total);
    memcpy(copy, globals.stream.buf, total);

    *type_ret = atoms.protocol;
    *value_ret = (XtPointer) copy;
    *length_ret = (unsigned long) total;
    *format_ret = EDITRES_FORMAT;

    return(TRUE);
}

/*	Function Name: SendFailure
 *	Description: Sends a failure message.
 *	Arguments: w - the widget to own the selection.
 *                 sel - the selection to assert.
 *	 	   ident - the identifier.
 *                 str - the error message.
 *	Returns: none.
 */

static void
SendFailure(Widget w, Atom sel, ResIdent ident, const char * str)
{
    BeginReply();
    ReplyPutString8(&globals.stream, str);
    SendCommand(w, sel, ident, Failure);
}

/*	Function Name: SendCommand
 *	Description: Completes the reply held in the global stream with its
 *                   header and offers it as the selection value.
 *	Arguments: w - the widget to own the selection.
 *                 sel - the selection to assert.
 *	 	   ident - the identifier.
 *                 reply_type - how the command went.
 *	Returns: none.
 */

static void
SendCommand(Widget w, Atom sel, ResIdent ident, EditResError reply_type)
{
    ReplyStream * stream = &globals.stream;

    if (stream->failed) {
	/* The reply grew past its limit or memory ran out. */
	ReplyReset(stream);
	ReplyPutString8(stream, "The reply is too large to send.");
	reply_type = Failure;
    }
    if (stream->failed || stream->buf == NULL)
	return;

    /*
     * The first HEADER_SIZE bytes of the buffer are reserved for the header:
     * the identifier, the command and the 32 bit payload length.
     */

    stream->buf[0] = ident;
    stream->buf[1] = (unsigned char) reply_type;
    stream->buf[2] = (unsigned char)((stream->size >> 24) & 0xFFu);
    stream->buf[3] = (unsigned char)((stream->size >> 16) & 0xFFu);
    stream->buf[4] = (unsigned char)((stream->size >> 8) & 0xFFu);
    stream->buf[5] = (unsigned char)(stream->size & 0xFFu);
    globals.reply_ready = TRUE;

/*
 * I REALLY want to own the selection.  Since this was not triggered
 * by a user action, and I am the only one using this atom it is safe to
 * use CurrentTime.
 */

    XtOwnSelection(w, sel, CurrentTime,
		   ConvertReturnCommand, NULL, NULL);
}

/************************************************************
 *
 * Generic Utility Functions.
 *
 ************************************************************/

/*	Function Name: WidgetWireId
 *	Description: The identifier a widget goes by on the wire: the low 32
 *                   bits of its address.
 */

static uint32_t
WidgetWireId(Widget w)
{
    return((uint32_t)((uintptr_t)w & 0xFFFFFFFFu));
}

/*	Function Name: ChildCount
 *	Description: Counts the children (popup, normal and otherwise)
 *                   of this widget.
 *	Arguments: parent - the parent widget.
 *                 normal - count normal children.
 *                 popup - count popup children.
 */

static unsigned int
ChildCount(Widget parent, Boolean normal, Boolean popup)
{
    unsigned long total = 0;

    if (XtIsComposite(parent) && normal) {
	CompositeWidget cw = (CompositeWidget) parent;
	total += cw->composite.num_children;
    }

    if (XtIsWidget(parent) && popup)
	total += parent->core.num_popups;

    return(total > UINT_MAX ? UINT_MAX : (unsigned int) total);
}

/*	Function Name: ChildAt
 *	Description: Returns one child of a widget.  Normal children come
 *                   first, then popup children.
 *	Arguments: parent, normal, popup - as for ChildCount.
 *                 index - the position, below ChildCount().
 */

static Widget
ChildAt(Widget parent, Boolean normal, Boolean popup, unsigned int index)
{
    if (XtIsComposite(parent) && normal) {
	CompositeWidget cw = (CompositeWidget) parent;

	if (index < cw->composite.num_children)
	    return(cw->composite.children[index]);
	index -= cw->composite.num_children;
    }

    if (XtIsWidget(parent) && popup && index < parent->core.num_popups)
	return(parent->core.popup_list[index]);

    return(NULL);
}

/*	Function Name: FindMember
 *	Description: Finds, among the children (popup, normal and otherwise)
 *                   of a widget, the one with a wire identifier.
 *	Returns: the child, or NULL.
 */

static Widget
FindMember(Widget parent, uint32_t id)
{
    unsigned int i, count = ChildCount(parent, TRUE, TRUE);
    Widget child;

    for (i = 0; i < count; i++) {
	child = ChildAt(parent, TRUE, TRUE, i);
	if (child != NULL && WidgetWireId(child) == id)
	    return(child);
    }
    return(NULL);
}

/*	Function Name: VerifyWidget
 *	Description: Finds the widget a path names.  The path is followed
 *                   from the root of the tree down through the live
 *                   hierarchy, so a widget that no longer exists, or a
 *                   number that is not a widget at all, is refused without
 *                   being dereferenced.
 *	Arguments: w - any widget in the tree.
 *                 path - the path to find; path->widget is set.
 *	Returns: an error message or NULL.
 */

#define WIDGET_GONE ("This widget no longer exists in the client.")

static const char *
VerifyWidget(Widget w, WidgetPath * path)
{
    Widget top, found;
    unsigned int i;

    path->widget = NULL;
    if (path->count == 0 || path->ids == NULL)
	return(WIDGET_GONE);

    for (top = w; XtParent(top) != NULL; top = XtParent(top)) {}

    if (WidgetWireId(top) != path->ids[0])
	return(WIDGET_GONE);
    found = top;

    for (i = 1; i < path->count; i++) {
	found = FindMember(found, path->ids[i]);
	if (found == NULL)
	    return(WIDGET_GONE);
    }

    path->widget = found;
    return(NULL);
}

/*	Function Name: ReplyPutWidget
 *	Description: Inserts the full parent hierarchy of this widget into the
 *                   reply as a widget path.
 *	Arguments: stream - the reply.
 *                 w - the widget to insert.
 */

static void
ReplyPutWidget(ReplyStream * stream, Widget w)
{
    Widget temp;
    uint32_t stack_ids[64];
    uint32_t * ids = stack_ids;
    size_t num_widgets = 0, i;

    for (temp = w; temp != NULL; temp = XtParent(temp))
	num_widgets++;

    if (num_widgets == 0 || num_widgets > EDITRES_MAX_CARD16) {
	stream->failed = TRUE;
	return;
    }

    if (num_widgets > sizeof(stack_ids) / sizeof(stack_ids[0])) {
	ids = (uint32_t *) malloc(sizeof(uint32_t) * num_widgets);
	if (ids == NULL) {
	    stream->failed = TRUE;
	    return;
	}
    }

    /*
     * Put the widgets into the list.
     * make sure that they are inserted in the list from parent -> child.
     */

    for (i = num_widgets, temp = w; temp != NULL; temp = XtParent(temp))
	ids[--i] = WidgetWireId(temp);

    ReplyPut16(stream, (unsigned int) num_widgets);
    for (i = 0; i < num_widgets; i++)
	ReplyPut32(stream, ids[i]);

    if (ids != stack_ids)
	free(ids);
}

/************************************************************
 *
 * Code to Perform SetValues operations.
 *
 ************************************************************/

/*	Function Name: 	DoSetValues
 *	Description: performs the setvalues requested.
 *	Arguments: w - a widget in the tree.
 *                 event - the event that caused this action.
 *                 stream - the reply to add to.
 *	Returns: NULL, or an error message.
 */

static const char *
DoSetValues(Widget w, const EditresEvent * event, ReplyStream * stream)
{
    const char * str;
    unsigned int i;
    unsigned int count = 0;

    /*
     * Only a value that is a string can be carried by the protocol: it has
     * no way to send the bytes of any other type.
     */

    if (!streq(event->res_type, XtRString))
	return("Only values of type String can be set.");

    ReplyPut16(stream, 0);	/* insert 0, will be overwritten later. */

    for (i = 0 ; i < event->num_paths; i++) {
	if ((str = VerifyWidget(w, &(event->paths[i]))) != NULL) {
	    if (count < EDITRES_MAX_CARD16) {
		ReplyPutPath(stream, &(event->paths[i]));
		ReplyPutString8(stream, str);
		count++;
	    }
	}
	else
	    ExecuteSetValues(event->paths[i].widget, event,
			     event->paths + i, stream, &count);
    }

    /*
     * Overwrite the first 2 bytes with the real count.
     */

    ReplyPatch16(stream, 0, count);
    return(NULL);
}

/*	Function Name: ExpandToolkitMessage
 *	Description: Expands the %s conversions of an Xt default message with
 *                   the parameters Xt supplied.  Nothing else of the message
 *                   is interpreted, so a message that asks for more
 *                   parameters than there are, or for any other conversion,
 *                   cannot read or write outside the parameters.
 *	Returns: a malloc'ed string, or NULL.
 */

static char *
ExpandToolkitMessage(const char * msg, String * params, Cardinal num_params)
{
    size_t len = 0, pos = 0;
    Cardinal next;
    const char * p;
    char * result;
    int pass;

    if (msg == NULL)
	return(NULL);

    result = NULL;
    for (pass = 0; pass < 2; pass++) {
	next = 0;
	pos = 0;
	for (p = msg; *p != '\0'; p++) {
	    const char * piece = p;
	    size_t piece_len = 1;

	    if (p[0] == '%' && p[1] == 's') {
		piece = (params != NULL && next < num_params &&
			 params[next] != NULL) ? params[next] : "";
		next++;
		piece_len = strlen(piece);
		p++;
	    }
	    else if (p[0] == '%' && p[1] == '%')
		p++;

	    if (pass == 1)
		memcpy(result + pos, piece, piece_len);
	    pos += piece_len;
	}
	if (pass == 0) {
	    len = pos;
	    result = (char *) malloc(len + 1);
	    if (result == NULL)
		return(NULL);
	}
    }
    result[len] = '\0';
    return(result);
}

/*	Function Name: FormatMessage
 *	Description: printf into a malloc'ed string.
 *	Returns: the string, or NULL.
 */

static char *
FormatMessage(const char * format, ...)
{
    va_list ap;
    int needed;
    char * result;

    va_start(ap, format);
    needed = vsnprintf(NULL, 0, format, ap);
    va_end(ap);
    if (needed < 0)
	return(NULL);

    result = (char *) malloc((size_t)needed + 1);
    if (result == NULL)
	return(NULL);

    va_start(ap, format);
    (void) vsnprintf(result, (size_t)needed + 1, format, ap);
    va_end(ap);
    return(result);
}

/*	Function Name: HandleToolkitErrors
 *	Description: Handles X Toolkit Errors.
 *	Arguments: name - name of the error.
 *                 type - type of the error.
 *                 class - class of the error.
 *                 msg - the default message.
 *                 params, num_params - the extra parameters for this message.
 *	Returns: none.
 */

/* ARGSUSED */
static void
HandleToolkitErrors(String name, String type, String class, String msg, String *params, Cardinal * num_params)
{
    SVErrorInfo * info = &globals.error_info;
    char * text = NULL;
    const char * value;

    if (info->event == NULL || info->stream == NULL)
	return;

    if (name == NULL) name = "";
    if (type == NULL) type = "";
    if (class == NULL) class = "";
    if (msg == NULL) msg = "";
    value = (info->event->value != NULL) ? info->event->value : "";

    if ( streq(name, "unknownType") ) {
	text = FormatMessage("The `%s' resource is not used by this widget.",
			     info->event->name);
    } else if ( streq(name, "noColormap") ) {
	text = ExpandToolkitMessage(msg, params,
				    (num_params != NULL) ? *num_params : 0);
    }
    else if (streq(name, "conversionFailed") || streq(name, "conversionError"))
    {
	text = FormatMessage("Could not convert the string '%s' for the `%s' resource.",
			     value, info->event->name);
    } else {
	text = FormatMessage("Name: %s, Type: %s, Class: %s, Msg: %s",
			     name, type, class, msg);
    }

    /*
     * Insert this info into the reply, and update the count.
     */

    if (*(info->count) < EDITRES_MAX_CARD16) {
	(*(info->count))++;
	ReplyPutPath(info->stream, info->entry);
	ReplyPutString8(info->stream, (text != NULL) ?
			text : "The message could not be formatted.");
    }
    free(text);
}

/*	Function Name: ResourceTypeOf
 *	Description: Looks up the type of a resource of a widget, among its
 *                   normal resources and the constraint resources its parent
 *                   adds.
 *	Returns: the type name, which belongs to the toolkit, or NULL.
 */

static const char *
ResourceTypeOf(Widget w, const char * name)
{
    XtResourceList list;
    Cardinal num, i;
    const char * found = NULL;

    XtGetResourceList(XtClass(w), &list, &num);
    for (i = 0; i < num && found == NULL; i++) {
	if (streq(list[i].resource_name, name))
	    found = list[i].resource_type;
    }
    XtFree((char *) list);

    if (found == NULL && XtParent(w) != NULL) {
	XtGetConstraintResourceList(XtClass(XtParent(w)), &list, &num);
	for (i = 0; i < num && found == NULL; i++) {
	    if (streq(list[i].resource_name, name))
		found = list[i].resource_type;
	}
	XtFree((char *) list);
    }
    return(found);
}

/*	Function Name: RetainString
 *	Description: Makes a copy of a string that is never freed.  A widget
 *                   given a String resource value may keep the pointer, so
 *                   a copy that stays valid is the only safe value to hand
 *                   it.  The total is limited so that a client cannot use
 *                   this to exhaust memory.
 *	Returns: the copy, or NULL when the limit is reached.
 */

static const char *
RetainString(const char * text)
{
    size_t size = strlen(text) + 1;
    RetainedString * entry;

    if (size > EDITRES_MAX_RETAINED - globals.retained_size)
	return(NULL);

    entry = (RetainedString *) malloc(sizeof(RetainedString) + size);
    if (entry == NULL)
	return(NULL);

    memcpy(entry->text, text, size);
    entry->size = size;
    entry->next = globals.retained;
    globals.retained = entry;
    globals.retained_size += size;
    return(entry->text);
}

/*	Function Name: ExecuteSetValues
 *	Description: Performs a setvalues for a given command.
 *	Arguments: w - the widget to perform the set_values on.
 *                 sv_event - the set values event.
 *                 entry - the widget path this is for.
 *                 stream, count - the reply and its entry count.
 *	Returns: none.
 */

static void
ExecuteSetValues(Widget w, const EditresEvent * sv_event, const WidgetPath * entry, ReplyStream * stream, unsigned int * count)
{
    XtErrorMsgHandler old;
    const char * type = ResourceTypeOf(w, sv_event->name);
    const char * value = sv_event->value;

    SVErrorInfo * info = &globals.error_info;
    info->event = sv_event;	/* No data can be passed to */
    info->stream = stream;	/* an error handler, so we */
    info->count = count;	/* have to use a global, YUCK... */
    info->entry = entry;

    /*
     * A resource of type String is set to the very pointer it is given, and
     * the widget may keep it.
     */

    if (type != NULL && streq(type, XtRString)) {
	value = RetainString(sv_event->value);
	if (value == NULL) {
	    if (*count < EDITRES_MAX_CARD16) {
		(*count)++;
		ReplyPutPath(stream, entry);
		ReplyPutString8(stream,
				"The limit for strings set on this client is reached.");
	    }
	    info->event = NULL;
	    info->stream = NULL;
	    return;
	}
    }

    old = XtAppSetWarningMsgHandler(XtWidgetToApplicationContext(w),
				    HandleToolkitErrors);

    XtVaSetValues(w, XtVaTypedArg,
		  sv_event->name, sv_event->res_type,
		  value, (int) sv_event->value_len + 1,
		  NULL);

    (void)XtAppSetWarningMsgHandler(XtWidgetToApplicationContext(w), old);
    info->event = NULL;
    info->stream = NULL;
}


/************************************************************
 *
 * Code for Creating and dumping widget tree.
 *
 ************************************************************/

/*	Function Name: 	DumpWidgets
 *	Description: Given a widget it builds a protocol packet
 *                   containing the entire widget heirarchy.
 *	Arguments: w - a widget in the tree.
 *                 event - the event that caused this action.
 *                 stream - the reply to add to.
 *	Returns: NULL
 */

#define TOOLKIT_TYPE ("Xt")

/* ARGSUSED */
static const char *
DumpWidgets(Widget w, const EditresEvent * event, ReplyStream * stream)
{
    unsigned int count = 0;

    /* Find Tree's root. */
    for ( ; XtParent(w) != NULL; w = XtParent(w)) {}

    /*
     * hold space for count, overwritten later.
     */

    ReplyPut16(stream, 0);

    DumpChildren(w, stream, &count, 0);

    /*
     * write out toolkit type (Xt, of course...).
     */

    ReplyPutString8(stream, TOOLKIT_TYPE);

    /*
     * Overwrite the first 2 bytes with the real count.
     */

    ReplyPatch16(stream, 0, count);
    return(NULL);
}

/*	Function Name: DumpChildren
 *	Description: Adds a widget and its descendants to the list.
 *	Arguments: w - the widget to dump.
 *                 stream - the stream to dump to.
 *                 count - number of dumps we have performed.
 *                 depth - how far below the root w is.
 *	Returns: none.
 */

/* This is a trick/kludge.  To make shared libraries happier (linking
 * against Xmu but not linking against Xt, and apparently even work
 * as we desire on SVR4, we need to avoid an explicit data reference
 * to applicationShellWidgetClass.  XtIsTopLevelShell is known
 * (implementation dependent assumption!) to use a bit flag.  So we
 * go that far.  Then, we test whether it is an applicationShellWidget
 * class by looking for an explicit class name.  Seems pretty safe.
 */
static Bool isApplicationShell(Widget w)
{
    register WidgetClass c;

    if (!XtIsTopLevelShell(w))
	return False;
    for (c = XtClass(w); c; c = c->core_class.superclass) {
	if (!strcmp(c->core_class.class_name, "ApplicationShell"))
	    return True;
    }
    return False;
}

static void
DumpChildren(Widget w, ReplyStream * stream, unsigned int *count, unsigned int depth)
{
    unsigned int i, num_children;
    unsigned long window;
    const char * class;

    /* The count and the length of a path are 16 bit fields. */
    if (stream->failed || *count >= EDITRES_MAX_CARD16 ||
	depth >= EDITRES_MAX_CARD16)
	return;

    (*count)++;

    ReplyPutWidget(stream, w);	/* Insert the widget into the stream. */

    ReplyPutString8(stream, XtName(w)); /* Insert name */

    if (isApplicationShell(w))
	class = ((ApplicationShellWidget) w)->application.class;
    else
	class = XtClass(w)->core_class.class_name;

    ReplyPutString8(stream, class); /* Insert class */

    if (XtIsWidget(w)) {
	if (XtIsRealized(w))
	    window = XtWindow(w);
	else
	    window = EDITRES_IS_UNREALIZED;
    }
    else
	window = EDITRES_IS_OBJECT;

    ReplyPut32(stream, (uint32_t)(window & 0xFFFFFFFFu)); /* Insert window id. */

    /*
     * Find children and recurse.
     */

    num_children = ChildCount(w, TRUE, TRUE);
    for (i = 0; i < num_children; i++) {
	Widget child = ChildAt(w, TRUE, TRUE, i);

	if (child != NULL)
	    DumpChildren(child, stream, count, depth + 1);
    }
}

/************************************************************
 *
 * Code for getting the geometry of widgets.
 *
 ************************************************************/

/*	Function Name: 	DoGetGeometry
 *	Description: retrieves the Geometry of each specified widget.
 *	Arguments: w - a widget in the tree.
 *                 event - the event that caused this action.
 *                 stream - the reply to add to.
 *	Returns: NULL
 */

static const char *
DoGetGeometry(Widget w, const EditresEvent * event, ReplyStream * stream)
{
    unsigned int i;
    const char * str;

    ReplyPut16(stream, event->num_paths);

    for (i = 0 ; i < event->num_paths; i++) {

	/*
	 * Send out the widget id.
	 */

	ReplyPutPath(stream, &(event->paths[i]));
	if ((str = VerifyWidget(w, &(event->paths[i]))) != NULL) {
	    ReplyPutBool(stream, True); /* an error occured. */
	    ReplyPutString8(stream, str);	/* set message. */
	}
	else
	    ExecuteGetGeometry(event->paths[i].widget, stream);
    }
    return(NULL);
}

/*	Function Name: ExecuteGetGeometry
 *	Description: Gets the geometry for each widget specified.
 *	Arguments: w - the widget to get geom on.
 *                 stream - stream to append to.
 *	Returns: none.
 */

static void
ExecuteGetGeometry(Widget w, ReplyStream * stream)
{
    int i;
    Boolean mapped_when_man = False;
    Dimension width = 0, height = 0, border_width = 0;
    Arg args[8];
    Cardinal num_args = 0;
    Position x = 0, y = 0;

    if ( !XtIsRectObj(w) || (XtIsWidget(w) && !XtIsRealized(w)) ) {
	ReplyPutBool(stream, False); /* no error. */
	ReplyPutBool(stream, False); /* not visable. */
	for (i = 0; i < 5; i++) /* fill in extra space with 0's. */
	    ReplyPut16(stream, 0);
	return;
    }

    XtSetArg(args[num_args], XtNwidth, &width); num_args++;
    XtSetArg(args[num_args], XtNheight, &height); num_args++;
    XtSetArg(args[num_args], XtNborderWidth, &border_width); num_args++;
    if (XtIsWidget(w)) {
	XtSetArg(args[num_args], XtNmappedWhenManaged, &mapped_when_man);
	num_args++;
    }
    XtGetValues(w, args, num_args);

    if (!(XtIsManaged(w) && mapped_when_man) && XtIsWidget(w)) {
	XWindowAttributes attrs;

	/*
	 * The toolkit does not maintain mapping state, we have
	 * to go to the server.
	 */

	if (XGetWindowAttributes(XtDisplay(w), XtWindow(w), &attrs) != 0) {
	    if (attrs.map_state != IsViewable) {
		ReplyPutBool(stream, False); /* no error. */
		ReplyPutBool(stream, False); /* not visable. */
		for (i = 0; i < 5; i++) /* fill in extra space with 0's. */
		    ReplyPut16(stream, 0);
		return;
	    }
	}
	else {
	    ReplyPutBool(stream, True); /* Error occured. */
	    ReplyPutString8(stream, "XGetWindowAttributes failed.");
	    return;
	}
    }

    XtTranslateCoords(w, -((int) border_width), -((int) border_width), &x, &y);

    ReplyPutBool(stream, False); /* no error. */
    ReplyPutBool(stream, True); /* Visable. */
    ReplyPut16(stream, (unsigned short) x);
    ReplyPut16(stream, (unsigned short) y);
    ReplyPut16(stream, width);
    ReplyPut16(stream, height);
    ReplyPut16(stream, border_width);
}

/************************************************************
 *
 * Code for executing FindChild.
 *
 ************************************************************/

/*	Function Name: PositionInChild
 *	Description: returns true if this location is in the child.
 *	Arguments: child - the child widget to check.
 *                 x, y - location of point to check in the parent's
 *                        coord space.
 *	Returns: TRUE if the position is in this child.
 */

static Boolean
PositionInChild(Widget child, int x, int y)
{
    Arg args[6];
    Cardinal num;
    Dimension width = 0, height = 0, border_width = 0;
    Position child_x = 0, child_y = 0;
    Boolean mapped_when_managed = False;

    if (!XtIsRectObj(child))	/* we must at least be a rect obj. */
	return(FALSE);

    num = 0;
    if (XtIsWidget(child)) {
	XtSetArg(args[num], XtNmappedWhenManaged, &mapped_when_managed); num++;
    }
    XtSetArg(args[num], XtNwidth, &width); num++;
    XtSetArg(args[num], XtNheight, &height); num++;
    XtSetArg(args[num], XtNx, &child_x); num++;
    XtSetArg(args[num], XtNy, &child_y); num++;
    XtSetArg(args[num], XtNborderWidth, &border_width); num++;
    XtGetValues(child, args, num);
    if (XtParent(child) != NULL && XtIsVendorShell(XtParent(child)))
    {
    	child_x = -((Position) border_width);
    	child_y = -((Position) border_width);
    }

    /*
     * The only way we will know of the widget is mapped is to see if
     * mapped when managed is True and this is a managed child.  Otherwise
     * we will have to ask the server if this window is mapped.
     */

    if (XtIsWidget(child) && !(mapped_when_managed && XtIsManaged(child)) ) {
	XWindowAttributes attrs;

	if (XtIsRealized(child))
	{
	    if (XGetWindowAttributes(XtDisplay(child),
				     XtWindow(child), &attrs) != 0) {
		if (attrs.map_state != IsViewable)
		    return(FALSE);
	    }
	}
    }

    return (x >= (int) child_x) &&
	   (x <= (int) child_x + (int) width + 2 * (int) border_width) &&
	   (y >= (int) child_y) &&
	   (y <= (int) child_y + (int) height + 2 * (int) border_width);
}

/*	Function Name: FindChildAt
 *	Description: Finds the child that actually contains the point shown.
 *	Arguments: parent - a widget that is known to contain the point
 *                 	    specified.
 *                 x, y - The point in coordinates relative to the
 *                        widget specified.
 *	Returns: the innermost widget containing the point.
 */

static Widget
FindChildAt(Widget parent, int x, int y)
{
    for (;;) {
	unsigned int i = ChildCount(parent, TRUE, FALSE);
	Widget found = NULL;

	while (i > 0) {
	    Widget child;

	    i--;
	    child = ChildAt(parent, TRUE, FALSE, i);
	    if (child != NULL && PositionInChild(child, x, y)) {
		found = child;
		break;
	    }
	}

	if (found == NULL)
	    return(parent);

	x -= found->core.x;
	y -= found->core.y;
	parent = found;
    }
}

/*	Function Name: DoFindChild
 *	Description: finds the child that contains the location specified.
 *	Arguments: w - a widget in the tree.
 *                 event - the event that caused this action.
 *                 stream - the reply to add to.
 *	Returns: an error message if the widget is gone, else NULL.
 */

static const char *
DoFindChild(Widget w, const EditresEvent * event, ReplyStream * stream)
{
    const char * str;
    Widget parent, child;
    Position parent_x, parent_y;

    if ((str = VerifyWidget(w, event->paths)) != NULL)
	return(str);

    parent = event->paths->widget;

    XtTranslateCoords(parent, (Position) 0, (Position) 0,
		      &parent_x, &parent_y);

    child = FindChildAt(parent, (int) event->x - (int) parent_x,
			(int) event->y - (int) parent_y);

    ReplyPutWidget(stream, child);
    return(NULL);
}

/************************************************************
 *
 * Procedures for performing GetResources.
 *
 ************************************************************/

/*	Function Name: DoGetResources
 *	Description: Gets the Resources associated with the widgets passed.
 *	Arguments: w - a widget in the tree.
 *                 event - the event that caused this action.
 *                 stream - the reply to add to.
 *	Returns: NULL
 */

static const char *
DoGetResources(Widget w, const EditresEvent * event, ReplyStream * stream)
{
    unsigned int i;
    const char * str;

    ReplyPut16(stream, event->num_paths); /* number of replys */

    for (i = 0 ; i < event->num_paths; i++) {
	/*
	 * Send out the widget id.
	 */
	ReplyPutPath(stream, &(event->paths[i]));
	if ((str = VerifyWidget(w, &(event->paths[i]))) != NULL) {
	    ReplyPutBool(stream, True); /* an error occured. */
	    ReplyPutString8(stream, str);	/* set message. */
	}
	else {
	    ReplyPutBool(stream, False); /* no error occured. */
	    ExecuteGetResources(event->paths[i].widget, stream);
	}
    }
    return(NULL);
}

/*	Function Name: ExecuteGetResources.
 *	Description: Gets the resources for any individual widget.
 *	Arguments: w - the widget to get resources on.
 *                 stream - the reply.
 *	Returns: none.
 */

static void
ExecuteGetResources(Widget w, ReplyStream * stream)
{
    XtResourceList norm_list = NULL, cons_list = NULL;
    Cardinal num_norm = 0, num_cons = 0;
    Cardinal i;

    /*
     * Get Normal Resources.
     */

    XtGetResourceList(XtClass(w), &norm_list, &num_norm);

    if (XtParent(w) != NULL)
	XtGetConstraintResourceList(XtClass(XtParent(w)),&cons_list,&num_cons);
    else
	num_cons = 0;

    /* The count is a 16 bit field: send no more resources than it can hold. */
    if (num_norm > EDITRES_MAX_CARD16)
	num_norm = EDITRES_MAX_CARD16;
    if (num_cons > EDITRES_MAX_CARD16 - num_norm)
	num_cons = EDITRES_MAX_CARD16 - num_norm;

    ReplyPut16(stream, num_norm + num_cons); /* how many resources. */

    /*
     * Insert all the normal resources.
     */

    for ( i = 0; i < num_norm; i++) {
	ReplyPut8(stream, NormalResource);
	ReplyPutString8(stream, norm_list[i].resource_name);
	ReplyPutString8(stream, norm_list[i].resource_class);
	ReplyPutString8(stream, norm_list[i].resource_type);
    }
    XtFree((char *) norm_list);

    /*
     * Insert all the constraint resources.
     */

    for ( i = 0; i < num_cons; i++) {
	ReplyPut8(stream, ConstraintResource);
	ReplyPutString8(stream, cons_list[i].resource_name);
	ReplyPutString8(stream, cons_list[i].resource_class);
	ReplyPutString8(stream, cons_list[i].resource_type);
    }
    XtFree((char *) cons_list);
}

static void
EditResCvtWarningHandler(String name,
				 String type,
				 String class,
				 String def,
				 String *params,
				 Cardinal *num_params)
{
    /* just ignore the warning */
    return;
}

/*	Function Name: GetResourceString
 *	Description: Reads one resource of a widget as a string.
 *	Arguments: w - the widget.
 *                 name - the resource name.
 *                 buffer, buffer_size - scratch space for a value that has
 *                                       to be formatted.
 *	Returns: the string, which is in the buffer, in the widget, in the
 *                 converter cache or a literal; NULL if the widget has no
 *                 such resource.
 */

static const char *
GetResourceString(Widget w, const char * name, char * buffer, size_t buffer_size)
{
    XtResourceList res_list, cons_list;
    Cardinal num_res, num_cons = 0, i;
    XtResource *res = NULL;
    union {
	char c;
	short s;
	int i;
	long l;
    } raw;
    long value;
    const char *string = "";
    Arg args[1];
    XrmValue to, from, to_color;

    /*
     * Look for the resource.
     */
    XtGetResourceList(XtClass(w), &res_list, &num_res);
    for (i = 0; i < num_res && res == NULL; i++)
    {
	if (0 == strcmp(res_list[i].resource_name, name))
	    res = &res_list[i];
    }

    cons_list = NULL;
    if (res == NULL && XtParent(w) != NULL)
    {
	XtGetConstraintResourceList(XtClass(XtParent(w)), &cons_list, &num_cons);
	for (i = 0; i < num_cons && res == NULL; i++)
	{
	    if (0 == strcmp(cons_list[i].resource_name, name))
		res = &cons_list[i];
	}
    }

    if (res == NULL)
    {
	/* Couldn't find resource */

	XtFree((char *)res_list);
	XtFree((char *)cons_list);
	return(NULL);
    }

    memset(&raw, 0, sizeof(raw));

    /* try to get the value in the proper size */
    switch (res->resource_size)
    {
    case sizeof(char):
	XtSetArg(args[0], res->resource_name, &raw.c);
	XtGetValues(w, args, 1);
	value = (long)raw.c;
	break;
    case sizeof(short):
	XtSetArg(args[0], res->resource_name, &raw.s);
	XtGetValues(w, args, 1);
	value = (long)raw.s;
	break;
    case sizeof(int):
	XtSetArg(args[0], res->resource_name, &raw.i);
	XtGetValues(w, args, 1);
	value = (long)raw.i;
	break;
    default:
	if (res->resource_size == sizeof(long)) {
	    XtSetArg(args[0], res->resource_name, &raw.l);
	    XtGetValues(w, args, 1);
	    value = raw.l;
	    break;
	}
	string = "bad size";
	goto done;
    }

    /*
     * If the resource is already String, no conversion needed.
     */
    if (strcmp(XtRString, res->resource_type) == 0)
    {
	if (res->resource_size != sizeof(char *))
	    string = "bad size";
	else if (value == 0)
	    string = "(null)";
	else
	    string = (const char *)(uintptr_t)value;
    }
    else
    {
	XtErrorMsgHandler old_handler;

	/*
	 * Ignore conversion warnings.
	 */
	old_handler = XtAppSetWarningMsgHandler(XtWidgetToApplicationContext(w),
					     EditResCvtWarningHandler);
	from.size = res->resource_size;
	from.addr = (XPointer)&raw;
	to.addr = NULL;
	to.size = 0;
	to_color.addr = NULL;
	to_color.size = 0;
	/*
	 * Special case for type Pixel.
	 */
	if (0 == strcmp(res->resource_type, XtRPixel)
	    && XtConvertAndStore(w, XtRPixel, &from, XtRColor, &to)
	    && XtConvertAndStore(w, XtRColor, &to, XtRString, &to_color))
	{
	    string = to_color.addr;
	}
	else if (XtConvertAndStore(w, res->resource_type,
				   &from, XtRString, &to))
	{
	    string = to.addr;
	}
	else
	{
	    /*
	     * Conversion failed, fall back to representing it as integer.
	     */
	    switch (res->resource_size)
	    {
	    case sizeof(char):
		snprintf(buffer, buffer_size, "%d", (unsigned char)value);
		break;
	    case sizeof(short):
		snprintf(buffer, buffer_size, "%d", (short)value);
		break;
	    case sizeof(int):
		snprintf(buffer, buffer_size, "%d", (int)value);
		break;
	    default:
		snprintf(buffer, buffer_size, "%ld", value);
		break;
	    }
	    string = buffer;
	    {
		size_t used = strlen(buffer);
		if (used < buffer_size)
		  snprintf(buffer + used, buffer_size - used, " (%s)",
			   res->resource_type);
	    }
	}
	/*
	 * Restore original warning handler.
	 */
	XtAppSetWarningMsgHandler(XtWidgetToApplicationContext(w), old_handler);
    }

    if (string == NULL)
	string = "";
  done:
    XtFree((char *)res_list);
    XtFree((char *)cons_list);
    return(string);
}

/*
 *	Function Name: DumpValues
 *	Description: Returns resource values to the resource editor.
 *	Arguments: event - the event that caused this action.
 *                 stream - the reply to add to.
 *	Returns: NULL
 */

static const char *
DumpValues(Widget w, const EditresEvent * event, ReplyStream * stream)
{
    const char * str;
    const char * res_value;
    char buffer[1024];

    /* put the count in the stream. */

    ReplyPut16(stream, 1);

    /* get the resource of the widget asked for by the */
    /* resource editor and insert it into the stream */
    if ((str = VerifyWidget(w, event->paths)) != NULL)
    {
	ReplyPutString8(stream, str);
    }
    else
    {
	res_value = GetResourceString(event->paths->widget, event->name,
				      buffer, sizeof(buffer));
	if (res_value == NULL) res_value = "NoValue";
	ReplyPutString8(stream, res_value);
    }
    return(NULL);
}

/************************************************************
 *
 * Code for Loading the EditresBlock resource.
 *
 ************************************************************/

/*	Function Name: CvStringToBlock
 *	Description: Converts a string to an editres block value.
 *	Arguments: dpy - the display.
 *                 args, num_args - **UNUSED **
 *                 from_val, to_val - value to convert, and where to put result
 *                 converter_data - ** UNUSED **
 *	Returns: TRUE if conversion was successful.
 */

/* ARGSUSED */
static Boolean
CvtStringToBlock(Display * dpy, XrmValue * args, Cardinal * num_args, XrmValue * from_val, XrmValue * to_val, XtPointer * converter_data)
{
    char ptr[BUFSIZ];
    static EditresBlock block;
#ifndef HAVE_XMU_N_COPY_ISO
     _XmNCopyISOLatin1Lowered(ptr, from_val->addr, sizeof(ptr));
#else
    XmuNCopyISOLatin1Lowered(ptr, from_val->addr, sizeof(ptr));
#endif
    if (streq(ptr, "none"))
	block = BlockNone;
    else if (streq(ptr, "setvalues"))
	block = BlockSetValues;
    else if (streq(ptr, "all"))
	block = BlockAll;
    else {
	Cardinal num_params = 1;
	String params[1];

	params[0] = from_val->addr;
	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
			"CvtStringToBlock", "unknownValue", "EditresError",
			"Could not convert string \"%s\" to EditresBlock.",
			params, &num_params);
	return(FALSE);
    }

    if (to_val->addr != NULL) {
	if (to_val->size < sizeof(EditresBlock)) {
	    to_val->size = sizeof(EditresBlock);
	    return(FALSE);
	}
	*(EditresBlock *)(to_val->addr) = block;
    }
    else
	to_val->addr = (XPointer) &block;

    to_val->size = sizeof(EditresBlock);
    return(TRUE);
}

#define XtREditresBlock ("EditresBlock")

/*	Function Name: LoadResources
 *	Description: Loads a global resource the determines of this
 *                   application should allow Editres requests.
 *	Arguments: w - any widget in the tree.
 *	Returns: none.
 */

static void
LoadResources(Widget w)
{
    static XtResource resources[] = {
        {"editresBlock", "EditresBlock", XtREditresBlock, sizeof(EditresBlock),
	 XtOffsetOf(Globals, block), XtRImmediate, (XtPointer) BlockNone}
    };

    for (; XtParent(w) != NULL; w = XtParent(w)) {}

    XtAppSetTypeConverter(XtWidgetToApplicationContext(w),
			  XtRString, XtREditresBlock, CvtStringToBlock,
			  NULL, (Cardinal) 0, XtCacheAll, NULL);

    XtGetApplicationResources( w, (XtPointer) &globals, resources,
			      XtNumber(resources), NULL, (Cardinal) 0);
}
