/* $XConsortium: debug.c /main/5 1995/07/15 20:38:27 drk $ */
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

#include "wsm_proto.h"
#include <stdarg.h>

static void PrintWindowInfo(
     String,
     size_t *,
     WSMWinInfo *
);
static void PrintWindowData(
     String,
     String,
     size_t *,
     WSMWinData *
);
static void PrintConfigFormat(
     String,
     size_t *,
     WSMConfigFormatReply *
);

#define STRSIZE  	(BUFSIZ * 10)

static void
DebugAppend(String str, size_t *used, const char *format, ...)
{
    va_list ap;
    int written;
    size_t available;

    if (*used >= STRSIZE - 1)
	return;

    available = STRSIZE - *used;
    va_start(ap, format);
    written = vsnprintf(str + *used, available, format, ap);
    va_end(ap);
    if (written < 0)
	return;
    if ((size_t)written >= available)
	*used = STRSIZE - 1;
    else
	*used += (size_t)written;
}

/************************************************************
 *
 *  This file contains routines to ease in debugging.
 *
 ************************************************************/

/*	Function Name: PrintRequest
 *	Description: Prints out a request in human readable form.
 *	Arguments: request - The request to print.
 *	Returns: str - The string the request is printed to.
 */

String
PrintRequest(WSMRequest *request)
{
    static char str[STRSIZE]; /* The vile hacks we do in debugging code. */
    size_t used = 0;
    register int i;

    str[0] = '\0';
    DebugAppend(str, &used, "Request: %s\n", _WSMReqTypeToName(request->any.type));

    switch (request->any.type) {
    case WSM_CONNECT:
	DebugAppend(str, &used, "Number of Versions: %d\nVersions: ",
		request->connect.num_versions);
	for (i = 0; i < request->connect.num_versions; i++) {
	    DebugAppend(str, &used, "%d", (int) request->connect.known_versions[i]);
	    if ((i + 1) < request->connect.num_versions)
		DebugAppend(str, &used, ", ");
	}
	DebugAppend(str, &used, "\n");
	break;
    case WSM_EXTENSIONS:
	DebugAppend(str, &used, "Number of Extension Suggestions: %d\n%s",
		request->extensions.num_extensions,
		"Extension Suggestions:\n        ");
	for (i = 0; i < request->extensions.num_extensions; i++) {
	    DebugAppend(str, &used, "%s", request->extensions.extension_suggestions[i]);
	    if ((i + 1) < request->extensions.num_extensions)
		DebugAppend(str, &used, ", ");
	}
	DebugAppend(str, &used, "\n");
	break;
    case WSM_CONFIG_FMT:
	DebugAppend(str, &used, "No other data for this request.\n");
	break;
    case WSM_GET_STATE:
	DebugAppend(str, &used, "Window: 0x%lX\nDiffs Allowed: %s\n",
		request->get_state.window,
		request->get_state.diffs_allowed ? "True" : "False");
	break;
    case WSM_SET_STATE:
	DebugAppend(str, &used, "Number of Windows with Info: %d\n",
		request->set_state.num_win_info_list);
	for (i = 0; i < request->set_state.num_win_info_list; i++) {
	    PrintWindowInfo(str, &used, request->set_state.win_info_list + i);
	}
	break;
    case WSM_REG_WINDOW:
	DebugAppend(str, &used, "Window: 0x%lX\n", request->register_window.window);
	break;
    case WSM_WM_GET_BACKGROUND_WINDOW:
    case WSM_WM_SET_BACKGROUND_WINDOW:
    case WSM_WM_WINDOWS:
    case WSM_WM_FOCUS:
    case WSM_WM_POINTER:
    case WSM_UNKNOWN:
	break;
    }

    return(str);
}

/*	Function Name: PrintReply
 *	Description: Prints out a reply in human readable form.
 *	Arguments: reply - The reply to print.
 *	Returns: str - The string the reply is printed to.
 */

String
PrintReply(WSMReply *reply)
{
    static char str[STRSIZE]; /* The vile hacks we do in debugging code. */
    register int i;
    size_t used = 0;

    str[0] = '\0';
    DebugAppend(str, &used, "Reply: %s\n", _WSMReqTypeToName(reply->any.type));

    switch (reply->any.type) {
    case WSM_CONNECT:
	DebugAppend(str, &used, "Version: %d\n", (int) reply->connect.version);
	break;
    case WSM_EXTENSIONS:
	DebugAppend(str, &used, "Number of Extensions: %d\n%s",
		reply->extensions.num_extensions,
		"Extensions:\n        ");
	for (i = 0; i < reply->extensions.num_extensions; i++) {
	    DebugAppend(str, &used, "%s", reply->extensions.extensions[i]);
	    if ((i + 1) < reply->extensions.num_extensions)
		DebugAppend(str, &used, ", ");
	}
	DebugAppend(str, &used, "\n");
	break;
    case WSM_CONFIG_FMT:
	PrintConfigFormat(str, &used, &(reply->config_format));
	break;
    case WSM_GET_STATE:
	DebugAppend(str, &used, "Number of Windows with Info: %d\n",
		reply->get_state.num_win_info_list);
	for (i = 0; i < reply->get_state.num_win_info_list; i++) {
	    PrintWindowInfo(str, &used, reply->get_state.win_info_list + i);
	}
	break;
    case WSM_SET_STATE:
	DebugAppend(str, &used, "No other data for this reply.\n");
	break;
    case WSM_REG_WINDOW:
        {
	    int num = reply->register_window.num_window_data;
	    WSMWinData *win_data = reply->register_window.window_data;

	    DebugAppend(str, &used, "Number of Data Attributes: %d\n", num);
	    for (i = 0; i < num; i++, win_data++) {
		PrintWindowData(str, "", &used, win_data);
	    }
	}
	break;
    case WSM_WM_GET_BACKGROUND_WINDOW:
    case WSM_WM_SET_BACKGROUND_WINDOW:
    case WSM_WM_WINDOWS:
    case WSM_WM_FOCUS:
    case WSM_WM_POINTER:
    case WSM_UNKNOWN:
	break;
    }

    return(str);
}

