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
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: dogs.c /main/5 1995/07/14 10:07:19 drk $"
#endif
#endif

/*****************************************************************************
*
*  dogs.c - Square & Dog widget demo source file.
*  
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <Xm/Xm.h>
#include <Xm/MessageB.h>
#include <Mrm/MrmPublic.h>
#include "Square.h"
#include "Dog.h"

XtAppContext  app_context;

#define k_dog1_id 1
#define k_dog2_id 2
#define k_dog3_id 3
#define k_help_id 4

static void create_cb(Widget w, XtPointer client_data, XtPointer call_data);
static void bark_cb(Widget w, XtPointer client_data, XtPointer call_data);
static void tb_cb(Widget w, XtPointer client_data, XtPointer call_data);
static void scale_cb(Widget w, XtPointer client_data, XtPointer call_data);
static void help_cb(Widget w, XtPointer client_data, XtPointer call_data);
static void exit_cb(Widget w, XtPointer client_data, XtPointer call_data);

static MrmHierarchy mrm_id;
static char *mrm_vec[]={"dogs.uid"};
static MrmCode mrm_class;
static MRMRegisterArg mrm_names[] = {
        {"create_cb", (caddr_t)create_cb },
        {"bark_cb", (caddr_t)bark_cb },
        {"tb_cb", (caddr_t)tb_cb },
        {"scale_cb", (caddr_t)scale_cb },
        {"help_cb", (caddr_t)help_cb },
        {"exit_cb", (caddr_t)exit_cb }
};

static Widget dog1_id;
static Widget dog2_id;
static Widget dog3_id;
static Widget help_id;

int
main(int argc, char **argv)
{
    Widget shell;
    Display *display;
    Widget app_main = NULL;
    Arg args[3];

    MrmInitialize ();
    SquareMrmInitialize();
    DogMrmInitialize();

    XtToolkitInitialize();
    app_context = XtCreateApplicationContext();
    display = XtOpenDisplay(app_context, NULL, argv[0], "Dogs",
			NULL, 0, &argc, argv);
    
    if (display == NULL) {
	    fprintf(stderr, "%s:  Can't open display\n", argv[0]);
	    exit(1);
    }

    XtSetArg (args[0], XtNallowShellResize, True);
    XtSetArg (args[1], XtNminWidth, 620);
    XtSetArg (args[2], XtNminHeight, 370);
    shell = XtAppCreateShell(argv[0], NULL, applicationShellWidgetClass,
			  display, args, 3);

    if (MrmOpenHierarchy(1, mrm_vec, NULL, &mrm_id) != MrmSUCCESS ||
	MrmRegisterNames(mrm_names, XtNumber(mrm_names)) != MrmSUCCESS ||
	MrmFetchWidget(mrm_id, "app_main", shell, &app_main, &mrm_class) != MrmSUCCESS ||
	!app_main) {
	fprintf(stderr, "%s: cannot load dogs.uid\n", argv[0]);
	exit(EXIT_FAILURE);
    }
    XtManageChild(app_main);
    XtRealizeWidget(shell);
    XtAppMainLoop(app_context);

    return 0;    /* make compiler happy */
}

static void
create_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    const int *id = (const int *)client_data;

    (void)call_data;
    if (!id)
	return;

    switch (*id) {
        case k_dog1_id: dog1_id = w; break;
        case k_dog2_id: dog2_id = w; break;
        case k_dog3_id: dog3_id = w; break;
        case k_help_id:
	    help_id = w;
	    XtUnmanageChild((Widget)XmMessageBoxGetChild(help_id,
				XmDIALOG_CANCEL_BUTTON));
	    XtUnmanageChild((Widget)XmMessageBoxGetChild(help_id,
				XmDIALOG_HELP_BUTTON));
	    break;
    }
}

static void
bark_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    const int *volume = (const int *)client_data;

    (void)call_data;
    if (!volume)
	return;
    XBell(XtDisplay(w), *volume);
}

static void
tb_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg args[1];
    Widget dog=NULL;
    const int *tag = (const int *)client_data;
    const XmToggleButtonCallbackStruct *cb =
	(const XmToggleButtonCallbackStruct *)call_data;

    (void)w;
    if (!tag || !cb)
	return;
    switch (*tag) {
	case(1) : dog = dog1_id; break;
	case(2) : dog = dog2_id; break;	
	case(3) : dog = dog3_id; break;	
    }
    if (!dog)
	return;
    XtSetArg(args[0], SquareNmakeSquare, cb->set);
    XtSetValues(dog, args, 1);
}

static void
scale_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg args[1];
    Widget dog = NULL;
    const int *tag = (const int *)client_data;
    const XmScaleCallbackStruct *cb =
	(const XmScaleCallbackStruct *)call_data;

    (void)w;
    if (!tag || !cb)
	return;
    switch (*tag) {
	case(1) : dog = dog1_id; break;
	case(2) : dog = dog2_id; break;	
	case(3) : dog = dog3_id; break;	
    }
    if (!dog)
	return;
    XtSetArg(args[0], DogNwagTime, cb->value);
    XtSetValues(dog, args, 1);
}

static void
help_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    Arg args[1];
    XmString name = (XmString)client_data;

    (void)w;
    (void)call_data;
    if (!name || !help_id)
	return;
    XtSetArg (args[0], XmNmessageString, name);
    XtSetValues(help_id, args, 1);
    XtManageChild(help_id);
}

static void
exit_cb(Widget w, XtPointer client_data, XtPointer call_data)
{
    (void)w;
    (void)client_data;
    (void)call_data;
    exit(0);
}