/************************************************************
 *
 *  Internal Routines.
 *
 ************************************************************/

/*	Function Name:  PrintWindowInfo
 *	Description: Prints the info about a single window.
 *	Arguments: str - String to stuff it into.
 *                 win_info - The window info to print.
 *	Returns: none.
 */

static void
PrintWindowInfo(
     String      str,
     size_t      *used,
     WSMWinInfo *win_info)
{
    register int i;
    int num = win_info->num_data_list;
    WSMWinData *win_data = win_info->data_list;

    DebugAppend(str, used, "Window: 0x%lX\nNumber of Data Attributes: %d\n",
		win_info->window, num);

    for (i = 0; i < num; i++, win_data++)
	PrintWindowData(str, "       ", used, win_data);
}

/*\tFunction Name:  PrintWindowData
 *\tDescription: Prints the data about a single window.
 *\tArguments: str - String to stuff it into.
 *                 win_data - The window data to print.
 *\tReturns: none.
 */

static void
PrintWindowData(
     String      str,
     String      tab_str,
     size_t      *used,
     WSMWinData *win_data)
{
    register int i;
    char *type_str = NULL;

    if (win_data->type == WSM_VALUE_DATA) {
	DebugAppend(str, used, "%sName: %s, Type: Value, Value: %ld\n", tab_str,
		    XrmQuarkToString(win_data->nameq), win_data->data.value);
	return;
    }

    switch(win_data->type) {
    case WSM_CHAR_LIST_DATA:
	type_str = "Char List";
	break;
    case WSM_SHORT_LIST_DATA:
	type_str = "Short List";
	break;
    case WSM_LONG_LIST_DATA:
	type_str = "Long List";
	break;
    case WSM_VALUE_DATA:
    case WSM_NONE:
	type_str = "<NONE>";
	break;
    }

    DebugAppend(str, used, "%sName: %s, Type: %s, Len: %d\n%s%s", tab_str,
		XrmQuarkToString(win_data->nameq), type_str,
		win_data->data_len, tab_str, "        ");

    for (i = 0; i < win_data->data_len; i++) {
	switch(win_data->type) {
	case WSM_CHAR_LIST_DATA:
	    DebugAppend(str, used, "%c(%d)", win_data->data.char_ptr[i],
			(int) win_data->data.char_ptr[i]);
	    break;
	case WSM_SHORT_LIST_DATA:
	    DebugAppend(str, used, "%d", (int) win_data->data.short_ptr[i]);
	    break;
	case WSM_LONG_LIST_DATA:
	    DebugAppend(str, used, "%ld", win_data->data.long_ptr[i]);
	    break;
	case WSM_VALUE_DATA:
	case WSM_NONE:
	    break;
	}
	if (i < (win_data->data_len - 1))
	    DebugAppend(str, used, ", ");
    }

    DebugAppend(str, used, "\n");
}

/*\tFunction Name: PrintConfigFormat
 *\tDescription: Prints the configuration format.
 *\tArguments: str - String to print the conf info into.
 *                 config_fmt - The configuration format reply.
 *\tReturns: none.
 */

static void
PrintConfigFormat(
     String                str,
     size_t                *used,
     WSMConfigFormatReply *config_fmt)
{
    register int i;

    DebugAppend(str, used, "Accepts Diffs: %s\n",
		(config_fmt->accepts_diffs ? "True" : "False"));

    for (i = 0; i < 3; i++) {
	WSMAttribute *fmt;
	register int j, num;

	switch (i) {
	case WSM_GLOBAL_FMT:
	    fmt = config_fmt->global_formats;
	    num = config_fmt->num_global_formats;
	    DebugAppend(str, used, "Global Formats - Number: %d\n", num);
	    break;
	case WSM_WINDOW_FMT:
	    fmt = config_fmt->window_formats;
	    num = config_fmt->num_window_formats;
	    DebugAppend(str, used, "Window Formats - Number: %d\n", num);
	    break;
	case WSM_ICON_FMT:
	    fmt = config_fmt->icon_formats;
	    num = config_fmt->num_icon_formats;
	    DebugAppend(str, used, "Icon Formats - Number: %d\n", num);
	    break;
	default:
	    fmt = NULL;
	    num = 0;
	    break;
	}

	for (j = 0; j < num; j++)
	    DebugAppend(str, used, "        Name: %s, Size: %d, Is_List: %s\n",
			XrmQuarkToString(fmt[j].nameq), (int) fmt[j].size,
			fmt[j].is_list ? "True" : "False");
    }
}
